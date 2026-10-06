"""Bring Operation Station Infiltration's code into Keen Worlds.

    python3 tools/kw_osi_port.py osi/src/src/mod/osi omnispeak/src

Operation Station Infiltration (Ceilick; programmed by Multimania; GPL) is
built on Omnispeak itself: its own versions of Omnispeak's Keen 5 files
(ck5_map.c, ck5_misc.c, ck5_obj1-3.c) and a file of new creatures
(ck5_obj4.c). Keen Worlds has Keen 5 too, so the mod's files are brought in
under names of their own: every function and variable they define with a
CK5_ or ck5_ name gets OS5_ or os5_ instead (kw_os5_*.c, kw_os5_ep.h). Its action file still names the functions
CK5_...; the engine looks those up as OS5_... while this world is running
(CK_ACT_GetFunction). Then come the fixes for what has changed in the
engine since the mod was written (FIXES below).
"""
import os, re, sys

FILES = ['ck5_map.c', 'ck5_misc.c', 'ck5_obj1.c', 'ck5_obj2.c', 'ck5_obj3.c', 'ck5_obj4.c', 'ck5_ep.h']
# (file, old, new): changes by hand after the renaming; an old text that is
# not found is reported.
FIXES = [
    # the sprite table entry is handed over by value now
    ('kw_os5_obj4.c',
     'VH_SpriteTableEntry *ste = VH_GetSpriteTableEntry(obj->currentAction->chunkLeft - ca_gfxInfoE.offSprites);',
     'VH_SpriteTableEntry steValue = VH_GetSpriteTableEntry(obj->currentAction->chunkLeft - ca_gfxInfoE.offSprites);\n'
     '\t\tVH_SpriteTableEntry *ste = &steValue;'),
]


def defined_names(texts):
    """The C names the mod's files define with a Keen 5 prefix: functions,
    and variables at file level. Only these are renamed; the names of
    actions, settings, strings, sounds and pictures (which are looked up in
    the game's own files) stay as they are."""
    names = set()
    for text in texts:
        for m in re.finditer(r'^[A-Za-z_][\w \t\*]*?\b((?:CK5|ck5)_\w+)\s*\(', text, re.M):
            names.add(m.group(1))
        for m in re.finditer(r'^(?:extern\s+)?[A-Za-z_][\w \t\*]*?\b((?:CK5|ck5)_\w+)\s*(?:\[[^\]]*\])?\s*(?:=|;)', text, re.M):
            names.add(m.group(1))
    return names


def new_name(name):
    return ('OS5_' if name.startswith('CK5_') else 'os5_') + name[4:]


def convert(text, names):
    # outside string literals: the defined names
    parts = re.split(r'("(?:[^"\\\n]|\\.)*")', text)
    for i in range(0, len(parts), 2):
        parts[i] = re.sub(r'\b(?:CK5|ck5)_\w+', lambda m: new_name(m.group(0)) if m.group(0) in names else m.group(0), parts[i])
    # inside them: only the names functions are registered under
    for i in range(1, len(parts), 2):
        if re.search(r'CK_ACT_Add(?:Col)?Function\(\s*$', parts[i - 1]) and parts[i][1:-1] in names:
            parts[i] = '"%s"' % new_name(parts[i][1:-1])
    text = ''.join(parts)
    text = text.replace('#include "ck5_ep.h"', '#include "kw_os5_ep.h"\n#include "kw_osi.h"')
    text = text.replace('#ifndef CK5_EP_H', '#ifndef KW_OS5_EP_H').replace('#define CK5_EP_H', '#define KW_OS5_EP_H')
    return text


def fix_misc(text):
    """kw_os5_misc.c: the episode's record as the engine has it now, and
    without the tables of numbers the engine has since moved into the
    game's variable files (OS5_DefineConstants and what it filled)."""
    text = re.sub(r'CK_EpisodeDef osi_episode = \{.*?\n\};\n',
                  'CK_EpisodeDef osi_episode = {\n\tEP_CK5,\n\t"CK5",\n\t"EPISODE.CK5",\n\t&OS5_ScanInfoLayer,\n\t&OS5_IsPresent,\n};\n',
                  text, count=1, flags=re.S)
    a = text.index('uint8_t os5_starWarsPalette[] = {')
    b = text.index('void OS5_FixupVolteSpot')
    return text[:a] + text[b:]


def main(src, out):
    sources = {name: open(os.path.join(src, name), encoding='latin1').read() for name in FILES}
    names = defined_names(sources.values())
    for name in FILES:
        text = convert(sources[name], names)
        target = 'kw_os5_' + name[4:]
        if target == 'kw_os5_misc.c':
            text = fix_misc(text)
        for f, old, new in FIXES:
            if f == target:
                if old not in text:
                    print('no match in %s: %s' % (target, old[:60].replace('\n', ' ')))
                text = text.replace(old, new)
        head = ('// KEEN WORLDS: Operation Station Infiltration\'s %s, converted by\n'
                '// tools/kw_osi_port.py (names changed from CK5_/ck5_ to OS5_/os5_; see there).\n' % name)
        open(os.path.join(out, target), 'w').write(head + text)
        print('wrote', target)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
