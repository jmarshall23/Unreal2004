<?php
//---Getting the mapname from a known mapid---
/*
	External vars used:

	$date					returns data of server start

	$included_get_fragsdate	global var, include control

	returns					total number of frags
*/
$DEBUG = 0;
$included_get_fragsdate = 1;		// file has been loaded once already

function get_fragsdate(&$date)
{
	$date="";						// Empty by default

	// timeFrame = 3  is alltime!
	$select		= "select kills-suicides as frags from global where timeFrame=3";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$frags		= $row[0];

	$select		= "select DATE_FORMAT(dateOfReset, '%M %D %Y') from timeframe where timeFrame=3";
	$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$date		= $row[0];

	return $frags;
}
?>
