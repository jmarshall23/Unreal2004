<?php
//---Generate the Header for each page---
// Logo line, navigation line, location/quick-search line, "spacer" line
/*
	External vars used:
	$spn			global var, used my main navigation links
	$spl			global var, used by location sting
	$sps			global var, used by mini search form
	$show_search_X	global var, sets number of entries search will show

	$head_title		set for each page
	$head_where		needs to be set for each page... use $head_nav_names[] ?

	../ut2003stats.css	style sheet used
*/

$head_where	 =
	"<span class=orange>".$head_where."</span>";	//###colors just for test

$head_navigation =
	$spn."<a href=\"main.php\">$head_nav_names[0]</a>".
	$spn."<a href=\"global.html\">$head_nav_names[1]</a>".
	$spn."<a href=\"matches.html\">$head_nav_names[3]</a>".
	$spn."<a href=\"players.html\">$head_nav_names[2]</a>".
	$spn."<a href=\"compare.php\">$head_nav_names[18]</a>".
	$spn."<a href=\"bestof.html\">$head_nav_names[4]</a>".
	$spn."<a href=\"mods.html\">$head_nav_names[5]</a>".
	$spn."<a href=\"maps.html\">$head_nav_names[6]</a>".
	$spn."<a href=\"servers.html\">$head_nav_names[7]</a>".
	$spn."<a href=\"sitemap.php\">$head_nav_names[8]</a>".
	$spn."<a href=\"faq.php\">$head_nav_names[9]</a>".
	$spn;

$head_search =
	"<input type=text size=16 name=query>".$sps.
	"<select name=querytype>".
	"<option value=nick>$head_search_names[0]".
	"<option value=playerID>$head_search_names[1]".
	"<option value=matchID>$head_search_names[2]".
	"<option value=server>$head_search_names[3]".
	"<option value=serverID>$head_search_names[4]".
	"<option value=map>$head_search_names[5]".
	"<option value=mod>$head_search_names[6]".
	"</select>".$sps.
	"<input type=submit name=searchbutton value=\"$head_search_button[0]\">".$sps
?>
<HTML>
<HEAD>
<TITLE>Unreal Tournament 2003 stats - <? print $head_title?></TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-1">
<LINK REL=stylesheet type="text/css" href="ut2003stats.css">
<SCRIPT LANGUAGE="JavaScript">
<!--
function nI(arg) {
	if (document.images) {
		rslt = new Image();
		rslt.src = arg;
		return rslt;
	}
}
function cI(arg1, arg2) { document[arg1].src = arg2; }
var preloadFlag = false;
function preloadImages() {
	if (document.images) {
		h_info2 = nI("pics/h_info2.jpg");
		preloadFlag = true;
	}
}
// -->
</SCRIPT>
</HEAD>
<BODY BGCOLOR=#243954 TEXT=#FFFFFF LINK=#FFCC66 VLINK=#FFCC66 alink=#FFCC66
      topmargin=0 leftmargin=0 marginheight=0 marginwidth=0>


<!-- Logo line -->
<TABLE WIDTH=100% BORDER=0 CELLPADDING=0 CELLSPACING=0 background="pics/h_fill.gif">
<TR>
<TD		><a href="main.php"><IMG SRC="pics/h_logo.jpg" WIDTH=160 HEIGHT=60 border=0></a></TD>
<TD align=center><a href="main.php"	ONMOUSEOVER="cI('h_info', 'pics/h_info2.jpg'); return true;" 
ONMOUSEOUT="cI('h_info', 'pics/h_info.jpg'); return true;">
	<IMG NAME="h_info" SRC="pics/h_info.jpg" WIDTH=354 HEIGHT=60 border=0></a></TD>
<TD align=right	><a href="main.php"><IMG SRC="pics/h_rgfx.jpg" WIDTH=186 HEIGHT=60 border=0></a></TD>
</TR>
</TABLE>

<!-- Navigation, Location & Search lines -->
<TABLE WIDTH=100% HEIGHT=48 BORDER=0 CELLPADDING=0 CELLSPACING=0 background="pics/h_navseach.gif">
<TR>
<TD COLSPAN=2 valign=top align=center nowrap><span class=navigation><?print $head_navigation?></span></TD>
</TR>
<TR>
<TD vcenter=top nowrap><span class=location><?print $spl.$head_location." ".$head_where?></span></TD>
<FORM METHOD=POST ACTION="search.php?show=<?print $show_search_X?>">
<TD vcenter=top align=right nowrap><?print $head_search?></TD>
</FORM>
</TR>
</TABLE>

<!-- Visual Spacer line -->
<TABLE WIDTH=100% BORDER=0 CELLPADDING=0 CELLSPACING=0>
<TR><TD COLSPAN=3><IMG SRC="pics/h_space.gif" WIDTH=100% HEIGHT=20></TD></TR>
</TABLE>
