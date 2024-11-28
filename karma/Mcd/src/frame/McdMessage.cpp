/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/23 15:44:24 $ - Revision: $Revision: 1.10.4.3 $

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <McdMessage.h>
#include <MeMessage.h>

#define ErrorCount 5

/** error message table */
McdErrorDescription
gMcdCoreErrorList[] =
{
#define ERR(id,severity,desc) {(id),(severity),(ErrorCount),(desc)}
#include "McdCoreErrorList.h"
#undef ERR
};


/** @internal */
static McdErrorDescription* McdGetErrorDescription (
    McdErrorDescription* ErrorList, int number)
{
  McdErrorDescription * desc = ErrorList;
  while (desc->m_errNum != kMcdMAXErrDefined )
  {
    if (desc->m_errNum == number) return desc;
    ++desc;
  }
  return desc;
}


/**
 * Reports an error from an error table which is not necessarily orderer.
 */
extern "C"
void McdError(McdErrorDescription* ErrorList, int errorCode,
    const char *message, const char *fn, const char *file, int line)
{
  McdErrorDescription * desc =
    McdGetErrorDescription(ErrorList,errorCode);

  char msg[512]="";
  const char *desc_msg;

  desc_msg = (errorCode > 0 && errorCode < kMcdMAXErrDefined)
      ? desc->m_description : "Undefined Internal Error";

  if (desc->m_errorCount<=0)
    return;

  sprintf(msg, "Mcd function %s (in file %s at line %d): %s%s",
    fn, file, line, desc_msg, message);
  if (desc->m_errorCount==1)
    strcat (msg, "[This message will not be repeated again!]\n");

#if 0
  desc->m_errorCount--;
#endif

  switch (desc->m_errorLevel)
  {
  case kMcdErrorTypeInfo:
    MeInfo(0,msg);
    break;

  case kMcdErrorTypeWarning:
    MeWarning(0, msg);
    break;

  case kMcdErrorTypeFatal:
    MeFatalError(0, msg);
    abort();
  default:

    break;
  }
}
