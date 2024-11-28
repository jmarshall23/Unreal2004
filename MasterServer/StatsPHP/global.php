<?php

$global_page = 'global';
	include "inc/global_vars.inc";			//getting global vars
	include "inc/db_login.inc";				//access to db
	include "inc/table.inc";				//data tables need this
	include "inc/timeFrameNumbers.inc";		//accesses db and gets the current TFN[]s

if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";

$head_title	= $head_title_l[$global_page];	//getting the page title
$head_where	= $head_where_l[$global_page];	//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];			// global var, language

$index_links =
//	"<a href=#pBasic>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pSummary>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pFrags>$index_info[2]</a> $index_info[3]<br>".
	"<a href=#pSuicides>$index_info[4]</a> $index_info[5]<br>".
	"<a href=#pEvents>$index_info[6]</a> $index_info[7]<br>".
//	"<a href=#pItems>$index_info[10]</a> $index_info[11]<br>".
//	"<a href=#pMaps>$index_info[8]</a> $index_info[9]<br>".
	"<a href=#pScores>$index_info[10]</a> $index_info[11]<br>".
	"<a href=#pMutators>$index_info[8]</a> $index_info[9]<br>";

$intro_title = $intro_title_l[$global_page];		// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";			//generate index and intro boxes
?>
<!- ---DATA TABLES---------------------------------------------------------- -->
<p>



<a name=pSummary></a>
<?php
	include "stats/table_globalSummary.inc";
?>
<p>

<?php
$info_text   = $info_text_l[$global_page];
	include "inc/table_info.inc";			//generate info box
?>
<p>


<a name=pFrags></a>
<?php
	$mode_globalKDSESc=0;					//kills/deaths
	include "stats/table_globalKDSESc.inc";
?>
<p>


<a name=pSuicides></a>
<?php
	$mode_globalKDSESc=1;					//suicides
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pEvents></a>
<?php
	$mode_globalKDSESc=2;					//events
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pScores></a>
<?php
	$mode_globalKDSESc=3;					//scores
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pMutators></a>
<?php
	$mode_globalKDSESc=4;					//mutators
	include "stats/table_globalKDSESc.inc";
?>
<p>


<!- ---FOOTER--------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>