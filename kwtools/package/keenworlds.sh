#!/bin/sh
# Start Keen Worlds. What the game prints goes to keenworlds-log.txt,
# which is the file to send along with a bug report.
cd "$(dirname "$(readlink -f "$0")")" || exit 1
./omnispeak /NOCOPY "$@" > keenworlds-log.txt 2>&1
status=$?
if [ $status -ne 0 ]; then
	echo "Keen Worlds stopped with an error. See keenworlds-log.txt:" >&2
	sed 's/\x1b\[[0-9;]*m//g' keenworlds-log.txt | grep -a '[a-zA-Z]\{4\}' | tail -5 >&2
fi
exit $status
