This release of Karma for Sony PS2 has been compiled with the
Sony GCC compiler and the Sony libraries, versions documented
in 'README_PS2.txt'.

The usual rules for PS2 compatibility apply, and in particular
ProDG versions up to 2.2 include SNSys GCC versions up to 2.95,
which are compatible with Karma when compiled with versions before
2.96 of the Sony GCC compiler.

To configure ProDG for compiling Karma components or projects:

* Make sure that the environment variable 'SN_PATH' is well defined, and
  that the file '%SN_PATH%\sn.ini' contains well defined paths and
  settings.

* From the '%SCE%\sce\ee\lib' directory copy into the toolkit's
  '%MEPS2%\3rdParty' directory the files 'app.cmd', 'ps2.lk'
  and 'crt0.s'. These are proprietary files of Sony and SNSys and we
  don't have permission to redistribute them ('%SCE%' should be
  replaced with the path leading to the Sony/SNSys devkit files,
  e.g. 'c:\usr\local').

* In the 'Project Settings' dialog:

  - Ensure that the 'C/C++>Preprocessor>Additional include directories'
    field is set to point to the '%MEPS2%\include' directory. The
    default value, '../../include' will work if the project files have
    been unzipped in the place mentioned in the point above.

  - Ensure that the 'Link>Input>Additional library path' field is set to
    point to the '%MEPS2%\lib.rel\ps2' directory for the release
    build and '%MEPS2%\lib.chk\ps2' directory for the debug build. The
    default value, '../../lib.{rel,chk}/ps2' will work if the project
    files have been unzipped in the place mentioned in the point above.

'%MEPS2% is whatever path leads to the directory containing Karma
for the PS2, e.g. 'c:\metoolkit'.

The project files have been developed and tested using the 'ps2cc' and
'ps2ld' frontends. Other frontends may require adjustements.

IMPORTANT NOTE FOR THOSE BUILDING USING GNU Make:

  If you are trying to build using the supplied Make files withing a
  CygWin environment, make sure that 'sh.exe' is a copy of 'bash.exe'.

  But the default CygWin setup installs 'ash' instead of 'bash' as
  'sh.exe', so beware. This can be fixed as simply as:

    bash -c "cp /bin/bash.exe /bin/sh.exe"
