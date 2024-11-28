#

# Run with perl -pi.bak filename

my $copyright =<<END_NOTICE;
  Notice:  This  software   and  its  accompanying  manuals   have  been
  developed  by Mathengine PLC ("MathEngine")  and the copyright and all
  other intellectual  property rights in them belong to  MathEngine. All
  rights  conferred   by  law  (including   rights  under  international
  copyright conventions)  are reserved to MathEngine.  This software may
  also  incorporate information  which  is confidential  to  MathEngine.
 
  Save  to  the  extent  permitted  by  law or  as  otherwise  expressly
  permitted by  MathEngine, this software and  the manuals must  not be
  copied (in  whole or in part), re-arranged,  altered or adapted in any
  way without the prior written consent of the Company. In addition, the
  information contained in the software  may not be disseminated without
  the prior written consent of the Company.

  (c) 2001 Mathengine PLC
END_NOTICE

if (!m/#define/)
{
	s/^.*copyright.*mathengine.*$/$copyright/i;
}
