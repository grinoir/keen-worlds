"""Compile the workshop scene and its abridged script; optionally make speech.

python kwtools/kw_alphamatic_opening.py ART_FOLDER HEADER
python kwtools/kw_alphamatic_opening.py --voices OUTPUT_FOLDER
Uses the established local Kokoro setup, cast and processing for speech.
"""
from pathlib import Path
from collections import defaultdict
import json
import re
import subprocess
import sys
from PIL import Image
from kw_title_art import EGA, palette_image, write_array
from kw_keen4_voice import KEEN, NARRATOR, LINDSEY, COUNCIL
from kw_keen789_voice import BARKER, SPOT, STYLE
from kw_landing_voice import speak_all, RATE

ROOT=Path(__file__).resolve().parents[1]
SCRIPT=ROOT/'data/mods/alphamatic/opening.json'
CAST={'Keen':KEEN,'Spot':SPOT,'Vorticon':BARKER,'Lindsey':LINDSEY,'Elder':COUNCIL}
SPEAKERS={'Keen':0,'Spot':1,'Vorticon':2,'Lindsey':3,'Elder':4}
VIEWS={'call':0,'blueprint':1,'mars':2}

def alpha_pixels(folder,name,im):
    palette=palette_image([(c>>16,(c>>8)&255,c&255) for c in EGA])
    rgb=im.convert('RGB').quantize(palette=palette,dither=Image.Dither.NONE)
    pixels=list(rgb.getdata());alpha=list(im.getchannel('A').getdata())
    indexed=[c if a>=128 else 16 for c,a in zip(pixels,alpha)]
    out=rgb.convert('RGBA');out.putalpha(im.getchannel('A').point(lambda a:255 if a>=128 else 0))
    out.save(folder/(name+'-ega.png'))
    out.resize((im.width*6,im.height*6),Image.Resampling.NEAREST).save(folder/(name+'-preview.png'))
    return indexed

def flight_sprites(folder):
    result=[]
    for name,source,w,h in [('station-metal','station-metal-source.png',124,104),
                            ('megarocket-flight','megarocket-flight-source.png',64,28)]:
        im=Image.open(folder/source).convert('RGBA')
        bounds=im.getchannel('A').point(lambda a:255 if a>=128 else 0).getbbox()
        assert bounds,'Missing '+name
        im=im.crop(bounds);scale=min(w/im.width,h/im.height)
        resized=im.resize((round(im.width*scale),round(im.height*scale)),Image.Resampling.BOX)
        canvas=Image.new('RGBA',(w,h));canvas.paste(resized,((w-resized.width)//2,h-resized.height))
        pixels=alpha_pixels(folder,name,canvas)
        if name=='station-metal': assert not ({2,10}&set(pixels)),'Physical station must not be green'
        result.append(pixels)
    return result

def actor_frames(folder):
    """Compile true-alpha art at its drawing size, without gameplay-sprite enlargement.

    Index 16 is transparent; black and gray inside the subject stay opaque.
    The helmet anchors keep Keen steady while his arms change pose.
    """
    sheet=Image.open(folder/'keen-poses-source.png').convert('RGBA')
    cells=[];anchors=[]
    for i in range(3):
        cell=sheet.crop((i*sheet.width//3,0,(i+1)*sheet.width//3,sheet.height))
        bounds=cell.getchannel('A').point(lambda a:255 if a>=128 else 0).getbbox()
        assert bounds,'Missing Keen pose'
        cell=cell.crop(bounds);cells.append(cell)
        greens=[x for y in range(cell.height//2) for x in range(cell.width)
                if (lambda p:p[3]>=128 and p[1]>110 and p[1]>p[0]*1.25 and p[1]>p[2]*1.25)(cell.getpixel((x,y)))]
        assert greens,'Missing green helmet'
        anchors.append((min(greens)+max(greens))/2)
    scale=64/max(im.height for im in cells)
    keen=[]
    for i,(im,anchor) in enumerate(zip(cells,anchors)):
        resized=im.resize((round(im.width*scale),round(im.height*scale)),Image.Resampling.BOX)
        canvas=Image.new('RGBA',(44,64))
        left=round(18-anchor*scale)
        assert left>=0 and left+resized.width<=44,'Keen gesture clipped'
        canvas.paste(resized,(left,64-resized.height))
        keen.append(canvas)
    spot=Image.open(folder/'spot-full-source.png').convert('RGBA')
    bounds=spot.getchannel('A').point(lambda a:255 if a>=128 else 0).getbbox()
    assert bounds,'Missing full-body Spot'
    spot=spot.crop(bounds);scale=min(32/spot.width,44/spot.height)
    resized=spot.resize((round(spot.width*scale),round(spot.height*scale)),Image.Resampling.BOX)
    canvas=Image.new('RGBA',(32,44));canvas.paste(resized,((32-resized.width)//2,44-resized.height))
    result=[]
    for name,im in [(f'keen-pose-{i}',im) for i,im in enumerate(keen)]+[('spot-full',canvas)]:
        result.append(alpha_pixels(folder,name,im))
    return result

def script():
    return json.loads(SCRIPT.read_text())

def voices(output):
    output=Path(output);output.mkdir(parents=True,exist_ok=True)
    data=script();jobs=defaultdict(dict);manifest={}
    entries=[('AL_CUT_INTRO.RAW',NARRATOR,data['intro'])]
    entries += [(f'AL_CUT_{i:02d}.RAW',CAST[speaker],words)
                for i,(speaker,words,_) in enumerate(data['lines'])]
    entries += [('AL_CUT_DEPART.RAW',NARRATOR,data['departure'])]
    for name,voice,words in entries:
        manifest[name]={'voice':voice,'style':STYLE.get(voice,{}),'text':words}
        if not (output/name).exists() and not (output/(name[:-4]+'.OGG')).exists():
            jobs[voice][name]=re.split(r'(?<=[.!?])\s+(?=[A-Z])',words)
    for voice,clips in jobs.items():
        print('Generating',voice,len(clips),'clips',flush=True)
        speak_all(clips,voice,str(output),**STYLE.get(voice,{}))
        for name in clips:
            dest=output/name;tmp=dest.with_suffix('.tmp')
            subprocess.run(['ffmpeg','-v','error','-y','-f','s16le','-ar',str(RATE),'-ac','1',
                            '-i',str(dest),'-af','volume=0.40','-f','s16le',str(tmp)],check=True)
            tmp.replace(dest)
    (output/'voice-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')

def compile_art(folder,header):
    folder=Path(folder);data=script()
    actors=actor_frames(folder)
    flight=flight_sprites(folder)
    with Path(header).open('w') as f:
        f.write('// Generated by kwtools/kw_alphamatic_opening.py; do not edit.\n')
        for name,source in [('workshop','workshop-source.png'),('drive','drive-source.png'),('orbit','mars-orbit-source.png')]:
            im=Image.open(folder/source).convert('RGB').resize((320,200),Image.Resampling.BOX)
            im=im.quantize(palette=palette_image([(c>>16,(c>>8)&255,c&255) for c in EGA]),dither=Image.Dither.NONE)
            assert len(set(im.getdata()))<=16
            write_array(f,'al_'+name+'Picture',im)
            im.save(folder/(name+'-ega.png'))
            im.resize((960,600),Image.Resampling.NEAREST).save(folder/(name+'-preview.png'))
        f.write('#define AL_KEEN_W 44\n#define AL_KEEN_H 64\n#define AL_SPOT_W 32\n#define AL_SPOT_H 44\n')
        f.write('static const uint8_t al_keenPosePixels[3][AL_KEEN_W*AL_KEEN_H] = {\n')
        for pixels in actors[:3]:
            f.write('{'+','.join(map(str,pixels))+'},\n')
        f.write('};\nstatic const uint8_t al_spotPixels[AL_SPOT_W*AL_SPOT_H] = {\n')
        f.write(','.join(map(str,actors[3]))+'};\n')
        f.write('#define AL_STATION_W 124\n#define AL_STATION_H 104\n#define AL_SHIP_W 64\n#define AL_SHIP_H 28\n')
        for name,pixels in zip(('al_stationPixels','al_shipPixels'),flight):
            f.write('static const uint8_t '+name+'[] = {'+','.join(map(str,pixels))+'};\n')
        f.write('static const char al_introText[]='+json.dumps(data['intro'])+';\n')
        f.write('static const char al_departureText[]='+json.dumps(data['departure'])+';\n')
        f.write('static const AL_SceneLine al_sceneLines[] = {\n')
        for i,(speaker,words,view) in enumerate(data['lines']):
            assert speaker in SPEAKERS and view in VIEWS
            f.write('{%d,%d,%s,%s,"AL_CUT_%02d.RAW"},\n' %
                    (SPEAKERS[speaker],VIEWS[view],json.dumps(speaker),json.dumps(words),i))
        f.write('};\n')
    print('Compiled 3 EGA backgrounds, 4 actor frames, station/ship sprites and',len(data['lines']),'dialogue turns')

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--voices': voices(sys.argv[2])
    elif len(sys.argv)==3: compile_art(*sys.argv[1:])
    else: sys.exit(__doc__)
