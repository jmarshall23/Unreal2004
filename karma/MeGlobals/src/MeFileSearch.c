/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/25 18:48:29 $ - Revision: $Revision: 1.21.6.5 $

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
#include <string.h>

#include <MeMemory.h>
#include <MeMessage.h>

#include <MeSimpleFile.h>

#if (defined PS2)
    const char *const MeFilePrefix = "host:";
#else
    const char *const MeFilePrefix = "";
#endif

static const char *const MeDefaultFileLocations[] =
{
#if (defined _XBOX)
    "D:\\Resources\\",
    "D:\\Resources\\ka_files\\",
#elif (defined NGC)
    "/Resources/",
    "/Resources/ka_files/",
#else
    "",
#endif
    "Resources/",
    "../Resources/",
    "../../Resources/",
    "../../../Resources/",
    "../../../../Resources/",
    "../bin/Resources/",
    "../../bin/Resources/",
    "../../../bin/Resources/",
    "../../../../bin/Resources/",
    "../../../../demos/Resources/",
    "../Resources/ka_files/",
    "../../Resources/ka_files/",
    "../../../Resources/ka_files/",
    "../../../../Resources/ka_files/",
    "../bin/Resources/ka_files/",
    "../../bin/Resources/ka_files/",
    "../../../bin/Resources/ka_files/",
    "../../../../bin/Resources/ka_files/",
    "../../../../demos/Resources/ka_files/",
    "Resources/ka_files/",
    NULL
};

const char * MEAPI MeGetDefaultFileLocation(int i)
{
    return MeDefaultFileLocations[i];
}

#ifndef FILENAME_MAX
#   define FILENAME_MAX 250
#endif

int MEAPI
MeOpenPrefixed(const char *const filename,
    const MeOpenMode_enum mode,const unsigned allPrefixes)
{
    char fullname[FILENAME_MAX];
    unsigned prefixlen;
    const char *prefix2;
    unsigned i;
    int f;

    if (MeFilePrefix == 0 || MeFilePrefix[0] == '\0')
    {
        fullname[0] = '\0';
        prefixlen = 0;
    }
    else
    {
        strncpy(fullname,MeFilePrefix,sizeof fullname);
        fullname[FILENAME_MAX-1] = '\0';
        prefixlen = strlen(fullname);
    }

    for (i = 0; (prefix2 = MeGetDefaultFileLocation(i)) != 0; i++)
    {
        fullname[prefixlen] = '\0';

        if (strncmp(filename,prefix2,strlen(prefix2)) != 0)
            strncat(fullname,prefix2,sizeof fullname-strlen(fullname));
        strncat(fullname,filename,sizeof fullname-strlen(fullname));

        fullname[FILENAME_MAX-1] = '\0';

        f = MeOpenRaw(fullname,mode);

        if (f >= 0 || !allPrefixes)
            break;

        
    }

    return f;
}

int MEAPI
MeOpenWithSearch(const char *filename, MeOpenMode_enum mode)
{
    return MeOpenPrefixed(filename,mode,1);
}

int MEAPI
MeOpen(const char *filename, MeOpenMode_enum mode)
{
    return MeOpenPrefixed(filename,mode,0);
}
