<?
	// authentication
	$loginsuccess = 0;
	if( isset($PHP_AUTH_USER) )
	{
        $link = mysql_connect("10.0.0.2", $PHP_AUTH_USER, $PHP_AUTH_PW);
        if( $link )
        {
	        if( mysql_select_db("ut2003") )
			{
				$loginsuccess = 1;
			}
		}
	}
	
	if( $loginsuccess != 1 )
	{
		header("WWW-Authenticate: Basic realm=\"access\"");
		header("HTTP/1.0 401 Unauthorized");
		echo "You must login to access this resource.\n";
		exit;
	}
	// end authentication
?>