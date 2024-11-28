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
can be built in any UNIX-like environment by using the standard GNU-style 
build procedure, which is to run the configure script (generated using GNU 
autoconf), which generates an appropriate makefile, and then execute the 
makefile using make.
Note: GNU make v3.77 or more recent is required.

Before building, export the PLATFORM environment variable and set it to the 
target platform for which you want to build. The types currently recognised 
are ps2, win32, linux and irix. Run the following commands to set the 
PLATFORM environment variable:
	export PLATFORM
	PLATFORM=win32

This is the sequence of commands to do a top-level build of all source code 
in the src directory, given that the PLATFORM variable has already been set:
	cd src
	make

Similarly you can build only the examples or tutorials or a single component 
by cd'ing into the subdirectory and executing the following commands 
(e.g. for tutorials):
	cd src/tutorials
	sh configure
	make release

The resulting example and tutorial executables are put in examples/bin
and tutorials/bin respectively.
And the component libraries are created in the components/lib directory.

The makefiles support the following build rules:

-  release - builds libraries and executables with no symbols and no 
   error/debug output

-  debug - builds debug-able libraries and executables. Both symbols and 
   error/debug output are enabled.

-  check_release - builds a release version (no symbols) but with 
   error/debug output enabled.


Using make: Platform-dependent Notes
------------------------------------

Win32

You can download the full suite of GNU tools for Win32 from:
http://sourceware.Cygnus.com/cygwin/
This includes all the relevant tools. We recommend building from the GNU 
shell prompt.

Microsoft Visual studio (cl.exe and link.exe) is required 
to build from within the cygnus environment. The PATH, INCLUDE and LIB 
variables must be setup to locate the MS tools (possibly using 
[installed location]\DevStudio\VC98\bin\vcvars32.bat )

Linux

Most recent Linux distributions (for example RedHat version 6 or later) come 
with all that you need. To build and run the examples, you need an OpenGL 
compatible implementation, for example any recent version (3 or later) of 
Mesa. A 3D-accelerated driver and X windows implementation, for example 
XFree86 4, is recommended.

Sony PS2, building from Linux

The Sony PlayStation2 toolchain v1.6 is required. Recent Linux distributions have 
the other required tools.

Sony PS2, building from Win32 using SN Systems ProDG

The Sony PS2 toolchain, recompiled for Win32 use, is included with the ProDG 
tools. You also need to download and install the GNU tools for Win32 as 
indicated in Win32 on page 29. The MathEngine Toolkits’ build system uses the 
default Sony PlayStation2 GNU linker; this produces binaries that are entirely 
compatible with those produced by the ProDG linker.

IRIX

You should have IRIX version 6.5 or later.
You need to download and install the GNU make program, version 3.77 or later:
http://www.gnu.org/gnulist/production/make.html

