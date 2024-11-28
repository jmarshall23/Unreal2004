<?php
//---Summing up the data for one player!---
/*
	External vars used:

	$playerID_globalSummary		page var, >0 per player, else all

	(no data base language data need, language strings set as global vars)
*/
$DEBUG = 0;
$playerid	= $playerID_globalSummary;
$mid 		= $mid_playeridSummary;

if( $mid > 0 )
{
	$where = "and mid=$mid";	//per player per mod
	$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
	$title_pre .= get_modname($mid)."<br>";
}
else
{
	$where = "";				//per player only
	$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
}

//Initializing vars and defining table header
unset($tablematrix);					//since matrix is remembered, needs to be unset!
$entry			= 'playerIDSummary';
$table_title	= $title_pre." ".$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];
$table_rows		= $table_rows_l[$entry];
$db_item 		= array("kills",     			//0 in players
						"suicides",
						"deaths",
						"frags",
						"efficiency",			//4

						"pkills",
						"akills",
						"pdeaths",
						"adeaths",				//8 skip this

						"wsuicides",
						"esuicides",

						"events",
						"multikills",
						"sprees",
						"totalScore",			//14 in players

						"playerMinutes",		//15 in player
						"playerConnects",
						"gamesFinished",
						"place1st",
						"place2nd",
						"place3rd",
						"placeRest",

						"teamScore",			//22 is in players... exception
						"teamKills",
						"teamDeaths",
						"teamWins",
    					"teamLosses"
    					);

//Collecting data
unset($data); 							// was needed!
$j=-1;
for( $i=0; $i<sizeof($db_item); $i++ )
{
	for( $t=1; $t<=3; $t++ )
	{
		$data[$i][$t] = 0;
		if( $i==4)		//doing average avoids COUNT !, "efficiency",			//4
		{
			$select	= 	"select avg(".$db_item[$i].") from players,player ".
						"where players.pid=player.pid and playerid=$playerid and timeFrame=$t ".
						"and timeFrameNumber=$TFN[$t] $where ";
		}
		else if( $i<=14 OR $i==22 )
		{
			$select	= 	"select sum(".$db_item[$i].") from players,player ".
						"where players.pid=player.pid and playerid=$playerid and timeFrame=$t ".
						"and timeFrameNumber=$TFN[$t] $where ";
        }
		else
			$select	= 	"select sum(".$db_item[$i].") from player ".
						"where playerid=$playerid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ";

		$link	=	mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
		$row	= mysql_fetch_row( $link );

		if( $i==4 )	// Efficiency
		{
			if( $row[0]>0. )
			{
				$row[0] *= 100;
	            $row[0] = sprintf ("%.1f",$row[0]);
				$row[0] .= "%";

			}
			else
				$row[0] = "&nbsp;";
        }

		$data[$i][$t] = $row[0];
	}

	if( $i != 8 )	//Skipping adeaths
	{
		$j++;
		// $tablematrix[row][column]
		$tablematrix[$j+1][0] = $table_rows[$i];
		$tablematrix[$j+1][1] = $data[$i][1];	//Timeframe week
		$tablematrix[$j+1][2] = $data[$i][2];	//Timeframe month
		$tablematrix[$j+1][3] = $data[$i][3];	//Timeframe all
	}
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
