<?php
//  SITEMAP  page strings   (lang/0_sitemap.inc)
// ---------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'sitemap'

$head_title_l[$entry]	=	"Site Map";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[8]";	//Location path: Home/Global

$index_info_l[$entry]
	= array(
		//	Anquor		Link		LinkInfo                	Title	 Description
			"mGlobal",	"Global",	"- Info summeries",			"Global Stats",
			"The ".
			"<a href=\"global.html\">$head_nav_names[1]</a> ".	// Global
			"stats page gives you an overview of what has been happening on all Unreal Tournament 2003 servers out there, that have connected to the UT2003stats master server sofar.",

			"mMatches",	"Matches",	"- Recently played game.",	"Matches",
			"For each Unreal Tournament 2003 server out there UT2003stats stores all the ".
			"match relevant data. On the ".
			"<a href=\"matches.html\">$head_nav_names[3]</a> ".	// Matches
			"stats pages you can then look up the most recent matches in detail."."<p>".
			"If you remember the MatchID you can easily find that match via the search feature (top of page).",

			"mPlayer",	"Player",	"- Stats &amp; Ranking",	"Player Stats & Ranking",
			"Are you ready for merciless ranking?"."<br>".
			"Want to check out how you have been doing in frags, deaths, suicides and events?"."<br>".
			"UT2003stats has been tracking your progress across the servers you played on!"."<br>".
			"Visit the ".
			"<a href=\"players.html\">$head_nav_names[2]</a> ".	// Players
			"stats pages player pages to find out more."."<p>".
			"To quickly find your individual stats use the player search feature (top of page) ".
			"and enter either search for your Nick name or your unique PlayerID.",

			"mCompare",	"Compare",	"players by ID.",	"Compare",
			"Each player in UT2003stats has a unique ID, the socalled ".
			"<span class=blue>"."PlayerID"."</span>".
			". With this 'handle' you are able to compare players on the ".
			"<a href=\"compare.php\">$head_nav_names[18]</a> ".	// Compare
			"stats pages in detail.",

			"mBest",	"Best of",	"Stats",					"Best of...",
			"Eager to find out who the top scorers are? To what extremes gamers went? Or simply interested in what maps, mods, etc. where played the most?"."<p>".
			"See the top fraggers, top FPH hogs, etc. on the ".
			"<a href=\"bestof.html\">$head_nav_names[4]</a> ".	// Best of
			"pages.",

			"mMods",	"Mods",		"- Gametype happenings",	"Mods",
			"Out of the box Unreal Tournament 2003 supports the gametypes Deathmatch, Team Deathmatch, Capture the Flag, Double Domination and Bombing Run."."<p>".
			"Check out how you did in them on the ".
			"<a href=\"mods.html\">$head_nav_names[5]</a> ".		//Mods
			"pages.",


			"mMaps",	"Map",		"favorites",				"Maps",
			"Curious what maps played the most? With what player counts and average up times, most frags, etc. ?"."<p>".
			"Read up on map related extravaganza on the ".
			"<a href=\"maps.html\">$head_nav_names[6]</a> ".		//Maps
    		"pages.",

			"mServers",	"Servers",	"out there",				"Servers",
			"Want to find servers that are visited a lot? With cool level loops and nifty mutators turned on?"."<p>".
			"Top servers on the ".
			"<a href=\"servers.html\">$head_nav_names[7]</a> ".	// Servers
    		"pages.",

			"mFAQ",		"FAQ",		"- Help Topics",			"F.A.Q.",
			"Wonder how the ranking works? What gets tracked? How the scores for a certain gametype are assigned? How to improve in ranking?"."<p>".
			"Came across a word / term / definition you have never heard of before? Look them up in the extensive Lexicon!"."<p>".
			"This and much more is in store for you on the ".
			"<a href=\"faq.php\">$head_nav_names[9]</a> ".		// FAQ
    		"pages.",

			);

$intro_title_l[$entry]	=	"Where to Find Stuff";

$intro_intro_l[$entry]	=	"...and other interesting nit-bits.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Looking for a site overview?<br>".
							"Look no further and read up on this site's features and navigation.";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";


$info_text_l[$entry] 	=	"";

?>