<?php
//---Convering digital minutes into minute:seconds---
/*
	External vars used:

	$minutes					7.41 minutes, a float

	$included_get_minutesecond	global var, include control

	returns			   			 7:25 minute:seconds string
*/
$DEBUG = 0;
$included_get_minutesecond = 1;		// file has been loaded once already

function get_minutesecond($minutes)
{
	$minute		= sprintf("%d",$minutes);
	$seconds	= ($minutes - $minute)*60;

	if( $seconds<10 )	$zero = "0";
	else				$zero = "";

	$minutesecond = sprintf("%d:%s%d", $minute, $zero, round($seconds) );

	return $minutesecond;
}
?>
