"""Pack Keen Worlds' voice clips: plain samples to Ogg Vorbis.

    python3 tools/kw_voice_pack.py omnispeak/data/voice [voice-originals]

The voice tools (kw_landing_voice.py, kw_keen4_voice.py, ...) write clips as
plain samples, <NAME>.RAW (signed 16-bit mono, low byte first, 22050 Hz).
That is 2.6 MB a minute. This turns every .RAW in the folder into <NAME>.OGG
(Ogg Vorbis, quality 1: about a twelfth of the size; the user compared
samples by ear on 2026-10-06 and chose this setting) and takes the .RAW
away - into the second folder if one is given, as a copy to go back to. The
game asks for a clip as <NAME>.RAW and plays <NAME>.OGG if it is there
(kw_music.c).

Run it after any voice tool. Needs ffmpeg with libvorbis.
"""
import os, shutil, subprocess, sys

RATE, QUALITY = 22050, '1'


def main(folder, keep=None):
    raws = sorted(f for f in os.listdir(folder) if f.endswith('.RAW'))
    if keep:
        os.makedirs(keep, exist_ok=True)
    before = after = 0
    for name in raws:
        src = os.path.join(folder, name)
        dst = os.path.join(folder, name[:-4] + '.OGG')
        subprocess.run(['ffmpeg', '-v', 'error', '-y', '-f', 's16le', '-ar', str(RATE), '-ac', '1', '-i', src,
                        '-c:a', 'libvorbis', '-q:a', QUALITY, '-f', 'ogg', dst + '.tmp'], check=True)
        # it must play back as long as it was (to the frame Vorbis rounds to)
        probe = subprocess.run(['ffprobe', '-v', 'error', '-f', 'ogg', '-show_entries', 'stream=sample_rate,channels',
                                '-of', 'csv=p=0', dst + '.tmp'], capture_output=True, text=True, check=True).stdout.strip()
        if probe != '%d,1' % RATE:
            sys.exit('%s: packed as %s, not %d Hz mono' % (name, probe, RATE))
        os.replace(dst + '.tmp', dst)
        before += os.path.getsize(src)
        after += os.path.getsize(dst)
        if keep:
            shutil.move(src, os.path.join(keep, name))
        else:
            os.remove(src)
    print('%d clips: %.1f MB -> %.1f MB' % (len(raws), before / 1e6, after / 1e6))


if __name__ == '__main__':
    if len(sys.argv) not in (2, 3):
        sys.exit(__doc__)
    main(*sys.argv[1:])
