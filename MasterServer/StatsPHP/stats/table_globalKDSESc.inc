<?php
//---List of weapons or damagetypes for Kills/Deaths or Suicides---
/*
	External vars used:

	$db_lid						global var, language used in db

	$mode_globalKDSESc			page var, 0: Frags/Deaths	1: Suicides
	$playerID_globalSummary		page var, >0 per player, else all
*/

if( $included_tablematrix_colintensity == 0 )	include "func/tablematrix_colintensity.inc";

$DEBUG		= 0;
$MODE		= $mode_globalKDSESc;
$playerid	= $playerID_globalSummary;
$mid 		= $mid_playeridSummary;

if( $playerid > 0 )
{
	$where = "and playerid=$playerid ";	//per Player
	if( $mid > 0 )	//per player per mod
	{
		if( $MODE == 6 )
			$where = $where."and pmods.mid=$mid";
		else
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
$table_blocks		= $head_timeframes;

if( $MODE == 0 )	$entry = 'globalKDSESc_KD';
if( $MODE == 1 )	$entry = 'globalKDSESc_S';
if( $MODE == 2 )	$entry = 'globalKDSESc_E';
if( $MODE == 3 )	$entry = 'globalKDSESc_Sc';
if( $MODE == 4 )	$entry = 'globalKDSESc_Mu';
if( $MODE == 5 )	$entry = 'globalKDSESc_M';
if( $MODE == 6 )	$entry = 'globalKDSESc_Mo';

$table_title	= $title_pre." ".$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];
$table_total	= $table_total_l[$entry];


// This gets all weapon ids, that are distinct,
// sums the kills for each weapon and makes sure the sum is positive.
unset($Tcount); unset($pos_id);
for( $t=3; $t>=1; $t-- )
{
	if( $MODE == 0 )		// Per Weapon Sums
		$select	= 	"select wid, sum(kills), sum(deaths) from weapons, player ".
					"where weapons.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"group by wid ";
	if( $MODE == 1 )		// Per DamageType Suicides
		$select	= 	"select sid, sum(suicides) from suicides, player ".
					"where suicides.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"group by sid ";
	if( $MODE == 2 )		// Per Event type
		$select	= 	"select eid, sum(events) from events, player ".
					"where events.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"group by eid ";
	if( $MODE == 3 )		// Per Score type
		$select	= 	"select scid, sum(scoresum), sum(scores) from scores, player ".
					"where scores.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"group by scid ";
	if( $MODE == 4 )		// Per mutator name
		if( $playerid > 0 )
			$select	= 	"select mutid, sum(pmutators) from pmutators, player ".
						"where pmutators.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
						"group by mutid ";
		else // global page access
			$select	= 	"select mutid, sum(mutators) as mutsum from mutators ".
						"where timeFrame=$t and timeFrameNumber=$TFN[$t] ".
						"group by mutid ";
	if( $MODE == 5 )		// Per map name
		$select	= 	"select mapid, sum(pmaps) from pmaps, player ".
					"where pmaps.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"group by mapid ";
	if( $MODE == 6 )		// Per mutator name
			$select	= 	"select pmods.mid, sum(pmods) from pmods, player ".
					"where pmods.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] $where ".
					"group by pmods.mid ";

	$link	= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$Tcount[$t] = mysql_num_rows($link);
	if( $Tcount[$t] == 0 ) continue; 						// skipping below

	for( $j=0; $j<$Tcount[$t]; $j++ )
	{
		$row	= mysql_fetch_row( $link );
        $id		= $row[0];									// wid, sid, eid, scid
		if( $t==3 ) $pos_id[$id]=$j;						// quick lookup where alltime places the infos!
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
		if( $MODE != 5 ) $name	= translate($name,$db_lang[$db_lid]);	// Mapname needs no translation!
		if( $MODE == 5 ) if( strlen($name)>20 ) $name = substr($name, 0, 17)."...";	// mapname		

		$shift = ($t-1) * $table_shift;
		$k = $pos_id[$id];									// lookup where alltime places infos!
		if( $MODE==0 OR $MODE==3 )
		{
			if( $t==3 ) 
				$tablematrix[$k+1][0]		= $name;		// overwrite 0th column once!
			$tablematrix[$k+1][1+$shift]	= $data;
			$tablematrix[$k+1][2+$shift]	= 0;
			$tablematrix[$k+1][3+$shift]	= $data2;
			$tablematrix[$k+1][4+$shift]	= 0;
		}
		else	//MODE 1,2,3,4
		{
			if( $t==3 ) 
				$tablematrix[$k+1][0] 		= $name;		// overwrite 0th column once!
			$tablematrix[$k+1][1+$shift]	= $data;
			$tablematrix[$k+1][2+$shift]	= 0;
        }
	}	// end $j loop
}		// end $t loop

// Find the max $Tcount line!
$maxcount = max($Tcount);

// Do all the "nicing" up, after all counts are known!
for( $t=3; $t>=1; $t-- )
{
	if( $Tcount[$t] == 0 ) continue; 						// skipping below

	$shift = ($t-1) * $table_shift;							//needed for some reason
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
