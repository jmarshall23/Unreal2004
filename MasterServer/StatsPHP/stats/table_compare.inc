<?php
//---List of weapons or damagetypes for Kills/Deaths or Suicides---
/*
	External vars used:

	$playerID, playerID2		the 2 players we want to compare
	$mid						the modid we want to check the db for. $mid='' is all mods!
	$timeFrame					the timeframe we want to do the compare for
*/
if( $included_get_nickname == 0 )				include "func/get_nickname.inc";
if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";
if( $included_tablematrix_colintensity == 0 )	include "func/tablematrix_colintensity.inc";

$DEBUG			= 0;
$MODE			= $mode_compare;
unset($playerid);
$playerid[0]	= $playerID;
$playerid[1]	= $playerID2;

// Special case of empty mid is, do all gametype compare
if( $mid=="" OR !isset($mid) )
	$where ="";
else
	$where = "and mid=$mid";

$title_pre	= "";
//$title_pre = "<span class=blue>".get_nickname($playerid)."</span>".$head_title_possessive."<br>";
//$title_pre .= get_modname($mid)."<br>";

//Initializing vars and defining table header
unset($tablematrix);
if( $MODE==0 OR $MODE==3 )
{
	$table_span		= array( 1, 4,0,0,0, 4,0,0,0, 4,0,0,0 );	//COLSPAN 4, 0 do not dump column!
	$table_shift	= 4;
}
else
{
	$table_span		= array( 1, 2,0, 2,0, 2,0 );
	$table_shift	= 2;
}

// Table COLSPAN headers
//$table_blocks		= $head_timeframes;
$table_blocks		= array(	
						"1. <span class=blue>".get_nickname($playerid[0])."</span>/".$playerid[0],
						"2. <span class=blue>".get_nickname($playerid[1])."</span>/".$playerid[1],
						$compare_diff." 1. - 2." );
$entry = "compare_$MODE";
$table_title	= $title_pre." ".$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];
$table_total	= $table_total_l[$entry];


// Getting all the infos for the 2 players
$t = $timeFrame;
$WHEREREST =	" and timeFrame=$t and timeFrameNumber=$TFN[$t] ".
				" and ( playerid=$playerid[0] OR playerid=$playerid[1] ) $where ";

// Getting merged list of possible IDs in tables, e.g. ids both players share
if( $MODE == 0 )		// Per Weapon Sums
	$select	= 	"select wid, sum(kills), sum(deaths) from weapons, player ".
				"where weapons.pid=player.pid $WHEREREST group by wid ";
if( $MODE == 1 )		// Per DamageType Suicides
	$select	= 	"select sid, sum(suicides) from suicides, player ".
				"where suicides.pid=player.pid $WHEREREST group by sid ";
if( $MODE == 2 )		// Per Event type
	$select	= 	"select eid, sum(events) from events, player ".
				"where events.pid=player.pid $WHEREREST group by eid ";
if( $MODE == 3 )		// Per Score type
	$select	= 	"select scid, sum(scoresum), sum(scores) from scores, player ".
				"where scores.pid=player.pid $WHEREREST group by scid ";
if( $MODE == 4 )		// Per mutator name
	$select	= 	"select mutid, sum(pmutators) from pmutators, player ".
				"where pmutators.pid=player.pid $WHEREREST group by mutid ";
if( $MODE == 5 )		// Per map name
	$select	= 	"select mapid, sum(pmaps) from pmaps, player ".
				"where pmaps.pid=player.pid $WHEREREST group by mapid ";
if( $MODE == 6 )		// Per mods name
	$select	= 	"select pmods.mid, sum(pmods) from pmods, player ".
				"where pmods.pid=player.pid $WHEREREST group by pmods.mid ";
$link		= mysql_query( $select );
$Allcount	= mysql_num_rows( $link );
			if( $DEBUG ) print "Found ".$Allcount." rows for \"$select\"<br>\n";
	
// Remember all seen IDs
unset($pos_id);
for( $j=0; $j<$Allcount; $j++ )
{
	$row			= mysql_fetch_row( $link );
	$id				= $row[0];							// wid, sid, eid, scid...
    $pos_id[$id]	= $j;								// remember row for this id
}

// Getting the data for each player now
unset($Pcount);
for( $p=0; $p<2; $p++ )
{
	if( $Allcount == 0 ) continue; 						//** fiX skipping below

	$WHEREREST =	" and timeFrame=$t and timeFrameNumber=$TFN[$t] ".
					" and playerid=$playerid[$p] $where ";

	// Getting per player sums
	if( $MODE == 0 )
		$select	= 	"select wid, sum(kills), sum(deaths) from weapons, player ".
					"where weapons.pid=player.pid $WHEREREST group by wid ";
	if( $MODE == 1 )
		$select	= 	"select sid, sum(suicides) from suicides, player ".
					"where suicides.pid=player.pid $WHEREREST group by sid ";
	if( $MODE == 2 )
		$select	= 	"select eid, sum(events) from events, player ".
					"where events.pid=player.pid $WHEREREST group by eid ";
	if( $MODE == 3 )
		$select	= 	"select scid, sum(scoresum), sum(scores) from scores, player ".
					"where scores.pid=player.pid $WHEREREST group by scid ";
	if( $MODE == 4 )
		$select	= 	"select mutid, sum(pmutators) from pmutators, player ".
					"where pmutators.pid=player.pid $WHEREREST group by mutid ";
	if( $MODE == 5 )
		$select	= 	"select mapid, sum(pmaps) from pmaps, player ".
					"where pmaps.pid=player.pid $WHEREREST group by mapid ";
	if( $MODE == 6 )
		$select	= 	"select pmods.mid, sum(pmods) from pmods, player ".
					"where pmods.pid=player.pid $WHEREREST group by pmods.mid ";

	$link3	= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link3)." rows for \"$select\"<br>\n";
	$Pcount[$p] = mysql_num_rows($link3);
	if( $Pcount[$p] == 0 ) continue; 						// skipping below, redundant actually!
	
	for( $j=0; $j<$Pcount[$p]; $j++ )
	{
		$row	= mysql_fetch_row( $link3 );
	    $id		= $row[0];									// wid, sid, eid, scid
	    $data	= $row[1];
	    if( $MODE == 3 )	$data = sprintf("%d",$data);	// Scores are float, change to INT for table!	
	    $data2	= $row[2];
	
		if( $MODE == 0 ) $select = "select weaponcode, mode from weapon where wid=$id";		//Looking up the weapon name
		if( $MODE == 1 ) $select = "select suicidecode from suicide where sid=$id";
		if( $MODE == 2 ) $select = "select eventcode from event where eid=$id";
		if( $MODE == 3 ) $select = "select scorecode from score where scid=$id";
		if( $MODE == 4 ) $select = "select mutatorcode from mutator where mutid=$id";
		if( $MODE == 5 ) $select = "select mapname from map where mapid=$id";
		if( $MODE == 6 ) $select = "select modcode from mod where mid=$id";
	
		$link2	= mysql_query( $select );
					if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select\"<br>\n";
		$row2	= mysql_fetch_row( $link2 );
	    $name	= $row2[0];
		if( $MODE == 0 ) $name .= "_".$row2[1]."";
		if( $MODE != 5 ) $name	= translate($name,$db_lang[$db_lid]);	// Do not translate mapname!
		if( $MODE == 5 ) if( strlen($name)>20 ) $name = substr($name, 0, 17)."...";	// mapname		
	
		$shift	= $p * $table_shift;
		$k		= $pos_id[$id];								// lookup where alltime places infos!
		if( $MODE==0 OR $MODE==3 )
		{
			$tablematrix[$k+1][0]			= $name;		// overwrite 0th column once!
			$tablematrix[$k+1][1+$shift]	= $data;
			$tablematrix[$k+1][2+$shift]	= 0;
			$tablematrix[$k+1][3+$shift]	= $data2;
			$tablematrix[$k+1][4+$shift]	= 0;
		}
		else	//MODE 1,2,3,4
		{
			$tablematrix[$k+1][0] 			= $name;		// overwrite 0th column once!
			$tablematrix[$k+1][1+$shift]	= $data;
			$tablematrix[$k+1][2+$shift]	= 0;
	    }

	}	// end $j loop
}		// end $p loop
	


// Generating the data for the difference column
for( $j=0; $j<$Allcount; $j++ )
{
	$s0	= 0 * $table_shift; 	// playerID1
	$s1	= 1 * $table_shift;		// playerID2
	$s2	= 2 * $table_shift;		// Difference playerID1 - playerID2
	$k 	= $j;
	if( $MODE==0 OR $MODE==3 )
	{
		$tablematrix[$k+1][1+$s2] =	$tablematrix[$k+1][1+$s0]-$tablematrix[$k+1][1+$s1];
		$tablematrix[$k+1][3+$s2] =	$tablematrix[$k+1][3+$s0]-$tablematrix[$k+1][3+$s1];
	}
	else	//MODE 1,2,3,4
	{
		$tablematrix[$k+1][1+$s2] =	$tablematrix[$k+1][1+$s0]-$tablematrix[$k+1][1+$s1];
    }
	$Pcount[2] = $Pcount[0];	// Dummy needed below
}

	
// Use the overall max value of lines
$maxcount = $Allcount;

// ALSO summing etc. 3. difference column
for( $p=0; $p<3; $p++ )	
{
	// Do all the "nicing" up, after all counts are known!
	if( $Pcount[$p] == 0 ) continue; 					// skipping below

	$shift = $p * $table_shift;						//needed for some reason
	if( $MODE==0 OR $MODE==3 )
	{
		$sumcols		= array( 1+$shift,			3+$shift			);
		$percentcols	= array( 1+$shift,2+$shift,	3+$shift,4+$shift	);
	}
	else		//MODE 1,2,3,4
	{
		$sumcols		= array( 1+$shift			);
		$percentcols	= array( 1+$shift,2+$shift	);
	}

    // Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $maxcount, $sumcols, $percentcols, $table_total );

	// Intensity range to columns (run before colhighlight())
	tablematrix_colintensity($tablematrix, 0+1, $maxcount, $sumcols );

	// Enhancing certain columns, e.g. highlithing 'em.
	tablematrix_colhighlight($tablematrix, 0, $maxcount+1, $sumcols );
}	// end $p loop


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
