#!/bin/sh
# shot.sh WORLD LEVEL NAME [episode]: build the software-rendering copy and save a picture of a level
S=/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad
cd /home/grinoir/Projects/CommanderKeen/omnispeak && rsync -a --exclude .git --exclude obj --exclude bin ./ $S/shot/omnispeak/ && cd $S/shot/omnispeak/src && (make -j8 RENDERER=sdl2sw 2>&1 | grep -E "error|undefined" | head -3)
cp ../bin/omnispeak $S/play/omnispeak-shot && cd $S/play && rsync -a /home/grinoir/Projects/CommanderKeen/play/mods/ mods/
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 60 ./omnispeak-shot /NOCOPY /EPISODE ${4:-5} /WORLD $1 /KWSHOT $1 $2 $S/shot/$3.ppm 2>&1 | grep -a "KW_\|rror\|arning: tried" | head
python3 -c "
from PIL import Image; im=Image.open('$S/shot/$3.ppm'); im.resize((640,400),Image.NEAREST).save('$S/shot/$3.png')"
