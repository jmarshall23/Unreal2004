<?php

$global_page = 'matches';
	include "inc/global_vars.inc";			//getting global vars
	include "inc/db_login.inc";				//access to db
	include "inc/table.inc";				//data tables need this
	include "inc/timeFrameNumbers.inc";		//accesses db and gets the current TFN[]s

if( $included_get_mapname == 0 )	include "func/get_mapname.inc";
if( $included_get_servername == 0 )	include "func/get_servername.inc";

$head_title	= $head_title_l[$global_page];	//getting the page title
$head_where	= $head_where_l[$global_page];	//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];	// global var, language

$index_links =
	"<a href=#pDM>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pTDM>$index_info[2]</a> $index_info[3]<br>".
	"<a href=#pCTF>$index_info[4]</a> $index_info[5]<br>".
	"<a href=#pDD>$index_info[6]</a> $index_info[7]<br>".
	"<a href=#pBR>$index_info[8]</a> $index_info[9]<br>".
	"<a href=#pDMO>$index_info[10]</a> $index_info[11]<br>".
	"<a href=#pTDMO>$index_info[12]</a> $index_info[13]<br>".
	"<a href=#pCTFO>$index_info[14]</a> $index_info[15]<br>".
	"<a href=#pDDO>$index_info[16]</a> $index_info[17]<br>".
	"<a href=#pBRO>$index_info[18]</a> $index_info[19]<br>".
	"<a href=#pO>$index_info[20]</a> $index_info[21]<br>";

$intro_title = $intro_title_l[$global_page];		// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!- ---DATA TABLES---------------------------------------------------------- -->
<p>

<?php
	$info_text   = $info_text_l[$global_page];
	include "inc/table_info.inc";			//generate info box
?>
<p>

<a name=pDM></a>
<?php
	$mode_matchesPlayed=0;					//DM
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pTDM></a>
<?php
	$mode_matchesPlayed=1;					//TDM
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pCTF></a>
<?php
	$mode_matchesPlayed=2;					//CTF
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pDD></a>
<?php
	$mode_matchesPlayed=3;					//DD
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pBR></a>
<?php
	$mode_matchesPlayed=4;					//BR
	include "stats/table_matchesPlayed.inc";
?>
<p>


<!- ---OTHER GAMETYPES--- -->
<pre>

</pre>
<a name=pDMO></a>
<?php
	$mode_matchesPlayed=5;					//DM
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pTDMO></a>
<?php
	$mode_matchesPlayed=6;					//TDM
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pCTFO></a>
<?php
	$mode_matchesPlayed=7;					//CTF
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pDDO></a>
<?php
	$mode_matchesPlayed=8;					//DD
	include "stats/table_matchesPlayed.inc";
?>
<p>

<a name=pBRO></a>
<?php
	$mode_matchesPlayed=9;					//BR
	include "stats/table_matchesPlayed.inc";
?>
<p>


<!- ---OTHER GAMETYPES--- -->
<pre>

</pre>
<a name=pO></a>
<?php
	$mode_matchesPlayed=10;					//O
	include "stats/table_matchesPlayed.inc";
?>
<p>

<pre>

</pre>
<?php
	$info_text   = $info_text_l2[$global_page];
	include "inc/table_info.inc";			//generate info box
?>
<p>

<!- ---FOOTER--------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>