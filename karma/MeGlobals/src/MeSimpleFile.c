/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 14:18:22 $ - Revision: $Revision: 1.7.6.4 $

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

#include <MeMemory.h>
#include <MeMessage.h>

#include <MeSimpleFile.h>

char *MEAPI MeLoadWholeFileHandle(const int handle, int *size)
{
    const int fileSize = MeLseek(handle,0,kMeSeekEND);
    void *destination = (*MeMemoryAPI.create)(fileSize);
    int read;

    MeInfo(39,"Reading: file size %d bytes",fileSize);

    MeLseek(handle,0,kMeSeekSET);
    read = MeRead(handle,destination,fileSize);
    if (read != fileSize)
        MeWarning(0,"MeLoadWholeFile bad length: size: %d, read: %d", 
            fileSize,read);
    MeClose(handle);

    *size = read;

    return destination;
}

void MEAPI MeSaveWholeFileHandle(const int handle,
    const char *data,const int size)
{
    MeInfo(39,"MeSaveWholeFile: file size %d bytes", size);
    MeWrite(handle,(void *) data,size);
}
