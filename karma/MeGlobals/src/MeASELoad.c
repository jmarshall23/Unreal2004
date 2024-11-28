/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.19.2.2 $

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

#include <MeStream.h>
#include <MeASELoad.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMath.h>

static void ASEObjectReset(MeASEObject* object, MeASEMaterialStore* materials)
{
    object->isLoaded = 0;

    object->numVerts = 0;
    object->numFaces = 0;
    object->numUvs = 0;

    object->verts = 0;
    object->faces = 0;
    object->uvs = 0;

    object->nextObject = 0;

    memset(object->name, 0, 256);

    object->matStore = materials;
}

static void ASEMaterialStoreReset(MeASEMaterialStore* materials)
{
    materials->numMaterials = 0;
    materials->materials = 0;    
}

/* 
    Take a string a token and return pointer to start of arguments,
    or NULL if token not found. 
*/
char* FindArgs(char* line, char* token)
{
    char* args = strstr(line, token);
    if(!args)
    {
        return args;
    }
    else
    {   
        int tokenLen = strlen(token);
        args += (tokenLen + 1);
        return args;
    }
}

void MEAPI MeASEObjectDestroy(MeASEObject* object)
{
    int i;

    /*  First (just once) destroy material store.
        This is a bit nasty as it assumes all objects use same materialstore */
    if (object->matStore->materials)
    {
        for (i=0; i < object->matStore->numMaterials; i++)
        {
            if (object->matStore->materials[i].subMaterials)
            {
                MeMemoryAPI.destroy(object->matStore->materials[i].subMaterials);
            }
        }

        MeMemoryAPI.destroy(object->matStore->materials);
    }
    if( object->matStore)
        MeMemoryAPI.destroy(object->matStore);
    
    /* Then destroy each ase object in turn. */
    while(object)
    {
        MeASEObject* tmp = object->nextObject;
        
        if (object->verts)
            MeMemoryAPI.destroy(object->verts);
        if (object->faces)
            MeMemoryAPI.destroy(object->faces);
        if (object->uvs)
            MeMemoryAPI.destroy(object->uvs);
        MeMemoryAPI.destroy(object);

        object = tmp;
    }
}

/* Included for backwards compatibility. */
MeASEObject* MEAPI MeASEObjectLoad(char* filename, 
                       MeReal xScale, MeReal yScale, MeReal zScale)
{
    return MeASEObjectLoadParts(filename, xScale, yScale, zScale, 0);
}


/* Load a .ase file as either one big MeASEObject, or each part as a seperate
MeASEObject. */
MeASEObject* MEAPI MeASEObjectLoadParts(char* filename, 
                       MeReal xScale, MeReal yScale, MeReal zScale,
                       MeBool asParts)
{
    MeASEObject* aseObj;
    MeStream fp;
    fp = MeStreamOpenWithSearch(filename, kMeOpenModeRDONLY);
    
    if (!fp)
    {
#ifdef _MECHECK
        MeWarning(0, "MeASEObjectLoad: Could not open file '%s'.", filename);
#endif
        return 0;
    }

    aseObj = MeASEObjectLoadPartsFromStream(fp, xScale, yScale, zScale, asParts);        

    MeStreamClose(fp);

    return aseObj;
}

MeASEObject* MEAPI MeASEObjectLoadPartsFromStream(MeStream fp, 
                       MeReal xScale, MeReal yScale, MeReal zScale,
                       MeBool asParts)                      
{
    char line[256];
    char *args;
    int index;
    int i;
    MeBool mappingchannel = 0, gotName = 0;
    int vertOffset = 0;
    int faceOffset = 0;
    int uvOffset = 0;
    MeBool isError = 0;
    MeASEObject* object = 0; /* The 'current' geometry object. */
    MeMatrix4 vertexTM;
    MeASEMaterialStore* materialList = 
        (MeASEMaterialStore*)MeMemoryAPI.create(sizeof(MeASEMaterialStore));

    ASEMaterialStoreReset(materialList);

    MeMatrix4TMMakeIdentity(vertexTM);

    while (MeStreamReadLine(line, 256, fp) && !isError)
    {
        /* Beginning of a geometry object. */
        if (strstr(line,"*GEOMOBJECT"))
        {
            mappingchannel = 0;

            /* If this is the first geomobject, always create one. */
            if(!object)
            {
                object = (MeASEObject*)MeMemoryAPI.create(sizeof(MeASEObject));
                ASEObjectReset(object, materialList);
                gotName = 0;
            }

            /* If we already have an aseobject, only create a new one if
                loading as seperate parts. */
            else if(asParts)
            {
                MeASEObject* oldObject = object;
                object = (MeASEObject*)MeMemoryAPI.create(sizeof(MeASEObject));
                ASEObjectReset(object, materialList);
                object->nextObject = oldObject;
                gotName = 0;
            }

            continue;
        }

        /* Store (first) object name. */
        if((args = FindArgs(line, "*NODE_NAME")) && (!gotName))
        {
            char name[256];
            if(sscanf(args,"\"%s\"", name) == 1)
            {
                name[strlen(name)-1] = 0;
                strncpy(object->name, name, 256);
                gotName = 1;
            }
            continue;
        }

        /* Reading the transform is VERY dubious! Should do it all at once. */
        if((args = FindArgs(line, "*TM_ROW0")))
        {
            float x,y,z;
            if (sscanf(args,"%f %f %f",&x,&y,&z) == 3)
            {
                vertexTM[0][0] = x;
                vertexTM[0][1] = y;
                vertexTM[0][2] = z;
            }
            continue;
        }

        if((args = FindArgs(line, "*TM_ROW1")))
        {
            float x,y,z;
            if (sscanf(args,"%f %f %f",&x,&y,&z) == 3)
            {
                vertexTM[1][0] = x;
                vertexTM[1][1] = y;
                vertexTM[1][2] = z;
            }
            continue;
        }

        if((args = FindArgs(line, "*TM_ROW2")))
        {
            float x,y,z;
            if (sscanf(args,"%f %f %f",&x,&y,&z) == 3)
            {
                vertexTM[2][0] = x;
                vertexTM[2][1] = y;
                vertexTM[2][2] = z;
            }
            continue;
        }

        if((args = FindArgs(line, "*TM_ROW3")))
        {
            float x,y,z;
            if (sscanf(args,"%f %f %f",&x,&y,&z) == 3)
            {
                vertexTM[3][0] = x;
                vertexTM[3][1] = y;
                vertexTM[3][2] = z;
            }
            continue;
        }


        if ((args = FindArgs(line,"*MESH_NUMVERTEX")))
        {
            vertOffset = object->numVerts;
            sscanf(args,"%d",&object->numVerts);
            object->numVerts += vertOffset;
            object->verts = (MeVector3 *) MeMemoryAPI.resize(object->verts,
                sizeof(MeVector3)*object->numVerts);
            continue;
        }

        if ((args = FindArgs(line,"*MESH_NUMFACES")))
        {
            faceOffset = object->numFaces;
            sscanf(args,"%d",&object->numFaces);
            object->numFaces += faceOffset;
            object->faces = (MeASEFace *) MeMemoryAPI.resize(object->faces,
                sizeof(MeASEFace)*object->numFaces);
            continue;
        }

        if ((args = FindArgs(line,"*MESH_VERTEX")))
        {
            float x,y,z;
            if (sscanf(args,"%d %f %f %f",&index,&x,&y,&z) == 4)
            {
                object->verts[index + vertOffset][0] = x * xScale;
                object->verts[index + vertOffset][1] = y * yScale;
                object->verts[index + vertOffset][2] = z * zScale;
            }
            continue;
        }

        if ((args = FindArgs(line,"*MESH_FACENORMAL")))
        {
            float n1,n2,n3;
            int temp;
            MeVector4 norm, transNorm;

            if (sscanf(args,"%d %f %f %f",&index,&n1,&n2,&n3) == 4)
            {

                norm[0] = n1; norm[1] = n2; norm[2] = n3; norm[3] = 0;

                MeMatrix4MultiplyVector(transNorm, vertexTM, norm);

                object->faces[index + faceOffset].normal[0] = transNorm[0];
                object->faces[index + faceOffset].normal[1] = transNorm[1];
                object->faces[index + faceOffset].normal[2] = transNorm[2];
            }

            for (i=0;i<3;i++)
            {
                if(!MeStreamReadLine(line,256,fp))
                {
#ifdef _MECHECK
                    MeWarning(0, "MeASEObjectLoad: MESH_FACENORMAL incomplete.");
#endif
                    isError = 1;
                }

                args = FindArgs(line,"*MESH_VERTEXNORMAL");
                if (sscanf(args,"%d %f %f %f",&temp,&n1,&n2,&n3) == 4)
                {
                    norm[0] = n1; norm[1] = n2; norm[2] = n3; norm[3] = 0;
                    
                    MeMatrix4MultiplyVector(transNorm, vertexTM, norm);
                    
                    object->faces[index + faceOffset].vNormal[i][0] = transNorm[0];
                    object->faces[index + faceOffset].vNormal[i][1] = transNorm[1];
                    object->faces[index + faceOffset].vNormal[i][2] = transNorm[2];
                }
            }

            continue;
        }


        if (strstr(line,"*MESH_MAPPINGCHANNEL"))
        {
            mappingchannel = 1;
            continue;
        }

        if ((args = FindArgs(line,"*MESH_FACE")))
        {
            int i1,i2,i3;
            if (sscanf(args,"%d: A: %d B: %d C: %d",&index,&i1,&i2,&i3) == 4)
            {
                object->faces[index + faceOffset].vertexId[0] = i1 + vertOffset;
                object->faces[index + faceOffset].vertexId[1] = i2 + vertOffset;
                object->faces[index + faceOffset].vertexId[2] = i3 + vertOffset;
            }
            if ((args = FindArgs(args,"*MESH_MTLID")))
            {
                if (sscanf(args,"%d",&i1) == 1)
                {
                    object->faces[index + faceOffset].subMaterialId = i1;
                }
            }

            continue;
        }

        if (!mappingchannel)
        {
            if ((args = FindArgs(line,"*MESH_NUMTVERTEX")))
            {
                uvOffset = object->numUvs;
                sscanf(args,"%d",&object->numUvs);
                object->numUvs += uvOffset;
                object->uvs = (MeASEUV *) MeMemoryAPI.resize(object->uvs,
                    sizeof(MeASEUV)*object->numUvs);
                continue;
            }

            if ((args = FindArgs(line,"*MESH_TVERT")))
            {
                float x,y,z;
                if (sscanf(args,"%d %f %f %f",&index,&x,&y,&z) == 4)
                {
                    object->uvs[index + uvOffset].u = x;
                    object->uvs[index + uvOffset].v = y;
                }
                continue;
            }

            if ((args = FindArgs(line,"*MESH_TFACE")))
            {
                int i1,i2,i3;
                if (sscanf(args,"%d %d %d %d",&index,&i1,&i2,&i3) == 4)
                {
                    /* Structure copy not pointers */
                    object->faces[index + faceOffset].map[0] = object->uvs[i1 + uvOffset];
                    object->faces[index + faceOffset].map[1] = object->uvs[i2 + uvOffset];
                    object->faces[index + faceOffset].map[2] = object->uvs[i3 + uvOffset];
                }
                continue;
            }
        }

        if ((args = FindArgs(line,"*MATERIAL_REF")))
        {
            int temp;

            sscanf(args,"%d",&temp);
            for (i=faceOffset; i<object->numFaces; i++)
            {
                object->faces[i].materialId = temp;
            }
            continue;
        }

        if ((args = FindArgs(line,"*MATERIAL_COUNT")))
        {
            sscanf(args,"%d",&materialList->numMaterials);
            materialList->materials = 
                (MeASEMaterial *) MeMemoryAPI.resize(materialList->materials,
                sizeof(MeASEMaterial)*materialList->numMaterials);
            continue;
        }

        if ((args = FindArgs(line,"*MATERIAL")))
        {
            int materialIndex;
            int submat = 0;
            
            if (sscanf(args,"%d",&materialIndex) == 1)
            {
                MeBool diffusemap_found= 0;
                MeBool inmaterial = 0;
                
                /* Allocate one default material incase we have no sub mats */
                materialList->materials[materialIndex].numSubs = 1;
                materialList->materials[materialIndex].subMaterials = 
                    (MeASESubMaterial *)MeMemoryAPI.createZeroed(sizeof(MeASESubMaterial));
                
                materialList->materials[materialIndex].subMaterials[submat].type = MeASEMaterialFlagNone;
                
                while (MeStreamReadLine(line, 256, fp) && submat < materialList->materials[materialIndex].numSubs)
                {
                    if (strstr(line,"*MATERIAL_NAME"))
                    {
                        diffusemap_found= 0;
                        inmaterial = 1;
                        continue;
                    }
                    
                    if ((args = FindArgs(line,"*NUMSUBMTLS")))
                    {
                        int temp;
                        sscanf(args,"%d",&temp);
                        materialList->materials[materialIndex].numSubs = temp;
                        materialList->materials[materialIndex].subMaterials = (MeASESubMaterial *) MeMemoryAPI.resize(
                            materialList->materials[materialIndex].subMaterials,
                            sizeof(MeASESubMaterial)*materialList->materials[materialIndex].numSubs);
                        continue;
                    }
                    
                    if ((args = FindArgs(line,"*MATERIAL_AMBIENT")))
                    {
                        float* ambient = materialList->materials[materialIndex].subMaterials[submat].ambient;
                        sscanf(args,"%f %f %f",&ambient[0],&ambient[1],&ambient[2]);
                        ambient[3] = 0;
                        continue;
                    }
                    
                    if ((args = FindArgs(line,"*MATERIAL_DIFFUSE")))
                    {
                        float* diffuse = materialList->materials[materialIndex].subMaterials[submat].diffuse;                        
                        sscanf(args,"%f %f %f",&diffuse[0],&diffuse[1],&diffuse[2]);
                        diffuse[3] = 1.0f;
                        continue;
                    }
                    
                    if ((args = FindArgs(line,"*MATERIAL_SPECULAR")))
                    {
                        float* specular = materialList->materials[materialIndex].subMaterials[submat].specular;                        
                        sscanf(args,"%f %f %f",&specular[0],&specular[1],&specular[2]);
                        specular[3] = 1.0f;
                        continue;
                    }
                    
                    
                    if (strstr(line,"*MAP_DIFFUSE"))
                    {
                        diffusemap_found = 1;
                        continue;
                    }
                    
                    
                    /* Diffuse bitmap */
                    if ((args = FindArgs(line,"*BITMAP")) && diffusemap_found)
                    {
                        int j = 0;

                        args += 1; /* skip starting " */

                        while(args[j] != '\"')
                        {
                            materialList->materials[materialIndex].subMaterials[submat].texFilename[j] = args[j];
                            j++;
                        }
                        materialList->materials[materialIndex].subMaterials[submat].texFilename[j] = '\0';
                        
                        materialList->materials[materialIndex].subMaterials[submat].type = MeASEMaterialFlagTexture;
                        
                        diffusemap_found = 0;

                        if (inmaterial)
                        {
                            submat++;
                            inmaterial = 0;
                        }

                        continue;
                    }
                    
                    /* Got to the end of a material with nothing interesting in it */
                    if (strstr(line, "}"))
                    {
                        if (inmaterial)
                        {
                            submat++;
                        }
                        inmaterial = 0;
                        continue;
                    }
                } /* 'while' reading material list */
            } /* 'if' read material number */
        } /* 'if' this is a material line */
    } /* 'while' we haven't reached the end of the file */

    if(!isError)
    {
        if(object)
            object->isLoaded = 1;
        return object;
    }
    else
    {
        if(object)
            MeASEObjectDestroy(object);
        return 0;
    }
}
