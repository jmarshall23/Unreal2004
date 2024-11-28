<?php
//---Show the best of data---
if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";
if( $included_tablematrix_colintensity == 0 )	include "func/tablematrix_colintensity.inc";
if( $included_get_nickname == 0 )				include "func/get_nickname.inc";
if( $included_get_modname == 0 )				include "func/get_modname.inc";

$DEBUG		= 0;
$MODE		= $mode_bestofData;
$mid 		= $modID;

if( $mid > 0 )
{
	$where = "and mid=$mid";
	$title_pre = "<span class=blue>".get_modname($mid)."</span><br>";
}
else
{
	$where = "";						//all Players (also unset cases!)
	$title_pre = "";
}
$limit = "limit ".$show_top_X;


//Initializing vars and defining table header
unset($tablematrix);
if( $MODE != 1 )
{
	$table_span		= array( 1, 4,0,0,0, 4,0,0,0, 4,0,0,0 );	//COLSPAN 4, 0 do not dump column!
	$table_shift	= 4;
}
else
{
	$table_span		= array( 1, 3,0,0, 3,0,0, 3,0,0 );
	$table_shift	= 3;
}
$table_blocks	= $head_timeframes;

if( $MODE == 0 ){ $entry = 'bFrags';		$field = 'frags';			}
if( $MODE == 1 ){ $entry = 'bFPH';			$field = 'FPH';				}	// needs manual overriding
if( $MODE == 2 ){ $entry = 'bSuicides';		$field = 'suicides';		}
if( $MODE == 3 ){ $entry = 'bScores';		$field = 'totalScore';		}
if( $MODE == 4 ){ $entry = 'bEvents';		$field = 'events';			}
if( $MODE == 5 ){ $entry = 'bMultikills';	$field = 'multikills';		}
if( $MODE == 6 ){ $entry = 'bSprees';		$field = 'sprees';			}
if( $MODE == 7 ){ $entry = 'bMinutes';		$field = 'playerMinutes';	}
if( $MODE == 8 ){ $entry = 'bConnects';		$field = 'playerConnects';	}
if( $MODE == 9 ){ $entry = 'bPlace1st';		$field = 'place1st';		}

$table_title	= $title_pre." ".$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$table_total	= $table_total_l[$entry];

// Generate table header line for all 3 timezones...
$head = $table_head_l[$entry];
$h=0;
$tablematrix[0][0] = $head[$h];
for( $t=1; $t<=3; $t++ )
{
	for( $i=1; $i<sizeof($head); $i++ )
	{
		$h++;
		$tablematrix[0][$h] .= $head[$i];
	}
}

// This gets all playerids
for( $t=1; $t<=3; $t++ )
{
	if( $MODE == 1 )
		$select	= 	"select playerid, ( sum(frags) * (60/sum(playerMinutes)) ) as $field, sum(playerMinutes) as pmin from players, player ".
					"where players.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] ".
					"$where group by playerid having $field<1000 and pmin>30 order by $field DESC $limit";
	else
		$select	= 	"select playerid, sum($field) as s$field from players, player ".
					"where players.pid=player.pid and timeFrame=$t and timeFrameNumber=$TFN[$t] ".
					"$where group by playerid having s$field>0 order by s$field DESC $limit";
	$link	= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$count	= mysql_num_rows($link);
	if( $count==0) continue;		// No data, so skip below.

	for( $j=0; $j<$count; $j++ )
	{
		$row	= mysql_fetch_row( $link );
        $id		= $row[0];		// playerid
        $data	= $row[1];
        $data2	= $row[2];  	//**not used ?

		if( $MODE == 1 ) $data = sprintf("%d",$data);	//INT()

        $nickname 	= get_nickname($id);
		$shift 		= ($t-1) * $table_shift;
		//if( $t==1 ) $tablematrix[$j+1][0+$shift] = $j+1;
		$tablematrix[$j+1][0] = $j+1;				//overwrite 0th column several times!

	    $namelink 	= "<a href=\"playerID.php?playerID=$id&modID=-1\">$nickname</a>";
	    $plidlink	= "<a href=\"playerID.php?playerID=$id&modID=-1\">$id</a>";
		$tablematrix[$j+1][1+$shift] = $namelink;
		$tablematrix[$j+1][2+$shift] = $plidlink;
		$tablematrix[$j+1][3+$shift] = $data;
		if( $MODE != 1 )
			$tablematrix[$j+1][4+$shift] = 0;			//percent values go here!
	}

	$shift 			= ($t-1) * $table_shift;		//needed for some reason

	unset($sumcols);	unset($highlight);	unset($percentcols);
	$sumcols		= array( 3+$shift			);
	$highlight		= $sumcols;
	if( $MODE != 1 )
		$percentcols	= array( 3+$shift,4+$shift	);

    // Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $count, $sumcols, $percentcols, $table_total );

	// Intensity range to columns (run before colhighlight())
	tablematrix_colintensity($tablematrix, 0+1, $count, $sumcols );

	// Enhancing certain columns, e.g. highlithing 'em.
	tablematrix_colhighlight($tablematrix, 0, $count+1, $highlight );

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
