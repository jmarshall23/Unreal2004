<?php
//  modID  page strings   (lang/0_modID.inc)
// -------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'modID'

$head_title_l[$entry]	=	"ModID Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[5]/$head_nav_names[17]=";	//Location path: Home/Global

$index_info_l[$entry]	= array("Mod",		"details",
								"Maps",		"running this mod",
								"Servers",	"running this mod",
								"Matches",	"running this mod"
								 );


$intro_title_l[$entry]	=	"Modification";

$intro_intro_l[$entry]	=	"...details on this gametype.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Read up on how popular this mod is.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";



//...Table...
$entry = 'modIDSummary';

$table_title_l[$entry]	=	"Mod Summary";

$table_text_l[$entry]	=	"Details on this mod.";

$table_head_l[$entry]	=	array(	"Mod",     		"ModID",		"Games",
									"Uptime[m]",	"Connects",		"Frags",
									"Kills",		"Suicides",		"Teamkills",
									"Events",		"Official" );

//...Table...
$entry = 'modIDMaps';

$table_title_l[$entry]	=	"Mod Maps";

$table_text_l[$entry]	=	"Maps that are run with this mod.";

$table_head_l[$entry]	=	array(	"N&deg;",		"Map Name",     "Games",
									"Uptime[m]",	"U%",			"Connects",
									"Frags",		"F%",			"Kills",		"Suicides",
									"Teamkills",	"Events",		"Official" );

//...Table...
$entry = 'modIDServers';

$table_title_l[$entry]	=	"Mod Servers";

$table_text_l[$entry]	=	"Servers playing this mod.";

$table_head_l[$entry]	=	array(	"N&deg;",		"Server",     	"ServerID",		"Games",
									"Uptime[m]",	"U%",			"Connects",
									"Frags",		"F%",			"Kills",		"Suicides",
									"Teamkills",	"Events" );

//...Table...
$entry = 'modIDMatches';
$table_title_l[$entry]	=	"Recent Matches";
$table_text_l[$entry]	=	"List of all the recently played matches for this mod."."<br>".
							"Filter: Only showing matches with Connects>1 and Score>0. C=Connects.";
$table_head_l[$entry]	= array("N&deg;",	"MatchID",	"Server",	"Map",		"C",	"Begin",
								"End",		"Frags",	"Kills",	"Suicides",	"Score" );



?>