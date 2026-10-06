#!/bin/sh
# auto.sh LEVEL "x,y,jump,pogo,fire" FRAMES NAME : play a Foray level with held input, save a picture after FRAMES
S=/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad
cd /home/grinoir/Projects/CommanderKeen/omnispeak && rsync -a --exclude .git --exclude obj --exclude bin ./ $S/asansw/omnispeak/ && cd $S/asansw/omnispeak/src && (CXXFLAGS="-fsanitize=address,undefined -g" LDFLAGS="-fsanitize=address,undefined" make -j8 RENDERER=sdl2sw 2>&1 | grep -E " error|undefined ref" | head -3)
cp ../bin/omnispeak $S/play/omnispeak-asansw && cd $S/play && rsync -a /home/grinoir/Projects/CommanderKeen/play/mods/ mods/
KW_AUTOINPUT="$2" KW_AUTOSHOT="$3:$S/shot/$4.ppm" SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ASAN_OPTIONS=detect_leaks=0 timeout 120 ./omnispeak-asansw /NOCOPY /EPISODE 5 /WORLD fitf /TEDLEVEL $1 > $S/run.log 2>&1; echo "exit $?"
sed 's/\x1b\[[0-9;]*m//g' $S/run.log | grep -av "id_rf.c:3[0-9][0-9]\|id_rf.c:4[0-9][0-9]\|dbopl.c\|id_fs.c:389\|software renderer" | grep -a "[a-zA-Z]\{4\}" | grep -av "║\|Thanks" | cut -c1-240 | head -8
python3 -c "
from PIL import Image; im=Image.open('$S/shot/$4.ppm'); im.resize((640,400),Image.NEAREST).save('$S/shot/$4.png')" 2>/dev/null
