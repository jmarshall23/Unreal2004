<?php

$global_page = 'main';
	include "inc/global_vars.inc";				//getting global vars
	include "inc/db_login.inc";					//access to db
	include "inc/table.inc";					//data tables need this
	include "inc/timeFrameNumbers.inc";			//accesses db and gets the current TFN[]s

$head_title	= $head_title_l[$global_page];		//getting the page title
$head_where	= $head_where_l[$global_page];		//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links = "";
for( $i=0; $i<(sizeof($head_nav_pages)); $i++ )
{
	$index_links .= "<a href=".$head_nav_pages[$i].">".$index_info[$i*2]."</a> ".$index_info[$i*2+1]."<br>";
}

$index_links .= "<br>";
$index_links .= "<a href=#News>".$index_info[22]."</a> ".$index_info[23]."<br>";
//$index_links .= "<a href=#Top10>".$index_info[24]."</a> ".$index_info[25]."<br>";


$intro_title = $intro_title_l[$global_page];	// global vars, language
$intro_text  = $intro_text_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<p>

<!-- ---N E W S--------------------------------------------------------- -->
<a name=News></a>
<?php
	$info_text   = $info_text_l2[$global_page];
	include "inc/table_info.inc";				//generate info box
?>
<p>

<!-- 

<a name=Top10></a>
<?php
	$info_text   = $info_text_l[$global_page];
	include "inc/table_info.inc";				//generate info box
?>
<p>

-->

<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>