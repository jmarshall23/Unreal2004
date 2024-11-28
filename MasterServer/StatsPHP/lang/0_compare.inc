<?php
//  MATCHES  page strings   (lang/0_matches.inc)
// ---------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'matches'

$head_title_l[$entry]	=	"Compare Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[18]";	//Location path: Home/Matches

$index_info_l[$entry]	= array("Compare",		"players by ID.",
								"Detailed",		"infos"
								);

$index_info_l2[$entry]	= array(
            //	Anquor			Link		Info
				"pKills",		"Kills",	"/ Deaths per weapon",		// mode = 0 ...
				"pSuicides",	"Suicides",	"per type",
				"pEvents",		"Events",	"- non-weapon related",
				"pScores",		"Scores",	"- totals per event",
				"pMutators",	"Mutators",	"- game variations",
				"pMaps",		"Maps",		"- where the action is",
				"pMods",		"Mods",		"- gametypes"
				);


$intro_title_l[$entry]	=	"Know your Enemy";

$intro_intro_l[$entry]	=	"...by analyzing his play data in detail!";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Wonder by which means exactly the other player scored more points? ".
							"Want to check if that player might be using 'better' weapons? ".
							"The Compare will uncover it all.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";
$intro_button_l[$entry]	=	"b_".$global_page.".gif";


//...Info on Recent Matches...
$info_text_l[$entry] 	=	"To compare players, look up their ".
							"<span class=blue>"."PlayerID"."</span>"."s ".  
							"and enter them in the compare fields below.";

//...3. COLSPAN HEADER OF the compare tables...
$compare_diff			=	"Difference:";  // 1. - 2.


//---Using global pages infos, for playerIDplayer pages as well---
// Without the mutators

//...Table...
$entry = 'compare_0';
$table_title_l[$entry]	=	"Weapon Kills/Deaths";
$table_text_l[$entry]	=	"For each in-game kill/death there is information about the weapons the ".
							"players held in hand. The kill weapon belongs to the killer, the death ".
							"weapon belongs to the victim.";
$table_head_l[$entry]	= array("Weapon",	"Kills", "K%", "Deaths", "D%",
											"Kills", "K%", "Deaths", "D%",
											"Kills", "K%", "Deaths", "D%" );
$table_total_l[$entry]	=	"(Total)";


//...Table...
$entry = 'compare_1';
$table_title_l[$entry]	=	"Suicides";
$table_text_l[$entry]	=	"The table shows all the suicides, not only those related to weapons ".
							"(e.g. splash damage) but also environment induced deaths (e.g. lava).";
$table_head_l[$entry]	= array("Cause", 	"Suicides", "S%",
											"Suicides", "S%",
											"Suicides", "S%" );
$table_total_l[$entry]	=	"(Total)";


//...Table...
$entry = 'compare_2';
$table_title_l[$entry]	=	"Events";
$table_text_l[$entry]	=	"All special events are shown here.";
$table_head_l[$entry]	= array("Type", 	"Events", "E%",
											"Events", "E%",
											"Events", "E%" );
$table_total_l[$entry]	=	"(Total)";


//...Table...
$entry = 'compare_3';
$table_title_l[$entry]	=	"Scores";
$table_text_l[$entry]	=	"Showing how often each score type was triggered.";
$table_head_l[$entry]	= array("Type", "Score", "Sc%", "Counts", "C%",
										"Score", "Sc%", "Counts", "C%",
										"Score", "Sc%",	"Counts", "C%" 	);
$table_total_l[$entry]	=	"(Total)";


//...Table...
$entry = 'compare_4';
$table_title_l[$entry]	=	"Mutators";
$table_text_l[$entry]	=	"Overall mutator use counts.";
$table_head_l[$entry]	= array("Name", 	"Mutator", "Mu%",
											"Mutator", "Mu%",
											"Mutator", "Mu%" );
$table_total_l[$entry]	=	"(Total)";


//...Table...
$entry = 'compare_5';
$table_title_l[$entry]	=	"Maps";
$table_text_l[$entry]	=	"Overall favorite maps joined.";
$table_head_l[$entry]	= array("Map Name",	"Map", "M%",
											"Map", "M%",
											"Map", "M%" );
$table_total_l[$entry]	=	"(Total)";


//...Table...
$entry = 'compare_6';
$table_title_l[$entry]	=	"Mods";
$table_text_l[$entry]	=	"Overall favorite gametypes joined.";
$table_head_l[$entry]	= array("Gametype", "Mod", "Mod%",
											"Mod", "Mod%",
											"Mod", "Mod%" );
$table_total_l[$entry]	=	"(Total)";





// ** NOT USED SOFAR

//...Table matchesPlayed...
$title_post				= "<br>"."Recent Matches";
$main_table_headers		= array("N&deg;",	"MatchID",	"Server",	"Map",		"Connects",	"Begin",
								"End",		"Frags",	"Kills",	"Suicides",	"Score" );

$pre_text_pure			= "Pure, mutator-free ";	//...Deathmatch...
$post_text_pure			= " matches.";

$pre_text_nonpure		= "Non-pure, ";  			//...DM Other...
$post_text_nonpure		= " matches running mutators.";

$pre_text_other			= "Non-retail ";			//...Custom Other...
$post_text_other		= " matches. Any combination of mods and mutators. Just tracking basic Deathmatch infos.";



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