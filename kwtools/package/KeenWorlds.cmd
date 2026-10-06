@echo off
rem Start Keen Worlds. What the game prints goes to keenworlds-log.txt,
rem which is the file to send along with a bug report.
cd /d "%~dp0"
omnispeak.exe /NOCOPY %* > keenworlds-log.txt 2>&1
