"""The narrator reads each world's story (Keen Worlds).

    python3 tools/kw_story_voice.py . omnispeak/data/voice [id ...]

(with ids, only those stories: keen4, fitf, ..., and "worlds" for Keen
Worlds' own)

Every game's story scrolls away into the distance when its world is first
chosen at the ship's controls. This makes a clip of each, STORY_<ID>.RAW
(ID: the world's id in capitals), in the narrator's voice; the engine plays
it while the story scrolls, in place of the story's music. The words are the
games' own: the variable ck_str_storyText of each world's files, read here
the way the engine reads them (the base episode's file, then the world's
own).

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import glob, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all
from kw_keen4_voice import STYLE, NARRATOR

K4, K5, K6 = 'omnispeak/data/keen4/STRINGS.CK4', 'omnispeak/data/keen5/STRINGS.CK5', 'play/STRINGS.CK6'
WORLDS = {   # id: the base episode's strings file (or None), the world's own folder (or None)
    'keen4': (K4, None), 'keen5': (K5, None), 'keen6': (K6, None),
    'keen7': (K4, 'keen7'), 'keen8': (K6, 'keen8'), 'keen9': (K5, 'keen9'),
    'keen95': (K4, 'keen95'), 'bounty': (K4, 'bounty'), 'ck55': (K5, 'ck55'), 'asylum': (K5, 'asylum'),
    'osi': (None, 'osi'), 'arealm': (None, 'arealm'), 'fitf': (None, 'fitf'),
}
SAID = {'Meurglys III': 'Meurglys Three', '#': 'Pi', 'VIII': 'Eight', 'BwB': 'Bean-with-Bacon', 'doesnt': "doesn't", '&': 'and'}


def story(root, base, folder):
    paths = [os.path.join(root, base)] if base else []
    if folder:
        paths += sorted(p for p in glob.glob(os.path.join(root, 'play/mods', folder, '*'))
                        if not p.lower().endswith(('.raw', '.exe', '.txt', '.pat')))
    text = None
    for p in paths:
        try:
            t = open(p, encoding='latin1').read()
        except (OSError, IsADirectoryError):
            continue
        for m in re.finditer(r'%string\s+ck_str_storyText((?:\s*(?:#[^\n]*\n|"(?:\\.|[^"\\])*"))+)', t):
            text = ''.join(re.findall(r'"((?:\\.|[^"\\])*)"', m.group(1)))
    return text


def spoken(text):
    # a blank line parts paragraphs (and the title's lines): each ends a sentence
    paras = [re.sub(r'\s+', ' ', p.replace('\\n', ' ').replace('\\"', '"')).strip() for p in text.split('\\n\\n')]
    out = []
    for p in paras:
        if not p:
            continue
        for a, b in SAID.items():
            p = p.replace(a, b)
        if p[-1] not in '.!?:':
            p += '.'
        out += [s.strip() for s in re.split(r'(?<=[.!?])\s+(?=[A-Z"])', p) if s.strip()]
    return out


def own_story(root, name='kw_ownStory'):
    """A story written in kw_world.c: Keen Worlds' own (kw_ownStory, read
    while it scrolls after the title: STORY_WORLDS.RAW), or the page from
    the ship's chart of a world that came with no story (kw_chartBounty,
    kw_chartAsylum)."""
    src = open(os.path.join(root, 'omnispeak/src/kw_world.c'), encoding='latin1').read()
    m = re.search(name + r'\[\] =((?:\s*"(?:\\.|[^"\\])*")+);', src)
    text = ''.join(re.findall(r'"((?:\\.|[^"\\])*)"', m.group(1)))
    return text.replace('-\\n', '-')        # (a word broken at its hyphen for the screen)


def main(root, folder, only=None):
    clips = {}
    for world, (base, own) in WORLDS.items():
        text = story(root, base, own)
        if text:
            clips['STORY_%s.RAW' % world.upper()] = spoken(text)
    clips['STORY_WORLDS.RAW'] = spoken(own_story(root))
    # (these two came with no story; without this they would get their base game's)
    clips['STORY_BOUNTY.RAW'] = spoken(own_story(root, 'kw_chartBounty'))
    clips['STORY_ASYLUM.RAW'] = spoken(own_story(root, 'kw_chartAsylum'))
    if only:
        clips = {name: lines for name, lines in clips.items() if name[6:-4].lower() in only}
    os.makedirs(folder, exist_ok=True)
    speak_all(clips, NARRATOR, folder, **STYLE.get(NARRATOR, {}))
    print('%d stories' % len(clips))


if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2], sys.argv[3:])
