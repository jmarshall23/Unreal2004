MathEngine Karma 
----------------

This release is the MathEngine Karma version 1.0.2 Gold release, that
includes:

- Karma Dynamics 
- Karma Collision 
- Karma Simulation Toolkit

The Karma Viewer (MeViewer2) is included to provide basic rendering and
a user interface for the example programs. The viewer supports both the
OpenGL and DirectX (version 7.0) APIs.  Demo executables and example
source code are provided.

NOTE: Please read the license agreement in the file named License before 
using any of the software in this distribution.

The target runtime and developer platforms for these toolkits are
PlayStation2, Windows 98, Windows 2000, Linux, and Irix. Note that
because Windows NT only supports DirectX up to version 3.0, it will only
run the example programs under OpenGL.

You may however install the DirectX SDK (version 7.0 or better) on
Windows NT that will allow you to compile MeViewer2 with DirectX 7
support, but the executables will still not run under Windows NT.

Downloading Karma
-----------------

Platform-specific archives are available for download from the downloads
page on the MathEngine corporate website <URL:http://www.mathengine.com/>

Untar or unzip the appropriate archive file for your platform into an
empty directory.

Documentation
-------------

Documentation for Karma is accessed through the 'index.html' file in the
'metoolkit' directory.

Note that the documentation references source code tutorials in its
discussions. You will find the code for these tutorials under
'metoolkit/src/tutorials'.

The documentation includes:

- An overview of Karma.
- Karma Simulation Toolkit.  Developer Guide.
- Karma Simulation Toolkit.  Reference Manual.
- Karma Dynamics.  Developer Guide.
- Karma Dynamics. Reference Manual.
- Karma Collision.  Developer Guide.
- Karma Collision. Reference Manual.
- Karma Viewer. Developer Guide.
- Utility Reference Manuals
  * Definitions & Tools 
  * Demo support
- File Format.  Developer Guide.
- Demo Documentation
- Glossary

The documentation is browser based, and consists of PDF and HTML files.

You will need a Web browser and Adobe Acrobat Reader. You can download
Acrobat Reader from <URL:http://www.adobe.com/>.

Browsers can be obtained from <URL:http://www.netscape.com/>,
<URL:http://www.microsoft.com/>.
 
Start 'metoolkit/index.html' to view the documentation. 

Please refer to the MathEngine website <URL:http://www.mathengine.com/>
for additional or updated documentation.

Example Programs and Demos
--------------------------

The bin directory contains executables for all demos, examples and
tutorials.

For the example programs and demos 'F1' displays/hides Help for the
example programs.

Karma Layout
------------

* The 'metookit/include' directory contains all the Karma include files.
* The 'metookit/lib.rel' directory contains all the release libraries.
* The 'metookit/lib.chk' directory' contains all the check libraries.
* The 'metookit/src/components' directory contains subdirectories for each
    module for which source code is provided.

- Mst         Karma Simulation Toolkit
- Mdt         Karma Dynamics 
- Mcd         Karma  Collision 
- MeApp       Demo Support
- MeGlobals   Various Utilities and Global Includes
- MeViewer2   Karma Renderer
- MeFile      XML file handling routines

The source code for the example and tutorial programs are respectively
located in the 'metookit/src/examples' and 'metookit/src/tutorials'
directories.

RenderWare
----------

RenderWare is fully supported by Karma but there are known compatibility
problems with the MSVCRT build.  The LIBC build of the libraries should
be used when developing with Renderware.  These are available to all
licensed developers. If you require the LIBC libraries for evaluation
purposes please contact MathEngine support.

Contacts
--------

The Corporate MathEngine website <URL:http://www.mathengine.com/>

Email:
  sales@mathengine.com    - sales information and general enquiries
  support@mathengine.com  - technical support for Karma
