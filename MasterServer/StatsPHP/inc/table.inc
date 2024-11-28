<?php

include "inc/template.inc";		//Paths are called from main folder level!

//include "inc/table_2cols.inc"; 	//uses a 2 column matrix, can display data in multiple column pairs
//include "inc/table_counter.inc"; 	//col 1 is a counter, that can be color coded (intensity, or spektrum)
					//2nd text string ("head"), following columns are data.
//include "inc/table_histogram.inc"; 	//...


function table($tc,$tm)
{
  // $tc 				tablecontrol, keyed string array, with control fieldes
  //	'mode'			t modes matrix of data used,
  //					p modes use 2 colums data only, but present them in n column pairs
  //					c modes: "c" standard head bg for counter, ci (intensity), cs (spectrum)
  //					h modes: "h" standard histogram table uses 2 column data
  //	'option'		show "head" or don't "nohead"
  //	'tabletitle'	table title
  // 	'showzero'		= 1 then 0 -> 0, else 0 -> &nbsp;
  //	'headclass'		class used for column heads (header bar)
  //	'headalign'		alignment of header row ("ALIGN=center", "" is "ALIGN=LEFT")
  //	'columnalign'	alignment of the other (data) rows
  //	'colheadclass'	class used for column heads (data)
  //	'colclass'		standard 'transparent' column data class
  //	'colaltclass'	'alternate color' column data class
  // $tm				tablematrix, 2D string array with all the data, row 0 = e.g. headers
  //					$tablematrix[row][column]
  //
  // inc/table.tpl	template used

  static $included_tablematrix;

  $mode=$tc['mode'];

  switch ($mode[0])
  {
	case "t":
	case "t3":								//Special 3 timeFrame header...
		if( $included_tablematrix != 1)
		{
			include "inc/table_matrix.inc";	//uses a straigh matrix for output
			$included_tablematrix = 1;		//include only once
		}
		tablematrix($tc,$tm);
		break;
/*###
	case "p":
		table2cols($tc,$tm);
		break;
	case "c":
		tablecounter($tc,$tm);
		break;
	case "h":
		tablehistogram($tc,$tm);
		break;
*/
  }
}
?>