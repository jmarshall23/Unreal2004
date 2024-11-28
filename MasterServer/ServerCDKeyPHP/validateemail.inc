<?
Function validemail($emailaddress)
{ 
	if (eregi("^[0-9a-z]([-_.]?[0-9a-z])*@[0-9a-z]([-.]?[0-9a-z])*\\.[a-z]{2,3}$", $emailaddress, $check))
	{
		return 1;
	}
	else
	{
		return 0; 
	}
}
?>
