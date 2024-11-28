<?php
//  MATCHIDMATCH  page strings   (lang/0_matchIDmatch.inc)
// ---------------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'matchIDmatch'

$head_title_l[$entry]	=	"MatchID Stats";

$head_where_l[$entry]	=	"$head_nav_names[0]/$head_nav_names[2]/$head_nav_names[14]=";	//Location path: Home/Players/MatchID=666 gets MatchID appended


$index_info_l[$entry]	= array("Match",		"- Details",
								"Summary",		"- Gameplay overview",
								"Score Board",	"- Who played Whom",
								"Kills",		"/ Deaths per weapon",
								"Suicides",		"per type",
								"Events",		"- non-weapon related",
								"Scores",		"- totals per event",
								"Mutators",		"- game types",
								"Gamerule",		"flags",
								);


$intro_title_l[$entry]	=	" ";	//###Player nick name is prepended, playerID appended e.g. AEon (666)

$intro_intro_l[$entry]	=	"...how this match went - an overview.";	//max len ca. 85 chars

$intro_text_l[$entry]	=	"Read up on the score distributions, who made how many points in score, ".
							"what events were triggered, what weapons were preferred...";

$intro_logo_l[$entry]	=	"i_logo_".$global_page.".gif";

$intro_button_l[$entry]	=	"b_".$global_page.".gif";

$info_text_l[$entry] 	=	"Find your favorite info right here! :)";



//...Table matchIDSummary...
$entry = 'matchIDSummary';

$table_title_l[$entry]	=	"Match Details";
$table_title_l2[$entry]	=	"Match Summary";

$table_text_l[$entry]	=	"Match data at one glance.";
$table_text_l2[$entry]	=	"Gameplay totals.";

$table_head_l[$entry]	= array( "MatchID",		"Server",	"ServerID",		"Map",
								 "Connects",	"Begin",	"Game Time[m]", "Map Change" );

$table_head_l2[$entry]	= array( "Team Score",	"Score",	"Frags",	"Kills",
								 "Suicides",	"TKs",		"Events",	"Mutators" );

//...Table matchIDSummary...
$entry = 'matchIDScoreBoard';

$table_title_l[$entry]	=	"Match Score Board";

$table_text_l[$entry]	=	"What transpired in this match."."<br>".
							"%Eff=Efficiency(K,D,S), MRank=Match Rank"."<br>".
							"RGain=Alltime rank points you 'gained' in this match/gametype.";

$table_text_l2[$entry]	=	"What transpired in this match.<br>".
							"Sc=Score, K=Kills, D=Deaths, S=Suicides, TK=Team Kills, MR=Match Rank, ".
							"%Eff=Efficiency(K,D,S,TK), RG=Alltime rank 'gain' in match.";

$table_head_l[$entry]	= array("N&deg;",
								"Player",	"PlayerID",	"Score",  "%Eff", "Kills",
								"Deaths",	"Suicides",	"Events", "MRank", "RGain", "Time[m]" );

$table_head_l2[$entry]	= array("N&deg;",
								"Player",	"PlayerID",	"Sc", "%Eff", "K", "D", "S", "TK", "MR", "RG", "Time",
								"Player",	"PlayerID",	"Sc", "%Eff", "K", "D", "S", "TK", "MR", "RG", "Time"  );	//TKs = Teamkills, Suis = Suicides

$table_total_l[$entry]	=	"";




//---Using global pages infos, for playerIDplayer pages as well---
// Without the mutators

//...Table matchidData_KD...
$entry = 'matchidData_KD';
$table_title_l[$entry]	=	"Weapon Kills/Deaths";
$table_text_l[$entry]	=	"For each in-game kill/death there is information about the weapons the ".
							"players held in hand. The kill weapon belongs to the killer, the death ".
							"weapon belongs to the victim.";
$table_head_l[$entry]	= array("Weapon", "Kills", "K%", "Deaths", "D%" );
$table_head_l2[$entry]	= array("Weapon", "Kills", "K%", "Deaths", "D%", "Kills", "K%", "Deaths", "D%" );
$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_S...
$entry = 'matchidData_S';
$table_title_l[$entry]	=	"Suicides";
$table_text_l[$entry]	=	"The table shows all the suicides, not only those related to weapons ".
							"(e.g. splash damage) but also environment induced deaths (e.g. lava).";
$table_head_l[$entry]	= array("Cause", "Suicides", "S%" );
$table_head_l2[$entry]	= array("Cause", "Suicides", "S%", "Suicides", "S%" );
$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_E...
$entry = 'matchidData_E';

$table_title_l[$entry]	=	"Events";

$table_text_l[$entry]	=	"All special events are shown here.";

$table_head_l[$entry]	= array("Type",	"Events", "E%" );
$table_head_l2[$entry]	= array("Type",	"Events", "E%", "Events", "E%"  );

$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_Sc...
$entry = 'matchidData_Sc';

$table_title_l[$entry]	=	"Scores";

$table_text_l[$entry]	=	"Showing how often each score type was triggered.";

$table_head_l[$entry]	= array( "Type", "Score", "Sc%", "Counts", "C%" );
$table_head_l2[$entry]	= array( "Type", "Score", "Sc%", "Counts", "C%", "Score", "Sc%", "Counts", "C%"  );

$table_total_l[$entry]	=	"(Total)";


//...Table matchidData_Mu...
$entry = 'matchidData_Mu';

$table_title_l[$entry]	=	"Mutators";

$table_text_l[$entry]	=	"The mutators in this match.";

$table_head_l[$entry]	= array( "Mutator", "Uses" );

$table_total_l[$entry]	=	"(Total)";

$head_block_x[$entry]	= array( "Match" ) ;		//**special block hack



//...Table...
$entry = 'matchidGameRules';

$table_title_l[$entry]	=	"Game Rules";

$table_text_l[$entry]	=	"Flags that where set for this match.";

$table_head_l[$entry]	= array( "Rule", "Value" );

$table_total_l[$entry]	=	"(Total)";


?>
