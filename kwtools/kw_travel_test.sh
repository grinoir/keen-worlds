#!/bin/sh
# kw4.sh LEVEL "input steps" FRAMES NAME [extra env...]: scripted run starting in Keen 4 (world travel tests)
S=/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad
cd /home/grinoir/Projects/CommanderKeen/omnispeak && rsync -a --exclude .git --exclude obj --exclude bin ./ $S/asansw/omnispeak/ && cd $S/asansw/omnispeak/src && (CXXFLAGS="-fsanitize=address,undefined -g" LDFLAGS="-fsanitize=address,undefined" make -j8 RENDERER=sdl2sw 2>&1 | grep -E " error|undefined ref" | head -3)
cp ../bin/omnispeak $S/play/omnispeak-asansw && cd $S/play && rsync -a /home/grinoir/Projects/CommanderKeen/play/mods/ mods/ && rsync -a /home/grinoir/Projects/CommanderKeen/omnispeak/data/megarocket/ megarocket/
KW_AUTOWAIT="$S/shot/kw4w" KW_AUTOLEVEL=$1 KW_AUTOINPUT="$2" KW_AUTOSHOT="$3:$S/shot/$4.ppm" SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ASAN_OPTIONS=detect_leaks=0 timeout 150 ./omnispeak-asansw /NOCOPY /EPISODE 4 /TEDLEVEL $1 2>&1 | sed 's/\x1b\[[0-9;]*m//g' | grep -a "KW_AUTO\|AddressSan\|SUMMARY\|runtime error: \|#[1-5] \|Quit" | grep -v "id_rf.c:3[0-9][0-9]\|id_rf.c:4[0-9][0-9]\|dbopl\|id_fs" | sed 's|/tmp/claude[^ ]*/src/||;s| -> .*||' | head -10
python3 -c "
from PIL import Image; Image.open('$S/shot/$4.ppm').resize((640,400),Image.NEAREST).save('$S/shot/$4.png')" 2>/dev/null
