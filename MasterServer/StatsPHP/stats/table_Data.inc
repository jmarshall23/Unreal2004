<?php
//---Showing the sums of data: Maps, Servers, Mods pages---
if( $included_get_modname == 0 )				include "func/get_modname.inc";
if( $included_get_mapname == 0 )				include "func/get_mapname.inc";
if( $included_get_servernameID == 0 )			include "func/get_servernameID.inc";
if( $included_get_svid == 0 )					include "func/get_svid.inc";
if( $included_tablematrix_sumpercent == 0 )		include "func/tablematrix_sumpercent.inc";
if( $included_tablematrix_colhighlight == 0 )	include "func/tablematrix_colhighlight.inc";

$DEBUG		= 0;
$MODE		= $mode_Data;
$mapid		= $mapID;
$serverid	= $serverID;
$mid		= $modID;

// Setting some where statements
$title_pre	= "";								//all Maps (also unset cases!)
$where		= "";
$limit		= "";
if( $MODE>=1 AND $MODE<=3 )						//	$mapid > 0
{
	$title_pre = "<span class=blue>".get_mapname($mapid)."</span><br>";
	$where = "where mapid=$mapid ";				// per map
}
if( $MODE>=5 AND $MODE<=7 )
	$title_pre = "<span class=blue>".get_servernameID($serverid)."</span><br>";
if( $MODE>=9 AND $MODE<=11 )
	$title_pre = "<span class=blue>".get_modname($mid)."</span><br>";

if( $MODE == 2 OR $MODE == 4 OR $MODE == 11 )
	$limit = "limit ".$show_servers_X;			// global 50;

//Initializing vars and defining table header
unset($tablematrix);

// The Specific Table info tag names
if( $MODE == 0 ) $entry = 'mapsTop';
if( $MODE == 1 ) $entry = 'mapIDSummary';
if( $MODE == 2 ) $entry = 'mapIDServers';		//	$show_servers_X	= 50;
if( $MODE == 3 ) $entry = 'mapIDMods';

if( $MODE == 4 ) $entry = 'serversTop';
if( $MODE == 5 ) $entry = 'serverIDSummary';
if( $MODE == 6 ) $entry = 'serverIDMaps';
if( $MODE == 7 ) $entry = 'serverIDMods';

if( $MODE == 8 ) $entry = 'modsTop';
if( $MODE == 9 ) $entry = 'modIDSummary';
if( $MODE == 10) $entry = 'modIDMaps';
if( $MODE == 11) $entry = 'modIDServers';

// Setting the table data
$table_mode 	= "t";							//no table header line!
$table_title	= $title_pre.$table_title_l[$entry];
$table_text		= $table_text_l[$entry];
$table_total	= $table_total_l[$entry];
$table_span		= "";							// not used

$tablematrix[0] = $table_head_l[$entry];
$table_blocks	= $head_teams;
$table_teamNo	= 1;


// Setting table data for the possible 2 teams
if( $MODE == 0 OR $MODE == 1 )		// Maps Info,  MapID summary
	$select	= 	"select mapid, sum(uptime) as suptime, sum(connects), sum(games), sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"$where group by mapid order by suptime DESC $limit";
if( $MODE == 2 )					// Map Server infos
	$select	= 	"select svid, sum(uptime) as suptime, sum(connects), sum(games), sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"$where group by svid order by suptime DESC $limit";
if( $MODE == 3 )					// Map Mod infos
	$select	= 	"select mid, sum(uptime) as suptime, sum(connects), sum(games), sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"$where group by mid order by suptime DESC $limit";
if( $MODE == 4 )					// Server infos
	$select	= 	"select server.svid, serverID, serverName, ipPort, firstConnect, serverRegion, ".
				"maps, uptime, lastUpdate, timeZone, connects from servers, server ".
				"where server.svid=servers.svid and timeFrame=3 and timeFrameNumber=1 ".
				"order by connects DESC $limit";
if( $MODE == 5 )					// ServerID summary
	$select	= 	"select server.svid, serverID, serverName, ipPort, firstConnect, serverRegion, ".
				"maps, uptime, lastUpdate, timeZone, connects from servers, server ".	//, adminName, adminEmail
				"where server.svid=servers.svid and timeFrame=3 and timeFrameNumber=1 and serverID=$serverID ";
if( $MODE == 6 )					// Server Map infos was (2)
	$select	= 	"select mapid, sum(uptime), sum(connects), sum(games) as sgames, sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"where svid=".get_svid($serverid)." group by mapid order by sgames DESC ";
if( $MODE == 7 )					// Server Mod infos
	$select	= 	"select mid, sum(uptime), sum(connects), sum(games) as sgames, sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"where svid=".get_svid($serverid)." group by mid order by sgames DESC ";
if( $MODE == 8 )					// Mods Top
	$select	= 	"select mid, sum(uptime), sum(connects), sum(games) as sgames, sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"group by mid order by sgames DESC ";
if( $MODE == 9 )					// Mod Summary
	$select	= 	"select mid, sum(uptime), sum(connects), sum(games) as sgames, sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"where mid=$mid group by mid order by sgames DESC ";
if( $MODE == 10 )					// Mod Maps
	$select	= 	"select mapid, sum(uptime), sum(connects), sum(games) as sgames, sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"where mid=$mid group by mapid order by sgames DESC ";
if( $MODE == 11 )					// Mod Servers
	$select	= 	"select svid, sum(uptime), sum(connects), sum(games) as sgames, sum(kills-suicides-teamkills) as frags, ".
				"sum(kills), sum(suicides), sum(teamkills), sum(events) from maps ".
				"where mid=$mid group by svid order by sgames DESC $limit";

$link	= mysql_query( $select );
$count	= mysql_num_rows( $link );
	if( $DEBUG ) print "Found ".$count." rows for \"$select\"<br>\n";


// Going through the sorted list of players for each team
for( $j=0; $j<$count; $j++ )
{
	$row 		= mysql_fetch_row( $link );
   	$id			= $row[0];

	// Need to do own per matrix line counts!

	if( ($MODE >= 0 AND $MODE <= 3) OR ($MODE >= 6 AND $MODE <= 11) )
	{
	   	$uptime		= $row[1];
	    $connects	= $row[2];
    	$games		= $row[3];
    	$frags		= $row[4];
    	$kills		= $row[5];
    	$suicides 	= $row[6];
    	$teamkills	= $row[7];
    	$events		= $row[8];
	}
	if( $MODE == 4 OR $MODE == 5 )
	{
	   	$serverid		= $row[1];
	    $serverName		= $row[2];
		// This is the only time the full name in server summary is shown!
		if( $MODE == 4 )
			if( strlen($serverName)>30 )  $serverName	= substr($serverName, 0, 27)."...";		// servername
	    $serverName		= htmlspecialchars($serverName);
    	$ipPort			= $row[3];
    	$firstConnect	= $row[4];
    	$serverRegion	= $row[5];
    	$maps		 	= $row[6];
    	$uptime			= $row[7];
    	$lastUpdate		= $row[8];
    	$timeZone		= $row[9];
    	$connects		= $row[10];
	}
/*
	if( $MODE == 5 )
	{
		$adminName		= htmlspecialchars($row[11]);
		$adminEmail		= htmlspecialchars($row[12]);
	}
*/

	//Looking up readable names
	if( $MODE == 0 OR $MODE == 1 OR $MODE == 6 OR $MODE == 10 )
		$select = "select mapname, maptitle, official  from map where mapid=$id";
	if( $MODE == 2 OR $MODE == 11 )
		$select = "select serverName, serverID from server where svid=$id";
	if( $MODE == 3 OR $MODE == 7 OR $MODE == 8 OR $MODE == 9 )
		$select = "select modcode, teamgame, official from mod where mid=$id"; 	// teamgame is a dummy
	if( $MODE == 4 OR $MODE == 5 )
		$select = "select serverName from server where serverID=$serverid";		// ACTUALLY NOT USED.
	$link2		= mysql_query( $select );
				if( $DEBUG ) print "Found ".mysql_num_rows($link2)." rows for \"$select\"<br>\n";
	$row2		= mysql_fetch_row( $link2 );

	if( $MODE == 3 OR $MODE == 7 OR $MODE == 8 OR $MODE == 9 )
	{
		$name		= $row2[0]; 	// do not convert modcode, used for lookups in DB...
		$name2		= $row2[1];
	}		
	else
	{
		$name		= $row2[0]; 	// do not convert modcode, used for lookups in DB...
		$name2		= $row2[1];
		if( $MODE == 0 OR $MODE == 1 OR $MODE == 6 OR $MODE == 10 )
		{
			if( strlen($name)>20 )  $name	= substr($name, 0, 17)."...";		// mapname
			if( strlen($name2)>20 ) $name2	= substr($name2, 0, 17)."...";		// maptitle
		}			
		else if( $MODE == 2 OR $MODE == 11 )
		{
			if( strlen($name)>20 )  $name	= substr($name, 0, 17)."...";		// servername
		}			
		$name		= htmlspecialchars($name);
		$name2		= htmlspecialchars($name2);
	}
	$official	= $row2[2];

	// Filling table with data...
	if( $MODE == 0 )
	{
		$maplink 	= "<a href=\"mapID.php?mapID=$id\">$name</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
		$tablematrix[$j+1][1]	= $maplink;		// mapname
		$tablematrix[$j+1][2]	= $uptime;
		//					3 	% uptime
		$tablematrix[$j+1][4]	= $connects;
		$tablematrix[$j+1][5]	= $games;
		$tablematrix[$j+1][6]	= $frags;
		//					7 	% frags
		$tablematrix[$j+1][8]	= $kills;
		$tablematrix[$j+1][9]	= $suicides;
		$tablematrix[$j+1][10]	= $teamkills;
		$tablematrix[$j+1][11]	= $events;
		$tablematrix[$j+1][12]	= $official?"Yes":"No";
	}
	if( $MODE == 1 )
	{
   		$tablematrix[$j+1][0]	= $name;		// mapname
		$tablematrix[$j+1][1]	= $name2;		// maptitle
		$tablematrix[$j+1][2]	= $uptime;
		$tablematrix[$j+1][3]	= $connects;
		$tablematrix[$j+1][4]	= $games;
		$tablematrix[$j+1][5]	= $frags;
		$tablematrix[$j+1][6]	= $kills;
		$tablematrix[$j+1][7]	= $suicides;
		$tablematrix[$j+1][8]	= $teamkills;
		$tablematrix[$j+1][9]	= $events;
		$tablematrix[$j+1][10]	= $official?"Yes":"No";
	}
	if( $MODE == 2 )
	{
		$serverid = $name2;
		$serverlink 	= "<a href=\"serverID.php?serverID=$serverid\">$name</a>";
		$serveridlink	= "<a href=\"serverID.php?serverID=$serverid\">$name2</a>";
   		$tablematrix[$j+1][0]	= $serverlink;
		$tablematrix[$j+1][1]	= $serveridlink;
		$tablematrix[$j+1][2]	= $uptime;
		//					3 	% uptime
		$tablematrix[$j+1][4]	= $connects;
		$tablematrix[$j+1][5]	= $games;
		$tablematrix[$j+1][6]	= $frags;
		//					7 	% frags
    	$tablematrix[$j+1][8]	= $kills;
		$tablematrix[$j+1][9]	= $suicides;
		$tablematrix[$j+1][10]	= $teamkills;
		$tablematrix[$j+1][11]	= $events;
	}
	if( $MODE == 3 )
	{
		$modname	= translate($name,$db_lang[$db_lid]);
		$modlink 	= "<a href=\"modID.php?modID=$id\">$modname</a>";
		$midlink	= "<a href=\"modID.php?modID=$id\">$id</a>";
   		$tablematrix[$j+1][0]	= $modlink;
		$tablematrix[$j+1][1]	= $midlink;
		$tablematrix[$j+1][2]	= $uptime;
		//					3 	% uptime
		$tablematrix[$j+1][4]	= $connects;
		$tablematrix[$j+1][5]	= $games;
		$tablematrix[$j+1][6]	= $frags;
		//					7 	% frags
    	$tablematrix[$j+1][8]	= $kills;
		$tablematrix[$j+1][9]	= $suicides;
		$tablematrix[$j+1][10]	= $teamkills;
		$tablematrix[$j+1][11]	= $events;
		$tablematrix[$j+1][12]	= $official?"Yes":"No";
	}
	if( $MODE == 4 )
	{
		$serverlink 	= "<a href=\"serverID.php?serverID=$serverid\">$serverName</a>";
		$serverIDlink	= "<a href=\"serverID.php?serverID=$serverid\">$serverid</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
   		$tablematrix[$j+1][1]	= $serverlink;
		$tablematrix[$j+1][2]	= $serverIDlink;
		$tablematrix[$j+1][3]	= $ipPort;
		$tablematrix[$j+1][4]	= $uptime;
		$tablematrix[$j+1][5]	= $maps;
		$tablematrix[$j+1][6]	= $connects;
    	$tablematrix[$j+1][7]	= $firstConnect." (".$serverRegion.")";
		$tablematrix[$j+1][8]	= $lastUpdate." (".$timeZone.")";
	}
	if( $MODE == 5 )
	{
   		$tablematrix[$j+1][0]	= $serverName;
		$tablematrix[$j+1][1]	= $serverid;
		$tablematrix[$j+1][2]	= $ipPort;
		$tablematrix[$j+1][3]	= $uptime;
		$tablematrix[$j+1][4]	= $maps;
		$tablematrix[$j+1][5]	= $connects;
		//$tablematrix[$j+1][6]	= $adminName;
		//$tablematrix[$j+1][7]	= $adminEmail;
    	$tablematrix[$j+1][6]	= $firstConnect." (".$serverRegion.")";
		$tablematrix[$j+1][7]	= $lastUpdate." (".$timeZone.")";
	}
	if( $MODE == 6 )
	{
		$maplink 	= "<a href=\"mapID.php?mapID=$id\">$name</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
		$tablematrix[$j+1][1]	= $maplink;		// mapname
		$tablematrix[$j+1][2]	= $games;
		$tablematrix[$j+1][3]	= $uptime;
		//					4 	% uptime
		$tablematrix[$j+1][5]	= $connects;
		$tablematrix[$j+1][6]	= $frags;
		//					7 	% frags
		$tablematrix[$j+1][8]	= $kills;
		$tablematrix[$j+1][9]	= $suicides;
		$tablematrix[$j+1][10]	= $teamkills;
		$tablematrix[$j+1][11]	= $events;
		$tablematrix[$j+1][12]	= $official?"Yes":"No";
	}
	if( $MODE == 7 )
	{
		$modname	= translate($name,$db_lang[$db_lid]);
		$modlink 	= "<a href=\"modID.php?modID=$id\">$modname</a>";
		$midlink	= "<a href=\"modID.php?modID=$id\">$id</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
   		$tablematrix[$j+1][1]	= $modlink;
		$tablematrix[$j+1][2]	= $midlink;
		$tablematrix[$j+1][3]	= $games;
		$tablematrix[$j+1][4]	= $uptime;
		//					5 	% uptime
		$tablematrix[$j+1][6]	= $connects;
		$tablematrix[$j+1][7]	= $frags;
		//					8 	% frags
    	$tablematrix[$j+1][9]	= $kills;
		$tablematrix[$j+1][10]	= $suicides;
		$tablematrix[$j+1][11]	= $teamkills;
		$tablematrix[$j+1][12]	= $events;
		$tablematrix[$j+1][13]	= $official?"Yes":"No";
	}
	if( $MODE == 8 )
	{
		$modname	= translate($name,$db_lang[$db_lid]);
		$modlink 	= "<a href=\"modID.php?modID=$id\">$modname</a>";
		$midlink	= "<a href=\"modID.php?modID=$id\">$id</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
   		$tablematrix[$j+1][1]	= $modlink;
		$tablematrix[$j+1][2]	= $midlink;
		$tablematrix[$j+1][3]	= $games;
		$tablematrix[$j+1][4]	= $uptime;
		//					5 	% uptime
		$tablematrix[$j+1][6]	= $connects;
		$tablematrix[$j+1][7]	= $frags;
		//					8 	% frags
    	$tablematrix[$j+1][9]	= $kills;
		$tablematrix[$j+1][10]	= $suicides;
		$tablematrix[$j+1][11]	= $teamkills;
		$tablematrix[$j+1][12]	= $events;
		$tablematrix[$j+1][13]	= $official?"Yes":"No";
	}
	if( $MODE == 9 )
	{
		$modname	= translate($name,$db_lang[$db_lid]);
   		$tablematrix[$j+1][0]	= $modname;
		$tablematrix[$j+1][1]	= $id;
		$tablematrix[$j+1][2]	= $games;
		$tablematrix[$j+1][3]	= $uptime;
		$tablematrix[$j+1][4]	= $connects;
		$tablematrix[$j+1][5]	= $frags;
    	$tablematrix[$j+1][6]	= $kills;
		$tablematrix[$j+1][7]	= $suicides;
		$tablematrix[$j+1][8]	= $teamkills;
		$tablematrix[$j+1][9]	= $events;
		$tablematrix[$j+1][10]	= $official?"Yes":"No";
	}
	if( $MODE == 10 )
	{
		$maplink 	= "<a href=\"mapID.php?mapID=$id\">$name</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
		$tablematrix[$j+1][1]	= $maplink;		// mapname
		$tablematrix[$j+1][2]	= $games;
		$tablematrix[$j+1][3]	= $uptime;
		//					4 	% uptime
		$tablematrix[$j+1][5]	= $connects;
		$tablematrix[$j+1][6]	= $frags;
		//					7 	% frags
		$tablematrix[$j+1][8]	= $kills;
		$tablematrix[$j+1][9]	= $suicides;
		$tablematrix[$j+1][10]	= $teamkills;
		$tablematrix[$j+1][11]	= $events;
		$tablematrix[$j+1][12]	= $official?"Yes":"No";
	}
	if( $MODE == 11 )
	{
		$serverid = $name2;
		$serverlink 	= "<a href=\"serverID.php?serverID=$serverid\">$name</a>";
		$serveridlink	= "<a href=\"serverID.php?serverID=$serverid\">$name2</a>";
		$tablematrix[$j+1][0]	= $j+1;			// counter
   		$tablematrix[$j+1][1]	= $serverlink;
		$tablematrix[$j+1][2]	= $serveridlink;
		$tablematrix[$j+1][3]	= $games;
		$tablematrix[$j+1][4]	= $uptime;
		//					5 	% uptime
		$tablematrix[$j+1][6]	= $connects;
		$tablematrix[$j+1][7]	= $frags;
		//					8 	% frags
    	$tablematrix[$j+1][9]	= $kills;
		$tablematrix[$j+1][10]	= $suicides;
		$tablematrix[$j+1][11]	= $teamkills;
		$tablematrix[$j+1][12]	= $events;
   }

}  // end count loop


// Add summary data to the tablematrix
if( $count > 0 )														//Avoid empty lines!
{
	unset($sumcols); unset($percentcols);
	if( $MODE == 0 OR $MODE == 2 OR $MODE == 3 )
	{
			$sumcols		= array( 2,		6	);
			$highlight		= $sumcols;
			$percentcols	= array( 2,3,	6,7	);
	}
	if( $MODE == 1 )
			$highlight		= array( 1 );
	if( $MODE == 4 )
			$highlight		= array( 4, 6 );
	if( $MODE == 5 )
			$highlight		= array( 0 );
	if( $MODE == 6 OR $MODE == 10 )
	{
			$sumcols		= array( 2, 3,		6	);
			$highlight		= array( 2 );
			$percentcols	= array( 	3,4,	6,7	);
	}
	if( $MODE == 7 OR $MODE == 8 OR $MODE == 11  )
	{
			$sumcols		= array( 3, 4,		7	);
			$highlight		= array( 3 );
			$percentcols	= array( 	4,5,	7,8	);
	}
	if( $MODE == 9 )
			$highlight		= array( 2 );
	
	// Calculating totals and percent values
	tablematrix_sumpercent($tablematrix, 1, $count, $sumcols, $percentcols, $table_total );
	
	// Enhancing certain columns, e.g. highlithing 'em.
	if( isset($sumcols) ) $lines=1;
	else $lines=0;
	tablematrix_colhighlight($tablematrix, 0, $count+$lines, $highlight );	//** in for now
}

//-------------------------------------------------------------------------
// show matrix
$size='s';								//'s'mall or 'n'normal possible
$tablecontrol = array (
	"mode"			=> $table_mode,		//t3 3 timeFrames header
	"option"		=> "head",
	"tabletitle"	=> $table_title,	//externally set
	"tabletext"		=> $table_text,		//externally set
	"tablespan"		=> $table_span,		//externally set (for t3 only)
	"tableblocks"	=> $table_blocks,	//externally set
	"indent"		=> $indent,			//externally set
	"ind_tb"		=> $ind_tb,			//externally set

	"showzero"		=> 1,				//0 shows &nbsp;
	"headclass"		=> $size."hbg",
	"colheadclass"	=> $size."tbg",
	"colclass"		=> $size,
	"colaltclass"	=> $size."abg",		//small shbg, stbg, s, sabg
	"headalign"		=> "ALIGN=center",	//"" is ALIGN=LEFT
	"columnalign"	=> "ALIGN=right",
	"counteralign"	=> "ALIGN=right"
);
table($tablecontrol,$tablematrix);
//-------------------------------------------------------------------------
?>
