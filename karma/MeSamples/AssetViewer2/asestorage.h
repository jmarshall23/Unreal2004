/* -*- mode: C; -*- */

/*
Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $
  
    Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.1.2.1 $
    
      This software and its accompanying manuals have been developed
      by Mathengine PLC ("MathEngine") and the copyright and all other
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

#ifndef AV_ASESTORAGE_H
#define AV_ASESTORAGE_H

#include "AssetViewer2.h"

/* storage for ases */
typedef struct _aseHolder *AseHolderPtr;

typedef struct _aseHolder
{
    char        *m_filename;
    MeStream     m_stream;
    
    AseHolderPtr m_next;
} AseHolder;

extern AseHolderPtr aseList;

AseHolderPtr CreateAseHolder(const char* filename, MeStream str);
void         DestroyAseHolder(AseHolderPtr aseHolder);
void         AddAseHolderToList(AseHolderPtr aseHolder);
MeBool       IsAseInList(const char* filename);
AseHolderPtr GetAseHolder(const char* filename);
void         RemoveAseHolderFromList(AseHolderPtr aseHolder);

void   MEAPI FlushAseList();

void   MEAPI NewAse(const char* filename, MeStream str, MeBool bOverwrite);

#endif