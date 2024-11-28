/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:40 $ - Revision: $Revision: 1.15.2.2 $

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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <MeXMLTree.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMisc.h>

static MeBool MEAPI ParseAttribute(char **attr, char **value, char **next)
{
    char *ptr = *attr;
    while (isspace(*ptr)) ptr++;
    *attr = ptr;
    while (!isspace(*ptr) && *ptr != '=') ptr++;
    if (*ptr != '=') return 0;
    *ptr = 0;
    ptr++;
    if (*ptr != '\"' && *ptr != '\'') return 0;
    ptr++;
    *value = ptr;
    ptr = strchr(ptr, '\"');
    if (!ptr) return 0;
    *ptr = 0;
    *next = ptr + 1;
    return 1;
}

/**
    Creates a PElement. The name is only stored when _MECHECK is defined.
*/
PElement *MEAPI PElementCreate(int type, char *name, void *data, CDataFreeFunc func, char *attrs)
{
    PElement *elem;
    elem = (PElement*)MeMemoryAPI.create(sizeof(PElement));
    elem->type = type;    
#ifdef _MECHECK
    if (name)
    {
        elem->name = MeMemoryAPI.create(strlen(name) + 1);
        strcpy(elem->name, name);
    }
#endif
    elem->childHead = 0;
    elem->cdata = data;
    elem->freeFunc = func;
    elem->attrHead = 0;
    
    PElementParseAttributes(elem, attrs);

    return elem;
}

/**
    Destroys a PElement, including the cdata which is allocated by the user.
*/
void MEAPI PElementDestroy(PElement *e)
{
    AttributeNode *node = e->attrHead, *temp;
    while (node)
    {   
        MeMemoryAPI.destroy(node->current->attr);
        MeMemoryAPI.destroy(node->current->value);
        MeMemoryAPI.destroy(node->current);
        temp = node->next;
        MeMemoryAPI.destroy(node);
        node = temp;
    }

#ifdef _MECHECK
    if (e->name)
        MeMemoryAPI.destroy(e->name);
#endif

    if (e->cdata && e->freeFunc)
		e->freeFunc(e->cdata);
    
    MeMemoryAPI.destroy(e);
}

/**
    Destroy all the children of a PElement but not the PElement itself.
*/
void MEAPI PElementDestroyChildren(PElement *e, PElement *parent, void *userdata)
{
    PElementNode *node, *temp;
    node = e->childHead;

    while (node)
    {
        PElementDestroy(node->current);
        temp = node->next;
        MeMemoryAPI.destroy(node);
        node = temp;
    }
}

/**
    Add an attribute to a PElement.
*/
void MEAPI PElementAddAttribute(PElement *e, char *attr, char *val)
{
    Attribute *at = (Attribute*)MeMemoryAPI.create(sizeof(Attribute));
    AttributeNode *node = (AttributeNode*)MeMemoryAPI.create(sizeof(AttributeNode));
    at->attr = MeMemoryAPI.create(strlen(attr) + 1);
    strcpy(at->attr, attr);
    at->value = MeMemoryAPI.create(strlen(val) + 1);
    strcpy(at->value, val);
    node->current = at;
    if (!e->attrHead)
    {
        e->attrHead = node;
        node->next = 0;
    }
    else
    {
        node->next = e->attrHead;
        e->attrHead = node;
    }
}

/**
    Parse the attribute string into a linked list.
*/
void MEAPI PElementParseAttributes(PElement *e, char *attrs)
{
    char *a = attrs, *val, *next = attrs;
    if (!attrs) return;
    while (*next)
    {
        if (!ParseAttribute(&a, &val, &next))
        {
#ifdef _MECHECK
            ME_REPORT(MeWarning(3,"Error parsing attribute '%s' in tag '%s'", a, e->name));
#endif
            return;
        }
        PElementAddAttribute(e, a, val);
        a = next;
    }
}

/**
    Inserts a PElement below its parent. 
*/
void MEAPI PElementInsert(PElement *e, PElement *parent)
{
    PElementNode *node;
    node = (PElementNode*)MeMemoryAPI.create(sizeof(PElementNode));
    node->current = e;

    if (!parent->childHead)
    {
        parent->childHead = node;
        node->next = 0;
    }
    else
    {
        node->next = parent->childHead;
        parent->childHead = node;
    }
}

/**
    Finds a PElement using the specified comparison function.
*/
PElement *MEAPI PElementFind(PElement *root, PElementCompareCB cb, void *k1, void *k2)
{
    PElementNode *node = root->childHead;

    while (node)
    {
        PElement *ret = 0;
        PElement *e = node->current;

        ret = cb(e, k1, k2);
        if (ret)
            return ret;
        
        ret = PElementFind(e, cb, k1, k2);
        
        if (ret)
            return ret;

        node = node->next;
    }
    return 0;
}

/**
    Return the string value of a given attribute.
*/
char *MEAPI PElementGetAttributeValue(PElement *elem, char *attr)
{
    AttributeNode *node = elem->attrHead;
    while (node)
    {
        Attribute *at = node->current;
        if (strcmp(at->attr, attr) == 0)
            return at->value;
        node = node->next;
    }
    return 0;
}

/**
    Compares two attributes.
*/
PElement *MEAPI PElementCompareAttributes(PElement *e, void *attr, void *attrVal)
{
    char *temp;
    temp = PElementGetAttributeValue(e, attr);
    if (temp && strcmp(temp, attrVal) == 0) 
        return e;
    return 0;
}

/** 
    Traverse tree depth first. If rootFirst is true, the callback will be called on the root
    first and work down to the leaves. If false the callback will get called on the leaves
    and work back up to the root.
*/
void MEAPI PElementTraverseAll(PElement *root, PElementCB cb, MeBool rootFirst, void *userdata)
{
    PElementNode *node = root->childHead;

    while (node)
    {
        PElement *e = node->current;
        if (rootFirst) 
            cb(e, root, userdata);
        
        PElementTraverseAll(e, cb, rootFirst, userdata);
        
        if (!rootFirst)
            cb(e, root, userdata);
        
        node = node->next;
    }
}

/**
    Initialize a PElement iterator. The first time PElementGetNext() is called the
    first child of this PElement is returned.
*/
void MEAPI PElementInitIterator(PElement *root, PElementIt *it)
{
    PElementNode *node = (PElementNode*)MeMemoryAPI.create(sizeof(PElementNode));
    node->current = root;
    node->next = 0;
    it->stackHead = node;
}

/**
    Returns the next PElement.
*/
PElement *MEAPI PElementGetNext(PElementIt *it)
{
    PElement *e;
    PElementNode *node, *child;

    node = it->stackHead;
    e = node->current;

    if (!e) return 0;
    
    /* pop node */
    it->stackHead = it->stackHead->next;
    MeMemoryAPI.destroy(node);

    /* add PElement's children */
    child = e->childHead;
    while (child)
    {
        node = (PElementNode*)MeMemoryAPI.create(sizeof(PElementNode));
        node->current = child->current;
        if (!it->stackHead)
        {
            it->stackHead = node;
            it->stackHead->next = 0;
        }
        else
        {
            node->next = it->stackHead;
            it->stackHead = node;
        }
        child = child->next;
    }

    node = it->stackHead;
    if (!node) return 0;

    return node->current;
}

/**
    Destroys the PElement iterator.
*/
void MEAPI PElementIteratorDestroy(PElementIt *it)
{
    PElementNode *node = it->stackHead;
    while (node)
    {
        PElementNode *temp = node->next;
        MeMemoryAPI.destroy(node);
        node = temp;
    }
}

/**
    Finds the first PElement with an attribute of the specified value.
*/
PElement *MEAPI PElementLookup(PElement *root, char *attr, char *val)
{
    PElementIt it;
    PElement *e = root;
    PElementInitIterator(root, &it);

    while (e = PElementGetNext(&it))
    {
        char *v = PElementGetAttributeValue(e, attr);
        if (v && strcmp(v, val) == 0)
        {
            PElementIteratorDestroy(&it);
            return e;
        }
    }

    PElementIteratorDestroy(&it);
    return 0;
}


