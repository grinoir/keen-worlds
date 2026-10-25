"""Generate the shipboard arcade's two lines in Keen's established voice.

    python3 kwtools/kw_arcade_voice.py OUTPUT_FOLDER

Uses the same bm_fable voice and 1.2 speed as Keen's other conversations.
KW_KOKORO_DIR supplies the installed Kokoro package; KW_KOKORO_CACHE and
KW_KOKORO_DTYPE optionally select an existing local model. No online speech
service is used. Pack the resulting RAW files with kw_voice_pack.py.
"""
from pathlib import Path
import subprocess
import sys

from kw_keen4_voice import KEEN, STYLE
from kw_landing_voice import speak_all, RATE

LINES = {
    'ARCADE_HELLO.RAW': [
        "Hmm, don't have time to play with myself.",
        "Well... maybe one game!",
    ],
    'ARCADE_REMAKE.RAW': [
        "Hey! There's an updated version of my first adventure with enhanced graphics!",
        "Which one should I play?",
    ],
}


def main(folder):
    Path(folder).mkdir(parents=True, exist_ok=True)
    speak_all(LINES, KEEN, folder, **STYLE[KEEN])
    # These short, emphatic clips overshoot after quality-1 Vorbis encoding.
    # Leave headroom so the game's 16-bit decoder does not clip their peaks.
    for name in LINES:
        path = Path(folder) / name
        temporary = path.with_suffix('.headroom')
        subprocess.run(['ffmpeg', '-v', 'error', '-y', '-f', 's16le',
                        '-ar', str(RATE), '-ac', '1', '-i', str(path),
                        '-af', 'volume=0.60', '-f', 's16le', str(temporary)], check=True)
        temporary.replace(path)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
