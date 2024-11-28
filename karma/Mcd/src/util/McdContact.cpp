/* -*-c++-*-
 *===============================================================
 * File:        McdContact.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.16.2.2 $
 * $Date: 2002/04/04 15:28:58 $
 *
 *================================================================
 */


#include <McdContact.h>
#include "McdCheck.h"
#include "lsVec3.h"
#include <stdio.h>
#include <MeMemory.h>
#include <MeMath.h>
#include <McdModelPair.h>
#include <MeMessage.h>

/*----------------------------------------------------------------
 * McdContact implementation
 *----------------------------------------------------------------
 */

/**
   Reduce the number of contacts.
   The approximate average normal of the input contacts is
   needed and can be obtained from McdIntersectResult.normal
   Returns actual number of contacts that were produced.
   If @a inFaceNormalsFirst is non-zero, contacts involving a
   face (rather than only edges or vertices)
   are given priority.
   @a inContacts is the input array of McdContacts of size @a inNumContacts.
   @a outContacts is the output array of McdContacts.
   @a inMaxContactCount is the desired number of contacts. The actual number,
   which could be smaller than this number is returned by the function.
   @see McdIntersectResult
 */

bool Degenerate(McdContact *p0, McdContact *p1, MeReal scale) 
{
    const MeReal eps2 = 0.0001f;

    const bool p = MeVector3DistanceSqr(p0->position,p1->position)< eps2 * scale * scale;
    const bool n = MeVector3Dot(p0->normal,p1->normal) > (1.0f-eps2);

    return p && n;
}


/* returning (p1-p0)x(p2-p0).n */

inline MeReal DirectedArea(const McdContact *p0, 
                           const McdContact *p1,
                           const McdContact *p2, 
                           const MeReal *n) 
{
    const MeReal *v0 = p0->position;
    const MeReal *v1 = p1->position;
    const MeReal *v2 = p2->position;
    
    return (v2[2]-v0[2]) * (n[0]*(v1[1]-v0[1]) - n[1]*(v1[0]-v0[0]))
        + (v2[1]-v0[1]) * (n[2]*(v1[0]-v0[0]) - n[0]*(v1[2]-v0[2]))
        + (v2[0]-v0[0]) * (n[1]*(v1[2]-v0[2]) - n[2]*(v1[1]-v0[1]));
}


typedef struct _McdContactLink 
{
    McdContact *contact;
    MeReal area;
    struct _McdContactLink *next, *prev;
} McdContactLink;


typedef struct 
{
    McdContactLink *link;
    McdContactLink *head;
    McdContactLink *free;
    int ct;
} McdContactList;

void initList(McdContactList *list, int size) 
{
    int i;
    for(i=0;i<size-1;i++) list->link[i].next=list->link+i+1;
    list->link[size-1].next = 0;
    list->head=0;
    list->free = list->link;
    list->ct = 0;
}

McdContactLink *newElt(McdContactList *list, McdContact *contact) 
{
    McdContactLink *res = list->free;
    if(res) 
    {
        list->free = res->next;
        res->contact = contact;
        list->ct++;
    }
    
    MCD_CHECK_ASSERT_(res!=0, "newElt");
    return res;
}

void insAfter(McdContactLink *here, McdContactLink *addMe) 
{
    addMe->next = here->next;
    addMe->prev = here;
    here->next->prev = addMe;
    here->next = addMe;
}

void rmvElt(McdContactList *list, McdContactLink *elt) 
{
    list->head = elt->next;
    elt->next->prev = elt->prev;
    elt->prev->next = elt->next;
    elt->next = list->free;
    list->free = elt;
    list->ct--;
}


void UpdateHull(McdContactList *list, 
                McdContact *contact, 
                const MeReal *normal) 
{
    
    McdContactLink *p0 = list->head;
    McdContactLink *p1 = p0->next;
    McdContactLink *p2 = p1->next;
    
    MCD_CHECK_ASSERT_(p0 != p2, "UpdateHull");
    
    // Find transition edges
    McdContactLink *pKeepLast = NULL;
    McdContactLink *pKeepFirst = NULL;
    
    bool s0 = DirectedArea(p0->contact,contact,p1->contact,normal)<0;
    do {
        const bool s1 = DirectedArea(p1->contact,contact,p2->contact,normal)<0;
        if (s0^s1) 
        {
            if(s1)
                pKeepFirst = p1;
            else
                pKeepLast = p1;
        }
        s0 = s1;
        p0 = p1;
        p1 = p2;
        p2 = p2->next;
    } while (p0 != list->head);
    
    if (pKeepLast == NULL) 
    {
        MCD_CHECK_ASSERT_(pKeepFirst == NULL, "UpdateHull");
        // New point is inside hull.  Toss it.
        return;
    }
    
    MCD_CHECK_ASSERT_(pKeepFirst != NULL, "UpdateHull");
    // Remove all points from pKeepLast->Next() to pKeepFirst->Prev();
    // In their place, put p.
    
    for(p0 = pKeepLast->next; p0 != pKeepFirst; p0 = pKeepLast->next)
        rmvElt(list,p0);
    
    McdContactLink *p = newElt(list,contact);
    
    insAfter(pKeepLast,p);
    p->area = DirectedArea(pKeepLast->contact, contact, pKeepFirst->contact, normal);
    pKeepLast->area = DirectedArea(pKeepLast->prev->contact,pKeepLast->contact, contact, normal);
    pKeepFirst->area = DirectedArea(contact, pKeepFirst->contact, pKeepFirst->next->contact, normal);
}


inline int isFace(McdContact *contact) 
{
    return contact->dims>>8 == 2 || (contact->dims&3) == 2;
}

extern "C"
int MEAPI
McdContactSimplify(const MeReal *normal, 
                   McdContact *inContacts, 
                   const int inNumContacts,
                   McdContact *outContacts, 
                   const int inMaxContactPointCount,
                   const int inFaceNormalsFirst, 
                   MeReal scale)
{
    
    MCD_CHECK_ASSERT_(normal != NULL && outContacts != NULL && inContacts != NULL, "McdContactSimplify");
    
    const MeReal eps = 1.0e-4f * scale * scale;
    
    
    McdContactLink *p, *p0, *p1, *p2;
    int i;
    
    
    if (!inNumContacts || !inMaxContactPointCount) 
        return 0;
    
    /* make the first contact deepest, then never throw it away */
    McdContact *deepest = 0;
    MeReal sep = MEINFINITY;
    for(i=0;i<inNumContacts;i++) 
    {
        if(sep>inContacts[i].separation) 
        {
            sep = inContacts[i].separation;
            deepest = inContacts+i;
        }
    }
    if (deepest == NULL)
        return 0;

    if(inContacts!=deepest)
    {
        McdContact tmp = *inContacts;
        *inContacts = *deepest;
        *deepest = tmp;
    }
    
    if (inNumContacts == 1) 
    {
        *outContacts = *inContacts;
        return 1;
    }
    
    McdContactList list;
    list.link = (McdContactLink *)MeMemoryALLOCA((inMaxContactPointCount+1)*sizeof(McdContactLink));
    initList(&list,inMaxContactPointCount+1);
    MeReal area;
    
    p0 = newElt(&list, inContacts);
    list.head = p0;
    p0->next = p0->prev = p0;
    
    int cNum = 1; // position in old list
    
    if (inMaxContactPointCount == 1)
        goto done;
    
    while(cNum<inNumContacts && Degenerate(inContacts+cNum,p0->contact,scale))
        cNum++;
    
    if(cNum==inNumContacts)
        goto done;
    
    p1 = newElt(&list,inContacts+(cNum++));
    insAfter(p0, p1);
    
    if(cNum==inNumContacts || inMaxContactPointCount==2)
        goto done;
    
    // Now we have two non-degenerate points.  See if we can form a positive
    // area triangle
    
    
    while(cNum < inNumContacts) 
    {
        area = DirectedArea(p0->contact,p1->contact,inContacts + cNum,normal);
        if (area > eps) 
        {
            p2 = newElt(&list,inContacts+cNum++);
            insAfter(p1,p2);
            p0->area = p1->area = p2->area = area;
            break;
        } else if (area < -eps) 
        {
            p2 = newElt(&list,inContacts+cNum++);
            insAfter(p0,p2); // reorder for positive area
            p0->area = p1->area = p2->area = -area;
            break;
        }
        cNum++;
    }
    
    if(cNum==inNumContacts)
        goto done;
    
    // Now we have three points in the new list,
    // and we know we want at least three.  Try to maximize area.
    
    
    while (cNum < inNumContacts) 
    {        
        // for each point, update the convex hull, then
        // if we've exceeded the max # of points, remove the point
        // which decreases the area by the least.
        
        UpdateHull(&list,inContacts+cNum++,normal);
        
        if (list.ct > inMaxContactPointCount) 
        { // should just be one more
            MCD_CHECK_ASSERT_(list.ct == inMaxContactPointCount+1, "McdContactSimplify"); 
            
            area = list.head->area;
            McdContactLink *pMin = list.head->contact != inContacts ? list.head : list.head->next;
            p=list.head;
            do {
                if(p->area<area && p->contact!=inContacts) 
                {
                    pMin = p;
                    area = p->area;
                }
                p = p->next;
            } while(p!=list.head);
            
            p0 = pMin->prev;
            p1 = pMin->next;
            
            p0->area = DirectedArea(p0->prev->contact,p0->contact,p1->contact,normal);
            p1->area = DirectedArea(p0->contact,p1->contact,p1->next->contact,normal);
            
            rmvElt(&list,pMin);
        }
        
    }
    
    
done:
    MCD_CHECK_ASSERT_(list.ct>0 && list.ct <= inNumContacts, "McdContactSimplify");
        
    int count = list.ct;
    cNum = 0;
    
    if (inFaceNormalsFirst) 
    {
        for(p = list.head, i=0;i<count;i++) 
        {
            MCD_CHECK_ASSERT_(p && p->contact, "McdContactSimplify");
            const MeI16 dims = p->contact->dims;
            if ( (dims&0xFF) == 2 || (dims>>8) == 2) 
            {
                outContacts[cNum++] = *p->contact;
                McdContactLink *q = p->next;
                rmvElt(&list,p);
                p = q;
            } 
            else 
                p = p->next;                
        }
    }
    
    
    for(p = list.head, i=0, count = list.ct ;i<count; i++) 
    {
        outContacts[cNum++] = *p->contact;
        p = p->next;
    }
    
    return cNum;
}


