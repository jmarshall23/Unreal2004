<?php
//---Generating the search output---
/*
	External vars used:

	$playerID_globalSummary		page var, >0 per player, else all

*/
$DEBUG = 0;

if( $count > 0 )
{
	for( $i=0; $i<$show && $i < $count ; $i++ )	//mysql_num_rows($link)
	{
		$row = mysql_fetch_row( $link );
        $tablematrix[$i+1][0]=$i+1+$startat;

		switch ($querytype)
		{
			case "nick":
				//" select 0 nickname, 1 playerid, 2 lastUsed "
				$nickname = $row[0];
				$playerid = $row[1];
			    if( strlen($nickname)>20 ) 
					$nickname = substr($nickname, 0, 17)."...";
		    	$tablematrix[$i+1][1] = "<a href=\"playerID.php?playerID=$row[1]&modID=-1\">".htmlspecialchars($nickname)."</a>";
		    	$tablematrix[$i+1][2] = "<a href=\"playerID.php?playerID=$row[1]&modID=-1\">$playerid</a>";
		    	$tablematrix[$i+1][3] = $row[2];
				break;
			case "playerID":
				//" select 0 nickname, 1 playerid, 2 lastUsed "
				$playerid = $row[0];
				$nickname = $row[1];
			    if( strlen($nickname)>20 ) 
					$nickname = substr($nickname, 0, 17)."...";
		    	$tablematrix[$i+1][1] = "<a href=\"playerID.php?playerID=$row[0]&modID=-1\">$playerid</a>";
		    	$tablematrix[$i+1][2] = "<a href=\"playerID.php?playerID=$row[0]&modID=-1\">".htmlspecialchars($nickname)."</a>";
		    	$tablematrix[$i+1][3] = $row[2];
				break;
			case "matchID":
				//"select 0 matchid , 1 svid, 2 mapid, 3 mid, 4 startgame, 5 endgame, 6 playerconnects "
		        $matchlink	= "<a href=\"matchID.php?matchID=$row[0]&modID=$row[3]&playerID=-1\">$row[0]</a>";
				$server 	= get_servername($row[1]);
				$map	 	= get_mapname($row[2]);
		    	$tablematrix[$i+1][1] = $matchlink;
		    	$tablematrix[$i+1][2] = $server;
		    	$tablematrix[$i+1][3] = $map;
		    	$tablematrix[$i+1][4] = $row[4];	//startgame
		    	$tablematrix[$i+1][5] = $row[5];	//endgame
		    	$tablematrix[$i+1][6] = $row[6];	//connects
				break;
			case "server":
				//"select 0 serverName, 1 serverID, 2 ipPort, 3 uptime, 4 lastUpdate, 5 connects, 6 maps "
				$serverlink 	= "<a href=\"serverID.php?serverID=$row[1]\">".htmlspecialchars($row[0])."</a>";
				$serveridlink	= "<a href=\"serverID.php?serverID=$row[1]\">$row[1]</a>";
		    	$tablematrix[$i+1][1] = $serverlink;				// Server
		    	$tablematrix[$i+1][2] = $serveridlink;				// ServerID
		    	$tablematrix[$i+1][3] = htmlspecialchars($row[2]);	// IP:Port
		    	$tablematrix[$i+1][4] = $row[3];					// Uptime
		    	$tablematrix[$i+1][5] = $row[4];					// lastUpdate
		    	$tablematrix[$i+1][6] = $row[5];					// Connects
		    	$tablematrix[$i+1][7] = $row[6];					// Maps
				break;
			case "serverID":
				//"select 0 serverID, 1 serverName, 2 ipPort, 3 uptime, 4 lastUpdate, 5 connects, 6 maps "
				$serveridlink	= "<a href=\"serverID.php?serverID=$row[0]\">$row[0]</a>";
				$serverlink 	= "<a href=\"serverID.php?serverID=$row[0]\">".htmlspecialchars($row[1])."</a>";
		    	$tablematrix[$i+1][1] = $serveridlink;				// ServerID
		    	$tablematrix[$i+1][2] = $serverlink;				// Server
		    	$tablematrix[$i+1][3] = htmlspecialchars($row[2]);	// IP:Port
		    	$tablematrix[$i+1][4] = $row[3];					// Uptime
		    	$tablematrix[$i+1][5] = $row[4];					// lastUpdate
		    	$tablematrix[$i+1][6] = $row[5];					// Connects
		    	$tablematrix[$i+1][7] = $row[6];					// Maps
				break;
			case "map":
				//"select 0 mapname, 1 map.mapid, 2 maptitle, 3 sum(uptime), 4 sum(games), 5 sum(connects) ".
                $map	 	= get_mapname($row[1]);					// htmlspecialchars($row[0])
				$maplink 	= "<a href=\"mapID.php?mapID=$row[1]\">".$map."</a>";
        		$mapidlink 	= "<a href=\"mapID.php?mapID=$row[1]\">$row[1]</a>";
		    	$tablematrix[$i+1][1] = $maplink;					// Map Name
		    	$tablematrix[$i+1][2] = $mapidlink;					// MapID
		    	$tablematrix[$i+1][3] = htmlspecialchars($row[2]);	// Map Title
		    	$tablematrix[$i+1][4] = $row[3];					// Uptime
		    	$tablematrix[$i+1][5] = $row[4];					// Games
		    	$tablematrix[$i+1][6] = $row[5];					// Connects
				break;
			case "mod":
				//"select 0 translation, 1 mod.mid, 2 sum(events), 3 sum(uptime), 4 sum(games), 5 sum(connects) ".
				$modlink 	= "<a href=\"modID.php?modID=$row[1]\">".htmlspecialchars($row[0])."</a>";
				$midlink	= "<a href=\"modID.php?modID=$row[1]\">$row[1]</a>";
		    	$tablematrix[$i+1][1] = $modlink;					// Mod Name
		    	$tablematrix[$i+1][2] = $midlink;					// ModID
		    	$tablematrix[$i+1][3] = $row[2];					// Events
		    	$tablematrix[$i+1][4] = $row[3];					// Uptime
		    	$tablematrix[$i+1][5] = $row[4];					// Games
		    	$tablematrix[$i+1][6] = $row[5];					// Connects
				break;
			default:		//same as nick
				break;
		}
	}

	// Enhancing title
	$table_title .= " <span class=blue>".$tablematrix[0][$selectedcolumn]."</span>";	//$querytype

	//Focus on column data is sorted by
	for( $i=0; $i < sizeof($tablematrix); $i++ )
        	$tablematrix[$i][$selectedcolumn] = "<b>".$tablematrix[$i][$selectedcolumn]."</b>";


	//-------------------------------------------------------------------------
	// show matrix
	$size='s';								//'s'mall or 'n'normal possible
	$tablecontrol = array (
		"mode"			=> "t",
		"option"		=> "head",
		"tabletitle"	=> $table_title,	//externally set
		"tabletext"		=> $table_text,		//externally set
		"indent"		=> $indent,			//externally set
		"ind_tb"		=> $ind_tb,			//externally set

		"showzero"		=> 1,				//0 shows &nbsp;
		"headclass"		=> $size."hbg",
		"colheadclass"	=> $size."tbg",
		"colclass"		=> $size,
		"colaltclass"	=> $size."abg",		//small shbg, stbg, s, sabg
		"headalign"		=> "ALIGN=center",
		"columnalign"	=> "",				//"" is ALIGN=LEFT
		"counteralign"	=> "ALIGN=right"
	);
	table($tablecontrol,$tablematrix);
	//-------------------------------------------------------------------------
}
?>
