<?php
//  SEARCH  page strings   (lang/0_search.inc)
// --------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'search'

$head_title_l[$entry]	=	"Search";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[12]";	//Location path: Home/Search

$index_info_l[$entry]	= array("Search",	"parameters",
								"Results",	"of search"
								);

$intro_title_l[$entry]	=	"Quickly find...";

$intro_intro_l[$entry]	=	"...someone's stats with the <span class=blue>Search</span> at the top of the page.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Search for a player's nickname, his playerID, a specific server by ".
							"name or id, infos on a map or mod (gametype).";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";	//i_logo_search.gif

$intro_button_l[$entry]	=	"b_".$global_page.".gif";


$info_text_l[$entry] 	=	"Tip: If the search yields no results, try specifying a more general search ".
							"with less restricting search parameters, e.g. use <span class=blue>compare</span> ".
							"'$head_search_compare[0]' and '$head_search_case[0]'.";  //Phrase, Ignore Case



//...Table advancedSearch...
$table_title_l[$entry]	=	"Result(s) for";

$table_text_l[$entry]	=	"Tip: If you remember your PlayerID, you will have no trouble finding ".
							"your personal stats.";


$table_head_l['nick']		= array( "N&deg;", "Nick",		"PlayerID",	"Last Used"	);	//used for nick and playerID seach
$table_head_l['playerID']	= array( "N&deg;", "PlayerID",	"Nick",		"Last Used"	);
$table_head_l['matchID']	= array( "N&deg;", "MatchID",	"Server",	"Map",   	"Start", 	 "End",		"Connects" );

$table_head_l['server']		= array( "N&deg;", "Server",	"ServerID",	"IP:Port", 	"Uptime[m]", "Last Update",	"Connects", "Maps" );
$table_head_l['serverID']	= array( "N&deg;", "ServerID",	"Server",	"IP:Port",	"Uptime[m]", "Last Update",	"Connects", "Maps" );

$table_head_l['map']		= array( "N&deg;", "Map",	    "MapID",	"Title",	"Uptime[m]", "Games",	"Connects" );
$table_head_l['mod']		= array( "N&deg;", "Mod",		"ModID",	"Events",	"Uptime[m]", "Games",	"Connects" );

?>
