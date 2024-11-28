<?php
//---Getting the modname from a known mid---
/*
	External vars used:

	$tm			current table matrix with data
	$beg		first line we want to sum up
	$end		last line we want to sum up
	$sum		columns we want sum up, with total at bottom of table
	$percent	array with number pairs, data column/percent column
	$sumtext	summary line text

	Independantly define what columns get summed up and/or get percent rows as well.
	$sum		= array( 2, 3,		6	);
	$percent	= array( 3,4,	6,7	);
*/
$included_tablematrix_sumpercent = 1;				// file has been loaded once already

function tablematrix_sumpercent( &$tm, $beg, $end, $sum, $percent, $sumtext )
{
	$DEBUG = 0;
	if( $DEBUG ){ print print_r($tm); print_r($sum); print_r($percent);	}

	// If there is no data do not perform below
	if( $end == 0 ) return;

	// Summary line number
	$sumlineno	= $end+1;							// by default appending to bottom data row

	// Perform summing ?
	$sumcols = sizeof( $sum );

	// Perform summing ?
	$percentcols = sizeof( $percent ) / 2;			// data columns only counted, should be an even number

	// Sums... calculate those 1st...
	if( $sumcols > 0 )								// only do sums if data available
	{
		// Totals
		for( $c=0; $c<$sumcols; $c++ )				// Columns to sum up	$sum = array( 1, 3, 1+$shift, 3+$shift );
		{
			$datacol	= $sum[$c];
			$total[$datacol]	= 0;
			for( $j=$beg; $j<=$end; $j++ )			// Rows to sum up
				$total[$datacol] += $tm[$j][$datacol];	// Calculating total per requested column

			// Putting totals into bottom most data row
			$tm[$sumlineno][$datacol] = $total[$datacol];	// Calculating total per requested column
		}

		// Add a title to the sum line ?
		if( strlen($sumtext) > 0 )
			$tm[$sumlineno][0] =  $sumtext;
	}

	// Percent columns... calculate those...
	if( $percentcols > 0 )							// only do sums if data available
	{
		for( $c=0; $c<$percentcols; $c++ )			// Columns to sum up	$sum = array( 1, 3, 1+$shift, 3+$shift );
		{
			$datacol	= $percent[$c*2];
			$perccol	= $percent[$c*2+1];
			$ctotal		= $total[$datacol];

			// Calculating % values
			for( $j=$beg; $j<=$end; $j++ )			// Rows to calc percent values for
			{
				$kp = 0;
				if( $ctotal > 0 )
			 		$kp = $tm[$j][$datacol]/$ctotal*100;
		
				if( $tm[$j][$datacol]!="" )		// If the column is empty don't show % values!
					$tm[$j][$perccol] = sprintf("%.1f",$kp);
			}
		}
	}

}	// end func

?>
