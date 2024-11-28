<?php
//---Getting the serverid from a known svid---
/*
	External vars used:

	$playerid				sent as argument

	$included_check_playerid	global var, include control

	returns					returns 0,1 depending on if the playerid exists or does not
*/
$included_check_playerid = 1;		// file has been loaded once already

function check_playerid($playerid)
{
	$DEBUG = 0;
	$exists = 0;
	if( !isset($playerid) OR $playerid=="" ) return $exists;
	$select		= "select statsusername from playerid where playerid=\"".AddSlashes($playerid)."\"";
	$link		= mysql_query( $select );
	$count 		= mysql_num_rows($link);
				if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";
   	if( $count != 0 ) $exists = 1;
   	else			  $exists = 0;
	return $exists;
}
?>
