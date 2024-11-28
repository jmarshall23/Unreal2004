<?php
//  FAQ  page strings   (lang/0_sitemap.inc)
// -----
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'faq'

$head_title_l[$entry]	=	"The F.A.Q.";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[9]";	//Location path: Home/Global


// Used as Index and as Lexicon infos
$index_info_number[$entry]	=	3;				// Lexicon starts in row 3
$index_info_l[$entry]
	= array(
		//	Anquor		  	Link				LinkInfo                Description
			"fFAQ",			"F.A.Q.",			"- To the Questions",	"",
			"",				"",					"",						"",
			"",				"", 				"The Lexicon",			"",

			"fAD",			"Adrenaline",		"",                   	// Line 3 used for Lexicon only
			"Picking up adrenaline pills, or receiving them for special accomplishments, like capturing ".
			"the flag, will let you activate a movement-key combo, once you have collected 100 adrenaline points.".
			"The combo will give you temporary special powers.",


			"fAbb",			"Abbreviations",	"- F, K, D, S, E",
			"Common abbreviations in UT2003stats."."<br>".
			"- K = Kills, S = Suicides, F = Frags, D = Deaths, E = Events, TK = Team Kills, TD = Team Deaths "."<br>".
			"- DM = Deathmatch, TDM = Team Deathmatch, CTF = Capture the Flag, DD = Double Domination, BR = Bombing Run "."<br>".
			"- FPH = Frags per Hour "."<br>".
			"- [d] = Time in days, [h] = Time in hours, [m],[min] = Time in minutes, [s],[sec] = Time in seconds",

			"fDeaths",		"Deaths",			"(D)",
			"Number of times a player gets killed by another player. "."<br>".
			"- This does not include environment induced deaths, like trap doors. These and self kills are counted separately, as suicides. "."<br>".
			"- Team based deaths are counted as team deaths. "."<br>".
			"- In tables with weapon specific information, deaths are the number of times a player died holding that weapon. ",

			"fDodging",		"Dodging",			"",
			"Special move in Unreal Tournament 2003, that can be activated by tapping any movement key twice. Used by many good players to improve their maneuverability. ",

			"fEff",			"Efficiency",		"",
			"A ratio that denotes the player's kill skill by comparing it with his overall performance. A perfect efficiency is equal to 1 (100%), anything less than 0.5 (50%) is below average."."<br>".
			"Formula:   Kills / (Kills + Deaths + Suicides [+Team Kills]) ",

			"fEvents",		"Events",			"(E)",
			"Anything not related to frags, deaths, suicides or kills is hereby defined as an event. Typical events would be a flag capture (score related) or a flag drop (not score related). Events are mostly used to track all the other things going on in a game, that are not frag-related.",

			"fFB",			"First Blood",		"",
			"Special event awarded to the player who gets the first kill in a newly started match.",

			"fFPH",			"Frags Per Hour",	"(FPH)",
			"A ratio between the number of frags a player scores per one hour. 40 frags in 3 minutes will give you an impressive, but quite possible 800 FPH."."<br>".
			"Formula:   Frags / (Time played in hours)",

			"fFrags",		"Frags",			"(F)",
			"Derived from the netto score in free for all deathmatch games. A player's frag count is his kills minus his suicides. Note the team kills in the formula for team based games."."<br>".
			"Formula:   Frags = Kills - Suicides [- Team Kills]",

			"fHS",			"Head Shot",		"",
			"Special event awarded to the player's who kill due to a precise shot to the head of the victim, causing instant death. Only possible with the Lightning Gun.",

			"fKills",		"Kills",			"(K)",
			"Number of times a player kills another player."."<br>".
			"Note: UT2003stats only tracks human vs. human kills. Bot kills and other bot related events are *not* tracked.",

			"fMK",			"Multi Kills",		"",
			"Special event awarded to the player for killing other players in a certain time frame. Every time a player scores a kill he has up to 3 seconds to make another kill. So 2 kills in 3 seconds gets you a Double Kill, 3 kills in less than 6 seconds a Multi Kill and so on: "."<br>".
			"- Double Kill = 2 kills in 3 seconds "."<br>".
			"- Multi Kill = 3 kills in 6 seconds "."<br>".
			"- Mega Kill = 4 kills in 9 seconds "."<br>".
			"- Ultra Kill = 4 kills in 12 seconds "."<br>".
			"- Monster Kill = 5 kills in 15 seconds "."<br>".
			"- ... ",

			"fPing",		"Ping",				"",
			"Measure of your connect quality. Ping is the round trip delay in milliseconds that your computer has to the game server. Low values may not be all important for a fun game, but it sure helps.",

			"fSpree",		"Killing Sprees",	"",
			"Special event: If you manage to kill 5 or more opponents without dying yourself, you will be on a killing spree. If you kill more than 10 opponents, you are on a rampage, etc.:"."<br>".
			"- Killing Spree! 5 kills in a row"."<br>".
			"- Rampage! 10 kills in a row"."<br>".
			"- Dominating! 15 kills in a row"."<br>".
			"- Unstoppable! 20 kills in a row"."<br>".
			"- God Like! 25 kills in a row"."<br>".
			"- ... ",

			"fSuicides",	"Suicides",			"(S)",
			"Number of times a player dies due to action of their own cause. Suicides can be environment induced (drowning, getting crushed, falling) or weapon related (fatal splash damage from their own weapon).",

			"fTD",			"Team Deaths",		"(TD)",
			"Number of times a player in a team based game is killed by someone on their own team.",

			"fTK",			"Team Kills",		"(TK)",
			"Number of times a player in a team based game kills someone on their own team."."<br>".
			"Note: Team kills subtract from a players personal frags and thus the team frags as a whole.",
			);

$intro_title_l[$entry]	=	"Did you know that...";

$intro_intro_l[$entry]	=	"...BR stands for Bombing Run?";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Look up on all the frequently asked questions.<br>".
	  						"Find explanations for gametype rules and scoring events.<br>".
				  			"Or look up definitions in the lexicon...";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"";


// The Lexicon
$lex_title_l[$entry]	=	"The Lexicon";





// Questions for the FAQ:
$faq_title_l[$entry]	= 	"Frequently Asked Questions";

$faq_l[$entry] = array(
"What are the 'official' Unreal Tournament 2003 gametypes?",
	"Out of the box Unreal Tournament 2003 features the gametypes Deathmatch, Team Deathmatch, ".
	"Capture the Flag, Double Domination, and Bombing Run.",

"How does ranking work?",
	"Ranking is based on ELO Chess ranking."."<br>".
	"Everyone starts with the lower rank limit 0, points are based on how often you trigger positive ".
	"personal score events, e.g. kills, captures, assists etc. Against you score deaths, suicides, ".
	"teamkills, the other team scoring points against your team etc."."<br>".
	"The worse the other player or team is, against whom you score, the less points you will get, so ripping ".
	"of weak opponents will get you less rank points, than beating a stronger (in rank) opponent.",

"Are there different rank types?",
	"Yes, there are ranks for non-teamgames (DM), teamgames (TDM, CTF, BR, DD) and matches."."<br>".
	"- Non-teamgame ranks only take into account kills, deaths, suicides. "."<br>".
	"- Teamgame take into account any personal score you achieved: kills, critical frags, captures, ".
	"assists, dominations etc. depending on what game events are supported by the gametype."."<br>".
	"- The Match Rank (MRank) is reset at the beginning of each match, and gives you another ".
	"way to analyse the match, apart from the scores. Can be team or non-team based, obviously.",

"Scoring - Individual player points awarded?",
	"These depend on the gametype your are playing. There are individual player scores awarded for ".
	"special achievements, like captures, assists, etc. See the Score tables on the Players or Matches ".
	"subpages to find out how many points are actually awarded for each score type.",

"Team Scores?",
	"Aside from the (individual player) scoring in team based gametypes (TDM, CTF, BR, DD), ".
	"there are also so called Team Scores, that are awarded to your team as a whole, for fulfilling ".
	"a gametype specific objective:"."<br>".
	"- In Team Deathmatch, killing a player from the other team."."<br>".
	"- In Capture the Flag it is capturing the flag."."<br>".
	"- In Double Domination, 'holding' both domination points for more than 10 seconds."."<br>".
	"- In Bombing Run either throwing the ball through the goal, or jumping through the goal holding the ball."."<br>".
	"Note: A team based game is won by the Team Score, the individual player score sums do not matter!",

"UT2003stats: Stats servers / non-stats servers?",
	"UT2003stats only does stats tracking for servers that have stats tracking turned on! ".
	"You can only join such a server if you have 'Track Stats' turned on in the Settings|Network menu."."<br>".
	"Servers without stats tracking are marked in the server browser. Playing on these servers ".
	"will *not* track your stats, no matter if you have 'Track Stats' turned on or not!",

"UT2003stats: Turning on your personal stats tracking - 'Track Stats'",
	"To turn on world-wide personal stats tracking, go to your Settings menu under Network and turn on ".
	"'Track Stats'. Then pick a unique Stats Username and Stats Password, and from then on you will have ".
	"a unique PlayerID - a UT2003stats player handle - that lets you look up *your* stats on this site. "."<br>".
	"Note: Only the games you play in servers with stats tracking enabled will show up on the UT2003stats pages here!",


"More infos ...",
	"... coming.",


/*

// This could get us into tons of problems...
"How does the scoring for DeathMatch work?",
	"DM player scoring: +1 point per kill, -1 point per suicide.",

"How does the scoring for Team Deathmatch work?",
	"TDM player scoring: +1 point per kill, -1 point per suicide, -1 point per team kill (team member)"."<br>".
	"TDM team scoring: +1 point per kill",

"How does the scoring for Capture the Flag work?",
	"TDM player scoring: +1 point per kill, -1 point per suicide, -1 point per team kill,"."<br>".
	"+1 point per kill, -1 point per suicide, -1 point per team kill,"."<br>".

*/



);










/*
//...Table globalSummary...
$entry = 'globalSummary';

$table_title_l[$entry]	=	"Summary";

$table_text_l[$entry]	=	"Here you will find all the grand, world-wide, of this planet ;) totals in ".
	  			"frags, deaths, suicides, events, times, etc.";

$table_head_l[$entry]	= array("Entry", "Totals");

$table_rows_l[$entry]	= array("Kills", "Deaths", "Suicides", "Frags", "Sum K+S");

*/

?>  