README - BallHitsWall1
~ tutorial
last updated: Dec 1, 2000


Overview:

BallHitsWall1 uses the Collision Toolkit alone, without the Dynamics 
Toolkit. The scenario: a ball is thrown at a wall, bounces off the 
wall, bounces off the floor, and then exits the scene. The motion of
the ball is not being driven by the dynamics engine, rather, it is 
scripted by hand.

Note: There is no gravity in this simulation.


Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2
 


Discussion:

The motion is controlled by a very simple physics algorithm in 
BallHitsWall1.c itself. This program demonstrates how to use the 
Collision Toolkit to perform some basic tasks:

- Obtaining collision events.
- Identifying them (ball-wall collision vs. ball-floor collision).
- Calling the intersection function.
- Reading contact data structures.

The code for the motion algorithm shows you how to extract 
information from the contact data structures and how to use 
this information to produce appropriate changes in the motion 
of the rendered models - in this case both bouncing off the 
wall and floor. 

The program also shows how the new positions computed from the 
motion algorithm are used to update the coordinate systems for 
both the collision model and the rendered model.

Additionally, by changing one line of code, we can switch from 
sphere behavior to cube behavior. The collision will then produce
four contacts instead of just one.


Please direct any questions or comments regarding MathEngine 
Example Programs and Tutorials to support@mathengine.com .

======================================================================

Copyright (c) 1997-2002 MathEngine PLC

http://www.mathengine.com
