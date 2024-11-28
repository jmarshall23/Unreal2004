<?php
//  MAPS  page strings   (lang/0_mapID.inc)
// ------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'mapID'

$head_title_l[$entry]	=	"MapID Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[6]/$head_nav_names[15]=";	//Location path: Home/Global

$index_info_l[$entry]	= array("Summary",	"- Details on this map",
								"Servers",	"that ran this map",
								"Mods",		"this map was played on",
								"Matches",	"that ran this map"
								 );

$intro_title_l[$entry]	=	"Environments";

$intro_intro_l[$entry]	=	"...the places you folks like to hang out.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"In depth infos in this map<br>".
							"Servers that ran it and mods this map was played on.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";



//...Table...
$entry = 'mapIDSummary';

$table_title_l[$entry]	=	"Map Summary";

$table_text_l[$entry]	=	"Details on this map.";

$table_head_l[$entry]	=	array(	"Map Name",		"Map Title",
									"Uptime[m]",	"Connects",		"Games",
									"Frags",		"Kills",		"Suicides",
									"Teamkills",	"Events",		"Official" );


//...Table...
$entry = 'mapIDServers';

$table_title_l[$entry]	=	"Map Servers";

$table_text_l[$entry]	=	"Servers that are running this map.";

$table_head_l[$entry]	=	array(	"Server",		"ServerID",
									"Uptime[m]",	"U%",
									"Connects",		"Games",
									"Frags",		"F%",
									"Kills",		"Suicides",
									"Teamkills",	"Events" );

//...Table...
$entry = 'mapIDMods';

$table_title_l[$entry]	=	"Map Mods";

$table_text_l[$entry]	=	"The gametypes that ran this map.";

$table_head_l[$entry]	=	array(	"Mod",			"ModID",
									"Uptime[m]",	"U%",
									"Connects",		"Games",
									"Frags",		"F%",
									"Kills",		"Suicides",
									"Teamkills",	"Events", "Official" );

//...Table...
$entry = 'mapIDMatches';
$table_title_l[$entry]	=	"Recent Matches";
$table_text_l[$entry]	=	"List of all the recently played matches that ran this map."."<br>".
							"Filter: Only showing matches with Connects>1 and Score>0. C=Connects.";
$table_head_l[$entry]	= array("N&deg;",	"MatchID",	"Server",	"Map",		"C",	"Begin",
								"End",		"Frags",	"Kills",	"Suicides",	"Score" );

?>