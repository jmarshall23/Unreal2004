<?php
//---Getting the mapname from a known mapid---
/*
	External vars used:

	$mapid					sent as argument

	$included_get_mapname	global var, include control

	returns					mapname
*/
$DEBUG = 0;
$included_get_mapname = 1;		// file has been loaded once already

function get_mapname($mapid)
{
	$select		= "select mapname from map where mapid=$mapid";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$mapname	= $row[0];

	if( strlen($mapname)>20 ) 
			$mapname = substr($mapname, 0, 17)."...";

	return htmlspecialchars($mapname);
}
?>
