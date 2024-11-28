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

#include "AssetViewer2.h"
#include "asestorage.h"


AseHolderPtr aseList = 0;

AseHolderPtr CreateAseHolder(const char* filename, MeStream str)
{
    AseHolderPtr ah = (AseHolderPtr)MeMemoryAPI.create(sizeof(AseHolder));

    ah->m_filename = (char*)MeMemoryAPI.create(strlen(filename)+1);
    strcpy(ah->m_filename, filename);

    ah->m_stream = str;
    ah->m_next = 0;
    return ah;
}

void DestroyAseHolder(AseHolderPtr aseHolder)
{
    if(aseHolder)
    {
        if(aseHolder->m_filename)
            MeMemoryAPI.destroy(aseHolder->m_filename);
        if(aseHolder->m_stream)
            MeStreamClose(aseHolder->m_stream);

        MeMemoryAPI.destroy(aseHolder);
    }
}

void AddAseHolderToList(AseHolderPtr aseHolder)
{
    AseHolderPtr lst = aseList;
    if(!lst)
        aseList = aseHolder;
    else
    {
        while(lst->m_next)
            lst = lst->m_next;
        
        lst->m_next = aseHolder;
    }    
}

void RemoveAseHolderFromList(AseHolderPtr aseHolder)
{
    AseHolderPtr next;
    AseHolderPtr cur;
    
    cur = aseList;
    
    if(!cur)
        return;
    next = cur->m_next;
    
    if(cur == aseHolder)
    {
        aseList = cur->m_next;
        return;
    }
    else
    {
        while(next)
        {
            if(next == aseHolder)
            {
                cur->m_next = next->m_next;
                return;
            }
            
            cur = next;
            next = cur->m_next;
        }
    }
    return;    
}

MeBool IsAseInList(const char* filename)
{
    return GetAseHolder(filename)?1:0;
}

AseHolderPtr GetAseHolder(const char* filename)
{
    AseHolderPtr cur = aseList;
    while(cur)
    {
        if(!stricmp(filename,cur->m_filename))
            return cur;
        
        cur = cur->m_next;
    }
    return 0;    
}

void MEAPI FlushAseList()
{
    AseHolderPtr cur = aseList;
    AseHolderPtr next;

    while(cur)
    {
        next = cur->m_next;
        DestroyAseHolder(cur);
        cur = next;
    }
    aseList = 0;

    ResetApp();
}

void MEAPI NewAse(const char* filename, MeStream str, MeBool bOverwrite)
{
    AseHolderPtr exist;

    exist = GetAseHolder(filename);
    if(exist)
    {
        if(bOverwrite)
        {
            RemoveAseHolderFromList(exist);
            DestroyAseHolder(exist);
        }
        else
            return;
    }

    exist = CreateAseHolder(filename, str);
    AddAseHolderToList(exist);

    ResetApp();
}
