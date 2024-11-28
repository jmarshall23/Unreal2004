<?php

$global_page = 'mapID';
	include "inc/global_vars.inc";			//getting global vars
	include "inc/db_login.inc";			//access to db
	include "inc/table.inc";			//data tables need this

$head_title	= $head_title_l[$global_page];		//getting the page title
$head_where	= $head_where_l[$global_page].$mapID;

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links =
	"<a href=#pMapSummary>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pMapServers>$index_info[2]</a> $index_info[3]<br>".
	"<a href=#pMapMods>$index_info[4]</a> $index_info[5]<br>".
	"<a href=#pMapMatches>$index_info[6]</a> $index_info[7]";

$intro_title = $intro_title_l[$global_page];	// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!-- ---DATA TABLES--------------------------------------------------------- -->
<p>


<a name=pMapSummary></a>
<?php
	$mode_Data=1;						// details on this mapid
	include "stats/table_Data.inc";
?>
<p>

<a name=pMapServers></a>
<?php
	$mode_Data=2;						// servers running this mapid
	include "stats/table_Data.inc";
?>
<p>

<a name=pMapMods></a>
<?php
	$mode_Data=3;						// mods running this mapid
	include "stats/table_Data.inc";
?>
<p>

<!- ---ALL RECENT MATCHES--- -->

<a name=pMapMatches></a>
<?php
	$mode_matchesPlayed=12;				//All recent Matches
	include "stats/table_matchesPlayed.inc";
?>
<p>


<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>