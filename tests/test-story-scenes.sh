#!/bin/sh
# Render Keen Worlds' own story scenes (Calidune's arrival and ending, Spot's rescue)
# with the player's data and voices in an isolated save path.
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\nkw_voices = true\n' > "$work/OMNISPK.CFG"
cd "$game_data"
for scene in CDSCENE SPOTSCENE BRIDGESCENES; do
 mkdir -p "$work/$scene"
 if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOTALK=1 KW_AUTOCUTSCENE="$work/$scene" \
  timeout -k 2s 60s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /$scene > "$work/$scene/result.log" 2>&1; then
  tail -30 "$work/$scene/result.log"; exit 1
 fi
done
python3 - "$work" <<'PY'
from pathlib import Path
import sys
root=Path(sys.argv[1])
def check(scene,scenes,shots,clips,prefix,moving):
 folder=root/scene;log=(folder/'result.log').read_text(errors='replace')
 assert log.count('Story scene finished')==scenes and 'PASS:' in log,scene
 frames=sorted(folder.glob('*.ppm'))
 assert len(frames)==shots,(scene,len(frames))
 for path in frames:
  data=path.read_bytes()
  assert data.startswith(b'P6\n320 200\n255\n'),path
  assert len(data.split(b'\n',3)[3])==320*200*3,path
 a,b=moving
 assert (folder/a).read_bytes()!=(folder/b).read_bytes(),(a,b)
 assert log.count('KW_AUTOTALK: '+prefix)==clips,'Supply all %d %s clips'%(clips,prefix)
check('CDSCENE',2,14,5,'CD_CUT_',('4-sanda.ppm','4-sandb.ppm'))     # Keen materialises
check('SPOTSCENE',1,14,7,'SP_CUT_',('s5-homea.ppm','s5-homeb.ppm'))  # the cage opens
for prefix in ('KR_CUT_','BH_CUT_','AB_CUT_','K4_CUT_'):
 assert (root/'BRIDGESCENES'/'result.log').read_text(errors='replace').count('KW_AUTOTALK: '+prefix)>0,prefix
check('BRIDGESCENES',4,32,16,'',('k2-samea.ppm','k2-sameb.ppm'))  # the knock-off lands
print('PASS: Calidune, Spot rescue and bridge scenes (Krisle, bounty, Alphamatic, Shikadi signal): frames, 28 real speech clips and preserved game state')
PY
