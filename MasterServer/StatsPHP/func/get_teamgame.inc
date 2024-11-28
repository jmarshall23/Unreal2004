<?php
//---Getting the teamgame flag from a known $mid---
/*
	External vars used:

	$mid					sent as argument

	$included_get_teamgame	global var, include control

	returns					teamgame
*/
$included_get_teamgame = 1;		// file has been loaded once already

function get_teamgame($mid)
{
	$DEBUG = 0;
	$select		= "select teamgame from mod where mid=$mid";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$teamgame	= $row[0];
	return $teamgame;
}
?>
