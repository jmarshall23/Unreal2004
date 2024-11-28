<?php
//---Showing the sums of match related data, also used for player data---

if( $included_get_nickname == 0 )	include "func/get_nickname.inc";
if( $included_get_teamgame == 0 )	include "func/get_teamgame.inc";

$DEBUG		= 0;
$MODE		= $mode_matchIDdata;
$matchid	= $matchID_Summary;
$mid 		= $modID_Summary;
$playerid	= $playerID_Summary;	//-1 here!
$teamgame	= get_teamgame($mid);

// Setting some where statements
if( $playerid > 0 )
{
	//$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";	//too much!
	$title_pre = "";								//all Players (also unset cases!)
	$where = "and playerid=$playerid ";				//per Player
	$table_mode = "t";					            //player does not need team placement
}
else
{
	$title_pre = "";								//all Players (also unset cases!)
	$where = "";

	if( $teamgame ) $table_mode = "t3";
	else			$table_mode = "t";				// No extra header for DM games
}
$limit 		= "limit ".$show_top_X;


//Initializing vars and defining table header
unset($tablematrix);

// The Specific Table info tag names
if ($MODE == 0) $entry = 'matchidData_KD';
if ($MODE == 1)	$entry = 'matchidData_S';
if ($MODE == 2)	$entry = 'matchidData_E';
if ($MODE == 3)	$entry = 'matchidData_Sc';
if ($MODE == 4)	$entry = 'matchidData_Mu';

if ($MODE == 5)	$entry = 'matchidData_En';	// enemies - only used on matchIDplayer page! 
if ($MODE == 6)	$entry = 'matchidData_Vi';  // victims - both never used in teamgames 


// Setting the table data
$table_title	= $title_pre.$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$table_total	= $table_total_l[$entry];

if( $teamgame )
{
	if ($MODE == 0 OR $MODE == 3 )			//Frags
	{
		$table_span		= array( 1, 4,0,0,0, 4,0,0,0 );
		$table_shift	= 4;
    }
    else				// Suis, Events, Muatators
    {
		$table_span		= array( 1, 2,0, 2,0 );
		$table_shift	= 2;
    }
	$tablematrix[0] = $table_head_l2[$entry];
	$table_blocks	= $head_teams2;
	$table_teamNo	= 2;
}
else
{
	if ($MODE == 0 OR $MODE == 3 OR $MODE == 5 OR $MODE == 6 )	//Frags, Scores
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
	$table_teamNo	= 1;
}

// Special case mutator table fixes
if( $MODE == 4 OR $playerid > 0 )				// Match Player does not need the other column!
{
	$tablematrix[0] = $table_head_l[$entry];
	$table_blocks	= $head_block_x[$entry];	// Mutators only have 1 header always (ignored for Match Player)
	$table_teamNo	= 1;
}

// Setting table data for the possible 2 teams
if( $MODE == 0 )		// Per Weapon Sums
	$select	= 	"select wid, teamid, sum(matchweapons.kills), sum(matchweapons.deaths) from matchweapons, matchplayers ".
				"where matchweapons.mpid=matchplayers.mpid and matchid=$matchid $where ".
				"group by wid, teamid ";
if( $MODE == 1 )		// Per DamageType Suicides
	$select	= 	"select sid, teamid, sum(matchsuicides.suicides) from matchsuicides, matchplayers ".
				"where matchsuicides.mpid=matchplayers.mpid and matchid=$matchid $where ".
				"group by sid, teamid ";
if( $MODE == 2 )		// Per Event type
	$select	= 	"select eid, teamid, sum(matchevents.events) from matchevents, matchplayers ".
				"where matchevents.mpid=matchplayers.mpid and matchid=$matchid $where ".
				"group by eid, teamid ";
if( $MODE == 3 )		// Per Score type
	$select	= 	"select scid, teamid, sum(matchscores.scoresums), sum(matchscores.scores) from matchscores, matchplayers ".
				"where matchscores.mpid=matchplayers.mpid and matchid=$matchid $where ".
				"group by scid, teamid ";
if( $MODE == 4 )		// Per mutator name
	$select	= 	"select mutid, matchid, mutators from matchmutators ".		//matchid is a dummy, not used!
				"where matchid=$matchid ";
if( $MODE == 5 )		// Enemies - Victors where pid, so vid = you, playerID dummy column
	$select	= 	"select matchvictors.mpid, playerid, sum(matchvictors.kills) as skills from matchvictors, matchplayers ".
				"where matchvictors.mvid=matchplayers.mpid and matchid=$matchid $where ".
				"group by matchvictors.mpid order by skills DESC $limit ";
if( $MODE == 6 )		// Fragbait - Victors where pid = you locked
	$select	= 	"select matchvictors.mvid, playerid, sum(matchvictors.kills) as skills from matchvictors, matchplayers ".
				"where matchvictors.mpid=matchplayers.mpid and matchid=$matchid $where ".
				"group by matchvictors.mvid order by skills DESC $limit ";
$link	= mysql_query( $select );
$count	= mysql_num_rows( $link );
	if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

// Skip all below if the DB does not have this info!
if( $count != 0 )		
{ 
	// Going through the sorted list of players for each team
	unset($seenid);									// reset the sucker
	$l=-1;											// New line counter
	for( $j=0; $j<$count; $j++ )
	{
		$row 		= mysql_fetch_row( $link );
	   	$id			= $row[0];
	   	$teamid		= $row[1];
	
		// Need to do own per matrix line counts!
		if( !isset($seenid[$id]) )					// If first time seen, remember line, else do not overwrite
		{
			$l++;									// Starts at 0
			$seenid[$id] = $l;						// Rememeber in what line what event was first shown
	    }
	
		if( $MODE == 0 OR $MODE == 3 )				// K/D or Scores
		{
		    $skills		= sprintf("%d",$row[2]);	// Scores as INT, will not matter for K/Ds
		    $sdeaths	= sprintf("%d",$row[3]);
		}
		else
		    $sdata		= $row[2];					//suis, events, scores, mutators, enemies
	
		if( $MODE == 4 ) $teamid = 0;				//there is no 2nd column
	
		//Looking up readable names
		if( $MODE == 0 ) $select = "select weaponcode, mode  from weapon where wid=$id";
		if( $MODE == 1 ) $select = "select suicidecode from suicide where sid=$id";
		if( $MODE == 2 ) $select = "select eventcode from event where eid=$id";
		if( $MODE == 3 ) $select = "select scorecode from score where scid=$id";
		if( $MODE == 4 ) $select = "select mutatorcode from mutator where mutid=$id";
		if( $MODE == 5 OR $MODE == 6 ) 
						 $select = "select playerid from matchplayers where mpid=$id";
	
		$link2	= mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select\"<br>\n";
		$row2	= mysql_fetch_row( $link2 );
		if( $MODE == 0 ) $row2[0] .= "_".$row2[1]."";
		$name	= translate($row2[0],$db_lang[$db_lid]);
	
		if( $MODE == 5 OR $MODE == 6 )
		{
			$mplayerID	= $name;
			$nickname	= get_nickname($mplayerID);
		    $namelink 	= "<a href=\"playerID.php?playerID=$mplayerID&modID=-1\">$nickname</a>";
	    	$plidlink	= "<a href=\"playerID.php?playerID=$mplayerID&modID=-1\">$mplayerID</a>";
		}
	
		// Filling table with data...
		if( $playerid > 0 )
			$teamid = 0;					// Player table, we do NOT care on what team he was!
		$shift = $teamid * $table_shift;
	
		$s = $seenid[$id];					// Only use lines per e.g. event name once!
		if( $MODE >= 0 AND $MODE <= 4 )
		{
			if( $MODE == 0 OR $MODE == 3 )
			{
				$tablematrix[$s+1][0]			= $name;
				$tablematrix[$s+1][1+$shift]	= $skills;
				$tablematrix[$s+1][3+$shift]	= $sdeaths;	//score sum
			}
			else
			{
				$tablematrix[$s+1][0]			= $name;
				$tablematrix[$s+1][1+$shift]	= $sdata;
			}
		}
		else	//enemies, victims
		{
				$tablematrix[$s+1][0]	= $j+1;
				$tablematrix[$s+1][1]	= $namelink;
				$tablematrix[$s+1][2]	= $plidlink;
				$tablematrix[$s+1][3]	= $sdata;		//sum kills
		}
	
	}  // end count loop
	
	
	// Add summary data to the tablematrix
	$shift = ($table_teamNo-1) * $table_shift;
	if( $MODE >= 0 AND $MODE <= 4 )
	{
		if( $MODE == 0 OR $MODE == 3 )
		{
			if( $table_teamNo==2 )
			{
				$sumcols		= array( 1, 3,		1+$shift, 			3+$shift );
				$percentcols	= array( 1,2, 3,4, 	1+$shift, 2+$shift, 3+$shift, 4+$shift );
			}
			else
			{
				$sumcols		= array( 1,		3,  );
				$percentcols	= array( 1,2,	3,4 );
			}
		}
		else
		{
			if( $table_teamNo==2 )
			{
				$sumcols		= array( 1,		1+$shift);
				$percentcols	= array( 1,2,	1+$shift, 2+$shift );
			}
			else
			{
				$sumcols		= array( 1		);
				$percentcols	= array( 1,2	);
			}
		    if( $MODE == 4 ) 
		    	$percentcols	= array( );	// no percent stuff
		}
	}
	else	//enemies, victims
	{
			$sumcols		= array( 3,	 );
			$percentcols	= array( 3,4 );
	}
	
	// Remember the manually counter line number
	$count = $l+1;			// Went to $l-1, therefore +1
	
	// Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $count, $sumcols, $percentcols, $table_total );
	
	// Enhancing certain columns, e.g. highlithing 'em.
	tablematrix_colhighlight($tablematrix, 0, $count+1, $sumcols );	//** in for now

}	// end - if count

//-------------------------------------------------------------------------
// show matrix
$size='s';								//'s'mall or 'n'normal possible
$tablecontrol = array (
	"mode"			=> $table_mode,		//t3 3 timeFrames header
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
