README - Hinge
~ tutorial
last updated: Dec 1, 2000


Overview:

Demonstrates a hinge joint powered by a limited-force motor. 


Discussion:

A body is connected to the world with a hinge joint. The hinge can be 
motorized and you can set rotational limits.

The limits can either be "hard" (if the limit stiffness factor is 
high) or "soft". In terms of the simulation, a hard bounce reverses
the bodies' angular velocities in a single timestep, while a soft 
bounce may take many timesteps to complete.  If the limits are soft,
you can also set their damping so that, beyond the limits, the hinge
behaves like a damped spring.  If the limits are hard, you can 
instead set the limit restitution between zero and one to govern the
loss of angular momentum as the bodies rebound.

Note that the angular velocity damping is applied directly to the hinged 
bodies.  This damping is completely independent of the angular limits.

With the exceptions of position and orientation, the body in
this example is initialised with default properties.  Its mass
is thus 1 unit (kg). 


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2




Please direct any questions or comments regarding MathEngine 
Example Programs and Tutorials to support@mathengine.com .

======================================================================

Copyright (c) 1997-2002 MathEngine PLC

http://www.mathengine.com
