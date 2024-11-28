<?php
//$head_title		set for each page
//$head_where		needs to be set for each page... use $head_nav_names[] ?

$global_page = 'search';
	include "inc/global_vars.inc";				//getting global vars
	include "inc/db_login.inc";					//access to db
	include "inc/table.inc";					//data tables need this

if( $included_get_mapname == 0 )	include "func/get_mapname.inc";
if( $included_get_servername == 0 )	include "func/get_servername.inc";

$head_title	= $head_title_l[$global_page];		//." ".$querytype getting the page title
$head_where	= $head_where_l[$global_page];		//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];		//global var, language

$index_links =
	"<a href=#pASearch>$index_info[0]</a> $index_info[1]<br>".
	"<a href=#pData>$index_info[2]</a> $index_info[3]<br>";

$intro_title = $intro_title_l[$global_page];	//global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!- ---DATA TABLES----------------------------------------------------- -->
<p>

<?php
	include "inc/search_setup.inc";			//setting up search
?>
<!-- ------------------------------------------------------------------ -->

<a name=pASearch></a>
<form method=post action="search.php?show=<?print $show?>">
<table cellpadding=5 cellspacing=0 border=0>
<tr>
<td><?print $indent?></td>
<td class=search><?print $head_search_tags[0]?></td>
<td width=15>&nbsp;</td>
<td class=text>
	<input type=text size=30 name=query value="<?print htmlentities($query)?>">
	<select name=querytype>
	<option value="nick"		<?if($querytype=="" || $querytype=="nick")	print "selected";?>><?print $head_search_names[0]?>
	<option value="playerID"	<?if($querytype=="playerID")				print "selected";?>><?print $head_search_names[1]?>
	<option value="matchID" 	<?if($querytype=="matchID")					print "selected";?>><?print $head_search_names[2]?>
	<option value="server" 		<?if($querytype=="server")					print "selected";?>><?print $head_search_names[3]?>
	<option value="serverID"	<?if($querytype=="serverID")				print "selected";?>><?print $head_search_names[4]?>
	<option value="map"			<?if($querytype=="map")						print "selected";?>><?print $head_search_names[5]?>
	<option value="mod"			<?if($querytype=="mod")						print "selected";?>><?print $head_search_names[6]?>
	</select>
	<input type=submit name=searchbutton value="<?print $head_search_button[0]?>">
	<input type=reset name=resetbutton value="<?print $head_search_button[1]?>">
</td>
</tr>

<tr>
<td><?print $indent?></td>
<td class=search><?print $head_search_tags[1]?></td>
<td width=15>&nbsp;</td>
<td class=text>
	<select name=comparetype>
	<option value="phrase"	<?if($comparetype=="phrase" || $comparetype=="")print "selected";?>><?print $head_search_compare[0]?>
	<option value="begin"	<?if($comparetype=="begin")			print "selected";?>><?print $head_search_compare[1]?>
	<option value="end" 	<?if($comparetype=="end")			print "selected";?>><?print $head_search_compare[2]?>
	<option value="exact"	<?if($comparetype=="exact")			print "selected";?>><?print $head_search_compare[3]?>
	</select>
	<select name=casetype>
	<option value="icase"	<?if($casetype=="icase" || $casetype=="")	print "selected";?>><?print $head_search_case[0]?>
	<option value="case"	<?if($casetype=="case")				print "selected";?>><?print $head_search_case[1]?>
	</select>
</td>
</tr>

<tr>
<td><?print $indent?></td>
<td class=search><?print $head_search_tags[2]?></td>
<td width=15>&nbsp;</td>
<td class=text>
<select name=orderby>
<option value="primary"   <?if($orderby=="primary" || $orderby=="")		print "selected";?>><?php print $tablematrix[0][1]?>
<option value="secondary" <?if($orderby=="secondary")				print "selected";?>><?php print $tablematrix[0][2]?>
<option value="third"	  <?if($orderby=="third")				print "selected";?>><?php print $tablematrix[0][3]?>
</select>
<select name=sortorder>
<option value="ascending"  <?if($sortorder=="ascending" || $sortorder=="")	print "selected";?>><?print $head_search_order[0]?>
<option value="descending" <?if($sortorder=="descending")			print "selected";?>><?print $head_search_order[1]?>
</select>
</td>
</tr>



<tr>
<td><?print $indent?></td>
<td class=search><?print $head_search_tags[3]?></td>
<td width=15>&nbsp;</td>
<td class=text>
<?	if( $count > 0 ) {	?>
<?print $head_search_info[0]?>&nbsp;
<span class=blue><b><?print $count;?></b></span>&nbsp;
<?print $head_search_info[1]?>&nbsp;
<?print $head_search_info[2]?>&nbsp;
<?print $startat+1;?>&nbsp;
<?print $head_search_info[3]?>&nbsp;
<?print min($startat+$show, $count);?>
<?print $head_search_info[4]?>
<?	} else {		?>
<?print $head_search_info[5]?>
<?	}			?>
</td>
</tr>



<?php if( $count > 0 ){ ?>
<tr>
<td><?print $indent?></td>
<td class=search><?print $head_search_tags[4]?></td>
<td width=15>&nbsp;</td>
<td class=text>
<?php
if( $startat > 0 )
{
	print "<a href=\"search.php?startat=".($startat-$show)."&show=$show&query=".urlencode($query)."&querytype=$querytype&orderby=$orderby&sortorder=$sortorder\">$head_search_links[0]</a>";
	print "<img src=pics/t.gif width=40 height=5>";
}
else
	print "";
?>

<?php
if($startat > 0)
{
	print "<a href=\"search.php?show=$show&query=".urlencode($query)."&querytype=$querytype&orderby=$orderby&sortorder=$sortorder\">$head_search_links[1]</a>";
	print "<img src=pics/t.gif width=40 height=5>";
}
else
	print "";
?>

<?php
if( $startat + $show < $count )
{
	print "<a href=\"search.php?startat=".($startat+$show)."&show=$show&query=".urlencode($query)."&querytype=$querytype&orderby=$orderby&sortorder=$sortorder\">$head_search_links[2]</a>";
}
else
	print "";
?>
</td>
</tr>
<? } ?>

</table>
</form>

</TD></TR>
</TABLE>


<!-- ------------------------------------------------------------------ -->
<a name=pData></a>
<?php

	include "./inc/search_output.inc";	//generate search output
?>
<p>


<?php
$info_text   = $info_text_l[$global_page];
	include "inc/table_info.inc";			//generate info box
?>
<p>


<!- ---FOOTER--------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>
