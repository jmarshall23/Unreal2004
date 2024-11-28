<?php
// global_vars.inc
// ---------------

// All global vars are set in the file.
// These control how the output pages will be displayed.
// ALSO: ALL main language strings are set here, marked with ***


//---Super GLOBAL VAR---
//	$global_page	set by each page, determines what language strings need be included.
//			e.g. $global_page = 'players';


//---GLOBAL VARS---
$show_top_X 		= 10;				//Show top 10 in e.g. RunnersUp on players page
$show_search_X		= 30;				//!! 20 Show this many entries on the search page
$show_servers_X		= 30;				//Used by ServerID page... max number of servers shown
$show_matches_X		= 20;				//Used by playerudLastMastes, last 20 matches you played in
$show_rankzone_X 	= 5;				//Position in rank plus/minus 5, used on playerID mod page

$page_last_update = date("l, F jS Y - H:i (\G\M\T O)");	//### needs to show real time/date (foot.inc), not just current


//---DATA BASE related---
$db_name	= "ut2003stats";		//Connect to this database server
//$db_name	= "ut2003stats_test";	//Connect to this database server
$db_lid		= 0;					//Set this sites language (lid = language ID).
									//0 English, [1 German, ... ] to be defined
$db_lang	= array(	"en", 		//0 english (is $db_lid number!)
						"det",		//1 german
						"est", 		//2 spanish
						"frt", 		//3 french
						"itt"		//4 italian
					);


//---GAMETYPE infos---
$db_mod_tags 	= array(	'DM','TDM','CTF','DD','BR','DMO','TDMO','CTFO','DDO','BRO','O' );	//** Hardcoded
$db_mod_codes	= array(	0 => 'XGame.xDeathMatch',
							1 => 'XGame.xTeamGame',
							2 => 'XGame.xCTFGame',
							3 => 'XGame.xDoubleDom',
							4 => 'XGame.xBombingRun',
							5 => '!XGame.xDeathMatch',
							6 => '!XGame.xTeamGame',
							7 => '!XGame.xCTFGame',
							8 => '!XGame.xDoubleDom',
							9 => '!XGame.xBombingRun',
						   10 => '!XGame.xSurvival',
						   11 => '!XGame.xOther' );


//---PAGE names--- used by main page e.g.
$head_nav_pages	= array("main.php",				//0 also used as page titles
						"global.html",			//1
						"matches.html",			//2
						"players.html",			//3
						"compare.php",			//4
						"bestof.html",			//5
						"mods.html",			//6
						"maps.html",			//7
						"servers.html",			//8
						"sitemap.php",			//9
						"faq.php"				//10
						);


//---Translations---
include "func/translate.inc";			//translate() code to language //** include locally only!

//----INCLUDE control---
$included_check_playerid 			= 0;
$included_get_gametypes				= 0;
$included_get_rankzone				= 0;
$included_get_teamgame				= 0;
$included_get_nickname				= 0;
$included_get_modname				= 0;
$included_get_mapname				= 0;
$included_get_servername			= 0;
$included_get_servernameID			= 0;
$included_get_serverid				= 0;
$included_get_minutesecond			= 0;
$included_get_fragsdate				= 0;
$included_tablematrix_sumpercent	= 0;
$included_tablematrix_colhighlight	= 0;
$included_tablematrix_rowhighlight	= 0;
$included_tablematrix_colintensity	= 0;


//---DESIGN strings---
$indent	= "<img src=pics/t.gif width=30 HEIGHT=10>";	//Indent for all content, 30 pixel!
$ind_tb	= "<img src=pics/t.gif width=28 HEIGHT=10>";	//Indent for all content, 38 pixel!
$spt	= "<img src=pics/t.gif width=20 HEIGHT=10>";	//Space between index and intro boxes
$spn	= "<img src=pics/t.gif width=20 HEIGHT=17>";	//Used my main navigation links
$spl	= "<img src=pics/t.gif width=30 HEIGHT=15>";	//Used by location sting
$sps	= "<img src=pics/t.gif width=5  HEIGHT=2 >";	//Used by mini search form
$spi	= "<img src=pics/t.gif width=49 HEIGHT=25>";	//Used by index (quick reference) box, offset index title
$spr	= "<img src=pics/t.gif width=8  HEIGHT=53>";	//Used by intro box, space between pic and title
$spb	= "<img src=pics/t.gif width=10 HEIGHT=53>";	//Used by intro box, space between title & "top/right" button
$spf	= "<img src=pics/t.gif width=5  HEIGHT=25>";	//Used by info box, extra space between "!" and rounded frame

$index_width = 224 - 2 - 2*14;				//=194, index box, 2*left/right margin
$intro_width = 456 - 2 - 2*16;				//=422, intro box, 2*left/right margin
$intro_width2= 218;							//intro box for main page
$info_width  = 634 - 2 - 2*16;				//=600, info  box, 2*left/right margin



//---LANGUAGE STRINGS (***)---
// Includes controlled by $global_page and $db_lid vars.


// Global language strings, e.g. main navigation, loctation, search
$include_language = "lang/".$db_lid."_global_vars.inc";
include $include_language;

// Language strings for specific pages
$include_language = "lang/".$db_lid."_".$global_page.".inc";
include $include_language;

// -----------------------------------------------------------------------------


?>
