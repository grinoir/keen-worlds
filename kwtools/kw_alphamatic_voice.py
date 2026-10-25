"""Voice The Alphamatic's illustrated story/endings from decoded original text.

Usage: python3 kwtools/kw_alphamatic_voice.py AUDIT_TEXT_DIR OUTPUT_DIR
The audit tool extracts the layouts from the player's own v3.0 archive.
Portrait IDs identify the speakers. Recurring characters keep their established
voices; the bearded caller uses the elder voice requested for Keen Worlds.
A manifest records every turn for review/regeneration.
"""
from collections import defaultdict
from pathlib import Path
import json
import hashlib
import re
import subprocess
import sys
import tempfile
import os

from kw_keen4_voice import KEEN, NARRATOR, LINDSEY, COUNCIL
from kw_keen789_voice import BARKER, SPOT, STYLE
from kw_landing_voice import speak_all, RATE, VOICE, LANDINGS, COURSES, SAVED

VORTICON, ELDER, SHIKADI, VEO, COMPUTER = BARKER, COUNCIL, 'am_adam','am_fenrir','am_echo'
PORTRAITS = {14:VORTICON,15:ELDER,16:LINDSEY,17:SPOT,18:KEEN,19:KEEN,
             21:KEEN,41:SHIKADI,47:KEEN,48:LINDSEY,49:ELDER,65:VEO}
COMMAND = re.compile(r'\^([GT])(\d+),(\d+),(\d+)(?:,(\d+))?|\^[Ee]')


def turns(page):
    # Colours and text positions are layout, never changes of speaker.
    page=re.sub(r'\^[cC][0-9a-fA-F]','',page)
    page=re.sub(r'\^[BL][\d, ]+','',page)
    queue, output, used, last = [], [], False, 0

    def text(raw):
        nonlocal used
        for paragraph in re.split(r'\n\s*\n\s*\n',raw):
            words = re.sub(r'\s+',' ',paragraph).strip()
            if not words: continue
            narrator = words.startswith(('Keen climbs','Following his','Ten minutes later','Meanwhile','Setting his sights'))
            if words.startswith(("You've just completed", "You've just cheated")): continue
            if narrator:
                voice = NARRATOR
            elif queue:
                voice = queue.pop(0) if len(queue)>1 else queue[0]
                used = True
            else:
                voice = COMPUTER if words.startswith(('Codename:', 'Status:', 'Description:', 'Manufacturer:', 'Deployment:')) else NARRATOR
            # Timed redraws sometimes repeat the same spoken reaction.
            if (voice,words) not in output:
                output.append((voice,words))

    for command in COMMAND.finditer(page):
        text(page[last:command.start()]);last=command.end()
        if command[1]:
            picture=int(command[4])
            if command[1]=='T' and picture==20:
                queue=[COMPUTER];used=False
            elif command[1]=='G' and picture in PORTRAITS:
                if used: queue=[];used=False
                queue.append(PORTRAITS[picture])
    text(page[last:])
    return output


def clips_from(source):
    clips = {}
    for name, stem in [('story','STORY'),('ending','END'),('secret-ending','ENDS')]:
        pages = re.split(r'\^[Pp]',(source/(name+'.txt')).read_text(encoding='latin1'))[1:]
        for index, page in enumerate(pages):
            if (stem,index) in [('END',17),('ENDS',45)]: continue # author's closing note
            result=turns(page)
            # Some portraits remain on screen while another person replies.
            # These authored turns share one paragraph in the layout file.
            if stem=='STORY' and index==3:
                result[1]=(KEEN,result[1][1])
            if stem=='STORY' and index in (4,9,15):
                marker={4:'Woah, the Krile?',9:'Well this sounds familiar.',15:'How precise...'}[index]
                fixed=[]
                for voice,words in result:
                    if marker in words:
                        before,after=words.split(marker,1)
                        fixed.extend([(voice,before.strip()),
                                      (LINDSEY if index==4 else KEEN,marker+after)])
                    else: fixed.append((voice,words))
                result=fixed
                if index==9: result[-1]=(VORTICON,result[-1][1])
            if stem=='ENDS' and index==41:
                result=[(SHIKADI,words) for _,words in result]
            if result: clips[f'AL_{stem}_{index:02d}.RAW']=result
    clips['AL_KEEN_FUSE.RAW']=[(KEEN,'One of the four machines protecting the main elevator shaft: toast!')]
    clips['AL_KEEN_FUSE_KORATH.RAW']=[(KEEN,'That should enable the first elevator.')]
    clips['LAND_ALPHAM.RAW']=[(VOICE,' '.join(LANDINGS['alpham']))]
    clips['COURSE_ALPHAM.RAW']=[(VOICE,'A new course is open. '+COURSES['alpham']+'.')]
    clips['SAVED_ALPHAM.RAW']=[(VOICE,' '.join(SAVED['alpham']))]
    return clips


def main(source, output):
    source, output=Path(source),Path(output)
    output.mkdir(parents=True,exist_ok=True)
    clips=clips_from(source)
    (output/'voice-manifest.json').write_text(json.dumps(clips,indent=2)+'\n')
    # Keep generated turns so an interrupted job can resume without remaking
    # voices, and the short clips can be reviewed individually.
    parts=output/'parts';parts.mkdir(exist_ok=True)
    by_voice=defaultdict(dict)
    segments={}
    for clip, lines in clips.items():
        segments[clip]=[]
        for i,(voice,words) in enumerate(lines):
            digest=hashlib.sha256((voice+'\n'+words).encode()).hexdigest()[:10]
            name=f'{clip[:-4]}_{i:02d}_{digest}.RAW'
            segments[clip].append(name)
            by_voice[voice][name]=re.split(r'(?<=[.!?])\s+(?=[A-Z])',words)
    generated=0
    limit=int(os.environ.get('KW_VOICE_MAX_TURNS','0'))
    for voice,jobs in by_voice.items():
        pending={name:words for name,words in jobs.items() if not (parts/name).exists()}
        pending=list(pending.items())
        for at in range(0,len(pending),8):
            batch=pending[at:at+8]
            if limit:
                if generated>=limit:
                    print('Voice turns saved; run again to continue the remaining turns.',flush=True)
                    return
                batch=batch[:limit-generated]
            print(f'Generating {voice}: {len(batch)} turns',flush=True)
            speak_all(dict(batch),voice,str(parts),**STYLE.get(voice,{}))
            generated+=len(batch)
            if len(batch)<len(pending[at:at+8]):
                print('Voice turns saved; run again to continue.',flush=True)
                return
    for clip,names in segments.items():
        raw=bytearray()
        for i,name in enumerate(names):
            if i: raw+=bytes(2*int(RATE*.25))
            raw+=(parts/name).read_bytes()
        destination=output/clip
        destination.write_bytes(raw)
        # The established voice processing precedes this final headroom pass,
        # keeping the 16-bit Vorbis decoder below full scale.
        temporary=destination.with_suffix('.tmp')
        subprocess.run(['ffmpeg','-v','error','-y','-f','s16le','-ar',str(RATE),'-ac','1',
                        '-i',str(destination),'-af','volume=0.45','-f','s16le',str(temporary)],check=True)
        temporary.replace(destination)
    print(f'{len(clips)} Alphamatic voice clips generated')


if __name__=='__main__':
    if len(sys.argv)!=3: sys.exit(__doc__)
    main(*sys.argv[1:])
