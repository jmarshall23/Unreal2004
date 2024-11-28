<HTML>
<HEAD>
<TITLE>Unreal Tournament 2003 Server CD Key Generator</TITLE>
</HEAD>
<table cellpadding=4 cellspacing=1 bgcolor="#4A91B9" width=100%>
<tr bgcolor="#4A91B9">
<td><font size=+1 face="Verdana,Geneva,Arial,Helvetica,sans-serif" color="#ffffff">unreal tournament 2003 server cd key generator</font></td>
</tr>
</tr>
<tr bgcolor="#ffffff">
<td>&nbsp;</td>
</tr>
</table>
<P>
<font size=-1 face="Verdana,Geneva,Arial,Helvetica,sans-serif" color="#000000">
<?
	if( $email == "" || $HTTP_SERVER_VARS['REQUEST_METHOD'] != "POST" )
	{
?>
Thank you for your interest in running an Unreal Tournament 2003 dedicated server.  In order for your server to appear in the 
global server list and participate in UT2003 stats, your server needs to be allocated a "CD Key".
You may run as many servers as you like using the same key.
<P>
Please supply an email address and your server CD key will be mailed to you.  This email address will be recorded, however
Epic Games will not use this for any purpose other than to contact you in the event of possible abuse, cheating etc 
regarding your server.  Your email address will not be made available to any other party and you will not be subscribed to 
any mailing lists.
<P>
The email address you use must be capable of receiving MIME-encoded attachments.  Some antivirus scanners may reject the email
because it will contain a Windows .REG file attachment.

<form method=post action=cdkey.php>
email address:  <input type=text size=50 maxlength=50 name=email>  <input type=submit value="Submit">
</form>
Please press Submit only once.
<?
	}	
	else
	{
		Include("validateemail.inc");
		$email = strtolower($email);
	
		if( !validemail($email) )
		{
?>
Sorry, but the email address "<?print $email?>" does not appear to be a valid, working email address.  Press your browser's Back button and try again.
<?
		}
		else
		if( substr( $email, -12 ) == "@hotmail.com" || 
			substr( $email, -10 ) == "@yahoo.com" )
		{
?>
Sorry, we do not accept hotmail.com or yahoo.com addresses.  Press your browser's Back and use another address.
<?
		}
		else
		{
			mysql_connect("10.0.0.2", "servercd", "n8mfd83")
				or die ("Could not connect to database server.");
			mysql_select_db("ut2003")
				or die ("Could not connect to database.");

			$ip = $HTTP_SERVER_VARS['REMOTE_ADDR'];

			// check from email address
			$emailcount = mysql_query( "select cdkeyid from servercdkey where email='".AddSlashes($email)."' and timestamp > DATE_SUB( NOW(), interval 1 day)" );
			if( mysql_num_rows($emailcount) >= 5 )
			{
?>
Sorry, you can only request five cd keys per day per email address.
<?		
			}
			else
			{
				// check from ip
				$ipcount = mysql_query( "select cdkeyid from servercdkey where ip='".AddSlashes($ip)."' and timestamp > DATE_SUB( NOW(), interval 5 minute)" );
				if( mysql_num_rows($ipcount) >= 1 )
				{
?>
Sorry, your IP address has requested a cd key in the last five minutes.  Please wait a while and try again.
<?				
				}
				else
				{
					Include("mimemail.inc");
					
					$cdchars = "ABCDEFGHJLKMNPQRTUVWXYZ2346789";
					$randmax = strlen($cdchars)-1;
					
					while( 1 )
					{
						$cdkey = "SRVER-".
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ]."-".
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ]."-".
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ].
							$cdchars[ rand(0, $randmax) ];

						$checkdupe = mysql_query("select id from cdkey where md5hash=md5('$cdkey')");
						if( mysql_num_rows($checkdupe) == 0 )
							break;
					}

					mysql_query( "insert into cdkey (cdkey, md5hash,  batchid, serveronly) values ('$cdkey', md5('$cdkey'), 2, 'T')" );
					$cdkeyid = mysql_insert_id();
					if( $cdkeyid == 0 )
					{
?>
Sorry, something went wrong!  Please press back and try again.
<?
					}
					else
					{
						mysql_query( "insert into servercdkey (cdkeyid, timestamp, email, ip) values ($cdkeyid, now(), '".AddSlashes($email)."', '".AddSlashes($ip)."')" );		

						$regfile =		"Windows Registry Editor Version 5.00\r\n\r\n".
										"[HKEY_LOCAL_MACHINE\SOFTWARE\Unreal Technology\Installed Apps\UT2003]\r\n".
										"\"CDKey\"=\"$cdkey\"\r\n";

						$body =			"Thank you for requesting a UT2003 server CD key.  Your server CD key is\n\n".
										"$cdkey\n\n".
										"For Windows servers, save the attached UT2003_Server_CD_Key.reg and upload it to your ".
										"dedicated server.  Once it's there, double-click it to install it in the Windows registry. ".
										"You can also install it by hand by making a new String registry entry called CDKey in\n\n".
										"HKEY_LOCAL_MACHINE\SOFTWARE\Unreal Technology\Installed Apps\UT2003\n\n".					
										"Warning: do not double-click this .reg file on a Windows computer which already has a retail ".
										"UT2003 installation - it will overwrite your retail CD key with the server-only one.\n\n".
										"If this occurs, you will need to reinstall your copy of UT2003 from your original CDs.\n\n".
										"For Linux and MacOS X servers, save the attached \"cdkey\" file and place it in your ".
										"System directory.  ".
										"This file MUST be named \"cdkey\" - some email programs may suggest you save the file as ".
										"cdkey.dat or some other name.\n\n".
										"- Epic Games";
										
						$mail = new mime_mail();
						$mail->from = "servercdkeys@epicgames.com";
						$mail->to = "$email";
						$mail->subject = "Your UT2003 Server CD Key";
						$mail->body = $body;
						$mail->add_attachment("$regfile", "UT2003_Server_CD_Key.reg", "application/octet-stream");
						$mail->add_attachment("$cdkey\n", "cdkey", "application/octet-stream");
						$mail->send();
?>
Thank you.  Your server CD key has been sent to your mail box at <?print $email;?>.
<P>
- Epic Games
<?
					}
				}
			}
		}
	}
?>
</font>

</HTML>