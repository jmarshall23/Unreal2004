/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 16:24:38 $ - Revision: $Revision: 1.1.2.4 $

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

#include <stdlib.h>

#include <dolphin/mcc.h>
#include <dolphin/fio.h>
#include <dolphin/tty.h>

#include <MeMessage.h>
#include <MeSimpleFile.h>

/* Simple file handling */

#define MeNgcInitFIO            1
#define MeNgcInitTTY            1

#define MeOpenFilesMAX          42

static unsigned MeOpenNGCInit   = 1;
static FIOHandle                MeOpenFiles[MeOpenFilesMAX];

static unsigned MeNgcMCCInitialized = 0;

static void MeNgcMCCCallback(MCCSysEvent se)
{
	if (se == MCC_SYSEVENT_INITIALIZED)
        MeNgcMCCInitialized = 1;
}

static unsigned MeNgcMCCInit(void)
{
    MeInfo(1,"%s","Initializing MCC and FIO/TTY");

	if (!MCCInit(MCC_EXI_1,0,MeNgcMCCCallback))
    {
        MeFatalError(1,"'MCCInit()' on MCC_EXI_1 failed");
        return 0;
	}

    MeInfo(1,"Waiting for MCC to be initialized");
    while (!MeNgcMCCInitialized)
        continue;
    MeInfo(1,"MCC is initialized");

#if (MeNgcInitFIO)
    if (!FIOInit(MCC_EXI_1,MCC_CHANNEL_1,10))
    {
        MeFatalError(1,"'TTYInit()' failed");
        MCCExit();
        return 0;
	}
    MeInfo(1,"'FIOInit()' succeeded");
#endif

#if (MeNgcInitTTY)
    if (!TTYInit(MCC_EXI_1,MCC_CHANNEL_8))
    {
        MeFatalError(1,"'TTYInit()' failed");
        MCCExit();
        return 0;
	}
    MeInfo(1,"'TTYInit()' succeeded");
#endif

#if (MeNgcInitTTY)
    MeInfo(0,"Waiting for FIO server to start");
    while (!TTYQuery())
        continue;
    MeInfo(0,"TTY server started");
	TTYPrintf("TTY server started");
#endif

#if (MeNgcInitFIO)
    MeInfo(0,"Waiting for FIO server to start");
    while (!FIOQuery())
        continue;
    MeInfo(0,"FIO server started");
#if (MeNgcInitTTY)
	TTYPrintf("FIO server started");
#endif
#endif

    return 1;
}

int MEAPI MeOpenRaw(const char *filename, MeOpenMode_enum mode)
{
    int flag;

    if (MeOpenNGCInit)
    {
        register unsigned i;

        for (i = 0; i < MeOpenFilesMAX; i++)
            MeOpenFiles[i] = FIO_INVALID_HANDLE;

        MeNgcMCCInit();

        MeOpenNGCInit = 0;
    }

    switch (mode)
    {
    case kMeOpenModeRDONLY:
    case kMeOpenModeRDBINARY:
        flag = FIO_OPEN_RDONLY;
        break;
    case kMeOpenModeWRONLY:
        flag = (FIO_OPEN_WRONLY|FIO_OPEN_CREAT|FIO_OPEN_TRUNC);
        break;
    case kMeOpenModeRDWR:
        flag = FIO_OPEN_RDWR;
        break;
    default:
        MeFatalError(3,"Invalid mode parameter %d passed to MeOpen()",mode);
    }

    {
        unsigned i;
        FIOHandle f;

        for (i = 0; i < MeOpenFilesMAX
                && MeOpenFiles[i] != FIO_INVALID_HANDLE; i++)
            continue;

        if (i == MeOpenFilesMAX)
        {
            MeWarning(1, "More than '%d' files open in 'MeOpen()'",
                MeOpenFilesMAX);
            return -1;
        }

        f = FIOFopen(filename,flag);

        MeOpenFiles[i] = f;

        return (f != FIO_INVALID_HANDLE) ? i : -1;
    }
}

int MEAPI MeLseek(int file, int offset, MeSeekOrigin_enum origin)
{
    int o;

    switch (origin)
    {
    case kMeSeekSET: o = FIO_SEEK_TOP; break;
    case kMeSeekCUR: o = FIO_SEEK_CUR; break;
    case kMeSeekEND: o = FIO_SEEK_LAST; break;

    default:
        MeFatalError(3, "Invalid origin %d passed to 'MeLseek()'",
            origin);
    }

    return FIOFseek(MeOpenFiles[file], offset, o);
}

int MEAPI MeRead(int file, void * buf, int count)
{
    return FIOFread(MeOpenFiles[file],buf,count);
}

int MEAPI MeWrite(int file, void * buf, int count)
{
    return FIOFwrite(MeOpenFiles[file],buf,count);
}

int MEAPI MeClose(int file)
{
    if (file == -1)
        return -1;

    if (file < -1 || file >= MeOpenFilesMAX)
    {
        MeFatalError(3,"%s","Invalid file number passed to 'MeClose()'");
        /*NOTREACHED*/
        return -1;
    }
      
    if (MeOpenFiles[file] == FIO_INVALID_HANDLE)
    {
        MeWarning(1,"%s","File number %d not open in 'MeClose()'",file);
        return -1;
    }

    (void) FIOFflush(file);

    {
        const unsigned closed = FIOFclose(MeOpenFiles[file]);

        MeOpenFiles[file] = FIO_INVALID_HANDLE;

        return (closed) ? 0 : -1;
    }
}
