<?php

$global_page = 'compare';
	include "inc/global_vars.inc";					//getting global vars
	include "inc/db_login.inc";						//access to db
	include "inc/table.inc";						//data tables need this
	include "inc/timeFrameNumbers.inc";				//accesses db and gets the current TFN[]s

if( $included_get_mapname == 0 )	include "func/get_mapname.inc";
if( $included_get_servername == 0 )	include "func/get_servername.inc";
if( $included_get_gametypes == 0 )	include "func/get_gametypes.inc";

$head_title	= $head_title_l[$global_page];			//getting the page title
$head_where	= $head_where_l[$global_page];			//setting the loctation path per page

	include "page_head.php";

$index_info  = $index_info_l[$global_page];			// the compare links
$index_info2 = $index_info_l2[$global_page];		// the tables themself

// update this to make use the table infos...
$index_links =
	"<a href=#pCompare>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pResults>$index_info[2]</a> $index_info[3]<p>";

//if( $showdata )
	for( $i=0; $i<(sizeof($index_info2)/3); $i++ )
	{
	    $r = $i*3;
		$index_links .= "<a href=#".$index_info2[$r].">".$index_info2[$r+1]."</a> ".$index_info2[$r+2]."<br>";
	}

$intro_title	= $intro_title_l[$global_page];		// Global vars, language
$intro_intro	= $intro_intro_l[$global_page];
$intro_text		= $intro_text_l[$global_page];
$intro_logo		= $intro_logo_l[$global_page];
$intro_button	= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";			// Generate index and intro boxes

$head_compare_mods		= array();					// Will be read from DB...


// Get a current list of gametypes and mids from the DB
unset($head_gametypes);	unset($head_mids);
$head_gametypes = $head_allgametypes;				// Allgametypes, dummy, first entry!
$head_mids[0]	='';	
get_gametypes( $head_gametypes, $head_mids, $db_lang[$db_lid] ); 	


?>
<!- ---DATA TABLES---------------------------------------------------------- -->
<p>

<?php
	$info_text   = $info_text_l[$global_page];
	include "inc/table_info.inc";				//generate info box

	include "inc/compare_setup.inc";			//setting up compare / gadgets
?>
<p>

<!-- ------------------------------------------------------------------ -->

<?php
	// Only show tables if the playerIDs are both valid
	if( $showdata )
	{
		$faq = $faq_l[$global_page];
		for( $i=0; $i<(sizeof($index_info2)/3); $i++ )
		{
    		$f = $i*3;
			print "<a name=$index_info2[$f]></a>\n";
			$mode_compare=$i;
			include "stats/table_compare.inc";
			print "<p>\n";
		}				
	}
?>


<!- ---FOOTER--------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>