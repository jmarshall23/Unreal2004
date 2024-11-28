<?php
//  timeFrameNumbers
// ------------------
//
// gets the current timeFrameNumbers from db
// updated the global var $TFN[]
	$DEBUG = 0;
	$TFN	= array( 1,1,1,1 );				//init elements 0,1-3

	for( $t=1; $t<=3; $t++ )
	{
		$select	=	"select timeframe, currentTF from timeframe ".
					"where timeframe=$t order by currentTF desc limit 1 ";
		$link	= mysql_query( $select );
		$count	= mysql_num_rows( $link );
		if( $DEBUG ) print "Found ".$count." row(s) for \"$select\"<br>\n";

		if( $count==1)
		{
		   	$row = mysql_fetch_row( $link );
	        $TFN[$t] = $row[1];
		}
	}
?>