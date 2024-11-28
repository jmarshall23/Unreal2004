<?
	Include("login.php");
?>
<h2>Find CD Key</h2>
<hr>
<p>
<?
	if( $ban=="yes" && $cdkeyid != "" )
		$banquery = mysql_query("update cdkey set disabled='T' where id='$cdkeyid'");

	if( $unban=="yes" && $cdkeyid != "" )
		$unbanquery = mysql_query("update cdkey set disabled=NULL where id='$cdkeyid'");

	$where = "";
	if( $banmd5 != "" )
		$where = "md5(concat('1230asadkgjk358dmvmbjt6838320yjkdhnchjg4958', cdkey.md5hash)) = '$banmd5'";
	else
	if( $ip != "" )
		$where = "cdkey.lastseenip = '$ip'";
	else
	if( $md5hash != "" )
		$where = "cdkey.md5hash='$md5hash'";
	else
	if( $cdkeystart != "" )
		$where = "cdkey.cdkey like '$cdkeystart%'";
	else
	if( $cdkeyid != "" )
		$where = "cdkey.id='$cdkeyid'";

	if( $where != "" )
	{
		$cdkeys = mysql_query("select cdkey.id, cdkey.cdkey, cdkey.disabled, cdkey.serveronly, cdkeybatch.description, ".
		                      "cdkey.version, cdkey.platform, cdkey.lastseen, cdkey.lastseenip, md5(concat('1230asadkgjk358dmvmbjt6838320yjkdhnchjg4958', cdkey.md5hash)) ".
		                      "from cdkey, cdkeybatch where cdkeybatch.id=cdkey.batchid and $where limit 10");

		if( mysql_num_rows($cdkeys) == 0 )
		{
?>
No results found.
<?
		}
		else
		{		                      
?>
<table border=1>
<tr>
<th>ID</th>
<th>Key</th>
<th>CD Key Banned</th>
<th>Server Only</th>
<th>Batch Name</th>
<th>Version</th>
<th>Platform</th>
<th>Last Seen</th>
<th>IP</th>
<th>Server Ban String</th>
<th></th>
</tr>
<?
			for( $j=0;$j<mysql_num_rows($cdkeys);$j++ )
			{
				$row = mysql_fetch_row($cdkeys);		
?>
<tr>
<td><?print $row[0];?></td>
<td><?print $row[1];?></td>
<td><?print $row[2];?></td>
<td><?print $row[3];?></td>
<td><?print $row[4];?></td>
<td><?print $row[5];?></td>
<td><?print $row[6];?></td>
<td><?print $row[7];?></td>
<td><?print $row[8];?></td>
<td><?print $row[9];?></td>
<td>
<?
				if( $row[2]=="" )
					print "<a href=\"findcdkey.php?ban=yes&cdkeyid=$row[0]\">Ban CD Key</a>";
				else
					print "<a href=\"findcdkey.php?unban=yes&cdkeyid=$row[0]\">Unban CD Key</a>";				
?>
</td>
</tr>
<?
			}
?>
</table>
<?
		}
?>
<p>
<hr>		
<?
	}
?>
Search:
<form method=post action="findcdkey.php">
<table>
<tr>
<td>Ban MD5</td>
<td><input type=text name=banmd5 size=32></td>
</tr>

<tr>
<td>IP Address</td>
<td><input type=text name=ip size=15></td>
</tr>

<tr>
<td>CD Key MD5</td>
<td><input type=text name=md5hash size=32></td>
</tr>

<tr>
<td>CD Key starts with</td>
<td><input type=text name=cdkeystart size=23></td>
</tr>

</table>

<input type=submit value="Submit">
</form>