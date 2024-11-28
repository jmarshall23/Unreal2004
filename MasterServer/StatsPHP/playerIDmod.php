<?php
//---Subpage to playerID.php - only data calls---
/*
	External vars used:
	$playerID		sent as part of URL
*/
	$playerID_globalSummary = $playerID;	// part of URL
	$mid_playeridSummary	= $modID;		// part of URL
?>

<a name=pLastMatches></a>
<?php
	$mode_playeridLastMatches = 0;
	include "stats/table_playeridLastMatches.inc";
?>
<p>

<a name=pInRankZone></a>
<?php
	$mode_playeridRankZone = 0;
	include "stats/table_playeridRankZone.inc";
?>
<p>


<a name=pSummary></a>
<?php
	include "stats/table_playeridSummary.inc";	//uses $mid
?>
<p>


<a name=pFrags></a>
<?php
	$mode_globalKDSESc=0;					//kills/deaths
	include "stats/table_globalKDSESc.inc";
?>
<p>


<a name=pSuicides></a>
<?php
	$mode_globalKDSESc=1;					//suicides
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pEvents></a>
<?php
	$mode_globalKDSESc=2;					//events
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pScores></a>
<?php
	$mode_globalKDSESc=3;					//scores
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pMutators></a>
<?php
	$mode_globalKDSESc=4;					//mutators
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pMaps></a>
<?php
	$mode_globalKDSESc=5;					//maps
	include "stats/table_globalKDSESc.inc";
?>
<p>

<a name=pMods></a>
<?php
	$mode_globalKDSESc=6;					//mods
	include "stats/table_globalKDSESc.inc";
?>
<p>
