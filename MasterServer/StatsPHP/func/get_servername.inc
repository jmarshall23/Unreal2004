<?php
//---Getting the servername from a known svid---
/*
	External vars used:

	$svid						sent as argument

	$included_get_servername	global var, include control

	returns						servername
*/
$DEBUG = 0;
$included_get_servername = 1;		// file has been loaded once already

function get_servername($svid)
{
	$select		= "select serverName from server where svid=$svid";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$servername	= $row[0];

	if( strlen($servername)>20 ) 
			$servername = substr($servername, 0, 17)."...";

	return htmlspecialchars($servername);
}
?>
