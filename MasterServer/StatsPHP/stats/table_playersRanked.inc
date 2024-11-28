<?php
//---List top ranked players for all the gametypes 11 in total---
/*
	External vars used:

	$db_lid						global var, language used in db
	$db_lang[$db_lid]
	$db_mod_tags[]
	$db_mod_codes[]

	$mode_playersRanked			page var, sets the gametype table to be shown
*/
if( $included_get_nickname == 0 )				include "func/get_nickname.inc";

$DEBUG	= 0;
$MODE	= $mode_playersRanked;

//Initializing vars and defining table header
unset($tablematrix);
$table_span 	= array( 1, 3,0,0, 3,0,0, 3,0,0 );	//COLSPAN 3, 0 do not dump column!
$table_blocks	= $head_timeframes;
$limit = "limit ".$show_top_X;

// Getting mid, check if this is a teamgame or not
$select		= "select mid, teamgame from mod where modcode='$db_mod_codes[$MODE]'";
$link		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
$row		= mysql_fetch_row( $link );
$mid		= $row[0];
$teamgame	= $row[1];

// Getting the table data
$entry 			= 'playersRanked_'.$db_mod_tags[$MODE];
$table_title	= $table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];
$table_total	= $table_total_l[$entry];

// This gets all weapon ids, that are distinct,
// sums the kills for each weapon and makes sure the sum is positive.
for( $t=1; $t<=3; $t++ )
{
	if( !$teamgame )		// DM, DM other... not a teamgame
	{
//		$select	= 	"select player.pid, playerid, rankELO from players,player ".	//, kills-suicides as frags
//					"where players.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] and mid=$mid ".
//					"and rankELO>0 ORDER BY rankELO DESC $limit";
		$select	= 	"select player.pid, playerid, rankELO from player ". 
					"where timeFrame=$t and timeFrameNumber=$TFN[$t] and mid=$mid ".
					"and rankELO>0 ORDER BY rankELO DESC $limit";	//fixed
	}
	else					// CTF... a teamgame
	{
		$select	= 	"select pid, playerid, rankTeam from player ".					//, rankELO
					"where timeFrame=$t and timeFrameNumber=$TFN[$t] and mid=$mid ".
					"and rankTeam>0 ORDER BY rankTeam DESC $limit";
	}

	$link	= mysql_query( $select );
		if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";

	$count	= mysql_num_rows($link);
	for( $j=0; $j<$count; $j++ )
	{
		$row 		= mysql_fetch_row( $link );
	    $pid 		= $row[0];
	    $playerid	= $row[1];
	    $rank		= $row[2];		// rankELO or rankTeam
	    //$info		= $row[3];		// frags or rankELO

		$nickname	= get_nickname($playerid);

        $namelink 	= "<a href=\"playerID.php?playerID=$playerid&modID=-1\">$nickname</a>";
        $plidlink	= "<a href=\"playerID.php?playerID=$playerid&modID=-1\">$playerid</a>";

		$shift = ($t-1) * 3;
		//if( $t==1 )	$tablematrix[$j+1][0+$shift] = $j+1;
		$tablematrix[$j+1][0] = $j+1;							//overwrite 0th column several times!
		$tablematrix[$j+1][1+$shift] = $namelink;
		$tablematrix[$j+1][2+$shift] = $plidlink;
		$tablematrix[$j+1][3+$shift] = sprintf("%.1f",$rank);
		//$tablematrix[$j+1][4+$shift] = round($info,2);		//check what this does to ints like frags
	}

	// Make this column stand out
	$shift = ($t-1) * 3;
	for( $j=0; $j < $count+1; $j++ )
		$tablematrix[$j][3+$shift] = '<b>'.$tablematrix[$j][3+$shift].'</b>';

}		// end $t loop



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
