<?php
//---Showing the sums of data: Maps, Servers, Mods pages---
if( $included_get_nickname == 0 )				include "func/get_nickname.inc";
if( $included_get_modname == 0 )				include "func/get_modname.inc";
if( $included_get_mapname == 0 )				include "func/get_mapname.inc";
if( $included_get_servername == 0 )				include "func/get_servername.inc";
if( $included_get_serverid == 0 )				include "func/get_serverid.inc";
if( $included_get_minutesecond == 0 )			include "func/get_minutesecond.inc";
if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";

$DEBUG		= 0;
$MODE		= $mode_playeridLastMatches;
$playerid	= $playerID;
$mid		= $modID;

// Setting some where statements
$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
$limit		= "limit ".$show_matches_X;			// global 20
$where		= "";

if( $mid > -1)
	$where = "and mid=$mid";

//Initializing vars and defining table header
unset($tablematrix);

// The Specific Table info tag names
if( $MODE == 0 ) $entry = 'playeridLastMatches';

// Setting the table data
$table_mode 	= "t";							//no table header line!
$table_title	= $title_pre.$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$table_total	= $table_total_l[$entry];
$table_span		= "";							// not used

$tablematrix[0] = $table_head_l[$entry];
$table_blocks	= $head_teams;
$table_teamNo	= 1;

// Setting table data for the possible 2 teams
if( $MODE == 0 )
	$select	= 	"select matches.matchid, mid, mapid, svid, startmatch, score,  ".
				"(UNIX_TIMESTAMP(endgame)-UNIX_TIMESTAMP(startmatch))/60 as uptime ".
				"from matchplayers, matches ".
				"where matches.matchid=matchplayers.matchid and playerid=$playerid $where ".
				"order by startmatch DESC $limit";
$link	= mysql_query( $select );
$count	= mysql_num_rows( $link );
	if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

// Going through the sorted list of players for each team
for( $j=0; $j<$count; $j++ )
{
	$row 		= mysql_fetch_row( $link );
	if( $MODE == 0 )
	{
	   	$matchid	= $row[0];
	   	$mid		= $row[1];
	    $mapid		= $row[2];
    	$svid		= $row[3];
    	$startmatch	= $row[4];
    	$score		= sprintf("%d",$row[5]);
    	$uptime 	= get_minutesecond($row[6]);;		// in minutes
	}

	// Filling table with data...
	if( $MODE == 0 )
	{
        $matchlink		= "<a href=\"matchID.php?matchID=$matchid&modID=$mid&playerID=-1\">$matchid</a>";

   		$modname		= translate(get_modname($mid),$db_lang[$db_lid]);
		$modlink 		= "<a href=\"modID.php?modID=$mid\">$modname</a>";

		$mapname		= get_mapname($mapid);
		$maplink 		= "<a href=\"mapID.php?mapID=$mapid\">$mapname</a>";

		$serverid 		= get_serverid($svid);
    	$servername 	= get_servername($svid);
		$serverlink 	= "<a href=\"serverID.php?serverID=$serverid\">$servername</a>";
		$serveridlink	= "<a href=\"serverID.php?serverID=$serverid\">$serverid</a>";

		$tablematrix[$j+1][0]	= $j+1;			// counter
		$tablematrix[$j+1][1]	= $matchlink;
		$tablematrix[$j+1][2]	= $modlink;
		$tablematrix[$j+1][3]	= $maplink;
		$tablematrix[$j+1][4]	= $startmatch;
		$tablematrix[$j+1][5]	= $uptime;
		$tablematrix[$j+1][6]	= $score;
		$tablematrix[$j+1][7]	= $serverlink;
		$tablematrix[$j+1][8]	= $serveridlink;
	}

}  // end count loop


// Add summary data to the tablematrix
unset($sumcols); unset($percentcols);
if( $MODE == 0  )
	$highlight		= array( 4 );

// Calculating totals and percent values
tablematrix_sumpercent($tablematrix, 1, $count, $sumcols, $percentcols, $table_total );

// Enhancing certain columns, e.g. highlithing 'em.
if( isset($sumcols) ) $lines=1;
else $lines=0;
tablematrix_colhighlight($tablematrix, 0, $count+$lines, $highlight );	//** in for now


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
