<?php
//---Generate the Footer for each page---
// Copyright etc info, close HTML page

//External vars used:

//$page_last_update		global var, shows when last the stats where updated. ###
?>

<pre>

</pre>
<TABLE WIDTH=100% BORDER=0 CELLPADDING=0 CELLSPACING=0>
<TR>
<TD align=center><IMG SRC="pics/f_logo.gif" WIDTH=354 HEIGHT=27></TD>
</TR>
<TR>
<TD background="pics/f_map.gif" HEIGHT=30><IMG SRC="pics/t.gif" HEIGHT=30></TD>
</TR>
<TR>
<TD ALIGN=CENTER nowrap class=index>
<?print $foot_gen." "?>
<span class=blue>UT2003stats</span> (C) 2002
<?print $foot_by." "?>
<a href="http://www.epicgames.com" target=_new>Epic Games</a>.<br>
<?print $page_last_update;?><br>
<?php
//	if( $included_get_nickname == 0 )	include "func/get_fragsdate.inc";
//	$frags = get_fragsdate($date);
//	print "<span class=blue>".$frags."</span> ".$foot_frags." ".$date."<br>";
?>
<?print $foot_design." "?>
<a href="http://www.2design.org" target=_new>|2|</a><p>
</TD>
</TR>
</TABLE>
</BODY>
</HTML>