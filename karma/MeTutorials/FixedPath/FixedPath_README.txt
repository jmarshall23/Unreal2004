README - FixedPath
~ tutorial
Toolkits demonstrated: Dynamics
last updated: June 22, 2000


Overview:

FixedPath shows how to use a fixed path joint. 


Discussion:

A fixed path joint allows you to precisely control the position 
and/or rotation and linear and angular velocities of a dynamic 
body in such a way that it interacts properly with other dynamic 
objects. 

Applied use examples include controlling the body using key-frame 
animation or using input from a user's mouse motion.

In this example a fish box is constrained to move on a circular path 
and slide along its long axis. The box rotation is derived from 
the simulation, and no constraints are applied, allowing to 
rotate freely. 

The red sphere is used to show the position of the joint and is 
not a physics body. By default the joint acts on one body and 
the world. By #define-ing TWO_BODIES the joint can be made to 
involve two bodies: both the box and the ring.


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2


Keyboard controls:

Key     Playstation 2           Action
--------------------------------------
t       Triangle                toggles between a fixed 
                                path and a fixed path 
                                fixed orientation joint


This program uses the MathEngine Viewer. Please see the MathEngine Viewer 
Developer's Guide for more information if desired. Controls are:

Key                     Action
------------------------------
cursors                 move camera
ctrl+cursors            move light
F1                      Application-specific Help
F2                      change shade model
F3                      toggle texture
F4                      toggle axis display
F5                      toggle stats display
F6                      toggle text
F7                      toggle camera info
F8                      toggle display lists
F9                      toggle fullscreen
F10                     toggle pause
F11                     toggle mouse manipulation
F12                     toggle renderer help
<Esc>                   quit


Mouse Controls:

Mouse controls          Action
------------------------------
drag in center          orbit camera
drag near left edge     pan camera up/down
drag near bottom edge   pan camera left/right
drag near right edge    pan camera towards/away
drag near top edge      zoom camera in/out
SHIFT + pick graphic    translate object left/right and up/down
CTRL + pick graphic     translate object towards/away
ALT + pick graphic      rotate object


Please direct any questions or comments regarding MathEngine 
Example Programs and Tutorials to support@mathengine.com .

======================================================================

Copyright (c) 1997-2002 MathEngine PLC

http://www.mathengine.com
