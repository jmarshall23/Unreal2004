<?php
//---List of mods this player played in the 3 timeframes!---
if( $included_get_nickname == 0 )	include "func/get_nickname.inc";

$DEBUG = 0;

//Initializing vars and defining table header
unset($tablematrix);					//since matrix is remembered, needs to be unset!
unset($data);
$entry			= 'bestofMods';
$title_pre 		= "";
$table_title	= $title_pre.$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$tablematrix[0] = $table_head_l[$entry];

// Get number of mods... skipping dropped mods like survival (official=2)!
$select	= 	"select mid, modcode from mod where official!=2 ORDER by mid";
$link	=	mysql_query( $select );
$count	=	mysql_num_rows($link);
			if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";

//Showing all the
for( $i=0; $i<$count; $i++ )
{
	$row		= mysql_fetch_row( $link );
	$mid		= $row[0];
	$modcode	= $row[1];
	$modname 	= translate($modcode,$db_lang[$db_lid]);

	for( $t=1; $t<=3; $t++ )
	{
 		// Find out if there is data for this mod at all
		$select = 	"select pid from player where timeFrame=$t and timeFrameNumber=$TFN[$t] and mid=$mid ";
    	$link2	=	mysql_query( $select );
		$count2	=	mysql_num_rows( $link2 );
					if( $DEBUG ) print "Found ".$count2." rows for \"$select\"<br>\n";

        if( $count2>0 )
        {
			$modlink	= "<a href=\"bestofID.php?modID=$mid\">$modname</a>";
			$data[$i][$t] = $modlink;
        }
        else
			$data[$i][$t] = $modname;
	}

	// $tablematrix[row][column]
	$tablematrix[$i+1][0] = $i+1;
	$tablematrix[$i+1][1] = $data[$i][1];	//Timeframe week
	$tablematrix[$i+1][2] = $data[$i][2];	//Timeframe month
	$tablematrix[$i+1][3] = $data[$i][3];	//Timeframe all
}


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
