/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:40 $ - Revision: $Revision: 1.7.2.2 $

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
#include <stdarg.h>
#include <stdio.h>
#include <MePrecision.h>
#include <MeXMLOutput.h>
#include <MeMemory.h>

void MEAPI pushTag(MeXMLOutput *op, char *tag);
void MEAPI popTag(MeXMLOutput *op);
void MEAPI closeTag(MeXMLOutput *op);
void MEAPI newLine(MeXMLOutput *op);

/**
    Creates a context for outputting XML. This context works for one stream
    only.
*/
MeXMLOutput *MEAPI MeXMLOutputCreate(MeStream stream)
{
    MeXMLOutput *op = MeMemoryAPI.create(sizeof(MeXMLOutput));
    op->stream = stream;
    op->depth = 0;  /* depth is number of tags in stack */
    op->tagHead = 0;
    return op;
}

/**
    Destroys XML output context. Closes any open tags first. This must be called
    before the stream is closed.
*/
void MEAPI MeXMLOutputDestroy(MeXMLOutput *op)
{
    while (op->tagHead)
        closeTag(op);
    
    MeMemoryAPI.destroy(op);
}

/**
    Writes an element to a memory buffer.
*/
int MEAPI MeXMLWriteElement(MeXMLOutput *op, int parent, char *tag, ...)
{
    char buffer[1024];
    va_list args;

    while(op->depth > parent)
        closeTag(op);

    /* if there is a parent tag it contains an element (this one) */
    if (op->tagHead)
        op->tagHead->contents = kElement;
    
    pushTag(op,tag);

    /* account for any attributes */
    va_start(args, tag);
    vsprintf(buffer, tag, args);
    va_end(args);
    
    /* build line of file output */
    if (parent != 0)
        newLine(op);
    
    MeStreamWrite("<", 1, 1, op->stream);
    MeStreamWrite(buffer, strlen(buffer), 1, op->stream);
    MeStreamWrite(">", 1, 1, op->stream);

    return op->depth;
}

/**
    Writes formatted data to a memory block and closes the tag.
*/
void MEAPI MeXMLWritePCDATA(MeXMLOutput *op, char *format, ...)
{
    char buffer[1024];
    va_list args;

    va_start(args, format);

    vsprintf(buffer, format, args);
    va_end(args);

    MeStreamWrite(buffer, strlen(buffer), 1, op->stream);
    
    op->tagHead->contents = kPCDATA;

    closeTag(op);
}

/**
    Writes an XML comment.
*/
void MEAPI MeXMLWriteComment(MeXMLOutput *op, char *c, ...)
{
    char buffer[4096];
    va_list args;
    
    va_start(args, c);
    vsprintf(buffer, c, args);
    va_end(args);    

    MeStreamWrite("<!--\n", 5, 1, op->stream);
    MeStreamWrite(buffer, strlen(buffer), 1, op->stream);
    MeStreamWrite("\n-->\n", 5, 1, op->stream);
}

/*
    Add tag to stack of open tags.
*/
static void MEAPI pushTag(MeXMLOutput *op, char *tag)
{
    tagNode *node;
    char buffer[1024];
    
    /* strip attributes */
    strncpy(buffer,tag,1024);
    strtok(buffer," ");

    node = MeMemoryAPI.create(sizeof(tagNode));
    node->tag = MeMemoryAPI.create(strlen(buffer)+1);
    strcpy(node->tag,buffer);
    node->contents = kNothing;

    if (!op->tagHead)
    {
        op->tagHead = node;
        node->next = 0;
    }
    else
    {
        node->next = op->tagHead;
        op->tagHead = node;
    }
    op->depth++;
}

/*
    Pop tag from stack of open tags.
*/
static void MEAPI popTag(MeXMLOutput *op)
{
    struct tagNode *temp = op->tagHead->next;
    MeMemoryAPI.destroy(op->tagHead->tag);
    MeMemoryAPI.destroy(op->tagHead);
    op->tagHead = temp;
    op->depth--;
}

/*
    Write a closing XML tag to the memory block.
*/
static void MEAPI closeTag(MeXMLOutput *op)
{
    if (op->tagHead->contents == kElement)
        newLine(op);

    MeStreamWrite("</", 2, 1, op->stream);
    MeStreamWrite(op->tagHead->tag, strlen(op->tagHead->tag), 1, op->stream);
    MeStreamWrite(">", 1, 1, op->stream);

    popTag(op);
}

static void MEAPI newLine(MeXMLOutput *op)
{
    int i;

    MeStreamWrite("\n", 1, 1, op->stream);

    for (i = 0; i < op->depth-1; i++) 
        MeStreamWrite("\t", 1, 1, op->stream);
}

