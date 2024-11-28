@echo off

perl ../../../tools/dsptools/dspSourceCheck.pl
if errorlevel 1 goto rebuild

if not x%1 == x goto build

echo .
echo ERROR: Must specify build type
echo USAGE: %0 release
echo .
goto end

:build
if x%2 == xrebuild goto rebuild
if not exist makefile goto rebuild
bash -c 'make PLATFORM=win32 %1'
goto end

:rebuild
bash -c 'export PLATFORM=win32 ; cd .. ; cd .. ; ./configure'
bash -c 'export PLATFORM=win32 ; make %1'

:end
