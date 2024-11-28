/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 16:24:38 $ - Revision: $Revision: 1.9.6.5 $

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

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <eeregs.h>
#include <sifdev.h>

#include <MeMemory.h>
#include <MeMessage.h>

#include <MeSimpleFile.h>

/* Simple file handling */

int MEAPI MeOpenRaw(const char *filename, MeOpenMode_enum mode)
{
    int handle;
    int flag;

    switch (mode)
    {
    case kMeOpenModeRDONLY:
    case kMeOpenModeRDBINARY:
        flag = SCE_RDONLY;
        break;

    case kMeOpenModeWRONLY:
        flag = SCE_WRONLY|SCE_CREAT|SCE_TRUNC;
        break;

    case kMeOpenModeRDWR:
        flag = SCE_RDWR;
        break;

    default:
        MeFatalError(3,"Invalid mode parameter %d passed to MeOpen()",mode);
    }

    /*
       sceOpen: Weird behaviour, doesn't match documents. Valid file handles
       are integers >= 0. -2 means it couldn't find the file, -19 means you
       forgot to put 'host:' in front of the name, .....
    */

    handle = sceOpen(filename, flag);

    return handle;
}

int MEAPI MeLseek(int file, int offset, MeSeekOrigin_enum origin)
{
    int o;

    switch (origin)
    {
    case kMeSeekSET: o = SCE_SEEK_SET; break;
    case kMeSeekCUR: o = SCE_SEEK_CUR; break;
    case kMeSeekEND: o = SCE_SEEK_END; break;
    default:
        MeFatalError(3,"Invalid origin parameter %d passed to MeLseek()",
            origin);
    }

    return sceLseek(file, offset, o);
}

int MEAPI MeRead(int file, void * buf, int count)
{
    return sceRead(file, buf, count);
}

int MEAPI MeWrite(int file, void * buf, int count)
{
    return sceWrite(file, buf, count);
}

int MEAPI MeClose(int file)
{
    return sceClose(file);
}
