#!/bin/sh
# Keen's outpost: render its opening scene in Border Village with the
# player's data and voices, in an isolated save path. Every line is spoken
# with its own clip and the game's state is untouched. Then in play: the
# scene starts by itself when Keen is in the village, leaves him at the hut's
# door, and he walks in and talks to Spot. (The hut, its doors, shelves,
# keepsakes and Spot are also checked by /KWTEST.)
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\nkw_voices = true\n' > "$work/OMNISPK.CFG"
cd "$game_data"
mkdir -p "$work/shots"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOTALK=1 KW_AUTOCUTSCENE="$work/shots" \
 timeout -k 2s 60s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /OUTPOSTSCENE > "$work/result.log" 2>&1; then
 tail -30 "$work/result.log"; exit 1
fi
# In play: wait, walk right to the door, go in, walk over to Spot, press up
mkdir -p "$work/play"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOSTORY=1 KW_AUTOFINALE=1 KW_AUTOCUTSCENE="$work/play" \
 KW_AUTOINPUT="40:0,0,0,0,0;8:1,0,0,0,0;10:0,0,0,0,0;40:0,-1,0,0,0;20:0,0,0,0,0;9:1,0,0,0,0;10:0,0,0,0,0;1:0,-1,0,0,0;0,0,0,0,0" \
 KW_AUTOSHOT="200:$work/play/end.ppm" \
 timeout -k 2s 60s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /OUTPOSTPLAY > "$work/play.log" 2>&1; then
 tail -30 "$work/play.log"; exit 1
fi
python3 - "$work" "${KW_OUTPOST_SHOTS:-}" <<'PY'
from pathlib import Path
import shutil, sys
root = Path(sys.argv[1])
log = (root / 'result.log').read_text(errors='replace')
assert log.count('Story scene finished') == 1 and 'PASS:' in log, log[-2000:]
frames = sorted((root / 'shots').glob('*.ppm'))
assert len(frames) == 18, len(frames)
for path in frames:
    data = path.read_bytes()
    assert data.startswith(b'P6\n320 200\n255\n'), path
    assert len(data.split(b'\n', 3)[3]) == 320 * 200 * 3, path
shots = root / 'shots'
# Keen walks up to the hut; on Tuesday its windows are lit
assert (shots / 'o1-rolla.ppm').read_bytes() != (shots / 'o1-rollb.ppm').read_bytes()
assert (shots / 'o9-tuesdaya.ppm').read_bytes() != (shots / 'o9-tuesdayb.ppm').read_bytes()
spoken = [line.split('KW_AUTOTALK: ', 1)[1].split()[0] for line in log.splitlines() if 'KW_AUTOTALK: OP_CUT_' in line]
assert len(spoken) == 9, spoken
play = (root / 'play.log').read_text(errors='replace')
assert play.count("Keen's outpost: scene") == 1 and play.count('Story scene finished') == 1, 'the scene did not play once in play'
assert 'KW_AUTOFINALE: Keen says "See you, Spot."' in play, 'Keen did not talk to Spot'
end = [line for line in play.splitlines() if 'KW_AUTOSHOT:' in line]
assert end and 'level 1, Keen at tile 99,47' in end[0], end   # in the hut's room, by Spot
if sys.argv[2]:
    shutil.copytree(shots, sys.argv[2], dirs_exist_ok=True)
print('PASS: the outpost scene: 18 frames, 9 real speech clips (%s) and preserved game state; '
      'in play it starts once, and Keen goes into the hut and talks to Spot' % spoken[0])
PY
