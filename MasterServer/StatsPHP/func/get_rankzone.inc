<?php
//---For a pos in loop, find the previous and post entries, e.g. intelligent rank zone window---
/*
	External vars used:

	$beg					returning zone start
	$end					returning zone end
	$bot					lower limit of data window
	$pos					center of zone, e.g. focus of "search"
							since this can change, returning the new $pos
	$top					upper limit of data window
	$rankzone 				+- window around the

	$included_get_rankzone	global var, include control

	returns					$beg, $end
*/
$included_get_rankzone = 1;		// file has been loaded once already

function get_rankzone(&$beg, &$end, $bot, &$pos, $top, $rankzone)
{
	$DEBUG = 0;
	$spanzone = $rankzone*2 + 1; 			// Full window, e.g. max amount of entries we want to show

	if ( $pos - $rankzone >= $bot )
	{
        $beg = $pos - $rankzone;
		if ( $beg + $spanzone <= $top )
	        $end = $beg + $spanzone;
		else
		{
			$pos = $pos - ($beg + $spanzone - $top);
	        $end = $top;					// Hit end try to get 11 entries anyway
			if ( $end - $spanzone >= $bot )
		        $beg = $end - $spanzone;	// Player not in center
		    else     
		        $beg = $bot;
		}		        
	}	        
    else
    {     
		$pos = $rankzone + $bot;
        $beg = $bot;						// Hit top, player is number 1
		if ( $beg + $spanzone <= $top )
	        $end = $beg + $spanzone;
		else
	        $end = $top;
    }

	if( $DEBUG ) print "playeratpos".$pos." top".$top." spanzone".$spanzone." beg".$beg." end".$end."<br>";
	return 0;
}
?>
