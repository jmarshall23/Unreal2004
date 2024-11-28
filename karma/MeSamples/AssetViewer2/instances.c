/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.4.2.1 $

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
#include "visualization.h"

MeBool InstanceAsset(int assetID, const char* instancename, MeMatrix4Ptr matrix)
{
    MeFAsset *asset;
    
    asset = MeAssetDBLookupAsset(db, assetID);
    if(!asset)
        return 0;
    else
    {
        MeAssetInstanceCreate(af, asset, matrix);
        RebuildGraphics();
        return 1;
    }    
}

void InstanceAssets()
{
    /* call instanceasset for all in list */
    AssetInstancePtr inst = instances;
    
    while(inst)
    {
        InstanceAsset(inst->m_assetID, inst->m_instancename, inst->m_initTM);
        inst = inst->m_next;
    }
}

MeBool IsInstanceNameInUse(const char* instancename)
{
    if(GetInstanceByName(instancename))
        return 1;
    else
        return 0;
}

void DeleteInstance(AssetInstancePtr inst)
{
    if(!inst)
        return;

    if(inst->m_instancename)
        MeMemoryAPI.destroy(inst->m_instancename);

    MeMemoryAPI.destroy(inst);
}

AssetInstancePtr GetInstanceByName(const char* instancename)
{
    AssetInstancePtr cur;

    cur = instances;

    while(cur)
    {
        if( !strcmp(cur->m_instancename, instancename) )
            return cur;

        cur = cur->m_next;
    }
    return 0;
}

int AddInstance(int assetID, const char* instancename, MeMatrix4Ptr matrix)
{
    AssetInstancePtr inst;
    AssetInstancePtr lst;

    if(IsInstanceNameInUse(instancename))
        return 1;

    inst = (AssetInstance*)MeMemoryAPI.create(sizeof(AssetInstance));

    inst->m_assetID = assetID;
    inst->m_instancename = (char*)MeMemoryAPI.create(strlen(instancename)+1);
    
    memcpy(inst->m_instancename, instancename, strlen(instancename)+1);

    if(matrix)
        MeMatrix4Copy(inst->m_initTM, matrix);
    else
        MeMatrix4TMMakeIdentity(inst->m_initTM);
    
    /* add to end of list */
    inst->m_next = 0;
    lst = instances;
    if(!lst)
        instances = inst;
    else
    {
        while(lst->m_next)
            lst = lst->m_next;

        lst->m_next = inst;
    }

    ResetApp();
    
    return 0;
}

MeBool RemoveInstance(const char* instancename)
{
    AssetInstancePtr next;
    AssetInstancePtr cur;
    
    cur = instances;
    
    if(!cur)
        return 0;
    next = cur->m_next;
    
    if(!strcmp(cur->m_instancename, instancename) )
    {
        instances = cur->m_next;
        DeleteInstance(cur);
        ResetApp();        
        return 1;
    }
    else
    {
        while(next)
        {
            if(!strcmp(next->m_instancename, instancename))
            {
                cur->m_next = next->m_next;
                DeleteInstance(next);
                ResetApp();                
                return 1;
            }
            
            cur = next;
            next = cur->m_next;
        }
    }
    return 0;
}

void RemoveAllInstances()
{
    RemoveAllInstancesNoReset();
    ResetApp();    
}

void RemoveAllInstancesNoReset()
{
    AssetInstancePtr cur;
    AssetInstancePtr next;
    
    cur = instances;
    if(!cur)
        return;
    
    while(cur)
    {
        next = cur->m_next;
        DeleteInstance(cur);
        cur = next;
    }
    
    instances = 0;
}
