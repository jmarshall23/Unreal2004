<?php
//  MATCHES  page strings   (lang/0_matches.inc)
// ---------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'matches'

$head_title_l[$entry]	=	"Matches Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[3]";	//Location path: Home/Matches

$index_info_l[$entry]	= array("Deathmatch",		"- Pure matches",
								"Team Deathmatch",	"",
								"Capture The Flag",	"",
								"Double Domination","",
								"Bombing Run",		"",
								"DM Other",			"- Matches + Mutators",
								"TDM Other",		"",
								"CTF Other",		"",
								"DD Other",			"",
								"BR Other",			"",
								"Custom Other",		"- Matches"
								);

$intro_title_l[$entry]	=	"Last Match Owned?";

$intro_intro_l[$entry]	=	"...look up the details and find out!";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Just finished a game and want to find out how you and the other ".
							"folks did in that match? This page shows the most current matches ".
							"for all gametypes. ";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";
$intro_button_l[$entry]	=	"b_".$global_page.".gif";




//...Info on Recent Matches...
$info_text_l[$entry] 	=	"Note: Stats can only be shown for servers that have world stats tracking activated.";

$info_text_l2[$entry] 	=	"Tip: On the client you can look up the current game's ".
							"<span class=blue>"."MatchID"."</span> ".
							"by checking out the score board.";


//...Table matchesPlayed...
$title_post				= "<br>"."Recent Matches";
$main_table_headers		= array("N&deg;",	"MatchID",	"Server",	"Map",		"C",	"Begin",
								"End",		"Frags",	"Kills",	"Suicides",	"Score" );

$pre_text_pure			= "Pure, mutator-free ";	//...Deathmatch...
$post_text_pure			= " matches. "."<br>".
						  "Filter: Only showing matches with Connects>1 and Score>0. C=Connects.";

$pre_text_nonpure		= "Non-pure, ";  			//...DM Other...
$post_text_nonpure		= " matches running mutators."."<br>".
						  "Filter: Only showing matches with Connects>1 and Score>0. C=Connects.";

$pre_text_other			= "Non-retail ";			//...Custom Other...
$post_text_other		= " matches. Any combination of mods and mutators. Just tracking basic Deathmatch infos."."<br>".
						  "Filter: Only showing matches with Connects>1 and Score>0. C=Connects.";



// -------------------------------------------------------------------
// CODING - Generates texts from above arrays

$entry = 'matchesPlayed_DM';
$table_title_l[$entry]	=	$index_info_l[$global_page][0].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][0].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_TDM';
$table_title_l[$entry]	=	$index_info_l[$global_page][2].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][2].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_CTF';
$table_title_l[$entry]	=	$index_info_l[$global_page][4].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][4].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_DD';
$table_title_l[$entry]	=	$index_info_l[$global_page][6].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][6].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_BR';
$table_title_l[$entry]	=	$index_info_l[$global_page][8].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][8].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

//---Other---
$entry = 'matchesPlayed_DMO';
$table_title_l[$entry]	=	$index_info_l[$global_page][10].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][10].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_TDMO';
$table_title_l[$entry]	=	$index_info_l[$global_page][12].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][12].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_CTFO';
$table_title_l[$entry]	=	$index_info_l[$global_page][14].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][14].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_DDO';
$table_title_l[$entry]	=	$index_info_l[$global_page][16].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][16].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_BRO';
$table_title_l[$entry]	=	$index_info_l[$global_page][18].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][18].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'matchesPlayed_O';
$table_title_l[$entry]	=	$index_info_l[$global_page][20].$title_post;
$table_text_l[$entry]	=	$pre_text_other.$index_info_l[$global_page][20].$post_text_other;
$table_head_l[$entry]	=	$main_table_headers;


?>