========================================================================
                Unreal Tournament 2003 "README" notes.
========================================================================

Developed by	: Digital Extremes & Epic Games
Distributed by	: Atari


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
     3.6  NVIDIA 40.41 drivers
     3.7  GeForce 4 MX/Go issues
     3.8  Radeon 7x00 issues
     3.9  TNT2/ Kyro II/ Voodoo 3 issues
     3.10 OpenGL
     3.11 DirectX 8.1b
     3.12 Game is starting up windowed

  4  Controls
     
     4.1  Keyboard
     4.2  Mouse
     4.3  Speech menu

  5  Dedicated Network Servers

     5.1  Explanation
     5.2  Launching
     5.3  Multiple server per machine
     5.4  General performance guidelines
     5.5  More information
  
  6  Benchmarking

     6.1  Overview
     6.2  Possible issues
     6.3  Further information

  7  Online Stats Tracking - UT2003stats

     7.1  Overview
     7.2  Turning on *your* Stats Tracking

  8  Useful Web Links

  9  Technical Support

  10 Copyright Notice


========================================================================
 1 System Requirements
========================================================================

1.1 Minimum System Requirements
-------------------------------

Operating System:    Windows 98/Me/2000/XP (Linux has separate README)

Processor:           Pentium III or AMD Athlon 733 MHz processor

Memory:              128 MByte RAM

Hard Disk Space:     3 GByte free HD space

Video:               16 MB TNT2-class or other DirectX version 6 
                     compliant video card* 

Sound:               Windows compatible sound card. NVIDIA nForce or 
                     other motherboards/soundcards containing the Dolby 
                     Digital Interactive Content Encoder required for 
                     Dolby Digital audio.*

DirectX:             DirectX version 8.1b (on CD)
                     
Modem:               33.6K baud (for modem/Internet play)

Internet (TCP/IP) and LAN (TCP/IP) play supported.

1.2 Recommended System Requirements
-----------------------------------

Operating System:    Windows 98/Me/2000/XP (Linux has separate README)

Processor:           Pentium III or AMD Athlon 733 MHz processor

Memory:              256 MByte RAM

Hard Disk Space:     3 GByte free HD space

Video:               NVIDIA GeForce2 or ATI Radeon (32-128 MB VRAM 
                     recommended)*

Sound:               Sound Blaster Audigy series sound card*

DirectX:             DirectX version 8.1b (on CD)

Modem:               56K baud (for modem/Internet play)

Internet (TCP/IP) and LAN (TCP/IP) play supported.

* Indicates device should be compatible with DirectX, version 8.1 or
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

Unreal Tournament 2003's performance is highly dependent on the amount
of RAM you have in your machine, and the amount of memory that is
available. Machines with less memory will access their hard disk more
frequently to load data, which causes sporadic pauses in gameplay. Thus,
if you have a 128 MByte (or less) machine, you should make sure that
you don't have other unnecessary programs loaded in memory when playing
Unreal Tournament 2003.

How the game will perform under different RAM conditions:

  * 64 MByte or less: lots of swapping

  * 128 MByte: minimal swapping with default settings

  * 256 MByte: might swap in rare cases with highest texture detail

  * 512 MByte or more: shouldn't swap

2.4 Considering upgrading?
--------------------------

For people considering upgrading their machines, here are some tips based
on our experience running Unreal Tournament 2003 on a variety of
machines:

  1 The biggest performance gain in Unreal Tournament 2003 comes from
    having a state of the art graphics chip.

  2 The next upgrade that tends to improve Unreal Tournament 2003 
    performance is upgrading your CPU.

  3 Finally, lots of RAM helps. With memory prices continually falling,
    it's now reasonably affordable to upgrade to 256 MByte of system
    memory.

2.5 Getting better performance
------------------------------

By default Unreal Tournament 2003 picks hardware specific default
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

If Unreal Tournament 2003 stops with a "Ran out of virtual memory"
message, you need to free up more hard disk space on your primary drive
(C:) in order to play. Unreal Tournament 2003's large levels and rich
textures take up a lot of virtual memory.

On cards without hardware accelerated transform & lighting the game will
crash reporting a problem in "DrawPrimitive" with DirectX 8.1. The 
problem has been addressed by Microsoft for DirectX 8.1b which 
automatically gets installed in the installation process. Also certain
older NVIDIA driver exhibit the same problem in which gets the game will
ask you to upgrade.

3.2 Sound issues
----------------

If using a 3D sound accelerator such as the Sound Blaster Audigy series 
sound card, you can go into "Options/Audio" to enable EAX 3.0 by selecting
the "Hardware 3D Sound + EAX" options.

You need to upgrade to the latest version of Sound Blaster Audigy drivers 
in order to get acceptable 3D sound performance.

Using Unreal Tournament 2003 in conjunction with earlier versions of the
drivers MAY cause severe performance problems (major slowdowns in the
order of 30-50% while playing sound) in which case you should change the
option back to the default "Software 3D Audio".

If your computer is hooked up to a 5.1 speaker system, you should go into
"Options" and turn on "Hardware 3D Audio" to take advantage of 360-degree 
sound panning, which rocks.

3.3 Network play issues
-----------------------

The minimum speed connection for acceptable Internet play performance is
a 33.3K modem connection to your Internet Service Provider.

Known network play issues:

  * When a new player enters a network game, clients may experience a
    1/4-second pause while the mesh, skin, and other data is loaded for
    that player. This is by design.

  * Unreal Tournament 2003's Internet play performance is highly
    dependent on the bandwidth of your connection, the latency (ping
    time), and the packet loss.  The game is designed to be playable up
    to 300 msec ping times, 5% packet loss, and 33.3K connection speeds.
    Performance degrades heavily under worse latency, packet loss, and
    bandwidth connections.

  * If you have the Windows XP "Internet Connection Firewall" enabled,
    you won't be able to see LAN games in progress.  To disable it,
    go into the Windows XP Start Menu / Network Settings / Local
    Area Connection Properties / Advanced and clear the
    "Internet Connection Firewall" checkbox.

3.4 Control issues
------------------

Some PC keyboards can't recognize certain combinations of 3 or more
simultaneously pressed keys.

3.5 Direct3D
-----------------

Please ensure that you are running the latest drivers for your graphics
card as Unreal Tournament 2003 stresses the hardware and drivers to a
greater extent than most available games and we rely on a number of
bug fixes hardware vendors incorporated into their latest drivers. To
obtain the latest drivers please visit the website of your graphics
card manufacturer.

3.6 NVIDIA 40.41 drivers
------------------------

The NVIDIA 40.41 drivers are known to have visual flaws and performance 
problems (hitching) with Unreal Tournament 2003 which are not present in 
the 30.82 drivers. The issue has been addressed by NVIDIA and future 
drivers released by NVIDIA will contain the appropriate fixes.

If you are currently running the 40.41 drivers please either downgrade 
to 30.82 or install newer drivers when available.

3.7 GeForce 4 MX/Go issues
--------------------------

With GF4 MX/Go cards you might experience graphic corruption in 32 bit
mode. The problem has been addressed by NVIDIA and upcoming drivers will
contain the fix. A workaround for now is to play the game in 16 bit mode.

3.8 Radeon 7x00 issues
----------------------

Radeon 7x00 cards exhibit a couple of visual flaws like the hanging 
banners in BR-Anubis which will be addressed by us for the full version 
of the game.

We recommend using the 7.76 drivers (Catalyst 2.3) drivers as they
incorporate fixes for running Unreal Tournament 2003.

3.9  TNT2 / Kyro II / Voodoo 3 issues
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

3.10 OpenGL
-----------

The OpenGL renderer is not officially supported on Windows but might be 
a good choice on certain hardware / driver combinations as it might 
trigger fewer bugs in drivers. Unless you are experiencing serious visual 
flaws there is no reason to change to the OpenGL renderer though. Unlike 
Unreal Tournament, Unreal Tournament 2003 was designed around D3D and 
offers the best performance and visual quality with the D3D renderer.

Also please keep in mind that the OpenGL renderer is still work in 
progress and has higher system requirements than the D3D renderer. E.g. 
it relies on the presence of texture compression which rules out e.g. 
TNT2 cards.

The OpenGL renderer is known to not work correctly with pre-7.76 ATI 
drivers.

3.11 Obtaining DirectX 8.1b
---------------------------

DirectX 8.1b contains bug fixes in the DirectX runtime that are required
for the game to run on cards without hardware transform & lighting. E.g.
TNT2, Kyro I/II, Voodoo 3, Radeon 7000, ... DirectX 8.1b is included on
the CD and is automatically installed during the installation process.

Alternatively you can read below how to obtain is separately.

If you are running Windows 98/ME/2000 you can download DirectX 8.1b 
using the below link.

	http://www.microsoft.com/windows/directx/downloads/drx81.asp


For Windows XP you have to install QFE 321178 to upgrade your version
of DirectX to 8.1b

	http://support.microsoft.com/default.aspx?scid=kb;en-us;Q321178


Please note that Windows XP Service Pack 1 contains this fix.

	http://www.microsoft.com/WindowsXP/pro/downloads/servicepacks/sp1/default.asp

3.12 Game is starting up windowed
---------------------------------

If the game fails to obtain keyboard focus when launching it will start
up windowed. In this case you can click on the window and press ALT-ENTER
to switch to fullscreen mode.

========================================================================
 4 Controls
========================================================================

4.1 Keyboard
------------

You can bind new keys to controls by clicking on the "Controls" tab of
the Preferences menu inside of Unreal Tournament 2003. There are many 
controls to bind.

4.2 Mouse
---------

Although you can rely solely on your keyboard to move around in and
interact in Unreal Tournament 2003's 3D universe, using both the
keyboard and mouse simultaneously gives you much more fluid and
responsive control.

When you use the mouse to control your rotational movement and aiming you
gain a degree of precision and speed that players using keyboard-only
controls can't touch. The keyboard is best used for simple lateral and
forward/backward movement, and for jumping.

To master the default controls in Unreal Tournament 2003, keep your left
hand on the keyboard, using the arrow keys for movement, the 0-9 keys for
weapon selection, and the space bar for jumping. Your right hand operates
the mouse, controlling rotation, aiming, and firing. Of course, you can
customize these controls to suit your preferences via the Options Menu.

4.3 Speech Menu
---------------

Press the 'V' key to open the speech menu. You can now select various 
commands and taunts. Under the Orders submenu you can select a job to 
assign to a teammate. If your bot is a teammate they will automatically 
carry out your orders. The available orders are:

  Defend the Base.
        The ordered bot will immediately make his way to your base and
        protect it from enemy attack.

  Cover Me.
        The bot will find you and follow you attacking enemies that you
        encounter.

  Assault the Base.
        Orders the bot to go offensive. In CTF this order is replaced
        with "Capture the Enemy Flag."

  Hold this Position.
        The bot will find your current location and try to protect it
        from enemy attack.

  Freelance.
        Releases the bot from previous orders. A freelancing bot will
        make their own battlefield decisions.

If you give an order to "All" then every bot on your team will attempt to
complete the order. If you look at a bot the speech menu will have the
option to "Order This Bot."

========================================================================
 5 Dedicated Network Servers
========================================================================

5.1 Explanation
---------------

For optimal network play performance, you can launch a dedicated copy of
the Unreal Tournament 2003 server on a computer. This improves performance
compared to using a non-dedicated server but, of course, since it ties up 
the PC.

Anybody may freely run dedicated servers on the Internet; you don't need
to get permission or fill out any paperwork.  

5.2 Launching
-------------

You can launch a dedicated server by going through the regular Unreal
Tournament 2003 menu, select "Host Multiplayer Game | Game & Map, setting
the appropriate options under the 'Server' tab, being sure to check
'Dedicated Server'. This is what you'll want to do for quick LAN games
where you have an extra machine sitting around that can act as a
dedicated server.

Alternatively, you can launch a dedicated server from the command line by
running UnrealTournament2003.exe directly (which usually resides in the
c:\UnrealTournament2003\System directory, or the System subdirectory of
whatever other directory you installed the game in).  For example, to
launch the level "DM-Antalus", run:

   ucc server DM-Antalus.ut2

5.3 Multiple Servers Per Machine
--------------------------------

Each copy of the dedicated server can serve one and only one level at a
time.

However, you can run multiple level servers on one machine. To do this,
you must give each server a unique TCP/IP port number. The default port
number is 7777. To specify a port, use the following type of command
line:

   ucc server DM-Antalus.ut2 port=7778

5.4 General performance guidelines
----------------------------------

We find that an 600 MHz CPU can usually handle about 16 players with
decent performance. The performance varies with level complexity and
other machine speed factors, so your mileage may differ. Performance 
degrades as the number of players grows huge.

If you're running multiple servers simultaneously, Windows XP/NT/2k
outperforms Windows 9x due to its superior multitasking and TCP/IP
processing capabilities.

For best performance, we recommend having 128 MByte of memory per
running server. For example, for running 2 simultaneous servers your
machine should at least be equipped with 256 MByte of memory.

The Unreal Tournament 2003 server uses up at least 33.38 Kbits per 
second of outgoing bandwidth per player (on Internet), so if you run 
the server on a machine connected by a 28.8K modem, you'll only be 
able to support one client with decent performance. Dedicated servers 
that support many players generally require the outgoing bandwidth 
of a T1 line or better.

5.5 More Information
--------------------

Visit the Unreal Technology Page for extensive information about running
Unreal Tournament 2003 servers. Some topics addressed there include:

  * Remote server administration

  * Linux version of the server 

  * Server only download

  * Serving custom levels and game types

  * Administering public servers that appear in the server list

========================================================================
 6 Benchmarking
========================================================================

6.1 Overview
------------

Unreal Tournament 2003 comes with a large variety of benchmarking 
functionality. The easiest way to utilize it is by starting 
Benchmark.exe in the System directory of your Unreal Tournament 2003
installation directory. This will bring up a dialog box asking you to
pick a resolution and when you press okay, it will run 4 consecutive 
tests (two level flybys and two botmatches). It is important to not 
touch the mouse before the final dialog with the results pops up. The 
result dialog displays the average framerate of the two flybys and the 
two botmatches.

6.2 Possible issues
-------------------

The benchmark will always try to run at the highest detail settings so
results will not be directly comparable with cards on which the game 
can't render at the highest detail level (e.g. TNT2 and Kyro I/II
cards).

Due to the way the benchmarking works you have to manually mark the 
following two files as read- only before running Benchmark.exe on 
multiple cards.

INSTALL_DIR\Benchmark\Stuff\MaxDetail.ini
INSTALL_DIR\Benchmark\Stuff\MaxDetailUser.ini

For the botmatches the Benchmark.exe is running a game with just bots, 
with the player in spectator mode. In case the engine can't run at the
highest detail settings the outcome of the botmatch might differ 
leading to a different botmatch and therefore numbers not comparable 
to other results. This could also happen when comparing different kinds
of CPUs. So either watch the botmatches carefully when benchmarking to
ensure that they are the same every time or compare the number in 
brackets in benchmark.log. If the number is the same, the exact same
sequence was rendered.

6.3 Further information
-----------------------

In the Benchmark directory you will find a couple of batch files 
which can be used to run individual tests and to create a big batch 
file for automating the whole process of running at multiple 
resolutions.

The game will log a lot of information and stats while in benchmark 
mode and the results can be found in the subdirectories of the 
Benchmark folder. Of the most interest is the CSV folder, which 
contains the myriad of stats the engine collects on a per frame 
basis in the form of a "comma separated variables" file which e.g. 
can be opened in Microsoft Excel.

========================================================================
 7 Online Stats Tracking - UT2003stats
========================================================================

7.1 Overview
------------

Unreal Tournament 2003 has built in support for the freely available 
service UT2003stats. UT2003stats lets you track and display your scoring
and ranking and provides gameplay statistical analysis for your Internet 
games. 

For a detailed breakdown of your stats visit UT2003stats at:

	http://ut2003stats.epicgames.com

7.2 Turning on *your* Stats Tracking
------------------------------------

Go to the in-game Settings > Network > UT2003 Global Stats menu. Turn 
on UT2003stats tracking by clicking the "Track Stats" button, and 
choose a unique Stats Username and Stats Password combination. This 
will assign a unique PlayerID handle to you.

Note: Your online UT2003stats are *only* as safe as you are with your 
password.

========================================================================
 8 Useful Web Links
========================================================================

Visit for the latest updates, patches, and community events:

	http://www.unrealtournament2003.com

Information about other Digital Extremes games:

	http://www.digitalextremes.com

Information about other Epic games:

	http://www.epicgames.com

Latest news from the Epic development team. Great resources for 
programmers, mod authors, and enthusiasts:

	http://unreal.epicgames.com

Find out more about Atari Products:

	http://www.atari.com

========================================================================
 9 Technical Support
========================================================================

TECHNICAL SUPPORT (U.S. & Canada)
---------------------------------

Help Via the Internet Up-to-the-minute technical information about
Infogrames, Inc. products is generally available 24 hours a day, 7
days a week via the Internet at:

	http://www.ina-support.com

Through this site you'll have access to our FAQ (Frequently Asked
Questions) documents, our FTP (File Transfer Protocol) area where you 
can download patches if needed, our Hints/Cheat Codes if they're 
available, and an E-Mail area where you can get help and ask questions 
if you do not find your answers within the FAQ.

Note: In the event we must send you a Hint Sheet, FAQ document, 
patch or update disc via E-mail, we may require verifiable consent 
from a parent or guardian in order to protect children's privacy and 
safety online. Consent Forms are available at the web site listed 
above.

Help Via Telephone/Fax or Mail in the United States & Canada 

For phone assistance, call Infogrames, Inc. Tech Support at (425)
951-7106. Our Interactive Voice Response and Faxback system is 
generally available 24/7, providing automated support and allowing 
FAQ documents to be faxed to you immediately.

Great News! We've improved our Automated Systems so that you can get
product-specific Troubleshooting help more quickly. All you need to 
do is enter the product's Tech Support # when prompted to do so. 
This will take you directly to all of our known issues and solutions 
for this title. The product's Tech Support # is located in several 
places (on the CD label, package and/or plastic disc case) and is 
usually identified by a number such as 04-12345. When prompted by the 
Automated System, enter the last five digits of your product's Tech 
Support#. (For example, Tech Support # 04-12345 would require that you 
enter the "12345" portion of the number for that product.) Note: Some 
products simply feature a five-digit Tech Support # without an "04-" 
prefix.

Live support is generally available Monday through Friday, 8:00 AM 
until 6:00 PM (Pacific Time). Note: We may be closed on major 
holidays.

Before making your call, we ask that you be at your computer, have 
the following information available, and be ready to take notes:

- System Make and Model
- Processor Type
- Operating System, including version number if possible (such as
  WindowsR 98; WindowsR Me)
- RAM (Memory)
- Video and sound card data and drivers
- Any screen or error messages you've encountered (and where)

You may also fax in your Technical Support questions or problems to:
(425) 806-0480, or write to the address below.

Product Return Procedures in the United States & Canada

In the event our technicians at (425) 951-7106 determine that you 
need to forward materials directly to us, please include a brief 
letter explaining what is enclosed and why. Make sure you include 
the Return Merchandise Authorization Number (RMA#) supplied to you 
by the technician, and your telephone number in case we need to call 
you. Any materials not containing this RMA# will be returned to you
unprocessed. Send your materials to the following address:

Infogrames, Inc.
Attn: TS/CS Dept.
13110 NE 177th Place
Suite # B101, Box 180
Woodinville, WA 98072-9965
RMA #:

Warranty Policy in the United States & Canada

If our technicians determine that the product storage medium is found 
to be defective within ninety (90) days of original purchase, (unless 
otherwise provided by applicable law), Infogrames, Inc. will replace 
the item free of charge, to the original purchaser, if the item is 
accompanied by the original dated receipt and packaging. If you
do not have the original receipt, or if the warranty period has 
expired, Infogrames, Inc. will replace the product storage medium for 
a nominal fee.

Strategy Guide Available:

There is an Official Strategy Guide available for this game. Please
refer to our Strategy Guide Availability List at www.ina-support.com, 
or find out how to place an order by telephone or mail after selecting 
the product Unreal Tournament 2003 from the automated phone system menu 
at 425-951-7106.

========================================================================
 10 Copyright Notice
========================================================================

Ogg Vorbis Copyright © 2001, Xiphophorus

MathEngine and Karma and the MathEngine and Karma logos are 
registered trade marks or trade marks of MathEngine PLC, used under 
license.

Unreal® Tournament 2003 ©2002 Epic Games, Inc. Raleigh, N.C. USA. 

Unreal and the Unreal logo are registered trademarks of Epic 
Games, Inc. ALL RIGHTS RESERVED. 

All other trademarks and trade names are the property of their 
respective owners. 

Unreal Tournament 2003 was created by Digital Extremes and Epic Games,
Inc.. 

Manufactured and marketed by Infogrames, Inc., New York, New York, 
a subsidiary of Infogrames Entertainment, S.A., under license from 
Epic Games, Inc.
