<?php
//---Getting the players nickname from playerid table---
/*
	External vars used:

	$playerID				sent as argument

	$included_get_nickname	global var, include control

	returns					nickname
*/
$DEBUG = 0;
$included_get_nickname = 1;	// file has been loaded once already

function get_nickname($playerID)
{
	$select		= "select nickname from playerid where playerID=$playerID";
	$link = mysql_query( $select );
		if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row = mysql_fetch_row( $link );
	$nickname = $row[0];	//Avoid HTML Messups

    if( strlen($nickname)>20 ) 
		$nickname = substr($nickname, 0, 17)."..."; // substr("abcdef", 0, 4);  returns "abcd"
    
	return htmlspecialchars($nickname);
}
?>
