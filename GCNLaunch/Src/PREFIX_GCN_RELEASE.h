//	===========================================================================
//	PREFIX_PS2_RELEASE.h			©2000 Metrowerks Inc. All rights reserved.
//	===========================================================================
//
//	02/01/2000	kashima,	

#include <HW2_Release_Prefix.h>

#define	NDEBUG					/*	just for debugging	*/

#pragma	cats	off
#define __GCN__				1
#define DO_GUARD 			0
#define DO_CHECK			0
#define STATIC_LINK			1
#define __STATIC_LINK		1
typedef unsigned short 		u_short;
typedef unsigned char		u_char;

#define __const const

