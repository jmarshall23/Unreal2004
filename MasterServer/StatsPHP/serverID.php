<?php

$global_page = 'serverID';
	include "inc/global_vars.inc";			//getting global vars
	include "inc/db_login.inc";			//access to db
	include "inc/table.inc";			//data tables need this

$head_title	= $head_title_l[$global_page];		//getting the page title
$head_where	= $head_where_l[$global_page].$serverID;

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links =
	"<a href=#pServerSummary>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pServerMaps>$index_info[2]</a> $index_info[3]<br>".
	"<a href=#pServerMods>$index_info[4]</a> $index_info[5]<br>".
	"<a href=#pServerMatches>$index_info[6]</a> $index_info[7]<br>".
	"<a href=#pServerGameRules>$index_info[8]</a> $index_info[9]";

$intro_title = $intro_title_l[$global_page];	// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!-- ---DATA TABLES--------------------------------------------------------- -->
<p>


<a name=pServerSummary></a>
<?php
	$mode_Data=5;						// details on this serverid
	include "stats/table_Data.inc";
?>
<p>

<a name=pServerMaps></a>
<?php
	$mode_Data=6;						// servers running this mapid
	include "stats/table_Data.inc";
?>
<p>

<a name=pServerMods></a>
<?php
	$mode_Data=7;						// servers running this mid
	include "stats/table_Data.inc";
?>
<p>


<!- ---ALL RECENT MATCHES--- -->

<a name=pServerMatches></a>
<?php
	$mode_matchesPlayed=11;				// All recent Matches
	include "stats/table_matchesPlayed.inc";
?>
<p>


<a name=pServerGameRules></a>   					
<?php
	$matchID_GameRules = 1;				// Gamerules
	include "stats/table_matchidGameRules.inc";
?>
<p>


<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>