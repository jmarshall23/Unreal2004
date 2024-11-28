<?php
//---Summing up the data for all or one player!---
/*
	External vars used:

	$playerID_globalSummary		page var, >0 per player, else all

	(no data base language data need, language strings set as global vars)
*/
$DEBUG = 0;

//Initializing vars and defining table header
unset($tablematrix);					//since matrix is remembered, needs to be unset!
$table_title	= $table_title_l['globalSummary'];
$table_text		= $table_text_l['globalSummary'];
$tablematrix[0] = $table_head_l['globalSummary'];
$table_rows		= $table_rows_l['globalSummary'];
$db_item 		= array(	"kills",
							"suicides",
							"frags",     		//2
							"teamKills",
							"efficiency",		//4
							"fragsPerHour",
							"players",
							"playerMinutes",
							"playerConnects",
							"totalScore",
							"playersPerGame",	//10
							"gamesPlayed",
							"gamesFinished",
							"gamesPerDay",
							"maps",
							"mapChanges",
							"events",
							"mutators",
							"mods",
							"servers",
							"serverConnects",
							"serverTime"
							);

//Collecting data
for( $i=0; $i<sizeof($db_item); $i++ )
{
	for( $t=1; $t<=3; $t++ )
	{
		$data[$i][$t] = 0;
		if( $i!=2 )	// NOT frags
		{
			$select	= "select $db_item[$i] from global where timeFrame=$t and timeFrameNumber=$TFN[$t] ";
			$link	= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
			$row = mysql_fetch_row( $link );

			if( $i==4 )		$row[0] = sprintf("%d%%",$row[0]*100);		// Efficiency
			if( $i==10 )	$row[0] = sprintf("%d",$row[0]);			// playersPerGame INT!

			$data[$i][$t]	= $row[0];
		}else
			$data[$i][$t]	= $data[0][$t]-$data[1][$t];	//frags = kills - suicides
	}

	// $tablematrix[row][column]
	$tablematrix[$i+1][0] = $table_rows[$i];
	$tablematrix[$i+1][1] = $data[$i][1];	//Timeframe week
	$tablematrix[$i+1][2] = $data[$i][2];	//Timeframe month
	$tablematrix[$i+1][3] = $data[$i][3];	//Timeframe all
}


//-------------------------------------------------------------------------
// show matrix
$size='s';						//'s'mall or 'n'normal possible
$tablecontrol = array (
	"mode"			=> "t",
	"option"		=> "head",
	"tabletitle"	=> $table_title,	//externally set
	"tabletext"		=> $table_text,		//externally set
	"indent"		=> $indent,			//externally set
	"ind_tb"		=> $ind_tb,			//externally set

	"showzero"		=> 1,				//0 shows &nbsp;
	"headclass"		=> $size."hbg",
	"colheadclass"	=> $size."tbg",
	"colclass"		=> $size,
	"colaltclass"	=> $size."abg",		//small shbg, stbg, s, sabg
	"headalign"		=> "ALIGN=center",	//"" is ALIGN=LEFT
	"columnalign"	=> "ALIGN=right",
	"counteralign"	=> "ALIGN=right"
);
table($tablecontrol,$tablematrix);
//-------------------------------------------------------------------------
?>
