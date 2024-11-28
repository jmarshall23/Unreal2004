README - BallHitsWall2
~ tutorial
last updated: Dec 1, 2000


Overview:

BallHitsWall2 reproduces the behaviour of BallHitsWall1, only now 
the Dynamics Toolkit is being used to control the motion.


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2



Discussion:

Unlike BallHitsWall1, there is no motion code anywhere in the program. 
All motion is controlled by calls to the Dynamics Toolkit.

The MstBridge handles all communication and synchronisation 
between the  collision detection and the dynamics.
The only responsibility of the application programmer is 
to specify the geometrical and dynamical properties of the 
simulation (via the Collision Toolkit and the Dynamics Toolkit 
interfaces, respectively).

Additionally, by changing one line of code, we can switch from 
sphere behavior to cube behavior. 


Please direct any questions or comments regarding MathEngine 
Example Programs and Tutorials to support@mathengine.com .

======================================================================

Copyright (c) 1997-2002 MathEngine PLC

http://www.mathengine.com
