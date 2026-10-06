#!/bin/sh
# play.sh LEVEL [SECONDS]: run the real game loop in a Foray level, headless, under the sanitizers
S=/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad
cd /home/grinoir/Projects/CommanderKeen/omnispeak && rsync -a --exclude .git --exclude obj --exclude bin ./ $S/asansw/omnispeak/ && cd $S/asansw/omnispeak/src && (CXXFLAGS="-fsanitize=address,undefined -g" LDFLAGS="-fsanitize=address,undefined" make -j8 RENDERER=sdl2sw 2>&1 | grep -E " error|undefined ref" | head -3)
cp ../bin/omnispeak $S/play/omnispeak-asansw && cd $S/play && rsync -a /home/grinoir/Projects/CommanderKeen/play/mods/ mods/
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ASAN_OPTIONS=detect_leaks=0 timeout ${2:-40} ./omnispeak-asansw /NOCOPY /EPISODE 5 /WORLD fitf /TEDLEVEL $1 > $S/run.log 2>&1; echo "level $1 exit $?"
sed 's/\x1b\[[0-9;]*m//g' $S/run.log | grep -av "id_rf.c:3[0-9][0-9]\|id_rf.c:4[0-9][0-9]\|dbopl.c\|id_fs.c:389\|software renderer" | grep -a "[a-zA-Z]\{4\}" | grep -av "║" | cut -c1-240 | head -${3:-14}
