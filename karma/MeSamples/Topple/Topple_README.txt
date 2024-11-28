README - Topple
~ example program
Toolkits demonstrated: Collision and Dynamics
last updated: June 22, 2000


Overview:
This example shows the collision detection, Dynamics Event Manager
and Kea Dynamics working on a complicated environment.

Click and drag to rotate the view, then press 'space' to fire a ball.

You can 'kill' all the objects in the scene using 'k'. They will be
re-awakened if you hit them with a ball, or you can press 'w' to wake
everything.

Pressing F8 to disable draw-lists in the renderer lets you see which
objects are currently active (they go dark).
  

Hardware requirements:

PII 300MHz, 3d acceleration
Windows98 / Win2000 / WinNT / Linux / Irix / Playstation2


Controls:

Key             PlayStation 2           Action
----------------------------------------------
<Space>         Square                  fires a ball
<Return>        Start                   reset
a               Left Direction          toggles auto-evolve
f               Circle                  toggles friction
s               Right Direction         step evolve
w               Cross                   wake all objects
k               Triangle                kill all objects
F8              ---                     toggles a render-mode which 
                                        shades objects a darker color 
                                        if they are disabled.


On the PC this demo uses the MathEngine Viewer. Please see the 
MathEngine Viewer Developer's Guide for more information if desired. 
Controls are:

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


Discussion:

This example shows simple integration of collision with physics 
for numerous objects. You can set parameters for the contacts 
between objects. In this example we use three contact points 
between pairs of objects, with some friction and restitution. 
The Dynamics Event Manager is equally effective regardless of 
the number of bodies in your scene.

The rigid-body solver in the Dynamics Toolkit is very fast and 
stable. You can simply change settings for an object’s mass, 
friction, gravity, and so on to achieve the desired behaviour. 
No parameters need tuning to make this scene.

If you toggle friction off (f) while playing the game, you will 
see how important friction is in making a realistic simulation: 
without friction, the objects slide as if they were on a 
super-slippery ice-skating rink.

The Dynamics Event Manager of the Dynamics Toolkit allows for 
efficient processing by determining which objects need to be 
processed by the solver. Objects at rest are deactivated and 
not processed by the solver in the simulation. Collisions and 
forces will reactivate an object for processing.

Normally, objects that are at rest for a brief period become 
deactivated. You reactivate them by hitting them with a ball 
(or by causing another object to hit it).

But Topple also lets you press (k) to de-activate objects in 
the scene, and then re-activate them either by throwing balls, 
or by switching all of them them on by pressing (w). If you 
de-activate the objects while they are in motion, they will 
freeze into unlikely positions.

By using the F8 key, you can see which objects are disabled.


Please direct any questions or comments regarding MathEngine 
Example Programs and Tutorials to support@mathengine.com .

======================================================================

Copyright (c) 1997-2002 MathEngine PLC

http://www.mathengine.com
