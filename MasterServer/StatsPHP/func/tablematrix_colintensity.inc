<?php
//---Getting the modname from a known mid---
/*
	External vars used:

	$tm			current table matrix with data
	$beg		first line we want to check for intensity
	$end		last line we want to check for intensity
	$intensity	columns we want do intensity checking on
*/
$included_tablematrix_colintensity = 1;	// file has been loaded once already

function tablematrix_colintensity( &$tm, $beg, $end, $intensity )
{
	$DEBUG = 0;
	if( $DEBUG ){ print $beg." ".$end; print_r($tm); print_r($intensity); 	}

	// If there is no data do not perform below
	if( $end == 0 ) return;		// was $end-1

	// Perform intensity checking?
	$intensitycols = sizeof( $intensity );
    if( !($intensitycols > 0) ) return;

	// Enhance table data...
	for( $c=0; $c<$intensitycols; $c++ )
	{
		$column = $intensity[$c];

		// Copy data into an array
		for( $j=$beg; $j<=$end; $j++ )
			$data[$j] = $tm[$j][$column];
	     
     	// Find Max and Min values in that array.
        $max = max($data);
        $min = min($data);
        $range = $max-$min;
		if( $range == 0 ) $range = 1;

		for( $j=$beg; $j<=$end; $j++ )
		{
    		$value 		= $tm[$j][$column];	
            $i			= round( (9-(($value-$min)/$range)*9), 0 );		// 0 seems to be INT

			$pre	= "<span class=i$i>";
			$post	= "</span>";			

			$tm[$j][$column] = $pre.$tm[$j][$column].$post;
		}
	}	// end - enhancecols

}	// end func
?>
