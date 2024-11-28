<?php
//---------------------------------------------------------
//---SETTING UP THE COMPARE, all the possible parameters---

if( $included_check_playerid == 0 )		include "func/check_playerid.inc";

$DEBUG	= 0;

// First do a playerID check if it exists or does not! 
if($playerID!="")
	$playerID  = $playerID  + 0;				// Convert trash to a number!
if($playerID2!="")
	$playerID2 = $playerID2 + 0;

// Will pass trash to the DB !              
$existsPlayerID1 = check_playerid($playerID);
$existsPlayerID2 = check_playerid($playerID2);

if( $DEBUG ){	print "existsPlayerID1: ".$existsPlayerID1." ID $playerID <br>";
			    print "existsPlayerID2: ".$existsPlayerID2." ID $playerID2 <br>";	}
?>


<!-- ------------------------------------------------------------------ -->
<!-- SETTING UP THE GADGETS ------------------------------------------- -->
<!-- ------------------------------------------------------------------ -->

<a name=pCompare></a>
<?php	$url = "?playerID=$playerID&playerID2=$playerID2&tftype=$tftype&gametype=$gametype"; ?>
<!-- <form method=post action="compare.php<?print $url?>"> -->
<form method=post action="compare.php">
<table cellpadding=5 cellspacing=0 border=0>
<tr>
<td><?print $indent?></td>
<td class=search nowrap>1. <span class=blue><?print $head_compare_tags[0]?></span></td>
<td class=text>&nbsp;</td>
<td class=text>
	<input type=text size=10 maxlength=20 name=playerID value="<?print htmlentities($playerID)?>"></td>
<td class=text>&nbsp;</td>
<td class=search><?print $head_compare_tags[1]?></td>
<td class=text>&nbsp;</td>
<td class=search nowrap>2. <span class=blue><?print $head_compare_tags[2]?></span></td>
<td class=text>&nbsp;</td>
<td class=text>
	<input type=text size=10 maxlength=20 name=playerID2 value="<?print htmlentities($playerID2)?>"></td>
<td class=text>&nbsp;</td>
<td class=text nowrap>
	<input type=submit name=searchbutton value="<?print $head_compare_button[0]?>">
	<input type=reset name=resetbutton value="<?print $head_compare_button[1]?>">
</td>
</tr>

<tr>
<td><?print $indent?></td>
<td class=search><?print $head_compare_tags[3]?></td>
<td class=text>&nbsp;</td>
<td class=text COLSPAN=4 ALIGN=LEFT><?print $head_compare_tf[0]?>&nbsp;
	<select name=tftype>
	<option value="tf3"	<?if($tftype=="tf3" || $tftype=="")	print "selected";?>><?print $head_compare_timeframe[0]."\n"?>
	<option value="tf2"	<?if($tftype=="tf2")				print "selected";?>><?print $head_compare_timeframe[1]."\n"?>
	<option value="tf1"	<?if($tftype=="tf1")				print "selected";?>><?print $head_compare_timeframe[2]."\n"?>
	</select>&nbsp;&nbsp;
</td>

<td class=text COLSPAN=6><?print $head_compare_tf[1]?>&nbsp;
	<select name=gametype>
<?php	
		for( $g=0; $g < sizeof($head_gametypes); $g++ )
		{
			$option = "\t<option value=\"gt$g\"";
			if( $gametype=="gt$g" || ( $g==0 AND $gametype=="" ) ) 
			{
				$option .= " selected";
				$mid = $head_mids[$g];		// Remember the $mid on the spot!
			}	
			$option .= ">$head_gametypes[$g]\n";
			print $option;
		}
?>
	</select>&nbsp;&nbsp;
</td>
</tr>

<tr>
<td><?print $indent?></td>
<td class=search><?print $head_compare_tags[4]?></td>
<td width=15>&nbsp;</td>
<td class=text COLSPAN=9>
<?php	
	$showdata = 0;
	$pID = "".$playerID; $pID2 = "".$playerID2;	// The below would convert to INT, thus failing
	if( $pID=="" && $pID2=="" )
	{
    	$result = $head_compare_info[0];		// "Please enter 2 PlayerIDs into the above fields, and hit Compare."
	}
	else if( !$existsPlayerID1 && !$existsPlayerID2 )
	{
	    $result = $head_compare_warn[2];		// "Sorry, both PlayerIDs are invalid, please pick proper IDs.", 
	}
	else if( !$existsPlayerID2 )
	{
	    $result = $head_compare_warn[1];		// "Sorry, 2. PlayerID is invalid, please pick another one.", 
	}
	else if( !$existsPlayerID1 ) 
	{
	    $result = $head_compare_warn[0];		// "Sorry, 1. PlayerID is invalid, please pick another one.", 
	}
	else if( $playerID==$playerID2 )
	{
    	$result = $head_compare_info[1];		// "Please enter 2 different PlayerIDs in fields above, and try again."
	}
    else
    {
    	$result = $head_compare_info[2];		// "Found data for both PlayerIDs, see below:"
		$showdata = 1;							// Valid entries, start showing data!
	}	
	print $result;
?>
</td>
</tr>

</table>
</form>


<?php
	$timeFrame =  substr( $tftype, 2 ) + 0;
	if( $DEBUG ){
		 print "playerID 1: -".$playerID."-  playerID 2: -".$playerID2."-  <br>";
		 print "gametype: -".$gametype."-  tftype: -".$tftype."-  <br>";
		 print "timeFrame: -".$timeFrame."-<br>";
		 print "Gametype mid: -".$mid."-<br>";
		 print "Showdata: -".$showdata."-<br>";
	}
?>	

