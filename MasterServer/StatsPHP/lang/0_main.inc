<?php
//  MAIN  page strings   (lang/0_main.inc)
// ------
//
// LANGUAGE: 	lid=0	ENGLISH

$entry = $global_page;		//'main'

$head_title_l[$entry]	=	"Nav Legend";

$head_where_l[$entry]	=	"$head_nav_names[0]";	//Location path: Home/Global

$index_info_l[$entry]	= array("Home",		"- Top level",
								"Global",	"- General stats summary",
								"Matches",	"- Recently played games",
								"Players",	"- Detailed player stats",
								"Compare",	"- Players by ID",
								"Best of",	"- Overall top stats",
								"Mods",		"- Modification info",
								"Maps",		"- Favorite level list",
								"Servers",	"- Find most visited",
								"Site Map",	"- Site overview",
								"F.A.Q.",	"- More questions?",

								"News",		"- The latest stats infos",
								"Top Ranked", "in official gametypes"
							);

$intro_title_l[$entry]	=	"Keep track of your";

// Careful this needs to be texted to fit the BOX...
$intro_text_l[$entry]	=	"Check your ranking and prefs for all gametypes, ".
							"look up opponents, and find out so much more... ".
							"Welcome to stats for Unreal Tournament 2003.";

// Not used at present
$info_text_l[$entry] 	=	"Top ranked players for the official, mutator-free gametypes."."<br>".
							"To check out the other gametypes and ranks visit the ".
							"<a href=\"players.html\">$head_nav_names[2]</a> ".	// Players
							"pages.";


// -------------------------------------------------------------------
//...N E W S...

$info_text_l2[$entry] 	=	
"<b><span class=ptitle>"."News"."</span></b><p>".

"Before you send in an email read the <span class=blue>FAQ Infos</span> provided below, only then ".
" send in constructive feedback to ".
"<A HREF=\"mailto:ut2003statsFeedback@epicgames.com?subject=UT2003stats:&nbsp;Feedback\">UT2003stats: Feedback</A>".
" or infos on Bugs to ".
"<A HREF=\"mailto:ut2003statsFeedback@epicgames.com?subject=UT2003stats:&nbsp;Bugs\">UT2003stats: Bugs</A>".
". Do *not* send in game bugs, I code stats not the UT2003 game code!"."<p>".


"<b>"."10/12/2002 1:18PM"."</b> - Christoph"."<br>".
"Even though I have checked in speed improvements of 20% and then another 25%, ".
"<span class=blue><b>".
"stats are only catching up very slowly. Stats processing is behind by about 7 days now."."</b></span><p>".

"Comments/Fixes:"."<br>".
"- Checked in another stats processing speedup of about 25%. "."<br>".
"- In teamgames, negative player score events would always be treated as positive rank events. ".
"This was especially apparent for suicides. Fixed."."<br>".
"- For now Match Rank is turned off, e.g. = 0. This does *NOT* effect RankGain or your overall ranks, just Match Rank!"."<br>".
"- Some pages where showing exceedingly high numbers. This was caused by a (once every million log lines) crash, ".
"that would cause stats to process the same log lines over and over again."."<p>".

"Feedback:"."<br>".
"Thanx again for the email feedback, they where very helpful. ".
"Alas I need to spend so much time on optimizing, that had no time to add new features yet. "."<br>".
"Here is a list of some requested features:"."<br>".
"- Match progress graphs, showing player score vs. match time."."<br>".
"- Show more than the top 10 in ranking, e.g. scroll through the list."."<br>".
"- Show how many folks are actually in ranking for that gametype."."<br>".
"- Add more efficiency columns in tables."."<br>".
"- Games won/lost columns."."<br>".
"- FPH in a few other places."."<br>".
"Note that I can't promise anything, but I will do my best to get features into the pages."."<p>".


"<b>"."10/4/2002 8:36PM"."</b><br>".
"At the moment, the stats server is having trouble keeping up ".
"with the amount of data it has to process.  We're currently ".
"working on optimizing this so it can catch up."."<br>".

"This means:"."<br>".
"1. Servers will not show up in realtime!"."<br>".
"2. Your playerstats will lag, and show up many hours later!"."<br>".
"As long as you are seeing a non 0 MatchID, in ingame scoreboard, your data is being tracked!"."<br>".
"Sorry about this inconvenience."."<p>".

"- Added a huge section on Ranking and RankGain in the below FAQ Infos to help explain things (new stuff denoted by an *).".
"<p>".


"<b>"."10/2/2002 6:48PM"."</b><br>".
"Feature updates / fixes to UT2003stats:"."<br>".
"- Match player minutes only shown if player disconnected or match ended."."<br>".
"- MatchID stats page: Now shows a '-Match in Progress-' for still running matches."."<br>".
"- Match Scoreboard now shows % efficiency: Kills/(Kills+Deaths+Suicides+TKs)."."<br>".
"- All Recent Matches tables are filtered by Connects>1 and Score>0."."<br>".
"<p>".


"<b>"."10/1/2002 12:53PM"."</b><br>".
"Feature updates / fixes to UT2003stats:"."<br>".
"- FPH on Best of pages will only show folks who played for at least 30 minutes."."<br>".
"- Total frags served counter at the bottom of each page."."<br>".
"- Fixed negative RankGain values when winning. Happened if a match was not over yet."."<br>".
"- Now only maps of retail game are marked as official."."<br>".
"<p>".

"<b>"."9/28/2002 7:07PM"."</b><br>".
"Thanx for the constructive feedback, will be adding the suggested features in the next few days. ".
"Below are some frequently asked questions to help you along the way."."<p>".

"<b>"."9/26/2002 6:52PM"."</b><br>".
"Stats have gone live... whee :)"."<p>".


"<span class=ptitle>"."FAQ Infos"."</span><p>".

"<span class=blue>"."How do I find my stats, my PlayerID?"."</span><br>".
"Top right of page enter your nick and hit search."."<p>".

"<span class=blue>"."* Nick search yields 10 different PlayerIDs? Which one is my ID?"."</span><br>".
"- The best way to narrow it down is to play a game on a server with stats turned on."."<br>".
"- Then open the ingame scoreboard (F1) and note the MatchID."."<br>".
"- On the UT2003stats page then search for this MatchID."."<br>".
"- Click on the MatchID link and check out the Match Scoreboard."."<br>".
"- Your nickname and PlayerID will show up in that table."."<br>".
"<p>".

"<span class=blue>"."I can't find my stats, why are none shown?"."</span><br>".
"Most Demo/Retail servers do NOT have stats turned on. You need to play on servers with stats turned on!"."<p>".

"<span class=blue>"."How do I know if a server has stats turned on or off?"."</span><br>".
"- In the Demo, you can join any server and if the ingame scoreboard shows a MatchID, then it has stats turned on."."<br>".
"- Or, if you do not have 'Track Stats' in the Settings/Network menu turned on, a requestor will tell you that you attempted to join  a stats server."."<br>".
"- Retail UT2003 will also show servers with stats turned on in the ingame serverbrowser."."<p>".

"<span class=blue>"."How to I turn on stats tracking / How do I log into stats?"."</span><br>".
"0. This is basically automatic, no web login on this site required!"."<br>".
"1. In Demo/Retail Game, go to the Settings/Network menu and turn on 'Track Stats'."."<br>".
"2. Pick a unique Stats Username / Stats Password combo."."<br>".
"3. Play on servers that have stats turned on!"."<br>".
"4. Check ingame scoreboard for MatchID, shows up on stats servers."."<br>".
"5. Search for your nickname on the stats pages, and thus find your PlayerID. Or do so by searching for the MatchID."."<p>".

"<span class=blue>"."Can I change my nickname?"."</span><br>".
"Yes, you can change it ingame any time you like. UT2003stats will always show the nickname you used last."."<p>".

"<span class=blue>"."Will retail UT2003 stats be merged with Demo stats?"."</span><br>".
"Yes, if you keep your ingame Stats Username and Stats Password."."<p>".


"<span class=blue>"."* Details on Ranking?"."</span><br>".
"Intro: Ranking is based on the ELO Chess ranking system, where your new rank gets calculated from the ".
"old rank by not only taking into account the score between you and the opponent, but also what ".
"previous ranks you and your opponents had, also see the ".
"<a href=\"faq.php\">$head_nav_names[9]</a> ".		// FAQ
"page on this."."<p>".

"<span class=blue>"."* What effects my Rank? How do I get better?"."</span><br>".
"- Your rank is based on your personal scores. Every kill, death, capture, assist, etc."."<br>".
"- What you gain in rank, someone else must lose in rank."."<br>".
"- Fragging folks better than you, gets you more rank points, than fragging weak players."."<br>".
"- Getting fragged by weak players will subtract more points from your rank, than being fragged by strong opponents."."<br>".
"- Since Rank = 0 is the lower rank limit, and all players start with this rank, a Rank of 0 makes a player <span class=blue>average</span>."."<br>".
"- So if you want to keep your rank better than 0, you have to be better than average, ALL the time!"."<br>".
"- Does that make you a rank geek? Probably ;)"."<p>".

"<span class=blue>"."* Are there different Ranks?"."</span><br>".
"- For each gametype you played a Rank is generated for you."."<br>".
"- This is done separately for the 3 time frames: Week, Month, and AllTime."."<br>".
"- So you will have a Rank per gametype per week, a Rank per gametype per Month, and an Alltime Rank per gametype."."<br>".
"- There is also a MatchRank, that is only calculated for the match you played in and is 0 at the start of the match."."<br>".
"- There is no overall rank, that would transcend all gametypes. Would not make sense!"."<p>".

"<span class=blue>"."* What this Ranking is all about?"."</span><br>".
"- This ranking is not about ripping off weak players and thus finishing 1st in scores."."<br>".
"- This ranking is about defeating folks and keeping up that skill level (= Rank)."."<br>".
"- This makes global comparisons possible, since only beating good players keeps you top."."<br>".
"- You might get a high rank quickly by playing much, but just playing much will not get you a better rank ".
"since the more you play, the more you are 'exposed' to loosing your rank points."."<br>".
"- Since this is all based on Scores, e.g. in teamgames you get points for doing things positive for your team. ".
"This is as objective as ranking will get."."<p>".

"<span class=blue>"."* Why is Ranking so different from Scores?"."</span><br>".
"Because these are 2 different ways to assess what a player did in a game."."<br>".
"Scores ... say nothing about how you got to your points, e.g. you might well have ripped off weak players. ".
"Which is not a big deal if the others are newcomers and you are experienced. So scores are not of much value to assess your *real* skill."."<br>".
"Ranking ... on the other hand takes into account, whom you have been hitting, e.g. strong or weak players."."<br>".
"- Thus Ranking tries to calculate your actual skill level. How good you are doing against other players."."<br>".
"- And this lets us objectively compare players on totally different servers and maps."."<br>".
"- Note but not across different gametypes though!"."<p>".


"<span class=blue>"."* What's up with the RankGain shown on the Match pages?"."</span><br>".
"- First off: RankGain only 'hints' at how you are doing, by remembering your Alltime rank."."<br>".
"- Note: It does *not* use the weekly or monthly ranks, that are also tracked for the gametypes you played."."<br>".
"- Important: RankGain has *no* influence on your Ranks whatsoever. RankGain is calculated *from* your Ranks, and can fail ".
"(e.g. not show up) under the special cases mentioned below."."<p>".

"<span class=blue>"."* How does RankGain work?"."</span><br>".
"- At the start of the match your Alltime rank (BegRank) is looked up and at the end of the match or if you ".
"disconnect it is looked up again (EndRank)."."<br>".
"- Formula: RankGain = EndRank - BegRank "."<p>".

"<span class=blue>"."* The RankGain special cases?"."</span><br>".
"- As long as the match is NOT over, RankGain can not be calculated, so none is shown."."<br>".
"- As soon as the match is over and processed, all players will have a RankGain and a PlayerTime."."<br>".
"- Exception: If a game server is shut down during a match, important 'EndGame' infos are not sent, this makes ".
"it impossible to calculate RankGain, e.g. the fields stay empty."."<p>".

"<span class=blue>"."* Why are my RankGains negative?"."</span><br>".
"- There used to be a bug in the code that would calculate the RankGain even though the match was still ".
"up and running. Since the EndRank was not yet set, e.g. 0, negative RankGains would show up. This has been fixed!"."<br>".
"- So if you look up *old* matches the afore mentioned bug can still be seen, if the matches never ended properly."."<p>".

"<span class=blue>"."* I am still seeing negative RankGains. How come? Example:"."</span><br>".
"- You won a Deathmatch match, you made the most frags, but also some suicides and many deaths."."<br>".
"- Your RankGain may well be negative, because you got killed too often by weak players and therefore ".
"lost many points in rank, e.g. performing less well than your average skill."."<br>".
"- The guy 2nd in scores for that match, might have 2 frags less than you did, but half the number of ".
"deaths and no suicides."."<br>".
"- The guy 2nd guy therefore may well have a positive RankGain, since only a few events (suicides, deaths) reduced ".
"his rank."."<br>".
"- Note the RankGain will only be positive if you are actually playing better, not in scores but in a rank based ".
"skill assessment, than you were doing in the games before. So if you were really good and efficient the ".
"match before, your rank would set a very high level of skill you need to keep, to not lose it."."<br>".
"- So you *will* see negative RankGains since someone has to lose in rank while others win in rank."."<p>".

"<span class=blue>"."How soon can I check on my stats?"."</span><br>".
"Most of the time your stats will show up in realtime, at times it will take a bit longer."."<p>".

"<span class=blue>"."I lost my Stats Password, can you send it to me?"."</span><br>".
"We have *no* way to access your Stats Password. So do not lose it!"."<p>".

"<span class=blue>"."Are stats working with the Demo?"."</span><br>".
"Yes stats have been collecting data, and will continue to do so."."<p>";



?>