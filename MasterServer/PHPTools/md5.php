<?
	Include("login.php");
?>
<h2>UT2003 Package MD5 Page</h2>
<p>
<?
	if( $submit != "" )
	{
		$rev = mysql_query("select max(revision) from packagemd5");
		$row = mysql_fetch_row($rev);
		$maxrev = $row[0] + 1;

		if( $description == "" )
		{
			print "Error: you must enter a description";
			return;
		}
				
		$md5array = split("\n", $md5s);	
		for ($i=0; $i<count($md5array); $i++)
		{
			list( $guid, $md5, $file ) = split(" ", $md5array[$i] );
			$file = str_replace( "\r", "", $file);		
			if( strlen($guid) != 32 )
			{
				$i++;
				print "Error reading GUID on line $i";
				return;
			}
			if( strlen($md5) != 32 )
			{
				$i++;			
				print "Error reading MD5 line $i";
				return;
			}
			if( $file == "" )
			{
				$i++;			
				print "Error reading filename line $i";
				return;
			}
		}
		
		for ($i=0; $i<count($md5array); $i++)
		{
			list( $guid, $md5, $file ) = split(" ", $md5array[$i] );
			$file = str_replace( "\r", "", $file);
			$addmd5 = mysql_query("insert into packagemd5( guid, md5, revision, description) values ('$guid', '$md5', '$maxrev', '$description $file')");
		}

		print "New MD5s successfully added.<p>";
	}

	$md5s = mysql_query("select guid, md5, revision, description from packagemd5 order by guid, id");
?>
<a href="newmd5.php">Add new MD5s</a>
<table border=1>
<tr>
	<th>GUID</th>
	<th>Revision</th>
	<th>Description</th>
	<th>MD5</th>
</tr>
<?
	for( $j=0;$j<mysql_num_rows($md5s);$j++ )
	{
		$row = mysql_fetch_row($md5s);		
?>
<tr>
	<td>
	<?
		if( $row[0] != $lastguid )
			print $row[0];
		$lastguid = $row[0];
	?>
	</td>
	<td><?print $row[2];?></td>
	<td><?print $row[3];?></td>
	<td><?print $row[1];?></td>
</tr>
<?
	}
?>
</table>