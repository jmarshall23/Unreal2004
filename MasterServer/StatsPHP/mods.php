<?php

$global_page = 'mods';
	include "inc/global_vars.inc";			//getting global vars
	include "inc/db_login.inc";			//access to db
	include "inc/table.inc";			//data tables need this

$head_title	= $head_title_l[$global_page];	//getting the page title
$head_where	= $head_where_l[$global_page];	//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links =
	"<a href=#pModsTop>$index_info[0]</a> $index_info[1]";


$intro_title = $intro_title_l[$global_page];		// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!-- ---DATA TABLES--------------------------------------------------------- -->
<p>


<a name=pModsTop></a>
<?php
	$mode_Data=8;							//Mods
	include "stats/table_Data.inc";
?>
<p>




<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>