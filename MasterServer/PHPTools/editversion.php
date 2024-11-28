<?
	Include("login.php");
?>
<h2>Edit Version</h2>
<p>
<?
	if( $version != -1 )
	{
		$versions = mysql_query("select version, minnetver, maxnetver from versions order by version");
		$row = mysql_fetch_row($versions);		
	}
	else
	{
		$row[0] = "";
		$row[1] = "";
		$row[2] = "";
	}
?>
<form method=post action=editversion.php>
<input type=hidden name=version value="<?print $version?>">
<table border=1>
<tr>
	<th>Client Version</th>
	<th>Minimum Server Version</th>
	<th>Maximum Server Version</th>
	<th></th>
</tr>
<tr>
	<td><input type=text name=cliver value="<?print $row[0];?>"> +</td>
	<td><input type=text name=minver value="<?print $row[1];?>"></td>
	<td><input type=text name=maxver value="<?print $row[2];?>"></td>
</tr>
</table>
<input type=submit name=Submit value="Submit">
</form>