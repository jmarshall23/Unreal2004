                     Building the Source Code
                     ------------------------

Using Visual C++
----------------

To build all the source code using Visual C++:

1. Open src/MeToolkits.dsw 
2. Click Build -> Batch Build.
3. In the Batch Build dialog box, make sure that all components are selected.
4. Click Rebuild All in the dialog box.


Using make
----------

All the source code of this release, including the examples and tutorials, 
can be built in any UNIX-like environment with a sh-compatible shell by 
using the standard GNU-style build procedure. The makefiles depend on a 
central set of definitions and rules contained within the directory
src/components/build/makerules.

Prior to compilation, the core set of rules must be processed with a GNU 
autoconf configure script (generated using GNU autoconf), to generate an
appropriate central makefile. The makefiles throughout the tree will 
accomplish this task as needed, so no overt act is required to create this
file.

Note: GNU make v3.77 or more recent is required.

Before building, export the PLATFORM environment variable and set it to the 
target platform for which you want to build. The types currently recognized 
are ps2, win32, linux and irix. Run the following command to set the 
PLATFORM environment variable:
	export PLATFORM=win32

This is the sequence of commands to do a top-level build of all source code 
in the src directory, given that the PLATFORM variable has already been set:
	cd src
	make

Similarly you can build only the examples or tutorials or a single component 
by cd'ing into the subdirectory and executing the following commands 
(e.g. for tutorials):
	cd src/tutorials
	make release

The makefiles support the following build targets:

-  release - builds libraries and executables with no symbols and no 
   error/debug output

-  debug - builds debug-able libraries and executables. Both symbols and 
   error/debug output are enabled.

-  check_release - builds a release version (no symbols) but with 
   error/debug output enabled.


Resulting example and tutorial executables are put in examples/bin.rel and 
tutorials/bin.rel respectively for release versions. Debug versions will be
generated into examples/bin.dbg and tutorials/bin.dbg. Check_release builds 
will generate files into examples/bin.chk and tutorials/bin.chk, respectively.

The component libraries will be coompiled into the directories 
components/lib.rel, components/lib.dbg, and components/lib.chk.

Using make: Platform-dependent Notes
------------------------------------

Win32

You can download the full suite of GNU tools for Win32 from:
	http://sourceware.cygnus.com/cygwin/

This includes all the relevant tools. We recommend building from the GNU 
shell prompt.

Microsoft Visual Studio (cl.exe and link.exe) is required to build from 
within the Cygnus CYGWIN environment. The PATH, INCLUDE and LIB 
variables must be set up to locate the MS tools (possibly using 
[installed location]\DevStudio\VC98\bin\vcvars32.bat )

Linux

Most recent Linux distributions (for example RedHat version 6 or later) come 
with all that you need. To build and run the examples, you need an OpenGL 
compatible implementation, for example any recent version (3 or later) of 
Mesa. A 3D-accelerated driver and X windows implementation, for example 
XFree86 4, is recommended.

Sony PS2, building from Linux

The Sony PlayStation2 toolchain v1.6 is required. Recent Linux distributions
have the other required tools.

Sony PS2, building from Win32 using SN Systems ProDG

The Sony PS2 toolchain, recompiled for Win32 use, is included with the ProDG 
tools. You also need to download and install the GNU tools for Win32 as 
indicated in Win32 on page 29. The MathEngine Toolkits build system uses the 
default Sony PlayStation2 GNU linker; this produces binaries that are entirely 
compatible with those produced by the ProDG linker.

IRIX

You should have IRIX version 6.5 or later.
You need to download and install the GNU make program, version 3.77 or later:
http://www.gnu.org/gnulist/production/make.html

