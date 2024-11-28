<?php
//  GLOBAL_VARS    (lang/0_global_vars.inc)
// -------------
//
// LANGUAGE: 	lid=0	ENGLISH


//---MAIN WEB NAVIGATION ENTRIES---

$head_nav_names	= array("Home",				//0 also used as page titles
						"Global",			//1
						"Players",			//2
						"Matches",			//3
						"Best of",			//4
						"Mods",				//5
						"Maps",				//6
						"Servers",			//7
						"Site Map",			//8
						"FAQ",				//9

						"PlayerID",			//10  subpages
						"PlayerID per Mod",	//11
						"Search",			//12
						"Mod",				//13
						"MatchID",			//14
						"MapID",			//15
						"ServerID",			//16
						"ModID",			//17
						"Compare"			//18 main page, but will not hack it into all the other pages!
						);

//---LOCATION string (every page)---
$head_location			=	"Location:";

//---TABLE TITLES---
$head_title_possessive	=	"'s";		// e.g. AEon"'s" Weapon Kills (PlayerID Stats page)
                                        // German would be AEon"s"

//---TIMEFRAMES--- (in most table headers)
$head_timeframes		= array("Week", "Month", "Alltime" );

//---SEACH ENTRIES--- (on all pages and on search page)
$head_search_names		= array("Nick",	"PlayerID", "MatchID", "Server", "ServerID", "Map", "Mod");
$head_search_button		= array("Search", "Reset");
$head_search_tags		= array("Query", "Compare", "Sort by", "Results", "Skip to");		//names for search fields
$head_search_compare	= array("Phrase", "Must begin with", "Must end with", "Exact");
$head_search_case		= array("Ignore case", "Case sensitive");
$head_search_order		= array("Ascending", "Descending");
$head_search_links		= array("Previous entries", "First entry", "Next entries");
$head_search_info		= array("Found", 			//e.g. "Found 10 entries matching..."
								"entries matching your query.",
								"Displaying hits",            	//e.g. "Displaying hits 1 to 10."
								"to",
								".",
								"Sorry, no entries matching your query were found."
								);


//---COMPARE ENTRIES---
$head_compare_tags		= array("PlayerID", "vs.", "PlayerID","Compare", "Results" );
$head_compare_button	= array("Compare", "Reset");

$head_compare_timeframe	= array($head_timeframes[2],	// alltime
								$head_timeframes[1],	// month
								$head_timeframes[0]);	// week

$head_compare_tf		= array("Timeframe:", "Gametypes:" );

$head_compare_mods		= array();						// Will be read from DB...

$head_compare_warn		= array("Sorry, 1. PlayerID is invalid, please pick a valid ID.", 
								"Sorry, 2. PlayerID is invalid, please pick a valid ID.",
								"Sorry, both PlayerIDs are invalid, please pick valid IDs and try again."
								);

$head_compare_info		= array("Please enter 2 PlayerIDs into the above fields, and hit Compare.",
								"Please enter 2 different PlayerIDs in fields above, and try again.",
								"Found data for both PlayerIDs, see below:" );
$head_allgametypes		= array("ALL Gametypes");



//---TEAMS--- (Match Score Board etc.)
$head_teams			= array( "Free for all" );				//no teams gametype (DM, DMother, Custom Other for now)
$head_teams2		= array( "Team Red", "Team Blue" );		//teamid 0=red, 1=blue
$head_winner		= array( "Winner", "Loser" );			//e.g. Team Red (7 - Winner)  Team Blue (3)


//---PAGE FOOTERS---
$foot_gen			= "This page was generated with";
// UT2003stats (C) 2002
$foot_by			= "by";
// Epic Games.
// 123322
$foot_frags			= " frags served since ";
//2002-09-30 17:55:48  
$foot_design		= "Design by";
// |2|


//---MATCH STATUS---
$match_in_progress = array( "(Match has Ended)", "-Match in Progress-" );


?>
