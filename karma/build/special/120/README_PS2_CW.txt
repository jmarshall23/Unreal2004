This release of Karma for Sony PS2 has been compiled with the
Sony GCC compiler and the Sony libraries, versions documented
in 'README_PS2.txt'.

The usual rules for PS2 compatibility apply, and in particular
CodeWarrior versions up to 3.01 are directly compatible with
Karma when compiled with versions before 2.96 of the Sony GCC compiler.

IMPORTANT NOTE: if the release of Karma has been compiler with the Sony
compiler 2.96, please update your CodeWarrior installation as described
in:

    <URL:http://WWW.MetroWerks.com/games/playstation_2/?faq#toolchain>

To configure ProDG for compiling Karma components or projects:

Many versions of CodeWarrior for PS2 contain '.lcf' files that are
missing directives needed for compatibility with C++ code compiled
by GCC. MetroWerks support may have fixed '.lcf' files, or you
can add the following lines to your '.lcf' file:

          # GNU C++ exception table
          *     (.gcc_except_table)
          .     = ALIGN(0x10);
          __EXCEPTION_TABLE__ = .;

Add these lines after the definition of '__exception_table_end__'.

These actions must be done once or when the relevant paths are changed:

  * After selecting the 'Edit->Preferences...' menu entry, ensure that in
    'General->Source Trees' there are two definitions, one for 'SCE', the
    top directory of the Sony toolchain tree, and one for 'MEPS2', the top
    directory of the MathEngine Toolkit for PS2 tree.

  * Copy the file

      {Compiler}\Stationery\PlayStation2 - 2.0.0\c++\LinkSegment_PS2.lcf

    to '{MEPS2}/3rdParty/', or to the relevant project's directory if
    you need a per-project link configuration.

These actions must be done for every target in every project you create:

  * Add to the target 'Settings' 'Target->File Mappings' mappings for the
    extensions ".o" and ".a" as "Lib Import MIPS".

  * Add to the target 'Settings' 'Target->Access Paths' the 'User Paths'
    for the Sony toolchain, for example, for version 1.6.6 with the 2.2.x
    libraries:

      {SCE}ee/gcc/lib/gcc-lib/ee/2.9-ee-991111-01
      {SCE}ee/gcc/lib/gcc-lib/ee/2.9-ee-991111
      {SCE}ee/gcc/ee/lib
      {SCE}ee/lib

    They don't need to be recursively scanned.

  * Add to the target 'Settings' 'Target->Access Paths' the 'System
    Paths' for the Sony toolchain, for example, for version 1.6.6 with the
    2.1.3 libraries:

      {SCE}ee/gcc/lib/gcc-lib/ee/2.9-ee-991111-01/include
      {SCE}ee/gcc/ee/include
      {SCE}ee/include

    They don't need to be recursively scanned.

  * Add to the target 'Settings' 'Target->Access Paths' the 'User Paths'
    for the MathEngine Toolkit for PS2:

      {MEPS2}include
      {MEPS2}lib.rel/ps2		<- for release builds
      {MEPS2}lib.chk/ps2		<- for checked or debug builds

    They don't need to be recursively scanned.

  * Add to the target 'Settings' 'Target->Access Paths' the 'System Paths'
    for the MathEngine Toolkit for PS2:

      {MEPS2}include

    It does not need to be recursively scanned.

  * In the target 'Settings', set 'Target->MIPS Bare Target->Small Data'
    to zero.

  * In the target 'Settings', enable 'Language Settings->C/C++
    Language->Enums Always Int'.

  * In the target 'Settings', set the 'Language Settings->C/C++
    Language->Prefix File' field to "MeCWPS2Prefix.h" (for release builds)
    or "MeCWPS2PrefixCheck.h" (for checked or debug builds), or to the
    name of a header file of your own that includes either of them as
    appropriate.

These actions must be done for every project:

  * Add to the projects's 'Files' list the runtime libraries in
    '{MEPS2}lib.rel/ps2' (for a release build) or '{MEPS2}lib.chk/ps2'
    (for a checked or debug build).

  * Add to the project's 'Files' list:

      {SCE}ee/lib/crt0.s
      {Compiler}PS2 Support/gcc_wrapper.c
      {MEPS2}3rdParty/LinkSegment_PS2.lcf

All these actions have been incorporated in a sample project file,
'Program.mcp', included with the MathEngine Toolkit for PS2.

This project file can be copied to an appropriately named directory
under the '{Compiler}Stationery' directory and then used as CodeWarrior
project stationery.
