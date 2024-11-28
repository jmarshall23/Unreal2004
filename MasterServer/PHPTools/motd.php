<?
	Include("login.php");
?>
<h2>UT2003 News Page</h2>
<p>
<?
	$versions = mysql_query("select id, version, language, demo, message from motd order by version, id");
?>
<table border=1>
<tr>
	<th>Version</th>
	<th>Language</th>
	<th>Demo/Full</th>
	<th>News Text</th>
	<th></th>
</tr>
<?
	for( $j=0;$j<mysql_num_rows($versions);$j++ )
	{
		$row = mysql_fetch_row($versions);		
?>
<tr>
	<td><?print $row[1];?> +</td>
	<td><?print $row[2];?></td>
	<td>
<?
	if( $row[3] == "" )
		print "full";
	else
		print "demo";
?>
	</td>
	<td><?print substr($row[4], 0, 70);?>....</td>
	<td><a href="editmotd.php?id=<?print $row[0];?>">[edit]</a></td>
</tr>
<?
	}
?>
<tr>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td><a href="editmotd.php?id=-1">[new]</a></td>
</tr>
</table>