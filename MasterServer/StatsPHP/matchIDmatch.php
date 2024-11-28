<?php
//---Subpage to mathID.php - only data calls---
/*
	External vars used:
	$playerID		sent as part of URL
*/
	$matchID_Summary	= $matchID;
	$modID_Summary		= $modID;
	$playerID_Summary	= $playerID;
?>


<a name=pDetails></a>
<?php
	$matchID_Summary_part = 1;
	include "stats/table_matchidSummary.inc";
?>
<p>


<a name=pSummary></a>
<?php
	$matchID_Summary_part = 2;
	include "stats/table_matchidSummary.inc";
?>
<p>


<a name=pScoreBoard></a>
<?php
	include "stats/table_matchidScoreBoard.inc";
?>
<p>


<a name=pFrags></a>
<?php
	$mode_matchIDdata=0;					//kills/deaths
	include "stats/table_matchidData.inc";
?>
<p>


<a name=pSuicides></a>
<?php
	$mode_matchIDdata=1;					//suicides
	include "stats/table_matchidData.inc";
?>
<p>

<a name=pEvents></a>
<?php
	$mode_matchIDdata=2;					//events
	include "stats/table_matchidData.inc";
?>
<p>

<a name=pScores></a>
<?php
	$mode_matchIDdata=3;					//scores
	include "stats/table_matchidData.inc";
?>
<p>

<a name=pMutators></a>
<?php
	$mode_matchIDdata=4;					//mutators
	include "stats/table_matchidData.inc";
?>
<p>


<a name=pGameRules></a>   					
<?php
	$matchID_GameRules = 0;					//gamerules
	include "stats/table_matchidGameRules.inc";
?>
<p>
