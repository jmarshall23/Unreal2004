<?php
//---Getting the serverid from a known svid---
/*
	External vars used:

	$head_gametypes			array that will contain all the gametypes by name
	$head_mids				array that will contain all the gametypes by mod id
	$lang					$db_lang[$db_lid]

	$included_get_gametypes	global var, include control

	returns					$head_gametypes, $head_mids
*/
$included_get_gametypes = 1;		// file has been loaded once already

function get_gametypes(&$head_gametypes, &$head_mids, $lang )
{
	$DEBUG = 0;
	$select		= "select mid, modcode from mod";
	$link		= mysql_query( $select );
	$count		= mysql_num_rows($link);
				if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

	for( $i=0; $i < $count; $i++ )
	{
		$row		= mysql_fetch_row( $link );
		$mid		= $row[0];
		$modcode	= $row[1];

		// Skipping entry, special case, allmods
		$head_mids[$i+1] 		= $mid;
   		$modname				= translate($modcode,$lang);

		$head_gametypes[$i+1]	= htmlspecialchars($modname);
		if( $DEBUG ) print "modname: ".$modname."  modcode: ".$modcode."<br>";
	}
	return;
}
?>
