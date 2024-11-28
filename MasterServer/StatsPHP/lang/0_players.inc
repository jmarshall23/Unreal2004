<?php
//  PLAYERS  page strings   (lang/0_players.inc)
// ---------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'players'

$head_title_l[$entry]	=	"Players Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[2]";	//Location path: Home/Players

$index_info_l[$entry]	= array("Deathmatch",		"- Top ranked",
								"Team Deathmatch",	"",
								"Capture The Flag",	"",
								"Double Domination","",
								"Bombing Run",		"",
								"DM Other",			"- Ranked + Mutators",
								"TDM Other",		"",
								"CTF Other",		"",
								"DD Other",			"",
								"BR Other",			"",
								"Custom Other",		"- Ranked mods"
								);

// write an explaination box...
// 1. rankings 2
// 2. the other...


$intro_title_l[$entry]	=	"Find Your Stats";

$intro_intro_l[$entry]	=	"...with the <span class=blue>Search</span> at the top of the page.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Are you ready for merciless ranking?"."<br>".
							"Want to check out how you have been doing in frags?"."<br>".
							"The UT2003 master server has been tracking your progress!";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";	//i_logo_players.gif
$intro_button_l[$entry]	=	"b_".$global_page.".gif";




//...Info on Top DM...
$info_text_l[$entry] 	=	"The retail version gametypes are DM, TDM, CTF, DD, and BR! ".
							"The games ranked here are pure games, e.g. no mutators where activated. ".
							"'Other' denotes non-pure games, see below.";

$info_text_l2[$entry] 	=	"Excepting DM, the other gametypes (TDM, CTF, DD, and BR) are all team games. ".
							"The ranking is based on the individuals team contributions, e.g. ".
							"capturing the flag or defending team members, and also frags.";

$info_text_l3[$entry] 	=	"The 'Other ...' gametypes here are also part of the retail game, only that ".
							"the games where run with mutators activated. This makes these games non-pure.";

// "Custom other"
$info_text_l4[$entry] 	=	"This '".$index_info_l[$global_page][20]."' gametype represents all the other custom mods out there, that where ".
							"not part of the retail game. Only very basic tracking is done for this 'gametype'.";



//...TABLE PLAYERSRANKED...
$title_post				= "<br>"."Top Players";
$main_table_headers		= array("N&deg;",	"Player", "PlayerID", "Rank",
											"Player", "PlayerID", "Rank",
											"Player", "PlayerID", "Rank" );

$pre_text_pure			= "Pure, mutator-free ";	//...Deathmatch...
$post_text_pure			= " ranking.";

$pre_text_nonpure		= "Non-pure, ";  			//...DM Other...
$post_text_nonpure		= " ranking running mutators.";

$pre_text_other			= "Non-retail ";			//...Custom Other...
$post_text_other		= " mods. Any combination of mods and mutators. Just tracking basic Deathmatch infos.";




// -------------------------------------------------------------------
// CODING - Generates texts from above arrays

$entry = 'playersRanked_DM';
$table_title_l[$entry]	=	$index_info_l[$global_page][0].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][0].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_TDM';
$table_title_l[$entry]	=	$index_info_l[$global_page][2].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][2].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_CTF';
$table_title_l[$entry]	=	$index_info_l[$global_page][4].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][4].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_DD';
$table_title_l[$entry]	=	$index_info_l[$global_page][6].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][6].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_BR';
$table_title_l[$entry]	=	$index_info_l[$global_page][8].$title_post;
$table_text_l[$entry]	=	$pre_text_pure.$index_info_l[$global_page][8].$post_text_pure;
$table_head_l[$entry]	=	$main_table_headers;

//---Other---
$entry = 'playersRanked_DMO';
$table_title_l[$entry]	=	$index_info_l[$global_page][10].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][10].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_TDMO';
$table_title_l[$entry]	=	$index_info_l[$global_page][12].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][12].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_CTFO';
$table_title_l[$entry]	=	$index_info_l[$global_page][14].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][14].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_DDO';
$table_title_l[$entry]	=	$index_info_l[$global_page][16].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][16].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_BRO';
$table_title_l[$entry]	=	$index_info_l[$global_page][18].$title_post;
$table_text_l[$entry]	=	$pre_text_nonpure.$index_info_l[$global_page][18].$post_text_nonpure;
$table_head_l[$entry]	=	$main_table_headers;

$entry = 'playersRanked_O';
$table_title_l[$entry]	=	$index_info_l[$global_page][20].$title_post;
$table_text_l[$entry]	=	$pre_text_other.$index_info_l[$global_page][20].$post_text_other;
$table_head_l[$entry]	=	$main_table_headers;

?>