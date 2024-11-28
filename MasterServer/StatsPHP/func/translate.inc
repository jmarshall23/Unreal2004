<?php
//---Translate a code string into current language string thereof---
// $code		unique string in translations table, that is to be translated
//				under php this is NOT case dependant!
// $landid		the language tag for this site
//				e.g. en for english (global $db_lang[$db_lid], in most cases)
function translate($code,$langid)
{
	$DEBUG = 0;

	$select	= "select translation from translations where code='$code' and language='$langid'";
	$link	= mysql_query( $select );
	$count	= mysql_num_rows($link);
		if( $DEBUG ) print "Found ".$count." row(s) for \"$select\"<br>\n";

	if( $count>=1 )
	{
	   	$row = mysql_fetch_row( $link );
		return htmlspecialchars($row[0]);
	}
	else
		return $code;		// Failed translation, return junk input! :)
}
?>