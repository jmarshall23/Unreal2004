<?php
//  MODS  page strings   (lang/0_mods.inc)
// ------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'mods'

$head_title_l[$entry]	=	"Mod Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[5]";	//Location path: Home/Global


$index_info_l[$entry]	= array("Mods",	"- Most played" );


$intro_title_l[$entry]	=	"Modifications";

$intro_intro_l[$entry]	=	"...standard gametypes and community mod overview.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Out of the box Unreal Tournament 2003 supports the gametypes Deathmatch, ".
							"Team Deathmatch, Capture the Flag, Double Domination and Bombing Run.<br>".
							"Read up on how popular each mod is.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";


$info_text_l[$entry] 	=	"";



//...Table...
$entry = 'modsTop';

$table_title_l[$entry]	=	"Mods";

$table_text_l[$entry]	=	"Most popular gametypes.";

$table_head_l[$entry]	=	array(	"N&deg;",		"Mod",     		"ModID",		"Games",
									"Uptime[m]",	"U%",			"Connects",
									"Frags",		"F%",			"Kills",		"Suicides",
									"Teamkills",	"Events",		"Official" );


?>