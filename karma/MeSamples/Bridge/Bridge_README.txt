README - Bridge
~ example program
Toolkits demonstrated: Collision and Dynamics
last updated: June 22, 2000


Overview:

Balls are thrown at a plank bridge; the bridge responds to the 
shock in a realistic way.


Dynamics Discussion:

The planks are attached to each other with single ball and 
socket joints running down the centre of the bridge.


Collision Discussion:

This example illustrates how to selectively turn off collision 
detection for pairs of models that are close or touching, but 
whose geometrical interaction is being modeled by other means 
(in this case, by imposing a geometrical constraint into the 
dynamical model).


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2


Keyboard controls:

The program uses an automated cycle to throw two balls at a 
time at the bridge at pre-defined intervals. It varies the 
parameters each time it throws.


Key             Playstation 2           Action
----------------------------------------------
<Space>         Square                  shoot the ball
p               Start                   pause simulation
t               Circle                  toggle auto-shooting
s               Cross                   decrease time step by 0.001 s (s - slower)
f               Triangle                increase time step by 0.001 s (f - faster)



This demo uses the MathEngine Viewer. Please see the MathEngine Viewer 
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
