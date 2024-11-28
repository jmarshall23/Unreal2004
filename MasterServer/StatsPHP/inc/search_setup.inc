<?php
//---Setting up the search, all the possible parameters---

$DEBUG = 0;

// Putting order of sorting together
if( $orderby=="" || $orderby=="primary" )
{
	$orderbyclause = "col1";	$selectedcolumn = 1;
}
else if ( $orderby=="secondary" )
{
	$orderbyclause = "col2";	$selectedcolumn = 2;
}
else
{
	$orderbyclause = "col3";	$selectedcolumn = 3;
}

// SORTORDER
if( $sortorder=="descending" )
	$orderbyclause = $orderbyclause." desc";

// CASEMATCH
if( $casetype=="" || $casetype=="icase" )
	$casematch = "";
else
	$casematch = "BINARY";


// COMPARETYPE
switch ($comparetype)
{
	case "begin":
		$prejoker	= "";	$postjoker	= "%";
		break;
	case "end":
		$prejoker	= "%";	$postjoker	= "";
		break;
	case "exact":
		$prejoker	= "";	$postjoker	= "";
		break;
	case "phrase":
	default:
		$prejoker	= "%";	$postjoker	= "%";
}


//Initializing vars and defining table header
unset($tablematrix);							//since matrix is remembered, needs to be unset!
$table_title	= $table_title_l['search'];
$table_text		= $table_text_l['search'];
$startat 		= $startat + 0;

switch ($querytype)
{
	case "nick":
		$where		= 	"nickname like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";	//this is a PHRASE! only doing phrase checking
		$select		= 	"select count(nickname) from playerid where $where";
		$select2	= 	"select nickname as col1, playerid as col2, lastUsed as col3 from playerid where $where ".
						"order by $orderbyclause limit $startat,$show";
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";
		$tablematrix[0] = $table_head_l[$querytype];
		break;
	case "playerID":
		$where		=	"playerid like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";
		$select		=	"select count(playerid) from playerid where $where";
		$select2	=	"select playerid as col1, nickname as col2, lastUsed as col3 from playerid where $where ".
						"order by $orderbyclause limit $startat,$show";
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";
		break;
	case "matchID":
		$where		=	"matchid like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";
		$select		=	"select count(matchid) from matches where $where";
		$select2	=	"select matchid as col1, svid as col2, mapid as col3, mid, startmatch, endgame, playerconnects ".
						"from matches where $where order by $orderbyclause limit $startat,$show";
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";	//was startgame
		break;
	case "server":
		$where		=	"serverName like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";
		$select		=	"select serverID from server, servers ".
						"where servers.svid=server.svid and timeFrame=3 and timeFrameNumber=1 and $where ";
		$select2	=	"select serverName as col1, serverID as col2, ipPort as col3, uptime, lastUpdate, connects, maps ".
						"from server, servers where servers.svid=server.svid and timeFrame=3 and timeFrameNumber=1 and $where ".
						"order by $orderbyclause limit $startat,$show";	//had group by server.svid 
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";
		break;
	case "serverID":
		$where		=	"serverID like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";
		$select		=	"select serverID from server, servers ".
						"where servers.svid=server.svid and timeFrame=3 and timeFrameNumber=1 and $where ";
		$select2	=	"select serverID as col1, serverName as col2, ipPort as col3, uptime, lastUpdate, connects, maps ".
						"from server, servers where servers.svid=server.svid and timeFrame=3 and timeFrameNumber=1 and $where ".
						"order by $orderbyclause limit $startat,$show"; // had group by server.svid 
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";
		break;
	case "map":
		$where		=	"mapname like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";
		$select		=	"select mapname from map,maps where map.mapid=maps.mapid and $where ".
						"group by map.mapid";
		$select2	=	"select mapname as col1, map.mapid as col2, maptitle as col3, sum(uptime), sum(games), sum(connects) ".
						"from map,maps where map.mapid=maps.mapid and $where ".
						"group by map.mapid order by $orderbyclause limit $startat,$show";
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";
		break;
	case "mod":
		// Now with real time translation search!
		$where		=	"translation like $casematch \"$prejoker".AddSlashes($query)."$postjoker\"";
		$select		=	"select modcode from mod,maps,translations ".
						"where mod.mid=maps.mid and modcode=code and $where ".
						"group by mod.mid";
		$select2	=	"select translation as col1, mod.mid as col2, sum(events) as col3, sum(uptime), sum(games), sum(connects) ".
						"from mod,maps,translations where mod.mid=maps.mid and modcode=code and $where ".
						"group by mod.mid order by $orderbyclause limit $startat,$show";
						if( $DEBUG ) print "Select 1: \"$select\"<br>\nSelect 2: \"$select2\"<br>\n";
		break;
	default:		//same as nick
		break;
}

$tablematrix[0] = $table_head_l[$querytype];

if( $DEBUG ) print "where: ".$where."<br>\n";

// Making the database query
if( $query != "" )
{
	if( $DEBUG ) print "Querytype: \"$querytype\", Query: \"$query\"<br>";

	$link	= mysql_query( $select );
		if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select\"<br>\n";
	$row = mysql_fetch_row( $link );
	$count = $row[0];

	if( $querytype == "map" 	OR  $querytype == "mod"  OR 
		$querytype == "server"	OR	$querytype == "serverID" )
		$count = mysql_num_rows($link); 	// grouping and counting messes things up.

	if( $count > 0 )
	{
		if( $startat < 0 )
			$startat = 0;
		$link = mysql_query( $select2 );
		if( $DEBUG ) print "Found ".mysql_num_rows($link)." rows for \"$select2\"<br>\n";
	}
}


?>