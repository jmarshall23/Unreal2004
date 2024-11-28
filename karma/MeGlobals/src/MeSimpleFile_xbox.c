/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 16:24:38 $ - Revision: $Revision: 1.2.2.4 $

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
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <io.h>

#include <MeMessage.h>

#include <MeSimpleFile.h>

/* simple file handling */

int MEAPI MeOpenRaw(const char *filename, MeOpenMode_enum mode)
{
    int flag;
    int m;
    int file;

    switch (mode)
    {
    case kMeOpenModeRDONLY:
        flag = _O_RDONLY;
        break;
    case kMeOpenModeWRONLY:
        flag = (_O_WRONLY|_O_CREAT|_O_TRUNC);
        /* User should still be able to read the file afterwards! */
        m = (_S_IREAD|_S_IWRITE);
        break;
    case kMeOpenModeRDWR:
        flag = _O_RDWR;
        break;
    case kMeOpenModeRDBINARY:
        flag = (_O_BINARY|_O_RDONLY);
        break;

    default:
        MeFatalError(3, "Invalid mode parameter %d passed to MeOpen()",mode);
    }

    file = _open(filename, flag, m);

    return file;
}

int MEAPI MeLseek(int file, int offset, MeSeekOrigin_enum origin)
{
    int o;
    switch (origin)
    {
    case kMeSeekSET: o = SEEK_SET; break;
    case kMeSeekCUR: o = SEEK_CUR; break;
    case kMeSeekEND: o = SEEK_END; break;

    default:
        MeFatalError(3, "Invalid origin parameter %d passed to MeLseek()",
            origin);
    }

    return _lseek(file,offset,o);
}

int MEAPI MeRead(int file, void * buf, int count)
{
    return _read(file,buf,count);
}

int MEAPI MeWrite(int file, void * buf, int count)
{
    return _write(file,buf,count);
}

int MEAPI MeClose(int file)
{
    return _close(file);
}
