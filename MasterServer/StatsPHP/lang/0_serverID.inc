<?php
//  serverID  page strings   (lang/0_serverID.inc)
// ----------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'serverID'

$head_title_l[$entry]	=	"ServerID Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[7]/$head_nav_names[16]=";	//Location path: Home/Global

$index_info_l[$entry]	= array("Server",	"details",
								"Maps",		"played on this server",
								"Mods",		"run on this server",
								"Matches",	"played on this server",
								"Gamerule",	"flags"
								 );


$intro_title_l[$entry]	=	"Hang-Out";

$intro_intro_l[$entry]	=	"...infos about this server.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Curious about this server?";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";



//...Table...
$entry = 'serverIDSummary';

$table_title_l[$entry]	=	"Server Summary";

$table_text_l[$entry]	=	"Details on this map. Uptime in minutes.";

$table_head_l[$entry]	=	array(	"Server",		"ServerID",	"IP:Port",
									"Uptime",	"Maps",		"Connects",
									// "Admin Name",	"Admin Email",
									"First Connect", "Last Update"	);

//...Table...
$entry = 'serverIDMaps';

$table_title_l[$entry]	=	"Server Maps";

$table_text_l[$entry]	=	"Maps seen on this server.";

$table_head_l[$entry]	=	array(	"N&deg;",		"Map Name",     "Games",
									"Uptime[m]",	"U%",			"Connects",
									"Frags",		"F%",			"Kills",		"Suicides",
									"Teamkills",	"Events",		"Official" );

//...Table...
$entry = 'serverIDMods';

$table_title_l[$entry]	=	"Server Mods";

$table_text_l[$entry]	=	"Mods played on this server.";

$table_head_l[$entry]	=	array(	"N&deg;",		"Mod",     		"ModID",		"Games",
									"Uptime[m]",	"U%",			"Connects",
									"Frags",		"F%",			"Kills",		"Suicides",
									"Teamkills",	"Events",		"Official" );


//...Table...
$entry = 'serverIDMatches';
$table_title_l[$entry]	=	"Recent Matches";
$table_text_l[$entry]	=	"List of all the recently played matches on this server."."<br>".
							"Filter: Only showing matches with Connects>1 and Score>0. C=Connects.";
$table_head_l[$entry]	= array("N&deg;",	"MatchID",	"Server",	"Map",		"C",	"Begin",
								"End",		"Frags",	"Kills",	"Suicides",	"Score" );



//...Table...
$entry = 'serveridGameRules';
$table_title_l[$entry]	=	"Game Rules";
$table_text_l[$entry]	=	"Flags that where set on this server.";
$table_head_l[$entry]	= array( "Rule", "Value" );
$table_total_l[$entry]	=	"(Total)";


?>