<?php

$global_page = 'faq';
	include "inc/global_vars.inc";				//getting global vars
	include "inc/db_login.inc";					//access to db
	include "inc/table.inc";					//data tables need this

$head_title	= $head_title_l[$global_page];		//getting the page title
$head_where	= $head_where_l[$global_page];		//setting the loctation path per page

	include "page_head.php";

$index_info = $index_info_l[$global_page];		// global var, language

$index_links = "";
for( $i=0; $i<(sizeof($index_info)/4); $i++ )
{
    $r = $i*4;
	$index_links .= "<a href=#".$index_info[$r].">".$index_info[$r+1]."</a> ".$index_info[$r+2]."<br>";
}

$intro_title = $intro_title_l[$global_page];	// global vars, language
$intro_intro = $intro_intro_l[$global_page];
$intro_text  = $intro_text_l[$global_page];
$intro_logo  = $intro_logo_l[$global_page];
$intro_button= $intro_button_l[$global_page];

	include "inc/table_index_intro.inc";		//generate index and intro boxes
?>
<!-- ---DATA TABLES--------------------------------------------------------- -->
<p>

<pre>

</pre>
<a name=fFAQ></a>
<BLOCKQUOTE CLASS=TEXT>
<span class=ptitle><?print $faq_title_l[$global_page]?></span><p>

<?php
		$faq = $faq_l[$global_page];
		print "<blockquote class=text>\n";
		for( $i=0; $i<(sizeof($faq)/2); $i++ )
		{
    		$f = $i*2;
			print ($i+1).". <a href=#p".($i+1).">$faq[$f]</a><br>\n";
        }
		print "</blockquote><p>\n";

		for( $i=0; $i<(sizeof($faq)/2); $i++ )
		{
    		$f = $i*2;
?>
<a name=p<?print $i+1?>></a>
<b><?print $faq[$f]?></b>
<blockquote class=text>
<?print $faq[$f+1]?>
</blockquote>
<?php	}	?>
</BLOCKQUOTE>
<p>



<pre>

</pre>
<a name=fLexicon></a>
<BLOCKQUOTE CLASS=TEXT>
<span class=ptitle><?print $lex_title_l[$global_page]?></span><p>
<?php
		$start = $index_info_number[$global_page];
		for( $i=$start; $i<(sizeof($index_info)/4); $i++ )
		{
    		$l = $i*4;
?>
<a name=<?print $index_info[$l]?>></a>
<b><?print $index_info[$l+1]?></b>
<blockquote class=text>
<?print $index_info[$l+3]?>
</blockquote>
<?php	}	?>
</BLOCKQUOTE>
<p>



<!--
http://mail.epicgames.com/christoph/ut2003stats/faq.php#fFAQ

<?php
$info_text   = $info_text_l[$global_page];
	include "inc/table_info.inc";				//generate info box
?>
<p>
-->



<!-- ---FOOTER-------------------------------------------------------------- -->
<?php
	include "page_foot.php";
?>