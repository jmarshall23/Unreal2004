<?php
//---Getting the modname from a known mid---
/*
	External vars used:

	$tm			current table matrix with data
	$row		number of row we want to enhance
	$enhance	columns we want to enhance in that row, e.g. make bold
	            if unset() then do full row, if empty do nothing. 
*/
$included_tablematrix_rowhighlight = 1;	// file has been loaded once already

function tablematrix_rowhighlight( &$tm, $row, $enhance )
{
	$DEBUG = 0;
	if( $DEBUG ){ print $row; print_r($enhance); }

	// Perform enhancing ?
	$enhancecols = sizeof( $enhance );

	if( !(isset($enhance)==FALSE) AND $enhancecols == 0 ) 
		return;										// empty array, do nothing

	$pre	= "<b>";
	$post	= "</b>";

	// Enhance table data...
	if( $enhancecols > 0 )							// only enhance if data available
	{
		For( $c=0; $c<$enhancecols; $c++ )
		{
			$datacol	= $enhance[$c];
			$pos 		= strpos($tm[$row][$datacol],$pre);
			if( !($pos === false) AND $pos == 0 )	// Skip already enhanced lines!	
				continue;	
			$tm[$row][$datacol] = $pre.$tm[$row][$datacol].$post;
		}
	}
	else											// $enhance - not set, do full row.
	{
		$beg = 0; 	$end = sizeof( $tm[0] );
		for( $j=$beg; $j<=$end; $j++ )
		{
			$pos = strpos($tm[$row][$j],$pre);
			if( !($pos === false) AND $pos == 0 )	// Skip already enhanced lines!	
				continue;	
			$tm[$row][$j] = $pre.$tm[$row][$j].$post;
			
		}	
	}
}	// end func

?>
