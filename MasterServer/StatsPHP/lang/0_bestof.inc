<?php
//  BESTOF  page strings   (lang/0_bestof.inc)
// --------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'bestof'

$head_title_l[$entry]	=	"Best of Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[4]";	//Location path: Home/Global

$index_info_l[$entry]	= array(
						// Anquor		Link		Info
						"bGameType",	"Gametype",		"- Best Of pages",
						"bFrags",		"Frags",		"- Shreds 'em all",
						"bFPH",			"FPH",			"- Top quota fraggers",
						"bSuicides",	"Suicides",		"- Pro Self-killer",
						"bScores",		"Scores",		"- Goes for results",
						"bEvents",		"Events",		"- Does a lot",
						"bMultikills",	"Multikills",	"- Quick fraggers",
						"bSprees",		"Spress",		"- Consistent carnage",
						"bMinutes",		"Minutes",		"played",
						"bConnects",	"Connects",		"- Seen often",
						"bPlace1st",	"Placed 1st",	"in player scores",
						);

$intro_title_l[$entry]	=	"Best of the Best";

$intro_intro_l[$entry]	=	"...see if you are among them!";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Who fragged the most?<br>".
							"Who are the top scorers?<br>".
							"Who got the most captures?<br>".
							"Find out to what other extremes gamers out there went!";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";


//...Table...
$entry = 'bestofMods';
$table_title_l[$entry]	=		"Gametype Best Of";
$table_text_l[$entry]	=		"Here you can check out the Best Of pages for each gametype.";
$table_head_l[$entry]	= array("N&deg;", "Week", "Month", "Alltime" );

//...Table...
$entry = 'bFrags';
$table_title_l[$entry]	=		"Frags Best Of";
$table_text_l[$entry]	=		"The very best in Frags. F=Frags.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "F", "F%" );	// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bFPH';
$table_title_l[$entry]	=		"FPH Best Of";
$table_text_l[$entry]	=		"The very best in Frags Per Hour."."<br>".
								"Filter: Your total playtime must be more than 30 minutes.";	//and FPH < 1000 ;)
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "FPH" );	// 3 Times duplicated!
$table_total_l[$entry]	=		"";


//...Table...
$entry = 'bSuicides';
$table_title_l[$entry]	=		"Suicides Best Of";
$table_text_l[$entry]	=		"The very best in Suicides. S=Suicides.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "S", "S%" );	// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bScores';
$table_title_l[$entry]	=		"Scores Best Of";
$table_text_l[$entry]	=		"The very best in Scores. Sc=Scores.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "S", "Sc%" );	// 3 Times duplicated!
$table_total_l[$entry]	=	"";

//...Table...
$entry = 'bEvents';
$table_title_l[$entry]	=		"Events Best Of";
$table_text_l[$entry]	=		"The very best in Events. E=Events.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "E", "E%" );		// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bMultikills';
$table_title_l[$entry]	=		"Multikills Best Of";
$table_text_l[$entry]	=		"The very best in Multikills. Mk=Multikills.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "Mk", "Mk%" );	// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bSprees';
$table_title_l[$entry]	=		"Sprees Best Of";
$table_text_l[$entry]	=		"The very best in Sprees. Sp=Sprees.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "Sp", "Sp%" );		// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bMinutes';
$table_title_l[$entry]	=		"Player Minutes Best Of";
$table_text_l[$entry]	=		"The very best in Player Minutes. Mi=Minutes.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "Mi", "Mi%" );	// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bConnects';
$table_title_l[$entry]	=		"Player Connects Best Of";
$table_text_l[$entry]	=		"The very best in Player Connects. C=Connects.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "C", "C%" );	// 3 Times duplicated!
$table_total_l[$entry]	=		"";

//...Table...
$entry = 'bPlace1st';
$table_title_l[$entry]	=		"Placed 1st Best Of";
$table_text_l[$entry]	=		"The very best in Placed 1st.";
$table_head_l[$entry]	= array("N&deg;","Player","PlayerID", "1st", "Pl%" );		// 3 Times duplicated!
$table_total_l[$entry]	=		"";

?>