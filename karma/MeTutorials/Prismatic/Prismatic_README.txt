README - Prismatic
~ tutorial
Toolkits demonstrated: Dynamics
last updated: July 20, 2000


Overview:

Demonstrates a prismatic joint powered by a limited-force motor. 


Discussion:

Two bodies are connected to each other by a prismatic joint, which
limits their relative motion to a linear axis.  Alternatively, by
running "prismatic 1", one body is connected to the world by a 
prismatic joint.  In either case, the joint can be motorized and
you can set limits to the motion along the joint axis.

The limits can either be "hard" (if the limit stiffness factor is
high) or "soft". In terms of the simulation, a hard bounce reverses
the bodies' velocities in a single timestep, while a soft bounce 
may take many timesteps to complete.  If the limits are soft, you 
can also set their damping so that, beyond the limits, the joint 
behaves like a damped spring.  If the limits are hard, you can 
instead set the limit restitution between zero and one to govern
the loss of momentum as the bodies rebound.

With the exception of their position, the bodies in this example 
are initialised with default properties.  Their masses are thus 1
unit (kg).


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2


Keyboard controls:

Key     Playstation2            Action
--------------------------------------
s       Start                   switches the direction of the motor
z       Down                    decreases the desired velocity 
                                by 1 metre per second
x       Up                      increases the desired velocity 
                                by 1 metre per second
m       Right                   decreases maximum motor force
                                by 1 Newton
k       Left                    increases maximum motor force
                                by 1 Newton
t       Select                  toggles the limits on and off
i       L2                      reset the limit stiffness to maximum
o       R2                      clear the limit stiffness to zero
p       Triangle                increases the limit stiffness
                                by 1 Newton per metre
l       Cross                   decreases the limit stiffness
                                by 1 Newton per metre
j       Circle                  increases the limit damping
                                by 1 Newton second per metre
h       Square                  decreases the limit damping
                                by 1 Newton second per metre
q       R1                      increase the limit restitution by 0.01
a       L1                      decrease the limit restitution by 0.01


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
