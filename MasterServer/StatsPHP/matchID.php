<?php

if( $playerID > 0 )	$global_page = 'matchIDplayer';
else				$global_page = 'matchIDmatch';

	include "inc/global_vars.inc";						//getting global vars
	include "inc/db_login.inc";							//access to db
	include "inc/table.inc";							//data tables need this
	include "inc/timeFrameNumbers.inc";					//accesses db and gets the current TFN[]s

if( $included_get_nickname == 0 )				include "func/get_nickname.inc";
if( $included_get_modname == 0 )				include "func/get_modname.inc";
if( $included_get_mapname == 0 )				include "func/get_mapname.inc";
if( $included_get_servername == 0 )				include "func/get_servername.inc";
if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";

$head_title	= $head_title_l[$global_page];				//getting the page title

if( $playerID > 0 )
{
	$head_where	= $head_where_l[$global_page].$matchID."/$head_nav_names[10]=".$playerID;
}
else
	$head_where	= $head_where_l[$global_page].$matchID;	//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];				// global var, language


// Will have to check that URL parameters are actually set.
if( $playerID > 0 )
{
	$index_links =
		"<a href=#pMatchInfo>$index_info[0]</a> $index_info[1]<br>".
		"<a href=#pSummary>$index_info[2]</a> $index_info[3]<br>".
		"<a href=#pFrags>$index_info[4]</a> $index_info[5]<br>".
		"<a href=#pSuicides>$index_info[6]</a> $index_info[7]<br>".
		"<a href=#pEvents>$index_info[8]</a> $index_info[9]<br>".
		"<a href=#pScores>$index_info[10]</a> $index_info[11]<br>".
		"<a href=#pEnemies>$index_info[12]</a> $index_info[13]<br>".
		"<a href=#pFragBait>$index_info[14]</a> $index_info[15]";
}
else
{
	$index_links =
		"<a href=#pDetails>$index_info[0]</a> $index_info[1]<br>".
		"<a href=#pSummary>$index_info[2]</a> $index_info[3]<br>".
		"<a href=#pScoreBoard>$index_info[4]</a> $index_info[5]<br>".
		"<a href=#pFrags>$index_info[6]</a> $index_info[7]<br>".
		"<a href=#pSuicides>$index_info[8]</a> $index_info[9]<br>".
		"<a href=#pEvents>$index_info[10]</a> $index_info[11]<br>".
		"<a href=#pScores>$index_info[12]</a> $index_info[13]<br>".
		"<a href=#pMutators>$index_info[14]</a> $index_info[15]<br>".
		"<a href=#pGameRules>$index_info[16]</a> $index_info[17]<br>";
}


//###Problem with long names!!!!
//$intro_title = "<span class=nick>".$nickname."</span>".$intro_title_l[$global_page]."($playerID)";		// global vars, language
if( $playerID > 0 )
	$intro_title = "<span class=blue>".get_nickname($playerID)."</span>".$head_title_possessive;
else
	$intro_title = "<span class=blue>".get_modname($modID)."</span>";
$intro_intro	= $intro_intro_l[$global_page];
$intro_text		= $intro_text_l[$global_page];
$intro_logo		= $intro_logo_l[$global_page];
$intro_button	= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>

<!- ---DATA TABLES---------------------------------------------------------- -->
<p>


<?php
// Loading the appropritate page
include $global_page.".php";					//$global_page

?>

<!- ---FOOTER--------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>


