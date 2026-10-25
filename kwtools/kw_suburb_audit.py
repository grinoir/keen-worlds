"""Static Suburb Shenanigans evidence; never executes a DOS program or installs a world.

Usage: python3 kwtools/kw_suburb_audit.py SUBURB_FOLDER KEEN5E.EXE OUTPUT
The input folder has source/v1.0, source/v1.0.3_unofficial, source/v1.1 and source/v1.2.
"""
from collections import Counter
import contextlib
import hashlib
import io
import json
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys
import tempfile

import keenmap
from kw_accumulators import cstring
from kw_patconv import Game, parse_pat, strip_comments, cmd_actions, cmd_code, cmd_data

# Classification applies to byte directives, not to a claim that an entire
# creature is data-only. Several changed routines retain their stock names.
CONFIG_CODE = {
    0x7799:'ck_maxWarpLevel=23', 0xD2C2:'ck_maxEnterLevel=23',
    0x8C20:'CK_NumMusicLevels=28', 0x8C61:'ca_audStartMusic=190',
    0x8BF1:'native audio pointer table / ca_audStartMusic',
    0x8BFF:'native audio pointer table / ca_audStartMusic',
    0x8CF5:'native audio pointer table / ca_audStartMusic',
    0x8C0D:'ca_audNumSongs=16', 0xEC67:'CK_EndingMusicLevel (last write is 26)',
    0xEABD:'CK_HelpMusicLevel=25', 0xF808:'CK_GameOverMusicLevel=27',
    0x138EC:'CK5_KorathSpawnYOffset=-576', 0x12EFA:'CK5_ShikadiHealth=3',
    0x12B36:'CK5_MasterShoot action alias requires redirectable spawn',
    0x12B3D:'CK5_MasterShotYOffset=64',
    0x13306:'CK5_ShocksundJumpYVel=-80', 0x13495:'CK5_ShocksundJumpYVel=-80',
    0x132C9:'CK5_ShocksundStandChance=8',
    0x12A65:'CK5_SpindredFloorSlamVel=56', 0x12A32:'CK5_SpindredFloorBounces=65535',
    0x3CF0:'reuse CK_SecurityDoorOpen for 4x4 / tile -4 actions; verify timing and coordinates',
    0x11A52:'CK5_ShelleyBits0XVel=7 (custom bee spawn still needed)',
    0x11290:'CK5_AmptonSpawnYOffset=-496',
    0x1167B:'CK5_AmptonSilent is broader: use only if walk sounds are equivalent',
    0x12643:'CK5_RoboNumShots=1', 0x126E3:'CK5_RoboShotXVelocity=30',
    0x126BD:'CK5_RoboShotRightXOffset=293', 0x127F7:'CK5_SpirogripLaunchChance=40',
    0x126EE:'CK5_RoboShotYVelocity=0',
}
# These patches address segmented DOS storage, VGA timing or a feature already
# provided by the native renderer/cache. Their intent is retained, not the bytes.
NATIVE_ALREADY = {
    0x3CDF,0x7A72,0x7401,0x7679,0x21C01,0x1F795,0x1F816,
    0x13CC9,0x13C80,0x14403,0x6179,0x5C1F,0x35EB8,0x1E31F,
    0x1675F,0x19731,0xEB21,0xEC3B,0xEC4D,0xEC58,0xED11,0xED1F,0x14EE8,
}


def digest(data):
    return hashlib.sha256(data).hexdigest()


def huff(data, nodes, size):
    out = bytearray(); node = 254
    for byte in data:
        for bit in range(8):
            value = nodes[2*node + ((byte >> bit) & 1)]
            if value < 256:
                out.append(value); node = 254
                if len(out) == size:
                    return bytes(out)
            else:
                node = value - 256
    raise ValueError(f'Truncated Huffman stream: {len(out)}/{size}')


def previews(files, base, output, maps):
    """Static decoded tile art, deliberately not described as in-game captures."""
    from PIL import Image, ImageDraw
    from keenrender import Graphics
    destination = output/'previews';destination.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='suburb-pictures-') as temp:
        temp=Path(temp)
        for name in ('EGAGRAPH.CK5','EGAHEAD.CK5','EGADICT.CK5'):
            shutil.copy2(files[name.lower()],temp/name)
        shutil.copy2(base/'GFXINFOE.CK5',temp/'GFXINFOE.CK5')
        graph=Graphics(str(temp),5)
        sheet=Image.new('RGB',(4*340,6*236),(24,24,24));draw=ImageDraw.Draw(sheet)
        with tempfile.TemporaryDirectory(prefix='suburb-map-pictures-') as mapdir:
            mapdir=Path(mapdir)
            for name in ('MAPHEAD.CK5','GAMEMAPS.CK5'):shutil.copy2(files[name.lower()],mapdir/name)
            for row in maps:
                level=row['level'];_,w,h,planes=keenmap.level(mapdir,5,level)
                im=Image.new('RGBA',(w*16,h*16),(0,0,0,255))
                for i,(bg,fg) in enumerate(zip(planes[0],planes[1])):
                    at=(i%w*16,i//w*16)
                    tile=graph.tile(bg,False)
                    if tile:im.paste(tile,at)
                    if fg:
                        tile=graph.tile(fg,True)
                        if tile:im.alpha_composite(tile,at)
                im=im.convert('RGB');im.save(destination/f'map-{level:02}.png')
                marked=im.copy();pen=ImageDraw.Draw(marked)
                for a in row['actors']:
                    if a['info'] not in (1,2,3,70,108,109):continue
                    x,y=(p*16 for p in a['at'])
                    pen.rectangle((x,y,x+15,y+15),outline='yellow',width=2)
                    pen.text((x,y-11),str(a['info']),fill='yellow',stroke_width=1,stroke_fill='black')
                if level==0:
                    for n in sorted(set(e['level'] for e in row['map_entries'] if e['kind']=='0xc0')):
                        e=next(e for e in row['map_entries'] if e['kind']=='0xc0' and e['level']==n)
                        x,y=(p*16 for p in e['at']);pen.text((x,y),str(n),fill='yellow',stroke_width=2,stroke_fill='black')
                marked.save(destination/f'map-{level:02}-marked.png')
                thumb=marked.copy();thumb.thumbnail((332,210),Image.Resampling.NEAREST)
                x=level%4*340;y=level//4*236
                draw.text((x+4,y+3),f"{level}: {row['name'].replace(chr(10),' ')}",fill='white')
                sheet.paste(thumb,(x+4,y+20))
        sheet.save(destination/'map-contact-sheet.png')


def audit(root, exe, output):
    root, output = Path(root), Path(output)
    output.mkdir(parents=True, exist_ok=True)
    base = Path(__file__).resolve().parents[1]/'data/keen5'
    g = Game(str(exe), str(base))
    result = {'status': 'static audit only; no Suburb runtime implementation',
              'exe_sha256': digest(Path(exe).read_bytes()), 'versions': {}}
    for version in ('v1.0', 'v1.0.3_unofficial', 'v1.1', 'v1.2'):
        patch = next(p for p in (root/'source'/version).rglob('*') if p.name.lower() == 'pat.pat')
        files = {p.name.lower(): p for p in patch.parent.iterdir() if p.is_file()}
        folder = output/version; folder.mkdir(exist_ok=True)
        pats = parse_pat(patch)
        active = re.split(r'(?im)^\s*%end\b', strip_comments(patch.read_text(encoding='latin1')), maxsplit=1)[0]
        image = bytearray(g.img)
        filepats = []; extensions = []
        # Apply byte patches and patchfiles in authored order, including overlaps.
        commands = list(re.finditer(r'(?im)^\s*%(patchfile|patch)\s+\$([\da-f]+)([^%]*)', active))
        index = 0
        for command in commands:
            at = int(command[2], 16)
            if command[1].lower() == 'patchfile':
                name = command[3].strip().split()[0].strip('"')
                data = files[name.lower()].read_bytes()
                filepats.append({'offset': hex(at), 'file': name, 'bytes': len(data)})
            else:
                off, data = pats[index]; index += 1
                assert off == at
            assert at + len(data) < 0x100000
            if at + len(data) > len(image):
                extensions.append({'offset':hex(at),'bytes':len(data)})
                image.extend(bytes(at+len(data)-len(image)))
            image[at:at+len(data)] = data
        assert index == len(pats)
        for label, command in [('actions', cmd_actions), ('code', cmd_code), ('data', cmd_data)]:
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf): command(g, pats)
            (folder/(label+'.txt')).write_text(buf.getvalue())
        (folder/'story.txt').write_bytes(files['story.ck5'].read_bytes().split(b'\0')[0])
        dictionary = files.get('egadict.ck5')
        if dictionary:
            dictionary = dictionary.read_bytes()
        else:
            original = (base/'EGADICT.CK5').read_bytes()
            pos = g.img.find(original); assert pos >= 0
            dictionary = image[pos:pos+len(original)]
        nodes = struct.unpack('<510H', dictionary[:1020])
        graph = files['egagraph.ck5'].read_bytes()
        head = files['egahead.ck5'].read_bytes()
        starts = [int.from_bytes(head[i:i+3], 'little') for i in range(0, len(head), 3)]
        assert all(v == 0xFFFFFF or v <= len(graph) for v in starts)

        def chunk(number):
            start = starts[number]; assert start != 0xFFFFFF
            end = next((x for x in starts[number+1:] if x != 0xFFFFFF), len(graph))
            size = struct.unpack_from('<I', graph, start)[0]
            assert 0 < size < 2000000
            payload = graph[start+4:end]
            if len(payload) == size:
                if files.get('egadict.ck5'):
                    assert huff(payload, nodes, size) == payload
                return payload
            return huff(payload, nodes, size)

        texts = {}
        for label, number in [('help',4914), ('controls',4915), ('story-layout',4916),
                              ('credits',4917), ('ending',4918), ('secret-ending',4919), ('ordering',4920)]:
            data = chunk(number)
            (folder/(label+'.txt')).write_bytes(data)
            texts[label] = {'chunk':number, 'bytes':len(data), 'pages':len(re.findall(rb'\^P',data,re.I)), 'sha256':digest(data)}
        gfx = struct.unpack('<23H', (base/'GFXINFOE.CK5').read_bytes())
        back_count, fore_count = gfx[2:4]
        tiles = files['keen5.tli'].read_bytes()
        assert len(tiles) == 2*back_count + 7*fore_count
        misc = tiles[2*back_count + 5*fore_count:2*back_count + 6*fore_count]
        maps = []; planes_by_level = {}
        with tempfile.TemporaryDirectory(prefix='suburb-audit-') as temp:
            temp = Path(temp)
            for name in ('MAPHEAD.CK5','GAMEMAPS.CK5'):
                shutil.copy2(files[name.lower()], temp/name)
            _, offsets, _ = keenmap.load(temp,5)
            for level, offset in enumerate(offsets):
                if offset in (0,0xFFFFFFFF): continue
                internal,w,h,planes = keenmap.level(temp,5,level)
                assert all(len(p)==w*h for p in planes)
                assert max(planes[0]) < back_count and max(planes[1]) < fore_count
                names = []
                for table in (0x31AB4,0x31B18):
                    ptr = struct.unpack_from('<I',image,table+4*level)[0]
                    names.append(cstring(image,(ptr>>16)*16+(ptr&65535)))
                entrances = []; special = []; actors = []
                for i,(tile,info) in enumerate(zip(planes[1],planes[2])):
                    x,y = i%w,i//w
                    if level==0 and info>>8 in (0xC0,0xD0,0xF0):
                        entrances.append({'at':[x,y], 'kind':hex(info>>8),'level':info&255})
                    if (misc[tile]&127) in (2,3,4,5,6,7,8,9,10,20,21,32,33,34) or info>=0x100:
                        special.append({'at':[x,y],'tile':tile,'misc':misc[tile]&127,'info':info})
                    if info and info<0x100:
                        actors.append({'at':[x,y],'info':info})
                raw = struct.pack('<%dH'%(3*w*h),*(v for p in planes for v in p))
                planes_by_level[level] = planes
                maps.append({'level':level,'name':names[0],'entry':names[1],'header_name':internal,
                             'size':[w,h],'sha256_planes':digest(raw),'info_counts':dict(sorted(Counter(planes[2]).items())),
                             'map_entries':entrances,'special_tiles':special,'actors':actors})
        audio = files['audio.ck5'].read_bytes()
        ah = files['audiohed.ck5'].read_bytes()
        offsets = struct.unpack('<%dI'%(len(ah)//4),ah)
        assert all(0<=a<=b<=len(audio) for a,b in zip(offsets,offsets[1:]))
        explicit_audio = all(a==b or struct.unpack_from('<I',audio,a)[0]==b-a-4 for a,b in zip(offsets,offsets[1:]))
        # Every changed byte directive is retained with its exact source line.
        directives = []
        patch_text = patch.read_text(encoding='latin1')
        matches = list(re.finditer(r'(?im)^[ \t]*%patch\s+\$([\da-f]+)',patch_text))
        for match,(offset,data) in zip(matches,pats):
            if g.img[offset:offset+len(data)] == data:
                group,reason = 'plain Keen 5','unchanged bytes'
            elif offset in NATIVE_ALREADY:
                group,reason = 'plain Keen 5','native equivalent already exists; DOS implementation not transplanted'
            elif offset in CONFIG_CODE:
                # These two redirections have no current setting with exact semantics.
                group = 'native code' if offset in (0x12B36,0x1167B) else 'settings'
                reason = CONFIG_CODE[offset]
            elif offset >= g.dseg:
                group,reason = 'settings','action/table/text/palette metadata; custom function references still need native routines'
            elif offset >= 0x20000:
                group,reason = 'settings','relocated presentation text/table'
            else:
                group,reason = 'native code','modified gameplay, progression or presentation routine; inspect source comments and disassembly'
            directives.append({'line':patch_text.count('\n',0,match.start())+1,
                               'offset':hex(offset),'bytes':len(data),'new_hex':data.hex(),
                               'group':group,'reason':reason})
        version_result = {
            'files':{p.name:{'bytes':p.stat().st_size,'sha256':digest(p.read_bytes())} for p in sorted(files.values())},
            'active_byte_patch_directives':len(pats), 'patchfiles':filepats,'patch_directives':directives,
            'patch_groups':dict(Counter(d['group'] for d in directives)),
            'patches_beyond_original_executable':extensions,
            'graphics_header_entries':len(starts),'text_layouts':texts,
            'audio':{'chunks':len(offsets)-1,'music_start':190,'songs':16,'length_prefixed_uncompressed':explicit_audio},
            'music_table':list(struct.unpack_from('<28H',image,0x3067A)), 'maps':maps,
        }
        result['versions'][version] = version_result
        # Capture the useful semantic comparison values before the closure advances.
        version_result['decoded_texts'] = {k:(folder/(k+'.txt')).read_text(encoding='latin1') for k in texts}
        if version == 'v1.2':
            # Mechanical graph: same-level doors are not checkpoint transitions.
            connections = []
            for row in maps:
                w,h = row['size']; level=row['level']
                for actor in row['actors']:
                    if actor['info'] == 108:
                        connections.append({'from':level,'at':actor['at'],'kind':'checkpoint','to':level+1,
                                            'return_to_map':False})
                    elif actor['info'] == 109:
                        connections.append({'from':level,'at':actor['at'],'kind':'finish split pair',
                                            'complete':[level-1,level],'to':0})
                doors=[]; keys=[]
                for tile in row['special_tiles']:
                    if tile['misc'] == 2:
                        dest=[tile['info']>>8,tile['info']&255]
                        doors.append({**tile,'destination':dest,'same_level':True,
                                      'in_bounds':dest[0]<w and dest[1]<h,
                                      'dummy_or_zero':tile['info']==0})
                    elif 7 <= tile['misc'] <= 10:
                        keys.append({**tile,'key_colour':tile['misc']-7,
                                     'door_top_left':[tile['info']>>8,tile['info']&255]})
                row['doors']=doors;row['keyholders']=keys
            (folder/'connections.json').write_text(json.dumps(connections,indent=2)+'\n')
            # Extract popup strings directly from the patched image; speaker
            # identity is reviewed in the report, not guessed by the extractor.
            popups=[{'event':event,'address':hex(at),'text':cstring(image,at)} for event,at in
                    [('leave home',0x320A2),('bus blocked',0x31B9A),('bag found',0x31BE4)]]
            (folder/'popups.json').write_text(json.dumps(popups,indent=2)+'\n')
            casts = {
                'story-layout': ['Mom','Keen','Mom','unconfirmed caller','Brother','Brother','Keen','Brother',
                                 'Mom','Keen','unconfirmed offscreen voice','Keen','Mom','Mom','Keen','Mom',
                                 'Mom','Keen','sound effect','Keen','Mom','Keen','Mom','Keen','Mom','Keen','Keen'],
                'ending': ['Keen','Mortimer (android)','Keen','Keen','Keen','Keen','Keen','Keen','Keen','Keen',
                           'Keen','narrator (quoted phrase)','Keen','Mortimer (recording)','Keen','Mortimer (recording)',
                           'Keen','Brother','Keen','Brother','narrator (title)'],
            }
            casts['secret-ending'] = casts['ending']
            dialogue=[]
            for layout,cast in casts.items():
                text=(folder/(layout+'.txt')).read_text(encoding='latin1')
                quotes=list(re.finditer(r'"([^"]*)"',text));assert len(quotes)==len(cast)
                for i,(quote,speaker) in enumerate(zip(quotes,cast)):
                    dialogue.append({'layout':layout,'quote':i,'page':len(re.findall(r'\^P',text[:quote.start()],re.I)),
                                     'speaker':speaker,'text':quote[1]})
            dialogue += [{'layout':'popup','event':p['event'],'speaker':'Keen','text':p['text'],
                          'speaker_basis':'first-person words and Billy portrait chunks 90/91'} for p in popups]
            (folder/'dialogue.json').write_text(json.dumps(dialogue,indent=2)+'\n')
            with tempfile.TemporaryDirectory(prefix='suburb-disassembly-') as temp:
                binary=Path(temp)/'data.bin';binary.write_bytes(image)
                dump=[]
                for title,start,end in [('ending selector',0xEC2F,0xEC66),('bus auto-ending',0xFE65,0xFE8D),
                                        ('checkpoint and finish objects',0x6B90,0x6D70),('bag completion',0x6A79,0x6B11),
                                        ('4 by 4 door opening',0x3CF0,0x3D70)]:
                    run=subprocess.run(['objdump','-D','-b','binary','-m','i8086',
                                        f'--start-address={start}',f'--stop-address={end}',str(binary)],
                                       text=True,capture_output=True,check=True)
                    dump += [title,run.stdout.replace(str(binary),'patched-data-image')]
                (folder/'progression-disassembly.txt').write_text('\n'.join(dump))
            rows=['# Byte-patch classification','',
                  'All active directives are listed once. Settings that reference a changed',
                  'function are not sufficient until that native routine has been ported.','',
                  '| Line | Offset | Bytes | Group | Treatment |','|---:|---|---:|---|---|']
            rows += [f"| {d['line']} | {d['offset']} | {d['bytes']} | {d['group']} | {d['reason']} |" for d in directives]
            (folder/'patch-groups.md').write_text('\n'.join(rows)+'\n')
            previews(files,base,folder,maps)
        print(version, len(maps),'maps;',len(pats),'byte directives;',sum(v['pages'] for v in texts.values()),'layout pages')
    one,two = (result['versions'][v] for v in ('v1.1','v1.2'))
    onefiles = {k.lower():v for k,v in one['files'].items()}
    twofiles = {k.lower():v for k,v in two['files'].items()}
    result['comparison_1_1_to_1_2'] = {
        'identical_files':[k for k in onefiles.keys()&twofiles.keys() if onefiles[k]['sha256']==twofiles[k]['sha256']],
        'changed_files':[k for k in onefiles.keys()&twofiles.keys() if onefiles[k]['sha256']!=twofiles[k]['sha256']],
        'map_planes_identical':all(a['sha256_planes']==b['sha256_planes'] for a,b in zip(one['maps'],two['maps'])),
        'changed_text_layouts':[k for k in one['text_layouts'] if one['text_layouts'][k]['sha256']!=two['text_layouts'][k]['sha256']],
    }
    result['archives'] = {p.name:{'bytes':p.stat().st_size,'sha256':digest(p.read_bytes())} for p in sorted(root.glob('*.zip'))}
    (output/'inventory.json').write_text(json.dumps(result,indent=2)+'\n')


if __name__ == '__main__':
    if len(sys.argv)!=4: sys.exit(__doc__)
    audit(*sys.argv[1:])
