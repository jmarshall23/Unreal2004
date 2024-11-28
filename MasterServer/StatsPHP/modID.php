<?php

$global_page = 'modID';
	include "inc/global_vars.inc";			//getting global vars
	include "inc/db_login.inc";			//access to db
	include "inc/table.inc";			//data tables need this

$head_title	= $head_title_l[$global_page];		//getting the page title
$head_where	= $head_where_l[$global_page].$modID;

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links =
	"<a href=#pModSummary>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pModMaps>$index_info[2]</a> $index_info[3]<br>".
	"<a href=#pModServers>$index_info[4]</a> $index_info[5]<br>".
	"<a href=#pModMatches>$index_info[6]</a> $index_info[7]";

$intro_title = $intro_title_l[$global_page];	// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!-- ---DATA TABLES--------------------------------------------------------- -->
<p>


<a name=pModSummary></a>
<?php
	$mode_Data=9;						// details on this serverid
	include "stats/table_Data.inc";
?>
<p>

<a name=pModMaps></a>
<?php
	$mode_Data=10;						// servers running this mapid
	include "stats/table_Data.inc";
?>
<p>

<a name=pModServers></a>
<?php
	$mode_Data=11;						// servers running this mid
	include "stats/table_Data.inc";
?>
<p>

<!- ---ALL RECENT MATCHES--- -->

<a name=pModMatches></a>
<?php
	$mode_matchesPlayed=13;				//All recent Matches
	include "stats/table_matchesPlayed.inc";
?>
<p>



<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>