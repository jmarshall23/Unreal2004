<?php
//---Generate a HTML table from a string matrix---
/*
	External vars used:

###	$db_name		global var, name of the mysql database "ut2003stats" normally.

	See inc/table.inc for details.
*/


function tablematrix($tc,$tm)
{
  $DEBUG = 0;

  $t = new Template("inc/");  								// "./"template folder
  $t->set_file( "table", "table.tpl" );						// load table template
  $t->set_block("table", "TableCols", "TCols");				// order important! inside loop first!
  $t->set_block("table", "TableRows", "TRows");				// no longer used...

  // Setting vars
  $t->set_var('tabletitle', 	$tc['tabletitle']);
  $t->set_var('tabletext', 		$tc['tabletext']);
  $t->set_var('indent', 		$tc['indent']);
  $t->set_var('ind_tb', 		$tc['ind_tb']);

  // Size of Matrix
  $datarows = sizeof($tm);
  $datacols = sizeof($tm[0]);

  // Colspan tranking
  $mode = $tc['mode'];							// t normal, t3 with special header

  //---Table MODES---
  if( $mode=="t3" )
  {
	$t->set_var('background', "pics/mappingt3.gif" );		//** This is hardcoded!
	$colspan 		= $tc['tablespan'];
	$table_blocks	= $tc['tableblocks'];

	$startrow = -1;

	// Building special header locally! For Week, Month, Alltime... als element [-1]
	$i=0;
	for( $j=0; $j<$datacols; $j++ )
	{
		if( $colspan[$j]>0 AND $j>0 )			// Normal TD, do not output anything! (0_global_vars)
		{
			$tm[-1][$j] = $table_blocks[$i];
			$i++;
		}
        else
			$tm[-1][$j] = "";					// Designated spanned column
    }
  }
  else	// "t" default mode has COLSPAN set to 1 for all entries
  {
    $t->set_var('background', 	"pics/mapping.gif" );		//** This is hardcoded! Default
	$startrow = 0;
	for( $j=0; $j<$datacols; $j++ )
		$colspan[$j]=1;
  }

  if( $DEBUG ) print_r($colspan);

  if($tc['option']=='head' OR $tc['option']=='')		// not set option means show head line
	$option='head';
  else
	$option=$tc['option'];

  // Do table  (standard tables)
  for( $i=$startrow; $i<$datarows; $i++ )	//ROWS
  {
	$t->set_var('TCols', ''); 							// Using block fuer each line, reset it!
	for( $j=0; $j<$datacols; $j++ )			//COLUMNS
	{
		$colspaninfo="";								// all other rows have no colspan!
        if( $i==-1 )
        {
			if( $colspan[$j]==0)	continue;			// 3TF Heade, skip this <td>
			if( $colspan[$j]==1)	$colspaninfo="";	// Normal TD, do not output anything!
			else					$colspaninfo="COLSPAN=".$colspan[$j]." ";
		}
		$t->set_var('columspan',$colspaninfo);


		if ( $i==-1)
		{
			$t->set_var('columnalign',$tc['headalign']);
			$t->set_var('columnclass',$tc['colheadclass']);
		}
		else
        if( $i==0 AND $option=='head' )					// do header
    	{
			$t->set_var('columnalign',$tc['headalign']);
			$t->set_var('columnclass',$tc['headclass']);
    	}
    	else											// do rest of table
		{
			$t->set_var('columnalign',$tc['columnalign']);
			if($j==0)									// col 0 with background
				$t->set_var('columnclass', $tc['colheadclass']);
            else										// alternating data row coloring
        	{
				if( $i%2==1 )	$t->set_var('columnclass', $tc['colclass']);
				else			$t->set_var('columnclass', $tc['colaltclass']);
			}
		}

		// special cases, no data, or value = zero
		$data=$tm[$i][$j];
		if( sizeof($data)==0 )	$data="&nbsp;";			// no data
		if( $data=="0" AND $tc['showzero']!=1 )			// data is 0 and show zeros?
			$data="&nbsp;";

		$t->set_var('columndata', $data);
		$t->parse('TCols', 'TableCols', true);
	}	//end COLUMNS
	$t->parse('TRows', 'TableRows', true);				// no longer used...
  }		//end ROWS

  $t->pparse('output', 'table');
}




