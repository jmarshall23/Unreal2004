<?php
//  SERVERS  page strings   (lang/0_servers.inc)
// ---------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'servers'

$head_title_l[$entry]	=	"Server Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[7]";	//Location path: Home/Global

$index_info_l[$entry]	= array("Servers",	"- Most visited" );

$intro_title_l[$entry]	=	"Hang-Outs";

$intro_intro_l[$entry]	=	"...where you find folks as enthusiastic about gaming as you are.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Want to find servers that are visited a lot?<br>".
							"With cool level loops and nifty mutators turned on?<br>".
							"Just read on.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";


//...Table...
$entry = 'serversTop';

$table_title_l[$entry]	=	"Servers";

$table_text_l[$entry]	=	"Often frequented games servers."."<br>".
							"C=Player connects to the server, Uptime in Minutes."."<br>";
$table_head_l[$entry]	=	array(	"N&deg;",		"Server",		"ServerID",	"IP:Port",
									"Uptime",		"Maps",
									"C",		"First Connect", "Last Update"	);


?>