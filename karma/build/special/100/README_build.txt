Building the Source Code
------------------------

Using Visual C++
----------------

To build all the source code using Visual C++:

1. Open 'metoolkit\src\MeTookits.dsw'
2. Click 'Build>Batch Build'.
3. In the 'Batch Build' dialog box, make sure that all components are
   selected.
4. Click 'Rebuild All' in the dialog box.

When creating a new MicroSoft VC++ project, the following additional
libraries may need to be added:

- libcmt.lib (/MT) or msvcrt.lib (/MD)
- oldnames.lib
- kernel32.lib
- user32.lib
- netapi32.lib
- advapi32.lib
- gdi32.lib
- comdlg32.lib
- comctl32.lib
- wsock32.lib

To do this, go to 'Project>Settings', select the 'Link' tab, then
select 'Input' in the 'Category' list. You can now add those libraries
libraries to the 'Object/Libraries' modules text box.

Using make
----------

All the source code of this release, including the examples and
tutorials, can be built in any UNIX-like environment with a Bourne shell
compatible shell by using the standard GNU-style build procedure.

The 'Makefile's depend on a central set of definitions and rules
contained within the directory 'src/components/build/makerules'.

Prior to compilation, the core set of rules must be processed with a
'configure' script (generated using GNU autoconf), to generate an
appropriate central 'Makefile'. The 'Makefile's throughout the tree will
accomplish this task as needed, so no explicit action is required to
create this file.

  Note: GNU make v3.77  is required.

Before building, export the 'PLATFORM' environment variable and set it
to the target platform for which you want to build. The types currently
recognized are "ps2", "win32" and "linux". Run the following command to
set the 'PLATFORM' environment variable:

   export PLATFORM=win32

Here is the sequence of commands to do a top-level build of all source
code in the src directory, given that the 'PLATFORM' variable has
already been set:

   cd src
   make

Similarly you can build only the examples or tutorials or a single
component by going to that subdirectory and executing the following
commands (e.g. for tutorials):

   cd src/tutorials
   make release

The makefiles support the following build targets:

- "release": This builds libraries and executables with no symbols and
  no error/debug output.

- "debug": This builds debug-able libraries and executables. Both
  symbols and error/debug output are enabled.

- "check_release": This builds a release version (no symbols) but with
  error/debug output enabled.

Resulting example and tutorial executables are put in 'examples/bin.rel'
and 'tutorials/bin.rel' respectively for release versions.

Debug versions will be generated into 'examples/bin.dbg' and
'tutorials/bin.dbg'.

Check_release builds will generate files into 'examples/bin.chk' and
'tutorials/bin.chk', respectively.

The component libraries will be compiled into the directories
'components/lib.rel', 'components/lib.dbg', and 'components/lib.chk'.

Using make: Platform-dependent Notes
------------------------------------

Win32
-----

You can download the full suite of GNU tools for Win32 from:

  http://sourceware.cygnus.com/cygwin/

This includes all the relevant tools. We recommend building from the GNU
shell prompt.

Microsoft Visual Studio ('cl.exe' and 'link.exe') is required to build
from within the Cygnus CYGWIN environment. The 'PATH', 'INCLUDE' and
'LIB' variables must be set up to locate the MS tools (possibly using
[installed location]'\DevStudio\VC98\bin\vcvars32.bat').

Linux
-----

MathEngine currently supports the RedHat version 6 or later Linux
distributions.  To build and run the examples, you need an OpenGL
compatible implementation, for example any recent version (3 or later)
of Mesa. A 3D-accelerated driver and X windows implementation, for
example XFree86 4, is recommended.

MathEngine may support other Linux distributions for paying developers
only.  Please contact 'sales@mathengine.com' for additional information.

Sony PlayStation2: Building from Linux
--------------------------------------

The Sony Ps2 that the library is built with is specified in the
'README_ps2libs.txt' in the 'metoolkits/src' folder for the PlayStation
2 version of Karma.

Recent Linux distributions have the other required tools. Sony supplies
a default toolchain. There are 3 C development environments: the Sony
toolchain, Codewarrior from Metrowerks, and ProDG from SN Systems.

Sony PlayStation2: Building from Win32, using SN Systems
--------------------------------------------------------

The Sony PlayStation2 toolchain, recompiled for Win32 use, is included
with the ProDG package.  You also need to download and install the GNU
tools for Win32 as described above.  Karma's build system uses the
default Sony PlayStation2 GNU linker; this produces binaries that are
entirely compatible with those produced by the ProDG linker.

To build using the SN compiler on Playstation 2
-----------------------------------------------

Option 1: Use Make
------------------

If you have Cygwin installed, you should be able to build the examples
programs using the supplied makefiles.  For example, the makefile for
the Ballman example can be found in 'metoolkit\src\examples\BallMan'.

Using a bash shell type:

   export PLATFORM=ps2

then

   make debug
or

   make release

The executable binary will be placed in the appropriate '../bin.dbg' or
'../bin.rel' directory

Option 2: MS Developer Studio
-----------------------------

If you have installed the ProDG Visual Studio Integration plug-in you will
have to create the .DSP from the makefile.

In MSDev:

* Create a new PS2 EE project in normal way locating it in the same
  directory as the source files.

* Add the source files to the project 'Project Settings'.

* Add an additional include path to the Karma include directory.

* Add "PS2" to the preprocessor definitions (also "_MECHECK" for debug
  build).

* Add an additional library path to the ME 'lib.rel/ps2' or
  'lib.chk/ps2' directory.

* Add a selection of MathEngine libs, such as the following:

  - libMst.a
  - libMeApp.a
  - libMcdPrimitives.a
  - libMcdCommon.a
  - libMcdFrame.a
  - libMdt.a
  - libMdtBcl.a
  - libMdtKea.a
  - libMeViewer2.a
  - libMeGlobals.a

Note that these libs may change - you will need to check this.  You may
need to change the order to resolve the link dependencies.

When building the SN compiler will probably output a number of warnings.
You will probably not need to worry about these.

If you have problems, you should check that you have entered the correct
paths because the compiler may not make it obvious that there is a
problem with this.

To build using the Codewarrior compiler on Playstation 2
--------------------------------------------------------

To build the libraries use the project file in
'metoolkit\src\components\components.mcp'

To build the examples use the project file in
'metoolkit\src\examples\examples.mcp'.

To build the tutorials use the project file in
'metoolkit\src\tutorials\tutorials.mcp'

After selecting the 'Edit->Preferences...' menu entry, ensure that in
'General->Source Trees' there are two definitions, one for 'SCE', the
top directory of the Sony toolchain tree, and one for 'MEPS2', the top
directory of Karma for PS2 tree.

For example:

  SCE:       c:\usr\local\sce
  MEPS2:     c:\metoolkit

See the documentation for details on creating a new project.
