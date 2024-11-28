<?php

$global_page = 'bestofID';
	include "inc/global_vars.inc";				//getting global vars
	include "inc/db_login.inc";					//access to db
	include "inc/table.inc";					//data tables need this
	include "inc/timeFrameNumbers.inc";			//accesses db and gets the current TFN[]s

$head_title	= $head_title_l[$global_page];		//getting the page title
$head_where	= $head_where_l[$global_page].$modID;

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links = "";
for( $i=1; $i<(sizeof($index_info)/3); $i++ )	// skipping 0 to stay compatible with bestof.php
{
    $r = $i*3;
	$index_links .= "<a href=#".$index_info[$r].">".$index_info[$r+1]."</a> ".$index_info[$r+2]."<br>";
}

$intro_title = $intro_title_l[$global_page];	// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!-- ---DATA TABLES--------------------------------------------------------- -->
<p>

<?php
//--- Generating the best ofs for the 3 timezones ---
for( $f=1; $f<(sizeof($index_info)/3); $f++ )
{
	$j = $f*3;
	print "<a name=$index_info[$j]></a>\n";
	$mode_bestofData=$f-1;
	include "stats/table_bestofData.inc";
	print "<p>\n";
}
?>


<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>
