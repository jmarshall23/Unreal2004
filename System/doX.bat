@echo off
set type=%1
set insys=%2
set backupdir=%3

IF "%insys%"=="" GOTO NOSYS
set XBOX_SYS=/x %insys%
:NOSYS

IF "%type%"=="xbe" GOTO XBE
IF "%type%"=="ini" GOTO INI
IF "%type%"=="int" GOTO INT
IF "%type%"=="u" GOTO U
IF "%type%"=="maps" GOTO MAPS
IF "%type%"=="music" GOTO MUSIC
IF "%type%"=="staticmeshes" GOTO STATICMESHES
IF "%type%"=="sounds" GOTO SOUNDS
IF "%type%"=="textures" GOTO TEXTURES
IF "%type%"=="content" GOTO CONTENT
IF "%type%"=="all" GOTO ALL
IF "%type%"=="clean" GOTO CLEAN
IF "%type%"=="cleanall" GOTO CLEANALL
IF "%type%"=="backup" GOTO BACKUP

echo Option "%type%" unrecognized...
GOTO DONE

:CLEANALL
:CLEAN
@echo on
xbdel %XBOX_SYS% /f /r xe:\uc\*
xbdel %XBOX_SYS% /f /r xe:\uc
@echo off
IF "%type%"=="clean" GOTO DONE

:ALL

:XBE
@echo on
xbcp %XBOX_SYS% /y /f /d /t dsstdfx.bin xe:\UC\dsstdfx.bin
xbcp %XBOX_SYS% /y /f /d /t xbox.bin xe:\UC\xbox.bin
xbcp %XBOX_SYS% /y /f /d /t uc.xbe xe:\UC\uc.xbe
xbcp %XBOX_SYS% /y /f /d /t splash*.raw xe:\UC\
REM xbcp %XBOX_SYS% /y /f /d /t ..\XBoxDrv\E3Code\E3Userin.ini xs:\ffed30c9\E3Userin.ini

@echo off
IF "%type%"=="xbe" GOTO DONE

:INI
@echo on
xbdel %XBOX_SYS% /f xe:\UC\System\UC.ini
xbdel %XBOX_SYS% /f xe:\UC\System\User.ini
xbcp %XBOX_SYS% /y /f /d /t DefaultXbox.ini xe:\UC\System\Default.ini
xbcp %XBOX_SYS% /y /f /d /t DefUserXbox.ini xe:\UC\System\DefUser.ini
xbcp %XBOX_SYS% /y /f /d /t watchlist.txt	xe:\UC\System\watchlist.txt
@echo off
IF "%type%"=="ini" GOTO DONE

:INT
@echo on
xbcp %XBOX_SYS% /y /f /d /t *.int xe:\UC\System\
xbcp %XBOX_SYS% /y /f /d /t *.uce xe:\UC\System\
xbdel %XBOX_SYS% /f xe:\UC\System\XDemoMaps.int
@echo off
IF "%type%"=="int" GOTO DONE

:U
@echo on
xbcp %XBOX_SYS% /y /f /d /t *.u xe:\UC\System\
@echo off
IF "%type%"=="u" GOTO DONE

:CONTENT
@echo on
xbcp %XBOX_SYS% /y /f /d /t ..\Animations\*.ukx xe:\UC\Animations\
@echo off


:MAPS
@echo on
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\Entry.ut2 xe:\UC\Maps\Entry.ut2
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\DM-Vidona.ut2 xe:\UC\Maps\
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\DM-Insidious.ut2 xe:\UC\Maps\
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\DM-Maul.ut2 xe:\UC\Maps\
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\CTF-MaulTemp.ut2 xe:\UC\Maps\
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\CTF-Geothermal.ut2 xe:\UC\Maps\
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\DOM-ScorchedEarth.ut2 xe:\UC\Maps\
xbcp %XBOX_SYS% /y /f /d /t ..\Maps\BR-Anubis.ut2 xe:\UC\Maps\


@echo off
IF "%type%"=="maps" GOTO DONE

:SOUNDS
@echo on
xbcp %XBOX_SYS% /y /f /d /t ..\Sounds\*.uax xe:\UC\Sounds\
@echo off
IF "%type%"=="sounds" GOTO DONE

:MUSIC
@echo on
xbcp %XBOX_SYS% /y /f /d /t ..\Music\*.wav xe:\UC\Music\
@echo off
IF "%type%"=="music" GOTO DONE

:STATICMESHES
@echo on
xbcp %XBOX_SYS% /y /f /d /t ..\StaticMeshes\*.usx xe:\UC\StaticMeshes\
@echo off
IF "%type%"=="staticmeshes" GOTO DONE

:TEXTURES
@echo on
xbcp %XBOX_SYS% /y /f /d /t ..\Textures\*.utx xe:\UC\Textures\
xbcp %XBOX_SYS% /y /f /d /t ..\Textures\*.uce_utx xe:\UC\Textures\
@echo off
IF "%type%"=="textures" GOTO DONE

GOTO DONE

:BACKUP
IF "%XBOX_SYS%"=="" GOTO ERROR
IF "%backupdir%"=="" GOTO ERROR
@echo on
xbcp %XBOX_SYS% /y /f /t /r /s xe:\uc\*.* %backupdir% 
@echo off
GOTO DONE

:ERROR 
echo Error: Backup requires system and backup dir to be specified. 

:DONE
echo Done operation %type% on %XBOX_SYS%!
