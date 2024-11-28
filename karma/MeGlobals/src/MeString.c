/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.7.6.1 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#include <MeString.h>

#ifdef NEED_SNPRINTF
/* Unfortunately 'vsnprintf' and 'snprintf' are not ANSI C, and
   some platforms do not implement these (BSD) traditional and very
   useful functions. We could then use one of the many free
   implementations, e.g. http://www.ijs.si/software/snprintf/,
   but we cheat here and we just use the ANSI C functions.
   Given that these are prone to overflow, we ensure here that
   the default target string size is large.*/

int vsnprintf(char *const string,const size_t n,
                     const char *const format, va_list ap)
{
  (void) n;

  return vsprintf(string,format,ap);
}

int snprintf(char *const string,const size_t n,
                    const char *const format,...)
{
  va_list ap;
  int i;

  (void) n;

  va_start(ap,format);
  i = vsprintf(string,format,ap);
  va_end(ap);

  return i;
}
#endif /* NEED_SNPRINTF */
