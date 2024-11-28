========================================================================
                Unreal Tournament 2004 Release notes.
========================================================================

Developed by	: Epic Games & Digital Extremes
Distributed by	: Atari

========================================================================
                        Table of Contents
========================================================================

  1  System Requirements
     
     1.1  Minimum System Requirements
     1.2  Recommended System Requirements

  2  Performance
     
     2.1  Resolution
     2.2  CPU speed
     2.3  Memory
     2.4  Considering upgrading?
     2.5  Getting better performance

  3  Troubleshooting
   
     3.1  Crashes
     3.2  Sound issues
     3.3  Network play issues
     3.4  Control issues
     3.5  Direct3D
     3.6  TNT2/ Kyro II/ Voodoo 3 issues
     3.7  OpenGL
     3.8  DirectX 9,0b
     3.9  Game is starting up windowed
     3.10 Software rendering
     3.11 Joystick support
     3.12 Logitech mice

  4  Dedicated Network Servers

     4.1  Explanation
     4.2  Launching
     4.3  Multiple server per machine
  
  5  Online Stats Tracking - UT2004stats

     5.1  Overview
     5.2  Turning on *your* Stats Tracking

  6  Useful Web Links

  7  Technical Support

  8  Copyright Notice

========================================================================
 1 System Requirements
========================================================================

1.1 Minimum System Requirements
-------------------------------

Operating System:    Windows 98/Me/2000/XP

Processor:           Pentium III or AMD Athlon 1.0 GHz processor

Memory:              128 MByte RAM

Hard Disk Space:     6 GByte free

Video:               32 MB Windows compatible video card*
           
Sound:               Windows compatible sound card. NVIDIA nForce or 
                     other motherboards/soundcards containing the Dolby 
                     Digital Interactive Content Encoder required for 
                     Dolby Digital audio.*

DirectX:             DirectX version 9.0b or higher

                     DirectX is not required for using the software
                     renderer  
                     
Modem:               33.6K baud (for modem/Internet play)

Internet (TCP/IP) and LAN (TCP/IP) play supported.

1.2 Recommended System Requirements
-----------------------------------

Operating System:    Windows 98/Me/2000/XP 

Processor:           Pentium IV or AMD Athlon 2.0 GHz processor

Memory:              512 MByte RAM

Hard Disk Space:     6 GByte free 
 
Video:               NVIDIA GeForce 4 or ATI Radeon Hardware T&L card with
                     128 MB VRAM recommended

Sound:               Sound Blaster Audigy 2 ZS

DirectX:             DirectX version 9.0b or higher
                     
Modem:               Broadband Internet connection

Internet (TCP/IP) and LAN (TCP/IP) play supported.

* Indicates device should be compatible with DirectX, version 9.0b or
higher.

========================================================================
 2 Performance
========================================================================

2.1 Resolution
--------------

The resolution in which you run the game will have the greatest impact on 
performance if you are not in the lucky position of running the latest 
top of the line graphics cards. Running at 640x480 in 16 bit mode or even 
512x384 in 16 bit mode should provide decent performance even on older 
hardware.

2.2 CPU Speed
-------------

The game is also very sensitive to CPU speed, memory bandwidth, and cache
performance. Thus, it runs far better on leading-edge processors like
those of AMD and Intel. See section 1.2 for a guideline on recommended 
specs.

2.3 Memory
----------

Unreal Tournament 2004's performance is highly dependent on the amount
of RAM you have in your machine, and the amount of memory that is
available. Machines with less memory will access their hard disk more
frequently to load data, which causes sporadic pauses in gameplay. Thus,
if you have a 128 MByte (or less) machine, you should make sure that
you don't have other unnecessary programs loaded in memory when playing
Unreal Tournament 2004.

2.4 Considering upgrading?
--------------------------

For people considering upgrading their machines, here are some tips based
on our experience running Unreal Tournament 2004 on a variety of
machines:

  1 The biggest performance gain in Unreal Tournament 2004 comes from
    having a state of the art graphics chip.

  2 The next upgrade that tends to improve Unreal Tournament 2004 
    performance is upgrading your CPU.

  3 Finally, lots of RAM helps. With memory prices continually falling,
    it's now reasonably affordable to upgrade to 512 MByte of system
    memory.

2.5 Getting better performance
------------------------------

By default Unreal Tournament 2004 picks hardware specific default
values the first time you run the game which should result in decent
performance but there is always room left to tweak.

Resolution and texture detail levels have the greatest impact on 
performance, if you have a fast CPU. So you might want to tweak the 
settings to achieve the visual quality / performance tradeoff you 
desire.

If you have a fast graphics card, but a rather slow CPU you might want
to lower the World detail setting in the detail settings menu.

Last but not least, if you have both a fast CPU and a fast graphics
card you shouldn't have to read this :).

========================================================================
 3 Troubleshooting
========================================================================

3.1 Crashes
------------

If Unreal Tournament 2004 stops with a "Ran out of virtual memory"
message, you need to free up more hard disk space on your primary drive
(C:) in order to play. Unreal Tournament 2004's large levels and rich
textures take up a lot of virtual memory.  We recommend having 500 MByte 
of free hard disk space for running the game.

3.2 Sound issues
----------------

If using a 3D sound accelerator such as the Sound Blaster Audigy series 
sound card, you can go into "Options/Audio" to enable EAX by selecting
the "Hardware 3D Sound + EAX" options.

You need to upgrade to the latest version of Sound Blaster Audigy drivers 
in order to get acceptable 3D sound performance.

Using Unreal Tournament 2004 in conjunction with earlier versions of the
drivers MAY cause severe performance problems (major slowdowns in the
order of 30-50% while playing sound) in which case you should change the
option back to the default "Software 3D Audio".

If your computer is hooked up to a 5.1 speaker system, you should go into
"Options" and turn on "Hardware 3D Audio" to take advantage of 360-degree 
sound panning, which rocks.

There also is a safe mode available in case you run into any audio 
related problems like glitches/ hitches/ static or bad performance if
sound is enabled.

3.3 Network play issues
-----------------------

The minimum speed connection for acceptable Internet play performance is
a 33.3K modem connection to your Internet Service Provider.

Known network play issues:

  * When a new player enters a network game, clients may experience a
    1/4-second pause while the mesh, skin, and other data is loaded for
    that player. This is by design.

  * Unreal Tournament 2004's Internet play performance is highly
    dependent on the bandwidth of your connection, the latency (ping
    time), and the packet loss.  The game is designed to be playable up
    to 300 msec ping times, 5% packet loss, and 33.3K connection speeds.
    Performance degrades heavily under worse latency, packet loss, and
    bandwidth connections.

3.4 Control issues
------------------

Some PC keyboards can't recognize certain combinations of 3 or more
simultaneously pressed keys.

3.5 Direct3D
-----------------

Please ensure that you are running the latest drivers for your graphics
card as Unreal Tournament 2004 stresses the hardware and drivers to a
greater extent than most available games and we rely on a number of
bug fixes hardware vendors incorporated into their latest drivers. To
obtain the latest drivers please visit the website of your graphics
card manufacturer.

3.6  TNT2 / Kyro II / Voodoo 3 issues
-------------------------------------

Due to the lack of certain key functionality like texture compression and
cubemap support TNT2, Kyro II, Voodoo 3, G400, ... cards won't be able
to run the game at the full visual quality. As uncompressed textures are
much larger than the compressed ones we have to drastically scale down
both size and color depth of the textures in order to fit them into video
memory. This means you will notice banding artifacts - especially in the 
menus. 

The lack of cubemap support means that reflections won't look correct 
though during gameplay you'll most likely be hard pressed to spot the 
difference.

3.7 OpenGL
-----------

The OpenGL renderer is not officially supported on Windows but could be 
a good choice on certain hardware / driver combinations as it might 
trigger fewer bugs in drivers. Unless you are experiencing serious visual 
flaws there is no reason to change to the OpenGL renderer though. Unlike 
Unreal Tournament, Unreal Tournament 2004 was designed around D3D and 
offers the best performance and visual quality with the D3D renderer.

Also please keep in mind that the OpenGL renderer has higher system 
requirements than the D3D renderer. The OpenGL renderer is known to 
not work correctly with pre-7.76 ATI drivers.

3.8 Obtaining DirectX 9.0b
---------------------------

DirectX 9.0b will be included on the retail CD and be downloaded from
the below URL if it is not already installed on your system.

   http://www.microsoft.com/windows/directx/default.aspx

3.9 Game is starting up windowed
---------------------------------

If the game fails to obtain keyboard focus when launching it will start
up windowed. In this case you can click on the window and press ALT-ENTER
to switch to fullscreen mode.

3.10 Software rendering
------------------------

You can have the game pick sensible default settings for software 
rendering by launching it with the -software command line argument. This
will also pick the software renderer as the render device which is 
useful if you run into driver issues or don't have a 3D accelerated 
graphics card.

   ut2004 -software

3.11 Joystick support
---------------------

Unreal Tournament 2004 supports joystick and gamepad controllers.
Buttons are configured the same way as key bindings. 
Go to the settings menu, input tab, and configure controls section.

If you wish to change the default configuration for Analog sticks, 
you will have to edit your User.ini file. 
(which usually resides in the c:\UT2004\System directory, 
or the System subdirectory of whatever other directory you installed 
the game in).

In the [Engine.Input] section of the User.ini file, you'll find the
following:
JoyX=Axis aStrafe SpeedBase=300.0 DeadZone=0.1
JoyY=Axis aBaseY SpeedBase=300.0 DeadZone=0.1 Invert=-1
JoyZ=
JoyR=
JoyU=
JoyV=Axis aBaseX SpeedBase=2.0 DeadZone=0.4
JoySlider1=Axis aLookUp SpeedBase=2.0 DeadZone=0.4
JoySlider2=

It is possible to override these settings 
and customize different controls for the SpaceFighter, 
by creating the following aliases:

SpaceFighter_JoyX
SpaceFighter_JoyY
SpaceFighter_JoyZ
SpaceFighter_JoyR
SpaceFighter_JoyU
SpaceFighter_JoyV
SpaceFighter_JoySlider1
SpaceFighter_JoySlider2

Here are the default settings for the spacefighter controls:

// MouseX bound to JoyX
Aliases[32]=(Command="Axis aBaseX SpeedBase=100.0 DeadZone=0.1",
Alias="SpaceFighter_JoyX")

// MouseY bound to JoyY
Aliases[33]=(Command="Axis aLookUp SpeedBase=100.0 DeadZone=0.1",
Alias="SpaceFighter_JoyY")

// Rolling (=Jump+Stafe) bound to second analog stick, X axis
Aliases[34]=(Command="Axis aUp Speed=+300.0 
| Axis aStrafe SpeedBase=300.0 DeadZone=0.1",
Alias="SpaceFighter_JoyV")

// Throttle (=Forward) bound to throttle on flight joysticks
Aliases[35]=(Command="Axis aBaseY SpeedBase=300.0 DeadZone=0.1 Invert=-1",
Alias="SpaceFighter_JoySlider1")

Here are the most common use for these keys,
(Flight joystick convention, followed by game pad convention).
Please note that these are references, and might not apply to your
controller.

JoyX = main (or left) analog X axis
JoyY = main (or left) analog Y axis
JoyZ = main analog Z axis, or slider (as found on the Logitech(tm)
       Wingman gamepads)
JoyR = <unknown>
JoyU = <unknown>
JoyV = Z Rotation or right analog X axis (Rudder).
JoySlider1 = Throttle (Z, Slider) or right analog X axis.
JoySlider2 =

note that the "Point of View Hat" is bound to these, 
and can be changed like standard keys, in game:
Joy13 = MoveForward
Joy14 = StrafeRight
Joy15 = MoveBackward
Joy16 = StrafeLeft

3.12 Logitech Mice
---------------------

If you're experiencing difficulty in assigning commands to your Logitech mouse's
extra buttons (middle button, side buttons, etc.) and you've installed the
MouseWare software, you might try adding the following registry entry. 

Under "HKEY_LOCAL_MACHINE -> Software -> Logitech -> MouseWare -> CurrentVersion
-> GamingCompatibility" add a new string named "Unreal2004" with the value being set
to the game's location. The default entry for this would be "c:\UT2004\System\UT2004.EXE".
If you're uncertain about this, please contact Logitech technical support 
(http://www.logitech.com/).

========================================================================
 4 Dedicated Network Servers
========================================================================

4.1 Explanation
---------------

For optimal network play performance, you can launch a dedicated copy of
the Unreal Tournament 2004 server on a computer. This improves performance
compared to using a non-dedicated server but, of course, since it ties up 
the PC.

Anybody may freely run dedicated servers on the Internet; you don't need
to get permission or fill out any paperwork.  

4.2 Launching
-------------

You can launch a dedicated server by going through the regular Unreal
Tournament 2004 menu, select "Host Multiplayer Game | Game & Map, setting
the appropriate options under the 'Server' tab, being sure to check
'Dedicated Server'. This is what you'll want to do for quick LAN games
where you have an extra machine sitting around that can act as a
dedicated server.

Alternatively, you can launch a dedicated server from the command line by
running ut2004.exe directly (which usually resides in the
c:\UT2004\System directory, or the System subdirectory of
whatever other directory you installed the game in).  For example, to
launch the level "DM-Rankin", run:

   ucc server DM-Rankin.ut2

4.3 Multiple Servers Per Machine
--------------------------------

Each copy of the dedicated server can serve one and only one level at a
time.

However, you can run multiple level servers on one machine. To do this,
you must give each server a unique TCP/IP port number. The default port
number is 7777. To specify a port, use the following type of command
line:

   ucc server DM-Rankin.ut2 port=7778

========================================================================
 5 Online Stats Tracking - UT2004stats
========================================================================

5.1 Overview
------------

Unreal Tournament 2004 has built in support for the freely available 
service UT2004stats. UT2004stats lets you track and display your scoring
and ranking and provides gameplay statistical analysis for your Internet 
games. 

For a detailed breakdown of your stats visit UT2004stats at:

   http://ut2004stats.epicgames.com

5.2 Turning on *your* Stats Tracking
------------------------------------

Go to the in-game Settings > Network > UT2004 Global Stats menu. Turn 
on UT2004stats tracking by clicking the "Track Stats" button, and 
choose a unique Stats Username and Stats Password combination. This 
will assign a unique PlayerID handle to you.

Note: Your online UT2004stats are *only* as safe as you are with your 
password.

========================================================================
 6 Useful Web Links
========================================================================

Visit for the latest updates, patches, and community events:

   http://www.unrealtournament.com

Information about other Epic games:

   http://www.epicgames.com

Information about other Digital Extremes games:

   http://www.digitalextremes.com

Latest news from the Epic development team. Great resources for 
programmers, mod authors, and enthusiasts:

   http://unreal.epicgames.com

Find out more about Atari Products:

   http://www.atari.com

========================================================================
 7 Technical Support
========================================================================

TECHNICAL SUPPORT (U.S. & Canada)
Help Via the Internet

Up-to-the-minute technical information about Atari products is generally 
available 24 hours a day, 7 days a week via the Internet at:

				http://www.atarisupport.com

Through this site you’ll have access to our FAQ (Frequently Asked Questions) 
documents, our FTP (File Transfer Protocol) area where you can download 
patches if needed, our Hints/Cheat Codes if they’re available, and an E-Mail 
area where you can get help and ask questions if you do not find your answers 
within the FAQ.

Note: In the event we must send you a Hint Sheet, FAQ document, patch or 
update disc via E-mail, we may require verifiable consent from a parent 
or guardian in order to protect children’s privacy and safety online. Consent 
Forms are available at the web site listed above.

Help Via Telephone in the United States & Canada

For phone assistance, call Atari Technical Support at (425) 951-7106. Our 
Interactive Voice Response system is generally available 24/7, providing 
automated support solutions immediately.

Great News! We’ve improved our Automated Systems so that you can get product-
specific Troubleshooting help more quickly. All you need to do is enter the 
product’s Part # when prompted to do so. This will take you directly to all 
of our known issues and solutions for this title. The product’s Part # is 
located in several places (on the CD label, package and/or plastic disc 
case) and is usually identified by a number such as 04-12345. When prompted 
by the Automated System, enter the last five digits of your product’s Part #. 
(For example, Part # 04-12345 would require that you enter the “12345” portion 
of the number for that product.) Note: Some products simply feature a five-
digit Part # without an “04-” prefix.

Live support is generally available Monday through Friday, 8:00 AM until 6:00 
PM (Pacific Time). Note: We may be closed on major holidays.

Before making your call, we ask that you be at your computer, have the 
following information available, and be ready to take notes:
•	System Make and Model 
•	Processor Type
•	Operating System, including version number if possible (such as Windows® 
98; Windows® Me)
•	RAM (Memory)
•	Video and sound card data and drivers
•	Any screen or error messages you’ve encountered (and where)

Product Return Procedures in the United States & Canada

In the event our technicians at (425) 951-7106 determine that you need to 
forward materials directly to us, please include a brief letter explaining 
what is enclosed and why. Make sure you include the Return Merchandise 
Authorization Number (RMA#) supplied to you by the technician, and your 
telephone number in case we need to call you. You will receive the mailing 
address when the technician gives you the RMA#. Any materials not containing 
this RMA# will be returned to you unprocessed.

Warranty Policy in the United States & Canada

If our technicians determine that the product storage medium is found to 
be defective within ninety (90) days of original purchase, (unless otherwise 
provided by applicable law), Atari will replace the item free of charge, to 
the original purchaser, if the item is accompanied by the original dated 
receipt and packaging.

========================================================================
 8 Copyright Notice
========================================================================

Karma Physics Library Copyright 2003 Criterion Software Limited. Karma is a
trademark of Criterion Software Limited, used under license.

Microsoft Speech Software Development Kit, Version 5.1 Copyright 2003
Microsoft. 

Ogg Vorbis Copyright 2001-2003, Xiph.Org Foundation

Speex Copyright 2002-2003, Jean-Marc Valin/ Xiph.Org Foundation

Uses Miles Sound System. Copyright C 1991-2003 by RAD Game Tools, Inc.

MPEG Layer-3 playback supplied with the Miles Sound System
from RAD Game Tools, Inc.  MPEG Layer-3 audio compression technology
licensed by Fraunhofer IIS and THOMSON multimedia.

DivX (R) Video provided by DivXNetworks, Inc. COPYRIGHT (C) 2000-2003.
"DivX is a registered trademark of DivXNetworks, Inc. in the USA and other
countries.

Simple DirectMedia Layer library is used under the terms of the LGPL
License. The LGPL License may be found on the CD.

Linux and Mac version of OpenAL are used under the terms of the LGPL
License. The LGPL License may be found on the CD.

BSDish Copyright (c) 2002 Jorge Acereda <jacereda@users.sourceforge.net> &
Peter O'Gorman ogorman@users.sourceforge.net

Unreal® Tournament 2003 ©2004 Epic Games, Inc. Raleigh, N.C. USA. 

Unreal and the Unreal logo are registered trademarks of Epic 
Games, Inc. ALL RIGHTS RESERVED. 

All other trademarks and trade names are the property of their 
respective owners. 

Unreal Tournament 2004 was created by Digital Extremes and Epic Games,
Inc.. 

Manufactured and marketed by Infogrames, Inc., New York, New York, 
a subsidiary of Infogrames Entertainment, S.A., under license from 
Epic Games, Inc.
