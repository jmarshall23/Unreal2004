/*=============================================================================
	UnAuthenticate.h: removeable content authentication and encryption core code.    
	Copyright 2004 Epic Games, Inc. All Rights Reserved.

	Usage note: removal of this file ( from the source file pool and from Engine.h ) should not break the build;  
	use proper  #IFDEF fallbacks whenever using any of the constants defined below.

	Revision history:
		* Created by Erik de Neve
=============================================================================*/

/*-----------------------------------------------------------------------------
	Defines
-----------------------------------------------------------------------------*/

//
// Epic-only section.  Whenever issuing a code drop either remove this file, or remove this section and outcomment the "Safe replacements" section below.
//

#define UPTSCRAMBLEFACTOR		(1117695901)
#define UPTSCRAMBLEOFFSET		(1588635695)
#define PLEOBFKEYFACTOR			(46523)

// Combine obfuscated and non-obfuscated part, using base P where  2^15 < P < 2^16 = 46523	
#define OBFKEYREQUIRED (1)
#define OBFCREATEKEY(Mask,Key)    (( Mask + ( Key ^ Mask ) * PLEOBFKEYFACTOR ))
#define OBFUNWRAPKEY(Key)  (( Key / PLEOBFKEYFACTOR ) ^ ( Key % PLEOBFKEYFACTOR ))

//
// End of Epic-only section.
//


/*
//
// Safe replacements section.
//

 #define UPTSCRAMBLEFACTOR	(0)
 #define UPTSCRAMBLEOFFSET		(0)
 
 #ifdef OBFKEYREQUIRED
 #undef OBFKEYREQUIRED
 #endif

 #define OBFCREATEKEY(Mask,Key)	(0)
 #define OBFUNWRAPKEY(Key)			(0)

//
// End of safe replacements section.
//
*/


/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

