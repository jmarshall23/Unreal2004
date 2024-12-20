README - BallnChain
~ example program
Toolkits demonstrated: Collision and Dynamics
last updated: June 22, 2000


Overview:

Balls are thrown at a hanging chain; the chain responds to the 
impact in a realistic manner.


Dynamics Discussion:

The top link of the chain is fixed to the world at a specified 
location using a hinge joint. You can modify attributes and 
parameters.


Collision Discussion:

The chain is made up of toruses connected through joints. The collision 
detection is done only between the ball and the torii.


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2


Keyboard controls:

The program uses an automated cycle to throw a ball at the chain 
at pre-defined intervals from various angles.


Key             Playstation 2           Action
----------------------------------------------
<space>         -                       allows the user to fire additional balls at will
p               Start                   toggle pause
t               Circle                  toggle auto-shooting
f               Triangle                increase time step by 0.001 s (f - faster)
s               Cross                   decrease time step by 0.001 s (s - slower)


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
