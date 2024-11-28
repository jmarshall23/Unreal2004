<?php
//---Putting together the Score Board for the current Match---

if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";
if( $included_get_teamgame == 0 )				include "func/get_teamgame.inc";
if( $included_get_minutesecond == 0 )			include "func/get_minutesecond.inc";

$DEBUG	= 0;
//$MODE	= $mode_playersRanked;
$matchid	= $matchID_Summary;
$mid 		= $modID_Summary;
$playerid	= $playerID_Summary;	//-1 here!

//Initializing vars and defining table header
unset($tablematrix);

// Getting teamgame
$teamgame		= get_teamgame($mid);

// Setting the table data
$entry 			= 'matchIDScoreBoard';
$table_title	= $table_title_l[$entry];
$table_total	= $table_total_l[$entry];	//** todo

if( $teamgame )
{
	$tablematrix[0] = $table_head_l2[$entry];
	$table_text		= $table_text_l2[$entry];
	$table_span		= array( 1, 11,0,0,0,0,0,0,0,0,0,0, 11,0,0,0,0,0,0,0,0,0,0 );
	$table_shift	= 11;
	$table_blocks	= $head_teams2;
	$teamNumber		= 2;
}
else
{
	$tablematrix[0] = $table_head_l[$entry];
	$table_text		= $table_text_l[$entry];
	$table_span		= array( 1, 11,0,0,0,0,0,0,0,0,0,0 );
	$table_shift	= 11;
	$table_blocks	= $head_teams;
	$teamNumber		= 1;
}

// Setting table data for the possible 2 teams
$maxcount=0;
for( $team=0; $team<$teamNumber; $team++ )
{
	if( $teamgame )		// CTF... a teamgame
	{
		$select	= 	"select score, kills, deaths, suicides, teamkills, rankteam, playerid, playerminutes, (rankend-rankbeg) as rankgain, rankend ".
					"from matchplayers where matchid=$matchid and teamid=$team ".
					"ORDER BY score DESC ";
	}
	else				// DM, DM other... not a teamgame
	{
		$select	= 	"select score, kills, deaths, suicides, rankELO, (rankend-rankbeg) as rankgain, playerminutes, events, playerid, rankend ". 
					"from matchplayers where matchid=$matchid and teamid=$team ".
					"ORDER BY score DESC ";
	}

	$link	= mysql_query( $select );
	$count	= mysql_num_rows( $link );
		if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";
	if( $count == 0 ) continue;						// No data skip below
	
	if( $count > $maxcount ) $maxcount=$count;		// Find last line in table, it is line maxcount...

	// Going through the sorted list of players for each team
	for( $j=0; $j<$count; $j++ )
	{
		$row 		= mysql_fetch_row( $link );

		if( $teamgame )		// CTF... a teamgame
		{
			$score		= sprintf("%d",$row[0]);
		    $kills		= $row[1];
		    $deaths		= $row[2];
		    $suicides	= $row[3];
		    $teamkills	= $row[4];
		    $rankteam	= sprintf("%.1f",$row[5]);
		    $playerid	= $row[6];
		    $minutes	= get_minutesecond($row[7]);
		    $rankgain	= sprintf("%.1f",$row[8]);	//no space!	if( $rankgain > 0 ) $rankgain = "+".$rankgain;
		    $rankend	= $row[9];					//Making sure that rank gain has valid end value!

			$fraction	= $kills + $deaths + $suicides + $teamkills;
			if( $fraction == 0 ) $fraction=1;		// Safe devision
			$efficiency = sprintf("%.1f",($kills / $fraction) * 100 ) ;

		    if( $rankend == -1 )
		    {	
				$minutes  = "&nbsp;";
		    	$rankgain = "&nbsp;";
			}		    	
		}
		else
		{
			$score		= sprintf("%d",$row[0]);
		    $kills		= $row[1];
		    $deaths		= $row[2];
		    $suicides	= $row[3];
		    $rankteam	= sprintf("%.1f",$row[4]);
		    $rankgain	= sprintf("%.1f",$row[5]);
			if( $rankgain > 0 ) $rankgain = "+".$rankgain;
		    $minutes	= get_minutesecond($row[6]);
		    $events		= $row[7];
		    $playerid	= $row[8];
		    $rankend	= $row[9];					// Making sure that rank gain has valid end value!

			$fraction	= $kills + $deaths + $suicides;
			if( $fraction == 0 ) $fraction=1;		// Safe devision
			$efficiency = sprintf("%.1f",($kills / $fraction) * 100 ) ;

		    if( $rankend == -1 )
		    {
				$minutes  = "&nbsp;";
		    	$rankgain = "&nbsp;";
		    }
		}

		$nickname	= get_nickname($playerid);

		$namelink 	= "<a href=\"matchID.php?matchID=$matchid&modID=$mid&playerID=$playerid\">$nickname</a>";
        $plidlink	= "<a href=\"matchID.php?matchID=$matchid&modID=$mid&playerID=$playerid\">$playerid</a>";

		$shift = $team * $table_shift;
		$tablematrix[$j+1][0] = $j+1;		// Do this 2 times to ensure that we get ALL numbers!

		if( $teamgame )
		{
			$tablematrix[$j+1][1+$shift] = $namelink;
			$tablematrix[$j+1][2+$shift] = $plidlink;
			$tablematrix[$j+1][3+$shift] = $score;
			$tablematrix[$j+1][4+$shift] = $efficiency;
			$tablematrix[$j+1][5+$shift] = $kills;
			$tablematrix[$j+1][6+$shift] = $deaths;
			$tablematrix[$j+1][7+$shift] = $suicides;
			$tablematrix[$j+1][8+$shift] = $teamkills;
			$tablematrix[$j+1][9+$shift] = $rankteam;
			$tablematrix[$j+1][10+$shift]= $rankgain;
			$tablematrix[$j+1][11+$shift]= $minutes;
		}
		else
		{
			$tablematrix[$j+1][1+$shift] = $namelink;
			$tablematrix[$j+1][2+$shift] = $plidlink;
			$tablematrix[$j+1][3+$shift] = $score;
			$tablematrix[$j+1][4+$shift] = $efficiency;
			$tablematrix[$j+1][5+$shift] = $kills;
			$tablematrix[$j+1][6+$shift] = $deaths;
			$tablematrix[$j+1][7+$shift] = $suicides;
			$tablematrix[$j+1][8+$shift] = $events;
			$tablematrix[$j+1][9+$shift] = $rankteam;
			$tablematrix[$j+1][10+$shift]= $rankgain;
			$tablematrix[$j+1][11+$shift]= $minutes;
		}
	}

}	// end $team loop


// Getting the TeamScores
if( $teamgame && $maxcount!=0 )				// CTF... a teamgame, and data IS available!
{
	for( $team=0; $team<$teamNumber; $team++ )
	{
		// Getting the TeamScore per Team, putting it into the header
		$select	= 	"select sum(teamscores) from matchteamscores where matchid=$matchid and teamid=$team ";
		$link	= mysql_query( $select );
			if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
		$row 	= mysql_fetch_row( $link );
        if( $row[0]==NULL ) $row[0]=0;		//%% If other team does not score, then the entry will not exits in the DB
		$teamscore[$team] = $row[0];
	}

	if( $teamscore[0] >  $teamscore[1] )
	{
		$table_blocks[0] .= " (".$teamscore[0]." - ".$head_winner[0].")";	// [0] = "Winner"
		$table_blocks[1] .= " (".$teamscore[1].")";
	}
    else if( $teamscore[0] <  $teamscore[1] )
    {
		$table_blocks[0] .= " (".$teamscore[0].")";
    	$table_blocks[1] .= " (".$teamscore[1]." - ".$head_winner[0].")";
    }
	else // == draw
    {
		$table_blocks[0] .= " (".$teamscore[0].")";
    	$table_blocks[1] .= " (".$teamscore[1].")";
    }
}

if( $maxcount!=0 )										// data is available, else skip!
{
	unset($percentcols);
	$team  = 1;
	$shift = $team * $table_shift;
	if( $teamgame )
	{
			$sumcols		= array( 3, 5, 6, 7, 8,		3+$shift, 5+$shift, 6+$shift, 7+$shift, 8+$shift );
			$highlight		= array( 3,					3+$shift );
	}
	else
	{
			$sumcols		= array( 3, 5, 6, 7, 8	);
			$highlight		= array( 3 );
	}
	
	// Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $maxcount, $sumcols, $percentcols, $table_total );
	
	// Enhancing certain columns, e.g. highlithing 'em.
	if( isset($sumcols) ) $lines=1;
	else $lines=0;
	tablematrix_colhighlight($tablematrix, 0, $maxcount+$lines, $highlight );
}

//-------------------------------------------------------------------------
// show matrix
$size='s';								//'s'mall or 'n'normal possible
$tablecontrol = array (
	"mode"			=> "t3",			//t3 3 timeFrames header
	"option"		=> "head",
	"tabletitle"	=> $table_title,	//externally set
	"tabletext"		=> $table_text,		//externally set
	"tablespan"		=> $table_span,		//externally set (for t3 only)
	"tableblocks"	=> $table_blocks,	//externally set
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
