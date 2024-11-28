<?php
//---Most recent list of matches played for all the gametypes 11---
/*
	External vars used:

	$db_lid						global var, language used in db
	$db_lang[$db_lid]
	$db_mod_tags[]
	$db_mod_codes[]

	$mode_matchesPlayed			page var, sets the gametype table to be shown
*/
if( $included_get_svid == 0 )			include "func/get_svid.inc";
if( $included_get_servername == 0 )		include "func/get_servername.inc";
if( $included_get_mapname == 0 )		include "func/get_mapname.inc";
if( $included_get_modname == 0 )		include "func/get_modname.inc";
if( $included_get_servernameID == 0 )	include "func/get_servernameID.inc";

$DEBUG	= 0;
$MODE	= $mode_matchesPlayed;

//Initializing vars and defining table header
unset($tablematrix);	unset($table_span);
//$table_span = array( 1, 3,0,0, 3,0,0, 3,0,0 );	//COLSPAN 3, 0 do not dump column!
if( $MODE < 11 )
{
	$entry		= 'matchesPlayed_'.$db_mod_tags[$MODE];
	$title_pre	= "";
}	
else if( $MODE == 11 )								//All recent Matches
{
	$entry		= 'serverIDMatches';
	$title_pre	= "<span class=blue>".get_servernameID($serverID)."</span><br>";
}
else if( $MODE == 12 )
{
	$entry		= 'mapIDMatches';
	$title_pre	= "<span class=blue>".get_mapname($mapID)."</span><br>";
}
else if( $MODE == 13 )
{
	$mid 		= $modID;
	$entry		= 'modIDMatches';
	$title_pre	= "<span class=blue>".get_modname($mid)."</span><br>";
}

$table_title	= $title_pre.$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];
$table_total	= $table_total_l[$entry];
$limit			= "limit ".$show_matches_X;			// global 20

// Getting mid, check if this is a teamgame or not
if( $MODE < 11 )  // ** IS this code really needed ? Should make it more elegant!
{
	$select		= "select mid from mod where modcode='$db_mod_codes[$MODE]'";
	$link		= mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row		= mysql_fetch_row( $link );
	$mid		= $row[0];
}
else if( $MODE == 11 )
{
	$svid		= get_svid($serverID);
}

// Getting all MatchIDs that conform to this mid
if( $MODE < 11 OR $MODE == 13 )	$where	= 	"mid=$mid"; 	//was startgame
else if( $MODE == 11 ) 			$where	=	"svid=$svid"; 	//all matches for this server
else if( $MODE == 12 )			$where	= 	"mapid=$mapID";	//all matches for this map
$select		= 	"select matches.matchid, svid, mapid, playerconnects, startmatch, endgame, mid, sum(score) as sumscore ".
				"from matches, matchplayers ".
				"where $where and matches.matchid=matchplayers.matchid and playerconnects>1 ".
				"GROUP BY matches.matchid HAVING sumscore>0 ORDER BY matchid DESC $limit";	//all matches for this map
$link		= 	mysql_query( $select );
$count		=	mysql_num_rows($link);
					if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

// Going through the list of matches for this mod
for( $j=0; $j<$count; $j++ )
{
	$row		= 	mysql_fetch_row( $link );
	if( $DEBUG ) print_r($row);
	$matchid	= 	$row[0];
	$svid		= 	$row[1];
	$mapid		= 	$row[2];
	$connects	= 	$row[3];
	$startgame	= 	$row[4];
	$endgame	= 	$row[5];

	if( $MODE == 11 OR $MODE == 12 )
		$mid	= 	$row[6];

	$endgame	=	substr($endgame, strrpos($endgame, " "));    // only show time, cut date
	$mapname	=	get_mapname($mapid);
	$servername	=	get_servername($svid);
	$matchlink 	=	"<a href=\"matchID.php?matchID=$matchid&modID=$mid&playerID=-1\">$matchid</a>";

	$tablematrix[$j+1][0] = $j+1;
	$tablematrix[$j+1][1] = $matchlink;
	$tablematrix[$j+1][2] = $servername;
	$tablematrix[$j+1][3] = $mapname;
	$tablematrix[$j+1][4] = $connects;
	$tablematrix[$j+1][5] = $startgame;
	$tablematrix[$j+1][6] = $endgame;

	// Getting some extra data for this matchid
	$select2	= 	"select sum(kills), sum(suicides), sum(score) as sumscore ".
					"from matchplayers where matchid=$matchid group by matchid ";
	$link2		= 	mysql_query( $select2 );
						if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select2\"<br>\n";
	$row2		= 	mysql_fetch_row( $link2 );

	$kills		= 	$row2[0];
	$suicides	= 	$row2[1];
	$scores		=	sprintf("%d",$row2[2]);	// Score as INT
	$frags		= 	$kills-$suicides;

	$tablematrix[$j+1][7]  = $frags;
	$tablematrix[$j+1][8]  = $kills;		//deaths is the same
	$tablematrix[$j+1][9]  = $suicides;
	$tablematrix[$j+1][10] = $scores;
}


// Make this column stand out
for( $j=0; $j < $count+1; $j++ )
	$tablematrix[$j][1] = '<b>'.$tablematrix[$j][1].'</b>';



//-------------------------------------------------------------------------
// show matrix
$size='s';								//'s'mall or 'n'normal possible
$tablecontrol = array (
	"mode"			=> "t",				//t3 3 timeFrames header
	"option"		=> "head",
	"tabletitle"	=> $table_title,	//externally set
	"tabletext"		=> $table_text,		//externally set
	"tablespan"		=> $table_span,		//externally set (for t3 only)
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
