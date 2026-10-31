#!/bin/sh
# The Shikadi Station's two endings speak with its own clips (OS_END_*,
# OS_ENDS_*), never Keen 5's. Needs the player's Operation Station
# Infiltration files in mods/osi and the clips in voice/.
set -eu
binary=$(realpath "${1:-$(dirname "$0")/../../play/omnispeak}")
game_data=$(realpath "${2:-$(dirname "$0")/../../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOFINALE=1 KW_AUTOTALK=1 timeout -k 2s 60s \
 "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /OSIENDING > "$work/result.log" 2>&1; then
 tail -20 "$work/result.log"; exit 1
fi
log="$work/result.log" python3 - <<'PY'
import os, re
log = open(os.environ['log'], errors='replace').read()
normal, secret = log.split('OSIENDING: after the Korath fuse')
said = lambda part: re.findall(r'KW_AUTOTALK: (\S+)\.RAW', part)
assert not [c for c in said(log) if c.startswith('K5_')], 'Keen 5 spoke in the station ending'
want = ['OS_END_%02d' % n for n in range(9)]
assert said(normal) == want, said(normal)
want = ['OS_END_00', 'OS_END_01', 'OS_END_02'] + ['OS_ENDS_%02d' % n for n in range(3, 14)]
assert said(secret) == want, said(secret)
print('PASS: both Shikadi Station endings speak every page with its own clips')
PY
