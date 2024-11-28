/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/05/22 15:24:26 $ - Revision: $Revision: 1.16.2.8.4.1 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not`
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#include <string.h>
#include <MeAssetDBXMLIO.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeXMLParser.h>
#include <MeXMLOutput.h>
#include "MeAssetDBXMLIO_1_0.h"
#include "MeString.h"

#define N_KA_FILE_VERSIONS_SUPPORTED 1

static const char *Ka_File_Version_String[N_KA_FILE_VERSIONS_SUPPORTED] =
{
    "1.0"
};

static MeFAssetHandler MeFAssetHandlerFunc[N_KA_FILE_VERSIONS_SUPPORTED] = 
{
    Handle_KaFile_0_1    
};

static MeFAssetCreateFromFile MeFAssetCreateFunc[N_KA_FILE_VERSIONS_SUPPORTED] =
{
    KaFileCreate_1_0
};

static MeFAssetWriteXML MeFAssetWriteXMLFunc = KaFileWriteXML_1_0;

/***************************************************************\
  GENERIC INPUT/OUTPUT FILE USED BY ALL VERSIONS OF FILE FORMAT 
\***************************************************************/

/**
 * Returns the latest file format version string.
 */
const char *MEAPI GetLatestKaFileVersionString()
{
    return Ka_File_Version_String[N_KA_FILE_VERSIONS_SUPPORTED - 1];
}

/**
 * Create a context for importing XML files into an asset database. The
 * caller may create and pass in an optional MeIDPool. If an ID pool is not
 * passed in then the assets will not be created as uniquely identifiable,
 * although this can be done later. The caller must destroy
 * the ID pool.
 */
MeAssetDBXMLInput *MEAPI MeAssetDBXMLInputCreate(MeAssetDB *db, MeIDPool *IDPool)
{
    MeAssetDBXMLInput *input;
    input = (MeAssetDBXMLInput*)MeMemoryAPI.create(sizeof(MeAssetDBXMLInput));
    input->db = db;
    input->IDPool = IDPool;
    return input;
}

/**
 * Destroy XML input context.
 */
void MEAPI MeAssetDBXMLInputDestroy(MeAssetDBXMLInput *input)
{
    MeMemoryAPI.destroy(input);
}

/* TODO: implement this when the next version of the file format is needed */
static int MEAPI GetKaFileVersion(MeXMLInput *input)
{
    return 0;
}


/**
 * Read XML from a stream and populate the database.
 */
MeBool MEAPI MeAssetDBXMLInputRead(MeAssetDBXMLInput *i, MeStream stream)
{
    MeXMLInput *input;
    MeXMLError error;
    int version;

    MeXMLHandler handlers[] =
    {
        /* assumes version 1.0 at the moment */
        ME_XML_ELEMENT_HANDLER("KARMA", Handle_KaFile_0_1),
        ME_XML_HANDLER_END
    };

    PElement *xmlRoot = PElementCreate(0, "ROOT", 0, 0, 0);

    if (!i)
    {
        ME_REPORT(MeWarning(3, "Invalid MeAssetDBXMLInput object."));
        return 0;
    }

    if (!stream) 
    {
        ME_REPORT(MeWarning(3, "Invalid stream."));
        return 0;
    }

    input = MeXMLInputCreate(stream);
    version = GetKaFileVersion(input);

    MeXMLInputSetUserData(input, i);

    /* set off the parse loop which creates a tree of XML. */
    error = MeXMLInputProcess(input, handlers, xmlRoot);

    if (error != MeXMLErrorNone)
    {
        MeWarning(3,"Parse Error in file %s.\n%s", stream->filename, MeXMLInputGetErrorString(input));
        MeXMLInputDestroy(input);
        return 0;
    }

    MeXMLInputDestroy(input);

    /* Traverse the tree of XML and create all the assets */
    {
        PElementNode *node = xmlRoot->childHead;

        while (node)
        {
            PElement *e = node->current;
            MeFAsset *asset = MeFAssetCreateFunc[0](i->db, i->IDPool, e);
            MeAssetDBInsertAsset(i->db, asset);
            node = node->next;
        }
    }
    
    /* destroy the tree of XML */
    PElementTraverseAll(xmlRoot, PElementDestroyChildren, 0, 0);
    PElementDestroyChildren(xmlRoot, 0, 0);
    PElementDestroy(xmlRoot);

    return 1;
}

/**
 * Read XML from a stream and populate the database with the first asset found in file. + return ptr to it
 */
MeFAsset* MEAPI MeAssetDBXMLInputReadFirst(MeAssetDBXMLInput *i, MeStream stream)
{
    MeXMLInput *input;
    MeXMLError error;
    int version;
	MeFAsset *asset = NULL;

    MeXMLHandler handlers[] =
    {
        /* assumes version 1.0 at the moment */
        ME_XML_ELEMENT_HANDLER("KARMA", Handle_KaFile_0_1),
        ME_XML_HANDLER_END
    };

    PElement *xmlRoot = PElementCreate(0, "ROOT", 0, 0, 0);

    if (!i)
    {
        ME_REPORT(MeWarning(3, "Invalid MeAssetDBXMLInput object."));
        return NULL;
    }

    if (!stream) 
    {
        ME_REPORT(MeWarning(3, "Invalid stream."));
        return NULL;
    }

    input = MeXMLInputCreate(stream);
    version = GetKaFileVersion(input);

    MeXMLInputSetUserData(input, i);

    /* set off the parse loop which creates a tree of XML. */
    error = MeXMLInputProcess(input, handlers, xmlRoot);

    if (error != MeXMLErrorNone)
    {
        MeWarning(3,"Parse Error in file %s.\n%s", stream->filename, MeXMLInputGetErrorString(input));
        MeXMLInputDestroy(input);
        return NULL;
    }

    MeXMLInputDestroy(input);

    /* Traverse the tree of XML and create all the assets */
    {
        PElementNode *node = xmlRoot->childHead;

        if (node)
        {
            PElement *e = node->current;
            asset = MeFAssetCreateFunc[0](i->db, i->IDPool, e);
            MeAssetDBInsertAsset(i->db, asset);
            node = node->next;
        }
    }
    
    /* destroy the tree of XML */
    PElementTraverseAll(xmlRoot, PElementDestroyChildren, 0, 0);
    PElementDestroyChildren(xmlRoot, 0, 0);
    PElementDestroy(xmlRoot);

    return asset;
}


/**
 * Create an output context for writing an asset database to a stream
 * in XML.
 */
MeAssetDBXMLOutput *MEAPI MeAssetDBXMLOutputCreate(MeAssetDB *db)
{
    MeAssetDBXMLOutput *output;
    output = (MeAssetDBXMLOutput*)MeMemoryAPI.create(sizeof(MeAssetDBXMLOutput));
    output->db = db;
    output->fileHeader = 0;

    return output;
}

/**
 * Destroy XML output context.
 */
void MEAPI MeAssetDBXMLOutputDestroy(MeAssetDBXMLOutput *output)
{
    if (output->fileHeader)
        MeMemoryAPI.destroy(output->fileHeader);
    MeMemoryAPI.destroy(output);
}

/**
 * Write the whole database to a stream as formatted XML. Always writes the
 * latest version out, regardless of which version was read in.
 */
void MEAPI MeAssetDBXMLOutputWrite(MeAssetDBXMLOutput *output, MeStream stream)
{
    MeFAssetIt asset_it;
    MeFAsset *fa;
    char buffer[1024];
    char *pBuf = buffer;

    MeXMLOutput *op;
    MeXMLElementID karma, asset;

    op = MeXMLOutputCreate(stream);

    sprintf(buffer, "<?xml version=\"1.0\"?>\n\n");
    MeStreamWrite(buffer, strlen(buffer), 1, stream);

    if (output->fileHeader)
        MeXMLWriteComment(op, output->fileHeader);

    sprintf(buffer, "KARMA ka_file_version=\"%s\"", GetLatestKaFileVersionString());
    karma = MeXMLWriteElement(op, 0, buffer);

    if (MeAssetDBGetAssetCount(output->db) > 0)
    {
        MeAssetDBInitAssetIterator(output->db, &asset_it);

        while (fa = MeAssetDBGetAsset(&asset_it))
        {
            asset = MeFAssetWriteXMLFunc(op, fa, karma);
        }
    }

    MeXMLOutputDestroy(op);
}

/**
 * Outputs the specified header into all files as an XML comment.
 */
void MEAPI MeAssetDBXMLOutputSetFileHeader(MeAssetDBXMLOutput *output, char *header, ...)
{
    va_list args;
    char buf[4096];

    va_start(args, header);
    vsnprintf(buf, sizeof buf -1, header, args);
    va_end(args);
    
    if (output->fileHeader)
        MeMemoryAPI.destroy(output->fileHeader);

    output->fileHeader = MeMemoryAPI.create(strlen(buf) + 1);
    strcpy(output->fileHeader, buf);
}




