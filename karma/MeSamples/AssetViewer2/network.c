/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.8.2.1 $

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

#include "instances.h"
#include "initialization.h"
#include "simulation.h"
#include "visualization.h"
#include "commandline.h"
#include "network.h"
#include "asestorage.h"


#ifdef WIN32
#include "winsockAVNet.h"
#elif (defined _XBOX)
#include "XboxAVNet.h"
#endif

AVNetInitFunc    PlatformNetworkInit    =0;
AVNetCleanupFunc PlatformNetworkCleanup =0;
AVNetPollFunc    PlatformNetworkPoll    =0;

int NetworkInit()
{
    /* select network platform */
#ifdef WIN32
    PlatformNetworkInit     = AVWinsockInit;
    PlatformNetworkCleanup  = AVWinsockCleanup;
    PlatformNetworkPoll     = AVWinsockPoll;
#elif (defined _XBOX)
    PlatformNetworkInit     = AVXboxInit;
    PlatformNetworkCleanup  = AVXboxCleanup;
    PlatformNetworkPoll     = AVXboxPoll;
#endif

    if(!PlatformNetworkInit)
        return 1;

    return PlatformNetworkInit();
}

void NetworkCleanup()
{
    if(PlatformNetworkCleanup)
        PlatformNetworkCleanup();
}

void NetworkPoll()
{
    if(PlatformNetworkPoll)
        PlatformNetworkPoll(CommandParse);
}

void MakeRetString(const char* str, char** retString, unsigned int *retLength)
{
    *retLength = strlen(str)+1;    
    *retString = (char*)MeMemoryAPI.create(*retLength);
    (*retString)[0] = '\0';
    strcat(*retString, str);
}

int CommandParse(char* cmd, unsigned int cmdLength, char** retString, unsigned int *retLength)
{
    /* cmd must be at least one byte */
    if(cmdLength == 0)
    {
        *retLength = 0;
        *retString = 0;
        return 1;
    }

    /* first byte of cmd determines function */
    switch(cmd[0])
    {
    case 'a': /* flushDB */
        FlushAssetDB();
        MakeRetString("Asset DB Flushed OK", retString, retLength);
        break;

    case 'b': /* add to DB */
        DBAddParse(0, cmd+1, cmdLength-1, retString, retLength);
        break;

    case 'c': /* set DB */
        DBAddParse(1, cmd+1, cmdLength-1, retString, retLength);
        break;

    case 'd': /* add instance */
        AddInstanceParse(cmd+1, cmdLength-1, retString, retLength);
        break;

    case 'e': /* remove instance */
        RemoveInstanceParse(cmd+1, cmdLength-1, retString, retLength);
        break;

    case 'f': /* removeall (instances) */
        RemoveAllInstances();
        MakeRetString("All instances removed OK", retString, retLength);
        break;

    case 'g': /* reset (flushall) */
        FlushAll();
        MakeRetString("Asset DB and Instances Flushed OK", retString, retLength);        
        break;
        
    case 'h': /* get instance list */
        ReturnInstanceList(retString, retLength);
        break;
        
    case 'i': /* flush ase list */
        FlushAseList();
        MakeRetString("ASE List Flushed OK", retString, retLength);
        break;

    case 'j': /* add ASE to list */
        AseAddParse(cmd+1, cmdLength-1, retString, retLength);
        break;

    case 'k': /* return list of assetIDs and names */
        ReturnAssetList(retString, retLength);
        break;

    default:
        /* unrecognised command */
        MakeRetString("Unrecognised Command", retString, retLength);        
        return 1;
    }
    
    return 0; /* success */
}


void DBAddParse(MeBool bDoSetDB, char* xml, unsigned int xmlLength, char** retString, unsigned int *retLength)
{
    MeStream stream;
    
    stream = MeStreamCreateFromMemBuffer(xml, xmlLength, xmlLength);
    
    if(bDoSetDB)
        SetAssetDB(stream);
    else
        LoadAssetDB(stream);
    
    /* don't close stream - we don't free buffer here */
    MeMemoryAPI.destroy(stream);
    
    MakeRetString("AssetDB modified OK", retString, retLength);
}

void AddInstanceParse(char* params, unsigned int paramsLength, char** retString, unsigned int *retLength)
{
    /* params should be instName::assetName */
    unsigned int colonIndex = 0;
    unsigned int brktIndex = 0;
    unsigned int l;
    char* names;
    char* compoundAssetName;
    int assetID;

    if(paramsLength < 4)
    {
        MakeRetString("Bad params", retString, retLength);
        return;
    }
    
    for(colonIndex = 1; colonIndex < paramsLength; colonIndex++)
    {
        if(params[colonIndex]==':' && params[colonIndex-1]==':')
            break;
    }
    
    if(colonIndex == paramsLength-1)
    {
        MakeRetString("Bad params", retString, retLength);
        return;
    }
    
    names = (char*)MeMemoryAPI.create(paramsLength+1);
    memcpy(names, params, paramsLength);
    
    names[colonIndex-1] = '\0';
    names[paramsLength] = '\0';
    
    compoundAssetName = names+colonIndex+1;
    /* compoundAssetName should now be:  assetname (assetID) 
       AddInstance requires assetID */
    for(brktIndex = strlen(compoundAssetName)-1; brktIndex >= 0; brktIndex--)
    {
        if(compoundAssetName[brktIndex] == ')')
        {
            compoundAssetName[brktIndex] = '\0';
            break;
        }
    }
    if(brktIndex < 1)
    {
        MakeRetString("Bad params", retString, retLength);
        MeMemoryAPI.destroy(names);
        return;        
    }

    l = strlen(compoundAssetName);
    for(brktIndex = 0; brktIndex<l; brktIndex++)
    {
        if(compoundAssetName[brktIndex] == '(')
        {
            compoundAssetName += brktIndex +1;
            break;
        }
    }

    if(brktIndex>=l)
    {
        MakeRetString("Bad params", retString, retLength);
        MeMemoryAPI.destroy(names);
        return;        
    }
    
    /* compoundassetname is now AssetID */
    assetID = atoi(compoundAssetName);

    if (AddInstance(assetID, names, 0))
        MakeRetString("Instance name in use", retString, retLength);
    else
        MakeRetString("Instance added OK", retString, retLength);    
}

void RemoveInstanceParse(char* params, unsigned int paramsLength, char** retString, unsigned int *retLength)
{
    char* instname = (char*)MeMemoryAPI.create(paramsLength+1);
    memcpy(instname, params, paramsLength);
    instname[paramsLength] = '\0';
    if(RemoveInstance(instname))
        MakeRetString("Instance removed OK", retString, retLength);
    else
        MakeRetString("Instance not found", retString, retLength);
    
    MeMemoryAPI.destroy(instname);
}

void ReturnInstanceList(char** retString, unsigned int *retLength)
{
    AssetInstancePtr inst = instances;
    MeStream stream;
    
    stream = MeStreamOpenAsMemBuffer(100);
    
    while(inst)
    {
        MeFAsset *asset;
        MeStreamWrite(">",1,1,stream);
        asset = MeAssetDBLookupAsset(db, inst->m_assetID); 
        if(!asset)
        {
            char assetID[5];
            _snprintf(assetID,5,"%d", inst->m_assetID);
            MeStreamWrite(assetID, strlen(assetID), 1, stream);
        }
        else
        {
            char *assetname = MeFAssetGetName(asset);
            MeStreamWrite(assetname, strlen(assetname), 1, stream);
        }
        MeStreamWrite("::",1,1,stream);
        MeStreamWrite(inst->m_instancename, strlen(inst->m_instancename), 1, stream);        
        inst = inst->m_next;
    }
    *retString = stream->buffer;
    *retLength = stream->bufLength;
    MeMemoryAPI.destroy(stream);
}

void AseAddParse(char* params, unsigned int paramsLength, char** retString, unsigned int *retLength)
{
    /* params should be asename::ASE */
    unsigned int colonIndex = 0;
    char* filename;
    MeStream aseStream;
    
    if(paramsLength < 4)
    {
        MakeRetString("Bad params", retString, retLength);
        return;
    }
    
    for(colonIndex = 1; colonIndex < paramsLength; colonIndex++)
    {
        if(params[colonIndex]==':' && params[colonIndex-1]==':')
            break;
    }
    
    if(colonIndex == paramsLength-1)
    {
        MakeRetString("Bad params", retString, retLength);
        return;
    }
    
    filename = (char*)MeMemoryAPI.create(colonIndex);
    memcpy(filename, params, colonIndex-1);
    filename[colonIndex-1] = '\0';
    
    aseStream = MeStreamOpenAsMemBuffer(paramsLength-colonIndex);
    memcpy(aseStream->buffer, params+colonIndex+1, paramsLength-colonIndex-1);
    aseStream->bufLength = paramsLength-colonIndex-1;
    aseStream->curIndex = 0;
    
    NewAse(filename, aseStream, 1);
    
    MeMemoryAPI.destroy(filename);
    MakeRetString("ASE added OK", retString, retLength);
}

void ReturnAssetList(char** retString, unsigned int *retLength)
{
    /* Create list of assetIDs and names
    each entry is: assetname (assetID)
    entries are separated with >       */
    
    MeFAssetIt it;
    MeFAsset *asset; 
    MeStream stream;
    char *assetName;
    stream = MeStreamOpenAsMemBuffer(1000);

    MeAssetDBInitAssetIterator(db, &it);    
    while (asset = MeAssetDBGetAsset(&it))
    {
        char assetID[5];
        _snprintf(assetID,5,"%d", asset->id);
        assetName = MeFAssetGetName(asset);

        MeStreamWrite(">",1,1,stream);        
        MeStreamWrite(assetName,strlen(assetName),1,stream);
        MeStreamWrite(" (",2,1,stream);
        MeStreamWrite(assetID, strlen(assetID), 1, stream);
        MeStreamWrite(")",1,1,stream);                
    }
    *retString = stream->buffer;
    *retLength = stream->bufLength;
    MeMemoryAPI.destroy(stream);    
}