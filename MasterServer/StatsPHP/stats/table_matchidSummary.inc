<?php
//---Summing up the data the current match!---
/*
	External vars used:

	$matchid	= $matchID_Summary;
	$mid 		= $modID_Summary;
	$playerid	= $playerID_Summary;
*/

if( $included_get_minutesecond == 0 )
	include "func/get_minutesecond.inc";

if( $included_get_serverid == 0 )
	include "func/get_serverid.inc";

if( $included_get_teamgame == 0 )
	include "func/get_teamgame.inc";

$DEBUG = 0;
$matchid	= $matchID_Summary;
$mid 		= $modID_Summary;
$playerid	= $playerID_Summary;
$part		= $matchID_Summary_part;
$teamgame	= get_teamgame($mid);


if( $playerid > 0 )
{
	$title_pre	= "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
	$title_pre2	= "<span class=blue>".get_modname($mid)."</span><br>";
}
else
	$title_pre = "<span class=blue>".get_modname($mid)."</span><br>";

//Initializing vars and defining table header
unset($tablematrix);					//since matrix is remembered, needs to be unset!
$entry			= 'matchIDSummary';
if( $part == 1 )
{
	$table_title	= $title_pre2." ".$table_title_l[$entry];
	$tablematrix[0] = $table_head_l[$entry];
	$table_text		= $table_text_l[$entry];
}
else
{
	$table_title	= $title_pre." ".$table_title_l2[$entry];
	$tablematrix[0] = $table_head_l2[$entry];
	$table_text		= $table_text_l2[$entry];
}
$table_rows		= $table_rows_l[$entry];


// Getting all MatchIDs that conform to this mid
$select		= 	"select svid, mapid, playerconnects, startmatch, endgame, endCause ".
				"from matches where matchid=$matchid";	//was startgame
$link		= 	mysql_query( $select );
$count		=	mysql_num_rows($link);
					if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

// Getting the match summary data again...
$row		= 	mysql_fetch_row( $link );
$svid		= 	$row[0];
$mapid		= 	$row[1];
$connects	= 	$row[2];
$startgame	= 	$row[3];
$endgame	= 	$row[4];
$mapchange	= 	$row[5];

$endgame	=	substr($endgame, strrpos($endgame, " "));

$mapname	=	get_mapname($mapid);
$servername	=	get_servername($svid);
$serverid	=	get_serverid($svid);

$serverlink 	= "<a href=\"serverID.php?serverID=$serverid\">$servername</a>";
$serveridlink	= "<a href=\"serverID.php?serverID=$serverid\">$serverid</a>";

// Getting gametime
$select2	=	"select (UNIX_TIMESTAMP(endgame)-UNIX_TIMESTAMP(startmatch))/60 ".
				"from matches where matchid=$matchid ";
$link2		= 	mysql_query( $select2 );
			if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select2\"<br>\n";
$row2		= 	mysql_fetch_row( $link2 );
$gametime	=	get_minutesecond($row2[0]);
if( $endgame == 0 ) 						// Game has not ended yet!
{
	$gametime = "&nbsp;";
	if( $part == 1 && !($playerid>0) )		//"-Match in Progress-" 
		$table_title = "<span class=blue>".$match_in_progress[1]."</span><br>".$table_title;	
}


$tablematrix[1][0] = $matchid;
$tablematrix[1][1] = $serverlink;
$tablematrix[1][2] = $serveridlink;
$tablematrix[1][3] = $mapname;
$tablematrix[1][4] = $connects;
$tablematrix[1][5] = $startgame;
$tablematrix[1][6] = $gametime;
$tablematrix[1][7] = translate($mapchange,$db_lang[$db_lid]);


// Getting some more extra data for this matchid
if( $playerid > 0 && $part==2 )
{
	if($teamgame)	$rank = "rankteam";
	else			$rank = "rankELO";
	$select		= 	"select playerminutes, score, $rank, deaths, kills, suicides, teamkills, events, (rankend-rankbeg) as rankgain, rankend ".
					"from matchplayers where matchid=$matchid and playerid=$playerid";		//**using rankteam... hmmm
}
else
	$select		= 	"select sum(kills), sum(suicides), sum(score), sum(teamkills), sum(events) ".
					"from matchplayers where matchid=$matchid ";
	$link		= 	mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= 	mysql_fetch_row( $link );

if( $playerid > 0 && $part==2 )
{
	$minutes	= 	get_minutesecond($row[0]);		// sprintf("%.1f",$row[0]);
	$score		= 	sprintf("%d",$row[1]);			// Show score as INT
	$rank		= 	sprintf("%.1f",$row[2]);
	$deaths		= 	$row[3];
	$kills		= 	$row[4];
	$suicides	= 	$row[5];
	$teamkills	= 	$row[6];
	$events		= 	$row[7];
    $rankgain	=	sprintf("%.1f",$row[8]);
    if( $rankgain > 0 ) $rankgain = "+".$rankgain;
    $rankend	=	$row[9];						// Making sure that rank gain has valid end value!
    if( $rankend == -1 )	
    	$rankgain = "&nbsp;";

	$frags		=	$kills-$suicides;
	$nickname	=	get_nickname($playerid);
    $namelink 	=	"<a href=\"playerID.php?playerID=$playerid&modID=-1\">$nickname</a>";
    $plidlink	=	"<a href=\"playerID.php?playerID=$playerid&modID=-1\">$playerid</a>";

	$tablematrix[1][0]  = $namelink;
	$tablematrix[1][1]  = $plidlink;
	$tablematrix[1][2]  = $minutes;
	$tablematrix[1][3]  = $score;
	$tablematrix[1][4]  = $rank;
	$tablematrix[1][5]  = $rankgain;
	$tablematrix[1][6]  = $frags;
	$tablematrix[1][7]  = $kills;
	$tablematrix[1][8]  = $deaths;			//deaths is the same
	$tablematrix[1][9]  = $suicides;
	$tablematrix[1][10] = $teamkills;
	$tablematrix[1][11] = $events;

	$highlightcols		= array( 3 );		//scores
}
else
{
	if( $part==2 )
	{
		$kills		= 	$row[0];
		$suicides	= 	$row[1];
		$scores		= 	sprintf("%d",$row[2]);			// Show score as INT
		$teamkills	= 	$row[3];
		$events		= 	$row[4];
		$frags		= 	$kills-$suicides;

		// Getting the TeamScore per Team, putting it into the header
		if( $teamgame )
		{
			for( $team=0; $team<2; $team++ )
			{
				$select	= 	"select sum(teamscores) from matchteamscores where matchid=$matchid and teamid=$team ";
				$link	= mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
				$row 	= mysql_fetch_row( $link );
		        if( $row[0]==NULL ) $row[0]=0;		//%% If other team does not score, then the entry will not exits in the DB
				$teamscore[$team] = $row[0];
			}
			$teamscores = "".$teamscore[0]." : ".$teamscore[1]."";
		}
		else
			$teamscores = "-";				// Not a teamgame, thus no data!

		// Getting mutators
		$select2	=	"select sum(mutators) from matchmutators where matchid=$matchid";
		$link2		= 	mysql_query( $select2 );
					if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select2\"<br>\n";
		$row2		= 	mysql_fetch_row( $link2 );
		$mutators	=	$row2[0];

		$tablematrix[1][0] = $teamscores;
		$tablematrix[1][1] = $scores;
		$tablematrix[1][2] = $frags;
		$tablematrix[1][3] = $kills;		//deaths is the same
		$tablematrix[1][4] = $suicides;
		$tablematrix[1][5] = $teamkills;
		$tablematrix[1][6] = $events;
		$tablematrix[1][7] = $mutators;

		$highlightcols		= array( 0 );	//teamscore
	}
}

// Enhancing certain columns, e.g. highlithing 'em.
tablematrix_colhighlight($tablematrix, 0, 1, $highlightcols );


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
