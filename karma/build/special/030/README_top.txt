MathEngine Toolkits v0.3.0 Beta
-------------------------------

This release contains the MathEngine Toolkits v0.3.0 Beta release,
which includes:

-- Dynamics Toolkit 2.0 
-- Collision Toolkit 1.0.

The MathEngine Renderer (MeViewer2) is included to provide basic rendering
and a user interface for the example programs. MeViewer2 supports both 
the OpenGL and DirectX (version 7.0) APIs.
Demo executables and example source code are also provided.

NOTE: Please read the license agreement in the file named License before 
using any of the software in this distribution.

The target runtime and developer platforms for these toolkits are 
Windows 98, Windows 2000, Linux, PlayStation2 and Irix. Note that 
because Windows NT only supports DirectX up to version 3.0, it will 
only run the example programs under OpenGL.

You may however install the DirectX SDK (version 7.0 or better) on Windows NT
which will allow you to compile the Renderer (MeViewer2) with DirectX 7 
support, but the executables will still not be able to run under Windows NT.


Downloading the Toolkits
------------------------

Platform-specific archives are available for download from one the
MathEngine webpage: http://www.mathengine.com

Untar or unzip the appropriate archive file for your platform into an empty 
directory.

Documentation
-------------

Documentation for all toolkits are in the doc directory. Note that the 
documentation references source code tutorials in its discussions. You 
will find the code for these tutorials under src/tutorials.

The documentation includes:

-- Overview Document
-- Simulation Toolkit Developer's Guide
-- Simulation Toolkit Reference Manual
-- Dynamics Toolkit Developer's Guide
-- Dynamics Toolkit Reference Manual
-- Collision Toolkit Developer's Guide
-- Collision Toolkit Reference Manual
-- Viewer Toolkit Developer's Guide
-- 3DS Max Plug-in User's Guide
-- Utility Reference Manuals
	* Message Handler
	* Definitions & Tools
	* File Format
-- Demo Documentation

The documentation is browser-based, and consists of PDF files and HTML files.

Accordingly, you will need a Web browser and Adobe Acrobat Reader.
You can download Acrobat Reader from http://www.adobe.com.
You can download browsers from http://www.netscape.com and 
http://www.microsoft.com.
 
Start doc/index.html to view the home page for the documentation. 

Additional or updated documentation will be made available through the
MathEngine web site. See http://www.mathengine.com/metk/doc.

Example Programs and Demos
--------------------------

The bin directory contains executables for all demos, examples and tutorials.

For the example programs and demos 'F1' displays/hides Help for the example 
programs.

Toolkit Files
-------------

The include directory contains all the include files for all Toolkits,
and the lib.rel and lib.chk directories contains all the libraries.

The src/components directory contains subdirectories for each module for which
source code is provided:

-- Mdt         - the Dynamics Toolkit
-- Mst         - the Simulation Toolkit
-- MeApp       - an application framework
-- MeGlobals   - various utilities
-- MeMessage   - the Message Handler utility
-- MeViewer2   - the Viewer Toolkit
-- MeFile      - XML file handling routines

The source code for the example and tutorial programs is located in the 
src/examples and src/tutorials directories respectively.

RenderWare
----------
RenderWare is fully supported by the MathEngine toolkit but there are known 
compatibility problems with the MSVCRT build.
The LIBC build of the libraries should be used when developing with Renderware.
These are available to all licensed developers. If you require the LIBC 
libraries for evaluation purposes please contact MathEngine support.


Contacts
--------

See the following MathEngine websites:
    http://www.mathengine.com - corporate website

Or email:
    sales@mathengine.com    - for sales information
    support@mathengine.com  - for technical support using the Toolkits

