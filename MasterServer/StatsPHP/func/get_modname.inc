<?php
//---Getting the modname from a known mid---
/*
	External vars used:

	$mid		sent as argument

	$included_get_modname	global var, include control

	returns			modname
*/
$DEBUG = 0;
$included_get_modname = 1;	// file has been loaded once already

function get_modname($mid)
{
	global $db_lang;
	global $db_lid;

	$select		= "select modcode from mod where mid=$mid";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$modcode	= $row[0];
	$modname 	= translate($modcode,$db_lang[$db_lid]);

	return $modname;
}
?>
