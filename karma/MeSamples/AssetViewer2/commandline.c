/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.7.2.1 $

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
#include "visualization.h"
#include "commandline.h"
#include "instances.h"
#include "asestorage.h"
#include "network.h"

static int tokenize(char *string, ...)
{
    char **tokptr; 
    va_list arglist;
    int tokcount = 0;

    va_start(arglist, string);
    tokptr = va_arg(arglist, char **);
    while (tokptr) {
	while (*string && isspace((unsigned char) *string))
	    string++;
	if (!*string)
	    break;
	*tokptr = string;
	while (*string && !isspace((unsigned char) *string))
	    string++;
	tokptr = va_arg(arglist, char **);
	tokcount++;
	if (!*string)
	    break;
	*string++ = 0;
    }
    va_end(arglist);

    return tokcount;
}

void MEAPI DoCommandLineLoop(RRender *rc, void *userdata)
{
    char *help =
    "l <file>                               load a .ka file\n"
    "a                                      list assets in db\n"    
    "f                                      flush db\n"    
    "i <assetid> <instance name>            instance an asset\n"
    "r <instance name>                      remove instance\n"
    "d                                      remove all instances\n"
    "s                                      list instances\n"
    "z                                      list stored ases\n"
    "x                                      flush stored ases\n"
    "c <fullpathname> <asefilename>         store ase from file\n"
    "!                                      disable network\n"
    "q                                      return to simulation";

    input_t in;

    char *tok1, *tok2;

    puts(help);
    
    while (1)
    {
        putchar('>');
        fflush(stdout);
        
        if (!fgets(in, sizeof(input_t), stdin))
            break;
        
        if (strncmp(in, "?", 1) == 0)
        {
            puts(help);
        }
            
        else if (strncmp(in, "q", 1) == 0)
        {
            return;
        }

        /* load a whole .ka file */
        else if (strncmp(in, "l", 1) == 0)
        {
            MeStream stream;
            if (tokenize(in + 1, &tok1, 0) != 1) {
                puts("what?");
                continue;
            }
            
            stream = MeStreamOpen(tok1, kMeOpenModeRDONLY);
            
            if (!stream)
            {
                puts("file not found");
                continue;
            }
            
            LoadAssetDB(stream);
            MeStreamClose(stream);                        
        }
        
        else if (strncmp(in, "i", 1) == 0)
        {
            int assetID;
            if (tokenize(in + 1, &tok1, &tok2, 0) != 2) {
                puts("what?");
                continue;
            }
            
            // tok1 should be assetID
            assetID = atoi(tok1);
            if(AddInstance(assetID, tok2, 0))
                puts("Asset could not be created. Is is in the database?");
        }
        
        else if (strncmp(in, "r", 1) == 0)
        {
            if (tokenize(in + 1, &tok1, 0) != 1) {
                puts("what?");
                continue;
            }
            
            RemoveInstance(tok1);
        }        

        else if (strncmp(in, "d", 1) == 0)
        {
            RemoveAllInstances();
        }

        else if (strncmp(in, "s", 1) == 0)
        {
            /* list instances */
            AssetInstancePtr inst = instances;
            char* str;
            while(inst)
            {
                char assetID[5];                
                str = (char*)MeMemoryAPI.create(strlen(inst->m_instancename)+16);
                str[0] = '\0';
                strcat(str, inst->m_instancename);
                strcat(str, " (");
                _snprintf(assetID, 5, "%d", inst->m_assetID);
                strcat(str, assetID);
                strcat(str, ")");
                puts(str);
                MeMemoryAPI.destroy(str);
                inst = inst->m_next;
            }
        }

        else if (strncmp(in, "a", 1) == 0)
        {
            MeFAssetIt it;
            MeFAsset *asset;            
            MeAssetDBInitAssetIterator(db, &it);
            
            while (asset = MeAssetDBGetAsset(&it))
            {
                char *txt;
                char assetID[5];
                
                txt = (char*)MeMemoryAPI.create(strlen(MeFAssetGetName(asset))+10);
                txt[0] = '\0';
                _snprintf(assetID,5,"%d", asset->id);
                strcat(txt, assetID);
                strcat(txt, " (");
                strcat(txt, MeFAssetGetName(asset));
                strcat(txt, ")");
                puts(txt);
                MeMemoryAPI.destroy(txt);
            }
            
        }
        
        else if(strncmp(in, "f", 1) == 0 )
        {
            FlushAssetDB();
        }

        else if(strncmp(in, "z", 1) == 0 )
        {
            AseHolderPtr cur = aseList;
            while(cur)
            {
                puts(cur->m_filename);
                cur = cur->m_next;
            }
        }

        else if(strncmp(in, "x", 1) == 0 )
        {
            FlushAseList();
        }

        else if (strncmp(in, "c", 1) == 0)
        {
            MeStream asebuf, fileStr;
            char buf[1024];

            if (tokenize(in + 1, &tok1, &tok2, 0) != 2) {
                puts("what?");
                continue;
            }
            
            fileStr = MeStreamOpen(tok1, kMeOpenModeRDONLY);
            if(!fileStr)
            {
                puts("file not found");
                continue;
            }
            
            asebuf = MeStreamOpenAsMemBuffer(100000);
            asebuf->bGrowFast = 1;
            while( MeStreamReadLine(buf, 1024, fileStr) )
            {
                MeStreamWrite(buf, strlen(buf), 1, asebuf);
            }
            MeStreamClose(fileStr);
            MeStreamMemBufferFreeSlackSpace(asebuf);
            MeStreamRewind(asebuf);
            
            NewAse(tok2, asebuf, 1);            
        }        
        
        else if(strncmp(in, "!", 1) == 0 )
        {
            /* disable network */
            if(PlatformNetworkCleanup)
                PlatformNetworkCleanup();
            PlatformNetworkCleanup = 0;
            PlatformNetworkPoll = 0;
            PlatformNetworkInit = 0;            
        }

        else if(strncmp(in, "#", 1) == 0 )
        {
            /* resetapp */
            ResetApp();
        }

        else
        {
            puts("what?");
        }
    }
    
}