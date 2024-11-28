<?php
//---Showing the sums of match related data, also used for player data---

if( $included_get_nickname == 0 )	include "func/get_nickname.inc";
// if( $included_get_teamgame == 0 )	include "func/get_teamgame.inc";

$DEBUG		= 0;
$MODE		= $matchID_GameRules;
$matchid	= $matchID_Summary;
$mid 		= $modID_Summary;
$playerid	= $playerID_Summary;	//-1 here!
//$teamgame	= get_teamgame($mid);
$serverid	= $serverID;

// Setting vars
$title_pre 	= "";								//all Players (also unset cases!)
$where 		= "";
$table_mode = "t";
$limit 		= "";								//"limit ".$show_top_X;

//Initializing vars and defining table header
unset($tablematrix);

// The Specific Table info tag names
if ($MODE == 0) $entry = 'matchidGameRules';
if ($MODE == 1) $entry = 'serveridGameRules';

// Setting the table data
$table_title	= $title_pre.$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$table_total	= $table_total_l[$entry];

$table_span		= array( 1, 2,0 );
$tablematrix[0] = $table_head_l[$entry];
$table_blocks	= $head_teams;
$table_teamNo	= 1;


// Setting table data for the possible 2 teams
if( $MODE == 0 )	$select	= 	"select gameRules from matches where matchid=$matchid $where ";
if( $MODE == 1 )	$select	= 	"select gameRules from server where serverID=$serverid $where ";
$link		= mysql_query( $select );
$count		= mysql_num_rows( $link );
	if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";
$row 		= mysql_fetch_row( $link );
$gamerules	= $row[0];

// Remove left most \, then take gamerules appart on \
// e.g. \servermode\dedicated\adminname\Admin\adminemail\\goalscore\25\timelimit\15\minplayers\1
$gamerule	= explode( "\\", ltrim($gamerules, "\\") );
$count		= sizeof($gamerule);
if( $DEBUG ) { print "Gamerules: (".$gamerules.")  Count: (".$count.")<br>"; print_r($gamerule); }


// Skip all below if the DB does not have this info!
if( $count > 1  )									// unset = 0, no \ in there $count=1, but should at least be 2
{ 
	// Going through the sorted list of players for each team
	$flags = $count / 2;

	for( $j=0; $j<$flags; $j++ )
	{
		$k 		= $j*2;
	   	$rule	=  htmlspecialchars($gamerule[$k]);
	   	$flag	=  htmlspecialchars($gamerule[$k+1]);
		if( $flag=="" ) $flag = "&nbsp;";

		$tablematrix[$j+1][0]	= $rule;
		$tablematrix[$j+1][1]	= $flag;
	}  // end count loop
	
	
	// Add summary data to the tablematrix
	unset($percentcols); unset($sumcols);
	$highlight = array( 1 );

	// Remember the manually counter line number
	$count = $flags;
	
	// Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $count, $sumcols, $percentcols, $table_total );
	
	// Enhancing certain columns, e.g. highlithing 'em.
	tablematrix_colhighlight($tablematrix, 0, $count, $highlight );	//was $count+1

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
