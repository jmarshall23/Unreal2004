<?php
//  PLAYERIDPLAYER  page strings   (lang/0_playerIDplayer.inc)
// ----------------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'playerIDplayer'

$head_title_l[$entry]	=	"PlayerID Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[2]/$head_nav_names[10]=";	//Location path: Home/Players/PlayerID=666 get playerID appended


$index_info_l[$entry]	= array("Gametype",	"stats (DM,CTF,...)",
								"Summary",	"- K,D,S,F,E,...",  		//Short for Kills, Deaths, Suicides, Frags, Events
								"Kills",	"/ Deaths per weapon",
								"Suicides",	"per type",
								"Events",	"- non-weapon related",
								"Scores",	"- totals per event",
								"Mutators",	"- game variations",
								"Maps",		"- where the action is",
								"Mods",		"- gametypes",
								"Matches",	"- recently played"
								);


$intro_title_l[$entry]	=	" ";	//###Player nick name is prepended, playerID appended e.g. AEon (666)

$intro_intro_l[$entry]	=	"Find someone's stats with the <span class=blue>Search</span> at the top of the page.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Details on this player's overall performance.<br>".
							"Be it ranking, kills, deaths, suicides, events, mutators, maps played, mods preferred, ...";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";	//i_logo_players.gif

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"Find your favorite info right here! :)";



//...Table playerIDSummary...
$entry = 'playerIDSummary';

$table_title_l[$entry]	=	"Summary";

$table_text_l[$entry]	=	"All your data summed up here, for all gametypes.";

$table_head_l[$entry]	= array("Total", "Week", "Month", "Alltime" );

$table_rows_l[$entry]	= array("Kills",				// in players
								"Suicides",
								"Deaths",
								"Frags",
								"Average Efficiency",

								"Weapon Kills (primary)",
								"Weapon Kills (secondary)",
								"Weapon Deaths (primary)",
								"Weapon Deaths (secondary)",

								"Suicides Weapon related",
								"Suicides Environment related",

								"Events",
								"Multikills (all)",
								"Sprees (all)",
								"Player Score",			// in players

								"Player Minutes",		// in player
								"Player Connects",
								"Games Finished",
								"Placed 1st",
								"Placed 2nd",
								"Placed 3rd",
								"Placed lower",

								"Team Scores",
								"Team Kills",
								"Team Deaths",
								"Team Wins",
    							"Team Losses",
								);

//...Table...
$entry = 'playeridLastMatches';
$table_title_l[$entry]	=	"Matches";
$table_text_l[$entry]	=	"List of the your most recent matches, for this gametype.<br>".
							"PScore=Is the your score in this match.<br>";
$table_head_l[$entry]	= array("N&deg;",	"MatchID", 	"Mod",		"Map",		
											"Begin",	"Uptime[m]",
											"PScore", 
											"Server", "ServerID" );
$table_total_l[$entry]	=	"";


//...Table...
$entry = 'playerIDMods';

$table_title_l[$entry]	=	"Gametype Stats";		//e.g. AEon's Gametype Stats

$table_text_l[$entry]	=	"How you have been doing in specific gametypes. If there is no ".
							"link available then you did not play this gametype in the given ".
							"time frame.";

$table_head_l[$entry]	= array("N&deg;", "Week", "Month", "Alltime" );



//---Using global pages infos, for playerIDplayer pages as well---
// Without the mutators

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


//...Table globalKDSESc_Sc...
$entry = 'globalKDSESc_Sc';

$table_title_l[$entry]	=	"Scores";

$table_text_l[$entry]	=	"Showing how often each score type was triggered.";

$table_head_l[$entry]	= array("Type", "Score", "Sc%", "Counts", "C%",
										"Score", "Sc%", "Counts", "C%",
										"Score", "Sc%",	"Counts", "C%" 	);

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_Mu...
$entry = 'globalKDSESc_Mu';

$table_title_l[$entry]	=	"Mutators";

$table_text_l[$entry]	=	"Overall mutator use counts.";

$table_head_l[$entry]	= array("Name", 	"Mutator", "Mu%",
											"Mutator", "Mu%",
											"Mutator", "Mu%" );

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_M...
$entry = 'globalKDSESc_M';

$table_title_l[$entry]	=	"Maps";

$table_text_l[$entry]	=	"Overall favorite maps joined.";

$table_head_l[$entry]	= array("Map Name",	"Map", "M%",
											"Map", "M%",
											"Map", "M%" );

$table_total_l[$entry]	=	"(Total)";


//...Table globalKDSESc_Mo...
$entry = 'globalKDSESc_Mo';

$table_title_l[$entry]	=	"Mods";

$table_text_l[$entry]	=	"Overall favorite gametypes joined.";

$table_head_l[$entry]	= array("Gametype", "Mod", "Mod%",
											"Mod", "Mod%",
											"Mod", "Mod%" );

$table_total_l[$entry]	=	"(Total)";




// -------------------
?>