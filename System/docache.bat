@echo off
echo Renaming UT2004.ini and User.ini to prevent config values from being loaded...
del UT2004.bak
del User.bak
ren UT2004.ini UT2004.bak
ren User.ini User.bak
echo ..
echo ..
echo Exporting localized text...
ucc dumpint *.u
ucc dumpint ..\Maps\AS-*.ut2
ucc dumpint ..\Maps\BR-*.ut2
ucc dumpint ..\Maps\CTF-*.ut2
ucc dumpint ..\Maps\DM-*.ut2
ucc dumpint ..\Maps\DOM-*.ut2
ucc dumpint ..\Maps\ONS-*.ut2
ucc dumpint ..\Maps\TUT-*.ut2
echo ..
echo ..
echo Attempting to check out CacheRecords.ucl from Perforce...
p4 edit CacheRecords.ucl
echo Exporting cache records...
echo ..
echo ..
ucc exportcache *.*
echo ..
echo ..
echo Restoring UT2004.ini and User.ini...
copy UT2004.bak UT2004.ini
copy User.bak User.ini
del UT2004.bak
del User.bak
