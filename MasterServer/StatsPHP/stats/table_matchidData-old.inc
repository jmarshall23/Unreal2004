<?php
//---Showing the sums of match related data, also used for player data---
/*
	External vars used:

*/
$DEBUG		= 1;
$MODE		= $mode_matchIDdata;
$matchid	= $matchID_Summary;
$mid 		= $modID_Summary;
$playerid	= $playerID_Summary;	//-1 here!

// ** to fix
// Setting some where statements
/*
if( $playerid > 0 )
{
	$where = "and playerid=$playerid ";	//per Player
	if( $mid > 0 )	//per player per mod
	{
		$where = $where."and mid=$mid";
		$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
		$title_pre .= get_modname($mid)."<br>";
	}
	else			//per player only
		$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
}
else
{
		$where = "";						//all Players (also unset cases!)
		$title_pre = "";
}
*/
$where = "";						//all Players (also unset cases!)
$title_pre = "";



//Initializing vars and defining table header
unset($tablematrix);

// Getting teamgame
$select		= "select teamgame from mod where mid=$mid";
$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
$row		= mysql_fetch_row( $link );
$teamgame	= $row[0];


// The Specific Table info tag names
if ($MODE == 0) $entry = 'matchidData_KD';
if ($MODE == 1)	$entry = 'matchidData_S';
if ($MODE == 2)	$entry = 'matchidData_E';
if ($MODE == 3)	$entry = 'matchidData_Sc';
if ($MODE == 4)	$entry = 'matchidData_Mu';


// Setting the table data
$table_title	= $table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$table_total	= $table_total_l[$entry];	//** todo

if( $teamgame )
{
	if ($MODE == 0)		//Frags
	{
		$table_span		= array( 1, 4,0,0,0, 4,0,0,0 );
		$table_shift	= 4;
    }
    else				// Suis, Events, Scores, Muatators
    {
		$table_span		= array( 1, 2,0, 2,0 );
		$table_shift	= 2;
    }
	$tablematrix[0] = $table_head_l2[$entry];
	$table_blocks	= $head_teams2;
	$teamNumber		= 2;
}
else
{
	if ($MODE == 0)		//Frags
	{
		$table_span		= array( 1, 4,0,0,0 );
		$table_shift	= 4;
    }
    else
    {
		$table_span		= array( 1, 2,0 );
		$table_shift	= 2;
    }
	$tablematrix[0] = $table_head_l[$entry];
	$table_blocks	= $head_teams;
	$teamNumber		= 1;
}


// Setting table data for the possible 2 teams
$maxcount=0;
for( $team=0; $team<$teamNumber; $team++ )
{
	if( $MODE == 0 )		// Per Weapon Sums
		$select	= 	"select wid, sum(matchweapons.kills) as ksum, sum(matchweapons.deaths) as dsum from matchweapons, matchplayers ".
					"where matchweapons.mpid=matchplayers.mpid and matchid=$matchid and teamid=$team $where ".
					"group by wid having ksum>0 or dsum>0 ";
	if( $MODE == 1 )		// Per DamageType Suicides
		$select	= 	"select sid, sum(matchsuicides.suicides) as ssum from matchsuicides, matchplayers ".
					"where matchsuicides.mpid=matchplayers.mpid and matchid=$matchid and teamid=$team $where ".
					"group by sid having ssum>0 ";
	if( $MODE == 2 )		// Per Event type
		$select	= 	"select eid, sum(matchevents.events) as esum from matchevents, matchplayers ".
					"where matchevents.mpid=matchplayers.mpid and matchid=$matchid and teamid=$team $where ".
					"group by eid having esum>0 ";
	if( $MODE == 3 )		// Per Score type
		$select	= 	"select scid, sum(matchscores.scores) as scsum from matchscores, matchplayers ".
					"where matchscores.mpid=matchplayers.mpid and matchid=$matchid and teamid=$team $where ".
					"group by scid having scsum>0 ";
	if( $MODE == 4 )		// Per mutator name
		$select	= 	"select mutid, mutators from matchmutators ".
					"where matchid=$matchid ".
					"group by mutid having mutators>0 ";

	$link	= mysql_query( $select );
	$count	= mysql_num_rows( $link );
		if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

	if( $count > $maxcount) $maxcount=$count;	//Find last line in table, it is line maxcount...

	// Going through the sorted list of players for each team
	$total[$team][0] = 0;	$total[$team][1] = 0;

	for( $j=0; $j<$count; $j++ )
	{
		$row 		= mysql_fetch_row( $link );

		if( $MODE == 0 )
		{
	    	$id			= $row[0];
		    $skills		= $row[1];
		    $sdeaths	= $row[2];
		}
		else
		{
	    	$id			= $row[0];
		    $sdata		= $row[1];		//suis, events, scores, mutators
		}

		//Looking up readable names
		if( $MODE == 0 ) $select = "select weaponcode from weapon where wid=$id";
		if( $MODE == 1 ) $select = "select suicidecode from suicide where sid=$id";
		if( $MODE == 2 ) $select = "select eventcode from event where eid=$id";
		if( $MODE == 3 ) $select = "select scorecode from score where scid=$id";
		if( $MODE == 4 ) $select = "select mutatorcode from mutator where mutid=$id";
		$link2	= mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select\"<br>\n";
		$row2	= mysql_fetch_row( $link2 );
		$name	= translate($row2[0],$db_lang[$db_lid]);


		// Filling table with data...
		$shift = $team * $table_shift;
		if( $MODE == 0 )
		{
			if( $team==0 )
				$tablematrix[$j+1][0+$shift] = $name;
				$tablematrix[$j+1][1+$shift] = $skills;
				$tablematrix[$j+1][2+$shift] = '%';
				$tablematrix[$j+1][3+$shift] = $sdeaths;
				$tablematrix[$j+1][4+$shift] = '%';

			$total[$team][0] += $tablematrix[$j+1][1+$shift];
			$total[$team][1] += $tablematrix[$j+1][3+$shift];
		}
		else
		{
			if( $team==0 )
				$tablematrix[$j+1][0+$shift] = $name;
				$tablematrix[$j+1][1+$shift] = $sdata;
				$tablematrix[$j+1][2+$shift] = '%';

			$total[$team][0] += $tablematrix[$j+1][1+$shift];
		}
	}  // end count loop

	// Calculating % values
	for( $j=0; $j<$count; $j++ )
	{
		if ($MODE == 0)
		{
			$kp = 0;	$dp = 0;								//Avoiding devision by zero
			if( $ktotal > 0 )
		 		$kp = $tablematrix[$j+1][1+$shift]/$total[$team][0]*100;
			if( $dtotal > 0 )
			 	$dp = $tablematrix[$j+1][3+$shift]/$total[$team][1]*100;
			$tablematrix[$j+1][2+$shift] = sprintf("%.2f",$kp);
			$tablematrix[$j+1][4+$shift] = sprintf("%.2f",$dp);
		}
		else	//MODE 1,2,3,4
		{
	 		$sp = $tablematrix[$j+1][1+$shift]/$total[$team][0]*100;
			$tablematrix[$j+1][2+$shift] = sprintf("%.2f",$sp);
		}
	}
}		// end $team loop


for( $team=0; $team<$teamNumber; $team++ )
{
    $j = $maxcount;				// Last Line
	$shift = $team * $table_shift;

	if ($MODE == 0)
	{
		$tablematrix[$j+1][1+$shift] = $total[$team][0];
		$tablematrix[$j+1][3+$shift] = $total[$team][1];
	}
	else
		$tablematrix[$j+1][1+$shift] = $total[$team][0];
}


/*
// Calculating sums
for( $team=0; $team<$teamNumber; $team++ )
{
	if( $teamgame )		// CTF... a teamgame
	{
		$select	= 	"select SUM(score), SUM(deaths), SUM(suicides), SUM(teamkills), AVG(rankteam) from matchplayers ".
					"where matchid=$matchid and teamid=$team ";
	}
	else				// DM, DM other... not a teamgame
	{
		$select	= 	"select SUM(score), SUM(kills), SUM(deaths), SUM(suicides), AVG(rankteam), ".
					"SUM(playerminutes), SUM(events) from matchplayers ".	//**fix rankELO! later
					"where matchid=$matchid and teamid=$team ";
	}
	$link	= mysql_query( $select );
		if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row 		= mysql_fetch_row( $link );

    $j = $maxcount+1;	// Last Line

	if( $teamgame )		// CTF... a teamgame
	{
    	$sscore		= $row[0];
	    $sdeaths	= $row[1];
	    $ssuicides	= $row[2];
	    $steamkills	= $row[3];
	    $srankteam	= round($row[4],2);
	}
	else
	{
    	$sscore		= $row[0];
	    $skills		= $row[1];
	    $sdeaths	= $row[2];
	    $ssuicides	= $row[3];
	    $srankteam	= round($row[4],2);
	    $sminutes	= $row[5];
	    $sevents	= $row[6];
	}

	$shift = $team * $table_shift;
	$tablematrix[$j][0] = '';

	if( $teamgame )
	{
		$tablematrix[$j][1+$shift] = '';
		$tablematrix[$j][2+$shift] = '';
		$tablematrix[$j][3+$shift] = $sscore;
		$tablematrix[$j][4+$shift] = $sdeaths;
		$tablematrix[$j][5+$shift] = $ssuicides;
		$tablematrix[$j][6+$shift] = $steamkills;
		$tablematrix[$j][7+$shift] = $srankteam;
	}
	else
	{
		$tablematrix[$j][1+$shift] = '';
		$tablematrix[$j][2+$shift] = '';
		$tablematrix[$j][3+$shift] = $sscore;
		$tablematrix[$j][4+$shift] = $skills;
		$tablematrix[$j][5+$shift] = $sdeaths;
		$tablematrix[$j][6+$shift] = $ssuicides;
		$tablematrix[$j][7+$shift] = $srankteam;
		$tablematrix[$j][8+$shift] = $sminutes;
		$tablematrix[$j][9+$shift] = $sevents;
	}

	// Make this column stand out
	$shift = $team * $table_shift;
	$tablematrix[$j][3+$shift] = '<b>'.$tablematrix[$j][3+$shift].'</b>';

}	// end $team loop
*/



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
