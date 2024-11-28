@ECHO OFF
SETLOCAL

SET MAPLIST=Entry.ut2 DM-Vidona.ut2 DM-Insidious.ut2 DM-Maul.ut2 CTF-MaulTemp.ut2 CTF-Geothermal.ut2 DOM-ScorchedEarth.ut2 BR-Anubis.ut2

IF EXIST ..\PrevMaps RD ..\PrevMaps /s /q
IF EXIST ..\PrevTextures RD ..\PrevTextures /s /q
IF EXIST ..\PrevStaticMeshes RD ..\PrevStaticMeshes /s /q

IF EXIST ..\NewMaps RD ..\NewMaps /s /q
IF EXIST ..\NewTextures RD ..\NewTextures /s /q
IF EXIST ..\NewStaticMeshes RD ..\NewStaticMeshes /s /q

IF NOT EXIST ..\NewMaps MD ..\NewMaps
IF NOT EXIST ..\NewTextures MD ..\NewTextures
IF NOT EXIST ..\NewStaticMeshes MD ..\NewStaticMeshes

COPY ..\Textures\MapThumbnails.utx ..\NewTextures
COPY ..\Textures\PlayerSkins.utx ..\NewTextures
COPY ..\Textures\PlayerPictures.utx ..\NewTextures
COPY ..\Textures\*.uce_utx ..\NewTextures

UCC Editor.CutdownContent %MAPLIST%

MOVE ..\Maps ..\PrevMaps
MOVE ..\Textures ..\PrevTextures
MOVE ..\StaticMeshes ..\PrevStaticMeshes

MOVE ..\NewMaps ..\Maps
MOVE ..\NewTextures ..\Textures
MOVE ..\NewStaticMeshes ..\StaticMeshes

XCOPY /Y /I xbox.bin \UC-Build\
XCOPY /Y /I uc.xbe \UC-Build\
XCOPY /Y /I splash*.raw \UC-Build\
IF NOT EXIST \UC-Build\System MKDIR \UC-Build\System
COPY /Y DefaultXbox.ini \UC-Build\System\Default.ini
COPY /Y DefUserXbox.ini \UC-Build\System\DefUser.ini

XCOPY /Y /I *.int \UC-Build\System\
DEL /f \UC-Build\System\xdemomaps.int
XCOPY /Y /I *.uce \UC-Build\System\
XCOPY /Y /I *.u \UC-Build\System\

XCOPY /Y /I ..\Animations\*.ukx \UC-Build\Animations\

XCOPY /Y /I ..\Maps\*.ut2 \UC-Build\Maps\
XCOPY /Y /I ..\Sounds\*.uax \UC-Build\Sounds\
XCOPY /Y /I ..\Music\*.wav \UC-Build\Music\
XCOPY /Y /I ..\StaticMeshes\*.usx \UC-Build\StaticMeshes\
XCOPY /Y /I ..\Textures\*.utx \UC-Build\Textures\
XCOPY /Y /I ..\Textures\*.uce_utx \UC-Build\Textures\

RD ..\Maps /s /q
RD ..\Textures /s /q
RD ..\StaticMeshes /s /q

MOVE ..\PrevMaps ..\Maps
MOVE ..\PrevTextures ..\Textures
MOVE ..\PrevStaticMeshes ..\StaticMeshes
