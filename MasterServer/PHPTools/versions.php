<?
	Include("login.php");
?>
<h2>UT2003 Versions and News Page</h2>
<p>
<?
	$versions = mysql_query("select version, minnetver, maxnetver from versions order by version");
?>
<table border=1>
<tr>
	<th>Client Version</th>
	<th>Minimum Server Version</th>
	<th>Maximum Server Version</th>
	<th></th>
</tr>
<?
	for( $j=0;$j<mysql_num_rows($versions);$j++ )
	{
		$row = mysql_fetch_row($versions);		
?>
<tr>
	<td><?print $row[0];?> +</td>
	<td><?print $row[1];?></td>
	<td><?print $row[2];?></td>
	<td><a href="editversion.php?version=<?print $row[0];?>">[edit]</a></td>
</tr>
<?
	}
?>
<tr>
	<td></td>
	<td></td>
	<td></td>
	<td><a href="editversion.php?version=-1">[new]</a></td>
</tr>
</table>