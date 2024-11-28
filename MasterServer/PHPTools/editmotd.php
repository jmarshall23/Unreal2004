<?
	Include("login.php");
?>
<h2>Edit UT2003 News Page</h2>
<p>
<?
	function myAddSlashes($st)
	{ 
		if (get_magic_quotes_gpc()==1)
		{ 
			return $st; 
		}
		else
		{ 
			return AddSlashes($st); 
		} 
	} 

	if( $submit != "" )
	{
		if( $demo == "demo" )
			$demostr = "'T'";
		else
			$demostr = "null";

		if( $id == -1 )
		{
			mysql_query("insert into motd (version, language, demo, message) values ('".myAddSlashes($version)."', '".myAddSlashes($language)."', ".$demostr.", '".myAddSlashes($message)."')");
			$id = mysql_insert_id();
		}
		else
		{
			mysql_query("update motd set version='".myAddSlashes($version)."', language='".myAddSlashes($language)."', demo=$demostr, message='".myAddSlashes($message)."' where id='".myAddSlashes($id)."'");
		}	
	}
	if( $delete != "" && $confirmdelete != "" )
	{
		if( $id != -1 )
			mysql_query("delete from motd where id='".myAddSlashes($id)."'");
?>Done.<p>
<a href="motd.php">[Back to News page]</a>
<?
		return;
	}

	if( $id != -1 )
	{
		$motd = mysql_query("select version, language, demo, message from motd where id='".myAddSlashes($id)."'");
		
		if( mysql_num_rows($motd) == 0 )
		{
			print "That ID was not found";
			return;
		}
		$row = mysql_fetch_row($motd);
		
		$version = $row[0];
		$language = $row[1];
		if( $row[2] != "" )
			$demo = "demo";
		else
			$demo = "full";
		$message = $row[3];
	}

	if( $language == "" )
		$language = "int";
?>
<form method=post>
<table>
<tr><th>Version</th><td><input type=text name=version size=4 maxlength=4 value="<?print $version;?>"></td></tr>
<tr><th>Language</th><td><input type=text name=language size=3 maxlength=3 value="<?print $language;?>"></td></tr>
<tr><th>Demo/Full</th><td><select name=demo><option value="full" <?if($demo!="demo") print "selected";?>>full</option><option value="demo" <?if($demo=="demo") print "selected";?>>demo</option></td></tr>
<tr>
<th valign=top>News</th>
<td>
<textarea name="message" rows="20" wrap=virtual cols="80">
<?print $message;?>
</textarea>
</td>
</tr>
</table>
<input type=hidden name=id value="<?print $id;?>">
<input type=submit name=submit value="Update">
<?	if( $id != -1 )
	{
?>
<input type=checkbox name=confirmdelete value="confirm">Confirm
<input type=submit name=delete value="Delete">
<?
	}
?>
</form>
<a href="motd.php">[Back to News page]</a>