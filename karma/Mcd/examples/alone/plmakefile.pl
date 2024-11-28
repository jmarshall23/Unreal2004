$TOPDIR='../../..';

use lib '../../../tools/plmake';

require 'plmake.pl';

%aloneDefs =
 (
   '+includes' => '$TOPDIR/examples/util $TOPDIR/render/include $TOPDIR/mcd/include $TOPDIR/mdt/include $TOPDIR/mdtkea/include $TOPDIR/platform_globals/include',
 );
@aloneRules = (\%aloneDefs,@glutExtras);

DependentPlmake('../../../mdtkea/build', 'plmakefile.pl');
DependentPlmake('../../../mdt/build', 'plmakefile.pl');
DependentPlmake('../../../mcd/build', 'plmakefile.pl');
DependentPlmake('../../../render/build', 'plmakefile.pl');

Executable('alone',
	   'cdAlone.c',
	   'render McdDtBridge Mcd Mdt MdtKea',   #lib's
	   '',                   #dll's
	   @aloneRules
	  );
