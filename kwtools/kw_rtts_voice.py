"""Return to the Shadowlands' dialogue, spoken (Keen Worlds).

    python3 kwtools/kw_rtts_voice.py play/mods/rtts OUTPUT

The words are the game's own, as kwtools/kw_rtts.py put them in RTTS.CK4.
Keen, Princess Lindsey and the Oracle Janitor keep their voices from Keen 4
(kw_keen4_voice.py). The page scouts are this world's own: their voice is
chosen by ear from samples (art/voices/rtts-scout-cast.json); until then
their clips are left out and the scouts' windows stay silent.
"""
from pathlib import Path
import json
import re
import sys
from kw_landing_voice import speak_all
from kw_keen4_voice import KEEN, LINDSEY, JANITOR, STYLE as SHARED_STYLE

CAST_FILE = Path(__file__).resolve().parents[2] / 'art/voices/rtts-scout-cast.json'
selection = json.loads(CAST_FILE.read_text()) if CAST_FILE.exists() else {}
SCOUT = selection.get('voice') or 'pending_scout'
STYLE = dict(SHARED_STYLE)
if selection.get('voice'):
    STYLE[SCOUT] = selection.get('style', {})
SCOUT_LEVELS = range(1, 19)   # the levels a scout can be found in


def strings(var_file):
    """%string and %stringarray values of a variable file."""
    text = Path(var_file).read_text(encoding='latin-1')
    values = {}
    for m in re.finditer(r'%string\s+(\w+)\s+"((?:[^"\\]|\\.)*)"', text):
        values[m.group(1)] = m.group(2)
    for m in re.finditer(r'%stringarray\s+(\w+)\s*\n((?:\s*"(?:[^"\\]|\\.)*",?\s*\n?)+)', text):
        values[m.group(1)] = re.findall(r'"((?:[^"\\]|\\.)*)"', m.group(2))
    return values


def spoken(s):
    s = s.replace('\\n', ' ').replace('\\"', '"').replace("\\'", "'")
    return re.sub(r'\s+', ' ', s).strip()


def clips_from(game):
    v = strings(Path(game) / 'RTTS.CK4')
    clips = {
        'RT_LINDSEY1.RAW': (LINDSEY, v['ck4_str_lindsayMessage1']),
        'RT_LINDSEY2.RAW': (LINDSEY, v['ck4_str_lindsayMessage2']),
        'RT_KEEN_LINDSEY1.RAW': (KEEN, v['ck4_str_lindsayThanks1']),
        'RT_KEEN_LINDSEY2.RAW': (KEEN, v['ck4_str_lindsayThanks2']),
        'RT_JANITOR1.RAW': (JANITOR, v['ck4_str_janitor1']),
        'RT_JANITOR2.RAW': (JANITOR, v['ck4_str_janitor2']),
        'RT_KEEN_JANITOR.RAW': (KEEN, v['ck4_str_janitor3']),
        'RT_JANITOR4.RAW': (JANITOR, v['ck4_str_janitor4']),
        'RT_KEEN_CANTSWIM.RAW': (KEEN, v['ck4_str_cantSwim']),
        'RT_KEEN_WETSUIT.RAW': (KEEN, v['ck4_str_gotWetsuit']),
    }
    for level in SCOUT_LEVELS:
        clips[f'RT_SCOUT{level:02d}.RAW'] = (SCOUT, v['rtts_str_scout'][level])
        clips[f'RT_KEEN_SCOUT{level:02d}.RAW'] = (KEEN, v['rtts_str_keenScout'][level])
    return {name: (voice, spoken(words)) for name, (voice, words) in clips.items()}


def main(game, output):
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)
    by_voice = {}
    for name, (voice, words) in clips_from(game).items():
        if voice.startswith('pending_'):
            continue
        by_voice.setdefault(voice, {})[name] = re.split(r'(?<=[.!?])\s+(?=[A-Z])', words)
    for voice, clips in by_voice.items():
        speak_all(clips, voice, str(output), **STYLE.get(voice, {}))
    if SCOUT.startswith('pending_'):
        print('The scouts have no voice yet: choose one (art/voices/rtts_scout_*), then run this again.')


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
