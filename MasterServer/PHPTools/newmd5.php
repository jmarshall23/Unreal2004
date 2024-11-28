<?
	Include("login.php");
?>
<h2>UT2003 New MD5 List</h2>
<p>
Cut/paste the following command into a Command Prompt:
<hr>
ucc mastermd5 -w *.u
<hr>
Then cut/paste the output (between the -----'s) into the following textbox, fill in the patch description and press Submit.
<form method=post action="md5.php">
Description: <input type=text name=description size=50>
<textarea rows="20" wrap=none cols="100" name=md5s>
</textarea>
<input type=submit name="submit" value="Submit">
</form>

