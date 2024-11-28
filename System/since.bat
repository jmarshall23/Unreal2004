@echo off
rem usage: since number username depot

if A%1==AQ goto processchange

p4 changes -m 1 > lastchange.tmp
for /F "tokens=2 delims= " %%i IN (lastchange.tmp) DO set lastchange=%%i
del lastchange.tmp

set /a count= %lastchange% - %1 + 1
p4 changes -u %2 -m %count% %3... > changes.tmp
if exist output.tmp del output.tmp
for /F "tokens=2 delims= " %%i IN (changes.tmp) do if %%i GEQ %1 call %0 Q %%i %3 >> output.tmp
del changes.tmp
sort < output.tmp
del output.tmp
goto end

:processchange
p4 describe -s %2 | find "... " | find %3 > descr.tmp
for /F "tokens=1 delims=#" %%i IN (descr.tmp) do echo %%i
del descr.tmp

:end