<?php
//---Generate an Info Box---
/*
	External vars used:

	$head_title		used as index box title as well

	$indent			global vars: offsets, spacers etc.
	$spf
	$spt
	$info_width

	$info_text		info box infos set by each page
*/

if( strlen($info_text) )
{
?>

<TABLE WIDTH=695 BORDER=0 CELLPADDING=0 CELLSPACING=0>
<TR>
<TD><?print $indent?></TD>
<TD><?print $spf?></TD>
<TD><IMG SRC="pics/a_button.gif" WIDTH=61 HEIGHT=43></TD>
<TD>
	<TABLE WIDTH=634 BORDER=0 CELLPADDING=0 CELLSPACING=0>
	<TR><TD><IMG SRC="pics/a_top.gif" WIDTH=634 HEIGHT=12></TD></TR>
	<TR><TD background="pics/a_mid.gif">
		<TABLE WIDTH=<?print $info_width?> ALIGN=center BORDER=0 CELLPADDING=0 CELLSPACING=0>
		<TR><TD class=info><?print $info_text?></TD></TR>
		</TABLE>
	</TD></TR>
	<TR><TD><IMG SRC="pics/a_bot.gif" WIDTH=634 HEIGHT=13></TD></TR>
	</TABLE>
</TD>
</TR>
</TABLE>

<?
}
?>
