<?php
//---List of weapons or damagetypes for Kills/Deaths or Suicides---

if( $included_get_rankzone == 0 )				include "func/get_rankzone.inc";
if( $included_get_teamgame == 0 )				include "func/get_teamgame.inc";
if( $included_get_nickname == 0 )				include "func/get_nickname.inc";
if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";
if( $included_tablematrix_rowhighlight == 0 )	include "func/tablematrix_rowhighlight.inc";

$DEBUG		= 0;
$MODE		= $mode_playeridRankZone;
$playerid	= $playerID_globalSummary;
$mid 		= $mid_playeridSummary;
$teamgame	= get_teamgame($mid);
$title_pre	= "";
$where		= "";

if( $playerid > 0 && $mid > 0 )
{
	$where = $where."and mid=$mid";

	$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
	$title_pre .= get_modname($mid)."<br>";
}

//Initializing vars and defining table header
unset($tablematrix);
if( $MODE == 0 )
{
	$table_span		= array( 1, 4,0,0,0, 4,0,0,0, 4,0,0,0 );
	$table_shift	= 4;
}
$table_blocks		= $head_timeframes;

if( $MODE == 0 )	$entry = 'playeridInRankZone';

$table_title	= $title_pre." ".$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];
$table_total	= $table_total_l[$entry];

// This gets all weapon ids, that are distinct,
// sums the kills for each weapon and makes sure the sum is positive.
for( $t=1; $t<=3; $t++ )
{
    $beg = 0;	$end = 0;	// Init
	if( $MODE == 0 )		// Player In Rank Zone +-5 above below in ranking
	{
		if( $teamgame )	$rankmethod = "rankTeam";
		else			$rankmethod = "rankELO";
		$select	= 	"select playerid, $rankmethod from player,players ".
					"where players.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"order by $rankmethod DESC ";
		$link	= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
		$count	= mysql_num_rows($link);

		// At what position the player is ranked
		for( $j=0; $j<$count; $j++ )
		{
			$row	= mysql_fetch_row( $link );
	    	if ( $playerid == $row[0] ){  $db_playeratpos = $j; break; }
	    }	

		// This will try to get the full range 11, and shift things if possible.
		$rankzone = $show_rankzone_X;			//Position in rank plus/minus 5, used on playerID mod page
        $bot=0; 	$top=$count;
		$table_playerpos = $db_playeratpos; 	// Can be offset!

		get_rankzone( $beg, $end, $bot, $table_playerpos, $top, $rankzone );	// returns beg,end of zone for loop, bot,top are loop limits

	}

	$link	= mysql_query( $select );
	$count	= mysql_num_rows( $link );
				if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

 	if(	$MODE != 0 ){ $beg = 0;	$end = $count; }			// Default mode
    else
    {
		for( $j=0; $j<$beg; $j++ )							// Skip all the rows that we will not use
			$row = mysql_fetch_row( $link );
    }

    $k=-1;													
	for( $j=$beg; $j<$end; $j++ )
	{
		if( $beg > 0 ) $k++;								// New counter needed...
		else $k = $j;										// old counter
		
		$row	= mysql_fetch_row( $link );
        $id		= $row[0];		// playerid
        $data	= $row[1];		// rank

        
		$shift = ($t-1) * $table_shift;
		if( $MODE == 0 )
		{
			//"Player", 	"PlayerID",	"Rank"
			$nickname = get_nickname($id) ;
		    $namelink =	"<a href=\"playerID.php?playerID=$id&modID=-1\">$nickname</a>";
		    $plidlink =	"<a href=\"playerID.php?playerID=$id&modID=-1\">$id</a>";

			$tablematrix[$k+1][0]				= $k+1;
			$tablematrix[$k+1][1+$shift]		= $j+1;
			if( $j != $db_playeratpos )
			{
				$tablematrix[$k+1][2+$shift]	= $namelink;
				$tablematrix[$k+1][3+$shift]	= $plidlink;
			}
			else	// do not show link for the player, also used as visual "focus"
			{
				$tablematrix[$k+1][2+$shift]	= $nickname;
				$tablematrix[$k+1][3+$shift]	= $id;
			}
			$tablematrix[$k+1][4+$shift]		= sprintf("%.1f",$data);	// rank
        }
	}	// end $j count loop

	$shift = ($t-1) * $table_shift;		//needed for some reason
	unset($sumcols); unset($percentcols); unset($enhance);
	if( $MODE == 0 )
		$highlight		= array( 3+$shift );

    // Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $end-$beg, $sumcols, $percentcols, $table_total );

	// Enhancing certain columns, e.g. highlithing 'em.
	if( isset($sumcols) ) $lines=1;
	else $lines=0;
	tablematrix_colhighlight($tablematrix, 0, $end-$beg+$lines, $highlight );

	// Enhancing a certain row, of parts of a certain row, unset enhance, does full row!
	tablematrix_rowhighlight($tablematrix, $db_playeratpos+1, $enhance ); 	//** was $table_playerpos

}	// end $t loop



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
