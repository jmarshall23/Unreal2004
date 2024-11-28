<?php
//  MAPS  page strings   (lang/0_maps.inc)
// ------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'maps'

$head_title_l[$entry]	=	"Map Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[6]";	//Location path: Home/Global

$index_info_l[$entry]	= array( "Maps",	"- Most played !" );

$intro_title_l[$entry]	=	"Environments";

$intro_intro_l[$entry]	=	"...the places you folks like to hang out.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Curious what maps are played the most?<br>".
							"With what player counts and up times, most frags, ... ?<br>".
							"Details galore right on this page.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";



//...Table mapsTop...
$entry = 'mapsTop';

$table_title_l[$entry]	=	"Maps";

$table_text_l[$entry]	=	"Maps that where played the most.";

$table_head_l[$entry]	=	array(	"N&deg;",	"Map Name",
									"Uptime[m]","U%",		"Connects",		"Games",
									"Frags",	"F%",		"Kills",		"Suicides",
									"Teamkills","Events",	"Official" );

?>