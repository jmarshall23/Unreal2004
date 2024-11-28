<?php
//---Generate default Index and Intro Boxes---
/*
	$global_page =	'main'	is a bit different than other subpages (1 pic box)

	External vars used:

	$head_title		used as index box title as well

	$indent			global vars: offsets, spacers etc.
	$spi
	$spt
	$spr
	$spb
	$index_width
	$intro_width

	$index_links		index box infos set by each page

	$intro_title		intro box infos set by each page
	$intro_intro
	$intro_text
	$intro_logo		pic used as logo (i_logo.gif)
	$intro_button		button pic for intro box (b_topright.gif)
*/
?>

<table width=760 border=0 cellpadding=0 cellspacing=0>
<tr>
<td><?print $indent?><td>
<td valign=top>
	<TABLE WIDTH=224 BORDER=0 CELLPADDING=0 CELLSPACING=0>
	<TR>
	<TD HEIGHT=45 background="pics/q_top.gif" valign=top nowrap><span class=ptitle><?print $spi.$head_title?></span></TD>
	</TR>
	<TR><TD background="pics/q_mid.gif">
		<TABLE WIDTH=<?print $index_width?> ALIGN=center BORDER=0 CELLPADDING=0 CELLSPACING=0>
		<TR><TD class=index><?print $index_links?></TD></TR>
		</TABLE>
	</TD></TR>
	<TR>
	<TD><IMG SRC="pics/q_bot.gif" WIDTH=224 HEIGHT=12></TD>
	</TR>
	</TABLE>
</td>
<td><?print $spt?><td>
<? if($global_page != 'main') { ?>
<td valign=top>
	<TABLE WIDTH=456 BORDER=0 CELLPADDING=0 CELLSPACING=0>
	<TR>
	<TD WIDTH=154><IMG SRC="pics/<?print $intro_logo?>" WIDTH=154 HEIGHT=99></TD>
	<TD><?print $spr?></TD>
	<TD WIDTH=292 valign=top nowrap><span class=ptitle><?print $intro_title.$spb?>
	<IMG SRC="pics/<?print $intro_button?>" WIDTH=41 HEIGHT=41><br></span>
	<span class=intro><?print $intro_intro?></span>
	</TD>
	</TR>
	<TR><TD COLSPAN=3><IMG SRC="pics/i_top.gif" WIDTH=456 HEIGHT=12></TD></TR>
	<TR>
	<TD COLSPAN=3 background="pics/i_mid.gif">
		<TABLE WIDTH=<?print $intro_width?> ALIGN=center BORDER=0 CELLPADDING=0 CELLSPACING=0>
		<TR><TD class=intro><?print $intro_text?></TD></TR>
		</TABLE>
	</TD></TR>
	<TR><TD COLSPAN=3><IMG SRC="pics/i_bot.gif" WIDTH=456 HEIGHT=12></TD></TR>
	</TABLE>
</td>
<td><?print $indent?><td>
<? } else { ?>
<td valign=top>
	<TABLE WIDTH=464 HEIGHT=208 BORDER=0 CELLPADDING=0 CELLSPACING=0 BACKGROUND="pics/i_main_box.jpg">
	<TR><TD COLSPAN=2 valign=top><img src="pics/t.gif" height=10 width=20></TD></TR>
	<TR><TD COLSPAN=2 valign=top><span class=mtitle><?print $intro_title?></span></TD></TR>
	<TR><TD COLSPAN=2 valign=top><img src="pics/t.gif" height=80 width=20></TD></TR>
	<TR>
	<TD WIDTH=14 valign=top><img src=pics/t.gif height=10 width=14></TD>
	<TD valign=top>
		<table width=<?print $intro_width2?> border=0 cellpadding=0 cellspacing=0>
		<tr><td valign=top class=intro_main><?print $intro_text?></td></tr>
		</table>
	</TD>
	</TR>
	</TABLE>
</td>
<td width=23><img src=pics/t.gif height=30 width=23><td>
<? } ?>
</tr>
</table>
