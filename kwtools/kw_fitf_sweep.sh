#!/bin/sh
# Play every Foray in the Forest level for a few seconds with scripted input
# (walk, jump, pogo, shoot) in the real game loop, under the sanitizers,
# headless. Prints one line per level; anything but a KW_AUTOSHOT line is a
# problem. Uses the sanitizer build in the scratch folder (tools/kw_fitf_auto.sh
# builds it).
S=/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad
cd $S/play || exit 1
for lv in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 22 23 24 25 26; do
	r=$(KW_AUTOWAIT="$S/shot/sw" KW_AUTOLEVEL=$lv KW_AUTOINPUT="90:1,0,0,0,0;20:1,0,1,0,0;90:-1,0,0,0,0;20:0,0,0,1,0;60:1,0,0,0,1" KW_AUTOSHOT="260:$S/shot/sweep$lv.ppm" SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ASAN_OPTIONS=detect_leaks=0 timeout 120 ./omnispeak-asansw /NOCOPY /EPISODE 5 /WORLD fitf /TEDLEVEL $lv 2>&1 | sed 's/\x1b\[[0-9;]*m//g' | grep -a "KW_AUTOSHOT\|AddressSan\|SUMMARY\|runtime error: \|#[1-4] \|Quit\|[A-Za-z_]*: .*!" | grep -v "id_rf.c:3\|id_rf.c:4\|dbopl\|id_fs" | sed 's|/tmp/claude[^ ]*/src/||;s| -> .*||' | head -5 | tr '\n' '|')
	echo "L$lv: $r" | cut -c1-330
done
