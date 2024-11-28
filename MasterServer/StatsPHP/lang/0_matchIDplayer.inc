<?php
//  MATCHIDPLAYER  page strings   (lang/0_matchIDplayer.inc)
// ---------------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'matchIDplayer'

$head_title_l[$entry]	=	"Match Player";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[2]/$head_nav_names[14]=";	//Location path: Home/Players/MatchID=666 gets MatchID appended

$index_info_l[$entry]	= array("Match",		"- Infos",
								"Player",		"- Summary",
								"Kills",		"/ Deaths per weapon",
								"Suicides",		"per type",
								"Events",		"- non-weapon related",
								"Scores",		"- totals per event",
								"Enemies",		"- Kills against you",
								"Frag Bait",	"- Kills you did",
								);


$intro_title_l[$entry]	=	" ";	//###Player nick name is prepended, playerID appended e.g. AEon (666)

$intro_intro_l[$entry]	=	"...fetes and contributions to this match.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Check out all your gory stats for this match, right here!";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"Find your favorite info right here! :)";



//...Table matchIDSummary...
$entry = 'matchIDSummary';

$table_title_l[$entry]	=	"Match Info";
$table_title_l2[$entry]	=	"Player Summary";

$table_text_l[$entry]	=	"Match data at one glance.";
$table_text_l2[$entry]	=	"Player details for this match at one glance.<br>".
							"MRank=Match Rank, RGain=Alltime rank points you 'gained' in this match/gametype.";

$table_head_l[$entry]	= array( "MatchID",		"Server",	"ServerID",		"Map",
								 "Connects",	"Begin",	"Game Time[m]",	"Map Change" );

$table_head_l2[$entry]	= array( "Player",	"PlayerID",	"Player Time[m]",	"Score",	"MRank",	"RGain",
								 "Frags",	"Kills", 	"Deaths",			"Suicides",	"TKs",	"Events" );


//---Using global pages infos, for playerIDplayer pages as well---
//...Table matchidData_KD...
$entry = 'matchidData_KD';
$table_title_l[$entry]	=	"Weapon Kills/Deaths";
$table_text_l[$entry]	=	"For each in-game kill/death there is information about the weapons the ".
							"players held in hand. The kill weapon belongs to the killer, the death ".
							"weapon belongs to the victim.";
$table_head_l[$entry]	= array("Weapon", "Kills", "K%", "Deaths", "D%" );
$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_S...
$entry = 'matchidData_S';
$table_title_l[$entry]	=	"Suicides";
$table_text_l[$entry]	=	"The table shows all the suicides, not only those related to weapons ".
							"(e.g. splash damage) but also environment induced deaths (e.g. lava).";
$table_head_l[$entry]	= array("Cause", "Suicides", "S%" );
$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_E...
$entry = 'matchidData_E';

$table_title_l[$entry]	=	"Events";

$table_text_l[$entry]	=	"All special events are shown here.";

$table_head_l[$entry]	= array("Type",	"Events", "E%" );

$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_Sc...
$entry = 'matchidData_Sc';

$table_title_l[$entry]	=	"Scores";

$table_text_l[$entry]	=	"Showing how often each score type was triggered.";

$table_head_l[$entry]	= array( "Type", "Score", "Sc%", "Counts", "C%" );

$table_total_l[$entry]	=	"(Total)";



//...Table...
$entry = 'matchidData_En';

$table_title_l[$entry]	=	"Enemies";

$table_text_l[$entry]	=	"The folks that hit on you the most in kills.";

$table_head_l[$entry]	= array( "N&deg;",	"Player", "PlayerID", "Frags", "F%" );

$table_total_l[$entry]	=	"";


//...Table...
$entry = 'matchidData_Vi';

$table_title_l[$entry]	=	"Frag Bait";

$table_text_l[$entry]	=	"The folks you really clobbered in kills.";

$table_head_l[$entry]	= array( "N&deg;",	"Player", "PlayerID", "Frags", "F%" );

$table_total_l[$entry]	=	"";




?>
