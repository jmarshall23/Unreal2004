<?php
//---Getting the modname from a known mid---
/*
	External vars used:

	$tm			current table matrix with data
	$beg		first line we want to sum up
	$end		last line we want to sum up
	$enhance	columns we want to enhance, e.g. make bold
*/
$included_tablematrix_colhighlight = 1;	// file has been loaded once already

function tablematrix_colhighlight( &$tm, $beg, $end, $enhance )
{
	$DEBUG = 0;
	if( $DEBUG ){ print $beg." ".$end; print_r($tm); print_r($enhance); }

	// If there is no data do not perform below
	if( $end == 0 ) return;		// was $end-1

	$pre	= "<b>";
	$post	= "</b>";

	// Perform enhancing ?
	$enhancecols = sizeof( $enhance );

	// Enhance table data...
	if( $enhancecols > 0 )						// only enhance if data available
	{
		for( $c=0; $c<$enhancecols; $c++ )
		{
			$datacol	= $enhance[$c];
			for( $j=$beg; $j<=$end; $j++ )
				$tm[$j][$datacol] = $pre.$tm[$j][$datacol].$post;
		}
	}
}	// end func

?>
