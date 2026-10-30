"""The ship computer's landing announcements (Keen Worlds).

    python3 tools/kw_landing_voice.py omnispeak/data/voice omnispeak/src/kw_landing_text.h

For every world: one sentence of welcome and one of warning, spoken by the
Megarocket's computer when the ship has landed. The words are Keen Worlds'
own. This writes each as a sound clip (LAND_<ID>.RAW in the voice
folder, and COURSE_<ID>.RAW for the Chronicle's "a new course is open": signed 16-bit mono, low byte first, 22050 Hz) and all of them as text for
the card that is shown while the clip plays (a C header).

The voice is Kokoro (a free neural voice that runs on this computer), the
same woman's voice for every world, with two short notes before she speaks.
It is run through the copy of kokoro-js and its model in the user's
SignalweaveCMS project (KW_KOKORO_DIR, default ~/Projects/SignalweaveCMS; see
tools/kw_voice.mjs), then ffmpeg. Node, that folder and ffmpeg are
needed to run this tool, not to play the game.
"""
import json, os, subprocess, sys, tempfile, textwrap

HERE = os.path.dirname(os.path.abspath(__file__))

RATE = 22050
VOICE = 'af_heart'        # Kokoro's "Heart", an American woman
KOKORO_DIR = os.environ.get('KW_KOKORO_DIR', os.path.expanduser('~/Projects/SignalweaveCMS'))
# world id: the words, a sentence each
LANDINGS = {
    'suburb': ["You have now landed on Earth. Your own street.",
               "I am parked by the backyard clubhouse.", "Please try to catch the bus this time."],
    'alpham': ["You have now docked at the Alphamatic.", "VitaCorp's bigger armageddon machine.",
               "Please switch it off before we leave."],
    'keen4': ["You have now landed on Gnosticus Four.", "Beware of the local slugs.",
              "They are poisonous, and they will not say sorry."],
    'keen5': ["You have now docked at the Omegamatic.", "Beware of the local robots.",
              "They have lasers, and no sense of humour."],
    'keen6': ["You have now landed on Fribbulus Xax.", "Beware of the local Bloogs.",
              "They are big, they are green, and it is always lunch time."],
    'keen7': ["You have now landed on Krodacia.", "Beware of the local Krodacians.",
              "They collect keys. And visitors."],
    'keen8': ["You have now landed on Calidune.", "Beware of the local desert.",
              "All of it. Please drink some water."],
    'keen9': ["You have now docked at the HUB Command Center.", "Beware of the local Mortimer.",
              "He is one I Q point smarter than you, and he will mention it."],
    'arealm': ["You have now crashed on Krisle.", "Beware of the local Atroxians.",
               "Also, the ship is broken. This was not my fault."],
    'keen95': ["You have now landed on Gnosticus Four. Again.", "Beware of the local explosion.",
               "Something has taken the Oracle's batteries, and it was not me."],
    'ck55': ["You have now docked at the Omegamatic Two.", "Beware of the local robots.",
             "They are the same robots, but somebody has updated them."],
    'asylum': ["You have now landed at an address unknown.", "Beware of the local level design.",
               "Somebody built this place for fun, and it was not your fun."],
    'bounty': ["You have now landed on Meurglys Three.", "Beware of the local wildlife. And the local Elvis.",
               "Also, somebody has written wash me on my side."],
    'osi': ["You have now docked at the Shikadi Station.", "Beware of the local Shikadi.",
            "They are made of electricity, and they hold a grudge."],
    'fitf': ["You have now landed on Gnosticus Pi.", "Beware of the local trees.",
             "Some of them are not trees."],
}
# The Chronicle: a world has been saved and the ship has learned the course
# to the next. world id: the place's name as it is said.
COURSES = {
    'suburb': 'Earth. Your own street',
    'alpham': 'The Alphamatic',
    'keen4': 'Gnosticus Four', 'keen5': 'The Omegamatic', 'keen6': 'Fribbulus Zax', 'keen7': 'Krodacia',
    'keen8': 'Calidune', 'keen9': 'The HUB Command Center', 'arealm': 'Krizzle', 'fitf': 'Gnosticus Pi',
    'osi': 'The Shikadi Station', 'bounty': 'Meurglys Three', 'ck55': 'The Omegamatic Two', 'asylum': 'Address unknown', 'keen95': 'Aspermontis',
}
# A world has been saved: what the ship computer says when Keen is next in
# the cabin. world id: the words, a sentence each.
SAVED = {
    'suburb': ["Earth. Your own street. The school bag is found, and Billy has caught the bus.",
               "I will wait here until school is over."],
    'alpham': ["The Alphamatic is disabled.", "VitaCorp has lost another armageddon machine.",
               "I hope they kept the receipt."],
    'keen4': ["Gnosticus Four is saved.", "The Oracle is grateful.", "The slugs are not."],
    'keen5': ["The Omegamatic is switched off.", "The galaxy will not explode today.", "Well done."],
    'keen6': ["Fribbulus Xax is quiet again.", "The babysitter is safe.", "She is still cross with you."],
    'keen7': ["Krodacia is behind us.", "Seven keys, one door, and it was a trap.", "Of course it was."],
    'keen8': ["Calidune is done.", "I have sand in my circuits.", "And I was not even there."],
    'keen9': ["The Universe is not toast.", "I would like that noted in my log."],
    'arealm': ["Krisle is free, and the Krile have mended me.", "I feel much better.", "Do not crash me again."],
    'fitf': ["Gnosticus Pi is at peace.", "The trees say goodbye.", "I think."],
    'keen95': ["Aspermontis is safe.", "Eight batteries, all back where they belong.", "Now can we go home for dinner?"],
    'ck55': ["The Omegamatic Two is switched off.", "I suggest nobody builds a third."],
    'asylum': ["The Asylum is behind us.", "I do not want to talk about it."],
    'bounty': ["Meurglys Three is done.", "The bounty is collected.", "I am still dirty."],
    'osi': ["The Shikadi Station is offline.", "The shield is down.", "Fribbulus Xax, here we come."],
}
# How the words are written on the card, where that differs from how they are said.
WRITTEN = {'I Q': 'IQ', 'Gnosticus Four': 'Gnosticus IV', 'Omegamatic Two': 'Omegamatic II', 'Meurglys Three': 'Meurglys III'}


def speak_all(clips, voice, folder, speed=1.0, pitch=1.0, sounds=None):
    """Write clips (a dict: file name -> list of sentences) into folder:
    Kokoro speaks, ffmpeg puts two notes in front (the ship computer only)
    and makes it the game's format. speed: how fast Kokoro talks. pitch: the
    voice raised (and quickened) by this much afterwards, 1.06 a semitone.
    sounds: phonemes to replace, to mend a word the voice says badly (see
    tools/kw_voice.mjs)."""
    chime = voice == VOICE
    # KW_VOICE_ONLY_NEW=1: leave the clips that are there already (as .RAW
    # or packed as .OGG) and make only the new ones.
    if os.environ.get('KW_VOICE_ONLY_NEW'):
        clips = {name: words for name, words in clips.items()
                 if not os.path.exists(os.path.join(folder, name))
                 and not os.path.exists(os.path.join(folder, name[:-4] + '.OGG'))}
        if not clips:
            return
    with tempfile.TemporaryDirectory() as tmp:
        jobs = [{'voice': voice, 'speed': speed, 'sounds': sounds or {}, 'text': words,
                 'out': os.path.join(tmp, name + '.f32')} for name, words in clips.items()]
        jobs_file = os.path.join(tmp, 'jobs.json')
        json.dump(jobs, open(jobs_file, 'w'))
        r = subprocess.run(['node', os.path.join(HERE, 'kw_voice.mjs'), KOKORO_DIR, jobs_file],
                           capture_output=True, text=True)
        if r.returncode:
            sys.exit('Kokoro failed:\n' + r.stderr[-2000:])
        level = 'highpass=f=90,acompressor=threshold=-18dB:ratio=3:makeup=4,alimiter=limit=0.95'
        for name in clips:
            out = os.path.join(folder, name)
            speech = ['-f', 'f32le', '-ar', str(int(round(24000 * pitch))), '-ac', '1', '-i', os.path.join(tmp, name + '.f32')]
            if chime:
                args = ['-f', 'lavfi', '-i', 'sine=frequency=1320:duration=0.12:sample_rate=24000',
                        '-f', 'lavfi', '-i', 'sine=frequency=1760:duration=0.18:sample_rate=24000',
                        '-f', 'lavfi', '-i', 'anullsrc=r=24000:cl=mono:d=0.3'] + speech + [
                        '-filter_complex', '[0]volume=0.3[a];[1]volume=0.3[b];[3]%s[v];'
                        '[a][b][2][v]concat=n=4:v=0:a=1[o]' % level, '-map', '[o]']
            else:
                args = speech + ['-af', level]
            subprocess.run(['ffmpeg', '-v', 'error', '-y'] + args + ['-ar', str(RATE), '-ac', '1', '-f', 's16le', out],
                           check=True)
            print('%-16s %5.1f s' % (name, os.path.getsize(out) / 2 / RATE))


def card(sentences):
    words = ' '.join(sentences)
    for said, written in WRITTEN.items():
        words = words.replace(said, written)
    lines = ['Ship computer:', '']
    for sentence in words.replace('. ', '.|').split('|'):
        lines += textwrap.wrap(sentence, 30)
    text = '\n'.join(lines)
    assert len(text) < 250, 'too long for one card: ' + text
    return text


def main(folder, header):
    lines = ['/* Generated by tools/kw_landing_voice.py -- do not edit by hand. */',
             '// What the ship computer says when the Megarocket has landed: the world\'s id,',
             '// the sound clip in the voice folder, the words for the card.',
                          'static const struct { const char *id, *file, *text; } kw_landings[] = {']
    speak_all({'LAND_%s.RAW' % wid.upper(): words for wid, words in LANDINGS.items()}, VOICE, folder)
    speak_all({'COURSE_%s.RAW' % wid.upper(): ['A new course is open.', place + '.'] for wid, place in COURSES.items()},
              VOICE, folder)
    for wid, words in LANDINGS.items():
        name = 'LAND_%s.RAW' % wid.upper()
        lines.append('\t{"%s", "%s", "%s"},' % (wid, name, card(words).replace('\n', '\\n')))
    lines.append('};')
    speak_all({'SAVED_%s.RAW' % wid.upper(): words for wid, words in SAVED.items()}, VOICE, folder)
    lines += ['// What it says when a world has been saved and Keen is back in the cabin.',
              'static const struct { const char *id, *file, *text; } kw_savedLines[] = {']
    for wid, words in SAVED.items():
        lines.append('\t{"%s", "SAVED_%s.RAW", "%s"},' % (wid, wid.upper(), card(words).replace('\n', '\\n')))
    lines.append('};')
    open(header, 'w').write('\n'.join(lines) + '\n')


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
