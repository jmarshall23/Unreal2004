<?php
//---Connect to database stats server---
/*
	External vars used:

	$db_name		global var, name of the mysql database "ut2003stats" normally.
*/

//mysql_connect("localhost", "root", "")

//mysql_connect("localhost", "httpd", "")
//	or die ("Could not connect so db server!");

//mysql_connect("10.0.0.2", "root", "")
//	or die ("Could not connect so db server!");

//mysql_connect("localhost", "root", "")
//	or die ("Could not connect so db server!");


while( mysql_connect("10.0.0.3", "root", "") == FALSE )
	sleep( 1 );		// seconds

mysql_select_db($db_name)
	or die ("Could not select database: $db_name !");
?>