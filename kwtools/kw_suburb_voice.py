"""Suburb's original words, with recurring cast imported and new casting explicit.

python3 kwtools/kw_suburb_voice.py suburb/audit/v1.2 OUTPUT
Brother's selected voice/style lives in art/voices/suburb-cast.json. Until
selected, pages containing him remain unvoiced rather than using a guessed cast.
"""
from collections import defaultdict
from pathlib import Path
import hashlib
import json
import os
import re
import subprocess
import sys
import tempfile
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import KEEN, NARRATOR
from kw_keen789_voice import MORTIMER, STYLE as SHARED_STYLE
from kw_packs_voice import MOM

CAST_FILE=Path(__file__).resolve().parents[2]/'art/voices/suburb-cast.json'
selection=json.loads(CAST_FILE.read_text()) if CAST_FILE.exists() else {}
BROTHER=selection.get('voice') or 'pending_brother'
STYLE=dict(SHARED_STYLE)
if selection.get('voice'):STYLE[BROTHER]=selection.get('style',{})


def clean(text):
    text=re.sub(r'\^[GTgt][\d, ]+','',text)
    text=re.sub(r'\^[Cc][\da-fA-F]','',text)
    text=re.sub(r'\^[BLbl][\d, ]+','',text)
    text=re.sub(r'\^[Ee]','',text)
    return re.sub(r'\s+',' ',text).strip()


def clips_from(source):
    source=Path(source);dialogue=json.loads((source/'dialogue.json').read_text())
    cast={'Keen':KEEN,'Mom':MOM,'Brother':BROTHER,'Mortimer (android)':MORTIMER,'Mortimer (recording)':MORTIMER}
    clips={}
    for layout,stem in [('story-layout','STORY'),('ending','END')]:
        roles={d['quote']:d['speaker'] for d in dialogue if d['layout']==layout}
        pages=re.split(r'\^[Pp]',(source/(layout+'.txt')).read_text(encoding='latin1'))[1:]
        quote_index=0
        for index,page in enumerate(pages):
            if layout=='ending' and index==len(pages)-1:continue # author's sequel advertisement
            turns=[];at=0
            for quote in re.finditer(r'"([^"]*)"',page):
                words=clean(page[at:quote.start()])
                if words:turns.append((NARRATOR,words))
                words=clean(quote[1]);role=roles[quote_index];quote_index+=1
                if words:turns.append((cast.get(role,NARRATOR),words))
                at=quote.end()
            words=clean(page[at:])
            if words:turns.append((NARRATOR,words))
            # Merge adjacent narration fragments, retaining the original words.
            merged=[]
            for voice,words in turns:
                if merged and merged[-1][0]==voice:merged[-1]=(voice,merged[-1][1]+' '+words)
                else:merged.append((voice,words))
            if merged:clips[f'SS_{stem}_{index:02d}.RAW']=merged
    for event,clip in [('leave home','SS_LEAVE.RAW'),('bus blocked','SS_NEEDBAG.RAW'),('bag found','SS_GOTBAG.RAW')]:
        words=next(p['text'] for p in json.loads((source/'popups.json').read_text()) if p['event']==event)
        clips[clip]=[(KEEN,clean(words))]
    return clips


def main(source,output):
    output=Path(output);output.mkdir(parents=True,exist_ok=True)
    clips=clips_from(source)
    (output/'suburb-voice-manifest.json').write_text(json.dumps(clips,indent=2)+'\n')
    parts=output/'suburb-parts';parts.mkdir(exist_ok=True)
    jobs=defaultdict(dict);segments={};pending=[]
    catalogue=os.getenv('KW_VOICE_CATALOGUE')=='1'
    for clip,turns in clips.items():
        segments[clip]=[]
        if any(v.startswith('pending_') for v,_ in turns) and not catalogue:
            pending.append(clip);continue
        for i,(voice,words) in enumerate(turns):
            # Include style so a cast/pitch change cannot reuse the wrong segment.
            key=hashlib.sha256((voice+json.dumps(STYLE.get(voice,{}),sort_keys=True)+words).encode()).hexdigest()[:10]
            name=f'{clip[:-4]}_{i:02d}_{key}.RAW';segments[clip].append(name)
            if not (parts/name).exists():jobs[voice][name]=[words]
    for voice,items in jobs.items():
        items=list(items.items())
        for at in range(0,len(items),8):
            print(f'Suburb voice {voice}: {at+1}/{len(items)} turns',flush=True)
            speak_all(dict(items[at:at+8]),voice,str(parts),**STYLE.get(voice,{}))
    for clip,names in segments.items():
        if clip in pending:continue
        data=bytearray()
        for i,name in enumerate(names):
            if i:data+=bytes(2*int(RATE*.25))
            data+=(parts/name).read_bytes()
        (output/clip).write_bytes(data)
    if pending:print('Awaiting brother casting; deferred pages: '+', '.join(pending),flush=True)


if __name__=='__main__':
    if len(sys.argv)!=3:sys.exit(__doc__)
    main(*sys.argv[1:])
