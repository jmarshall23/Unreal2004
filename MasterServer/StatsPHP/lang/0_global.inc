<?php
//  GLOBAL  page strings   (lang/0_global.inc)
// --------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'global'

$head_title_l[$entry]	=	"Global Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[1]";	//Location path: Home/Global

$index_info_l[$entry]	= array( // "Basic",	"- servers, times",
								"Summary",	"- K,D,S,F,E,...",  		//Short for Kills, Deaths, Suicides, Frags, Events
								"Kills",	"/ Deaths per weapon",
								"Suicides",	"per type",
								"Events",	"- non-weapon related",
								"Mutators",	"- game types",
								"Scores",	"- totals per event"
								);

$intro_title_l[$entry]	=	"Stats big time...";

$intro_intro_l[$entry]	=	"...a summary of world-wide happenings.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"The grand total in kills, frags, deaths, suicides, player connects, ".
							"maps played and much more info is summarized on this page.<p>".
							"Wondering how many folks out there are pounding UT2003?<br>".
							"Find out for the current week, current month and alltime!";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";


$info_text_l[$entry] 	=	"Note: Only pure servers running the standard UT2003 gametypes ".
							"(DM, TDM, CTF, BR, DD), with no mutators turned on, are shown here.";



//...Table globalSummary...
$entry = 'globalSummary';

$table_title_l[$entry]	=	"Summary";

$table_text_l[$entry]	=	"Here you will find all the grand, world-wide, of this planet ;) totals in ".
							"frags, deaths, suicides, events, times, etc.";

$table_head_l[$entry]	= array("Total", "Week", "Month", "Alltime" );

$table_rows_l[$entry]	= array("Kills",
								"Suicides",
								"Frags",
								"Teamkills",
								"Efficiency",
								"Frags per Hour",
								"Different Players",
								"Player Minutes",
								"Player Connects",
								"Player Scores",
								"Players per Game",
								"Games Played",
								"Games Finished",
								"Games per Day",
								"Different Maps",
								"Map Changes",
								"Events",
								"Mutators",
								"Mods",
								"Different Servers",
								"Server Connects",
								"Server Uptime"
								 );

//...Table globalKDSESc_KD...
$entry = 'globalKDSESc_KD';

$table_title_l[$entry]	=	"Weapon Kills/Deaths";

$table_text_l[$entry]	=	"For each in-game kill/death there is information about the weapons the ".
							"players held in hand. The kill weapon belongs to the killer, the death ".
							"weapon belongs to the victim.";

$table_head_l[$entry]	= array("Weapon",	"Kills", "K%", "Deaths", "D%",
											"Kills", "K%", "Deaths", "D%",
											"Kills", "K%", "Deaths", "D%" );

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_S...
$entry = 'globalKDSESc_S';

$table_title_l[$entry]	=	"Suicides";

$table_text_l[$entry]	=	"The table shows all the suicides, not only those related to weapons ".
							"(e.g. splash damage) but also environment induced deaths (e.g. lava).";

$table_head_l[$entry]	= array("Cause", 	"Suicides", "S%",
											"Suicides", "S%",
											"Suicides", "S%" );

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_E...
$entry = 'globalKDSESc_E';

$table_title_l[$entry]	=	"Events";

$table_text_l[$entry]	=	"All special events are shown here.";

$table_head_l[$entry]	= array("Type", 	"Events", "E%",
											"Events", "E%",
											"Events", "E%" );

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_Mu...
$entry = 'globalKDSESc_Mu';

$table_title_l[$entry]	=	"Mutators";

$table_text_l[$entry]	=	"Overall mutator use counts.";

$table_head_l[$entry]	= array("Type", 	"Mutator", "Mu%",
											"Mutator", "Mu%",
											"Mutator", "Mu%" );

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_Sc...
$entry = 'globalKDSESc_Sc';

$table_title_l[$entry]	=	"Scores";

$table_text_l[$entry]	=	"Showing how often each score type was triggered.";

$table_head_l[$entry]	= array("Type", "Score", "Sc%", "Counts", "C%",
										"Score", "Sc%", "Counts", "C%",
										"Score", "Sc%",	"Counts", "C%" 	);

$table_total_l[$entry]	=	"(Total)";

?>