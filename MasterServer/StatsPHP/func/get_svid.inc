<?php
//---Getting the serverid from a known svid---
/*
	External vars used:

	$svid					sent as argument

	$included_get_serverid	global var, include control

	returns					serverid
*/
$DEBUG = 0;
$included_get_svid = 1;		// file has been loaded once already

function get_svid($serverID)
{
	$select		= "select svid from server where serverID=$serverID";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$svid	= $row[0];
	return $svid;
}
?>
