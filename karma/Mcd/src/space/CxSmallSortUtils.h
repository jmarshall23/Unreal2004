/*********************************************************
CxSmallSortUtils.h
Written by Bryan Galdrikian & Dilip Sequeira
(c) 1999-2001 MathEngine, Inc.
*********************************************************/

#ifndef _CxSmallSortUtils_h_
#define _CxSmallSortUtils_h_

#include <MePrecision.h>
#include <MeMemory.h>
#include <MeMath.h>
#include <McdCheck.h>
#include <McdModelPair.h>
#include <CxSpace.h>
#include <McdSpace.h>

/*********************************************************
	This file contains basic utils used by the far field
 *********************************************************/

/*********************************************************
    Link:
    Basic double link, except the "next" and "previous"
    pointers are never null.  A removed link will point
    to itself.
 *********************************************************/

class Link {
friend class LinkList;

//  Fields:
protected:
    Link *mNext;
    Link *mPrev;

//  Functions:
public:
    Link() { mNext = mPrev = this; }
    // no virtual destructor needed here.

    Link *Next() const { return mNext; }

    Link *Prev() const { return mPrev; }

    void InsertBefore(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "Link::InsertBefore");

        mNext = inLink;
        mPrev = mNext->mPrev;
        mPrev->mNext = this;
        mNext->mPrev = this;
    }

    void InsertAfter(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "Link::InsertAfter");

        mPrev = inLink;
        mNext = mPrev->mNext;
        mPrev->mNext = this;
        mNext->mPrev = this;
    }

    void InsertRingBefore(Link *inLink) {
        MCD_ASSERT (inLink != NULL, "Link::InsertRingBefore");

        Link *tail = inLink->mPrev;
        mPrev->mNext = inLink;
        tail->mNext = this;
        inLink->mPrev = mPrev;
        mPrev = tail;
    }

    void InsertRingAfter(Link *inLink) {
        MCD_ASSERT (inLink != NULL, "Link::InsertRingAfter");

        mPrev->mNext = inLink->mNext;
        inLink->mNext->mPrev = mPrev;
        mPrev = inLink;
        inLink->mNext = this;
    }

    void Remove() {
        mNext->mPrev = mPrev;
        mPrev->mNext = mNext;
        mPrev = this;
        mNext = this;
        }

    // Combined functions, saving two assignments
    void RemoveAndInsertBefore(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "Link::RemoveAndInsertBefore");

        mNext->mPrev = mPrev;
        mPrev->mNext = mNext;
        mNext = inLink;
        mPrev = mNext->mPrev;
        mPrev->mNext = this;
        mNext->mPrev = this;
    }

    void RemoveAndInsertAfter(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "Link::RemoveAndInsertAfter");

        mNext->mPrev = mPrev;
        mPrev->mNext = mNext;
        mPrev = inLink;
        mNext = mPrev->mNext;
        mPrev->mNext = this;
        mNext->mPrev = this;
    }
};


/*********************************************************
    LinkList:
    Holds Links as defined above.  This list uses a root
    node for conditional-free insertion and deletion.  A
    ring of links may be extracted or inserted into this
    list, however, for times when a true ring is needed.

    21-May-99:
    Derived from Link, so that it may use the pooling
    macros.  This makes it a tree node, as well.

    6-Feb-00:
    Removed derivation from Link, since a pooling macro
    was made which does not require that its contents be
    derived from Link.  (Note: the LinkPool macro still
    exists, and is more efficient at deleting an entire
    linked list.)

    7-Mar-00:
    LinkPool macro no longer exists.
 *********************************************************/

class LinkList {
//  Fields:
private:
    Link mRoot;

//  Functions:
public:
    Link *Head() const { return mRoot.mNext != &mRoot ? mRoot.mNext : NULL; }
    Link *Tail() const { return mRoot.mPrev != &mRoot ? mRoot.mPrev : NULL; }

    bool Empty() const { return mRoot.mNext == &mRoot; }

    // Insert link at tail
    void InsertTail(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "LinkList::InsertTail");

        inLink->InsertBefore(&mRoot);
    }

    // Insert link at head
    void InsertHead(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "LinkList::InsertHead");

        inLink->InsertAfter(&mRoot);
    }

    // Remove link
    void Remove(Link *inLink) {
        MCD_ASSERT(inLink != NULL && inLink != &mRoot, "LinkList::Remove");

        inLink->Remove();
    }

    // Combined functions:
    void RemoveAndInsertTail(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "LinkList::RemoveAndInsertTail");

        inLink->RemoveAndInsertBefore(&mRoot);
    }

    void RemoveAndInsertHead(Link *inLink) {
        MCD_ASSERT(inLink != NULL, "LinkList::RemoveAndInsertHead");

        inLink->RemoveAndInsertAfter(&mRoot);
    }

    // Remove all links, forming a ring
    Link *Ring() {
        Link *ring = mRoot.mNext;
        if (ring == &mRoot) {
            return NULL;
        }
        mRoot.Remove();
        return ring;
    }

    // Insert a ring of links at tail (can be a single link)
    void InsertRingTail(Link *inRing) {
        MCD_ASSERT(inRing != NULL, "LinkList::InsertRingTail");

        inRing->InsertRingBefore(&mRoot);
    }

    // Insert a ring of links at head (can be a single link)
    void InsertRingHead(Link *inRing) {
        MCD_ASSERT(inRing != NULL, "LinkList::InsertRingHead");

        inRing->InsertRingAfter(&mRoot);
    }

    // Insert list at head (removes links from inList)
    void InsertHead(LinkList &inList) {
        Link *inRing = inList.Ring();
        if (inRing) {
            InsertRingHead(inRing);
        }
    }

    // Insert list at tail (removes links from inList)
    void InsertTail(LinkList &inList) {
        Link *inRing = inList.Ring();
        if (inRing) {
            InsertRingTail(inRing);
        }
    }

    // Remove head if list is not empty
    Link *RemoveHead() {
        Link *head = mRoot.mNext;
        if (head == &mRoot) {
            return NULL;
        }
        head->Remove();

        return head;
    }

    // Remove tail if list is not empty
    Link *RemoveTail() {
        Link *tail = mRoot.mPrev;
        if (tail == &mRoot) {
            return NULL;
        }
        tail->Remove();

        return tail;
    }

    // Use only if inLink is in the list (Somewhat unsafe!):
    void RotateTo(Link *inLink) {
        MCD_ASSERT(inLink != NULL && inLink != &mRoot, "RotateTo");

        Link *ring = Ring();
        MCD_ASSERT(ring != NULL, "LinkList::RotateTo");

        InsertRingHead(inLink);
    }

//  Iterator functions:

    Link *IterHead() const { return mRoot.mNext; }

    Link *IterTail() const { return mRoot.mPrev; }

    bool Valid(Link *inIter) const {
        MCD_ASSERT (inIter != NULL, "LinkList::Valid");

        return inIter != &mRoot;
    }

//  UNSAFE FUNCTIONS: Use only if you know something about the list!

    // Use only if the list is empty:
    void UNSAFE_SetRing(Link *inRing) {
        MCD_ASSERT(Empty() && inRing != NULL && inRing != &mRoot, "LinkList::UNSAFE_SetRing");

        mRoot.InsertBefore(inRing);
    }

    // Use only if inLink is in the list:
    void UNSAFE_RotateTo(Link *inLink) {
        MCD_ASSERT(inLink != NULL && inLink != &mRoot, "LinkList::UNSAFE_RotateTo");

        mRoot.Remove();
        mRoot.InsertBefore(inLink);
    }

    // Use only if the list is NOT empty:
    Link *UNSAFE_Ring() {
        Link *ring = mRoot.mNext;
        MCD_ASSERT(ring != &mRoot, "LinkList::UNSAFE_Ring");
        mRoot.Remove();
        return ring;
    }

    // Use only if the list is NOT empty:
    Link *UNSAFE_Head() {
        MCD_ASSERT(!Empty(), "LinkList::UNSAFE_Head");

        return mRoot.mNext;
    }

    Link *UNSAFE_Tail() {
        MCD_ASSERT(!Empty(), "LinkList::UNSAFE_Tail");

        return mRoot.mPrev;
    }

    Link *UNSAFE_RemoveHead() {
        MCD_ASSERT(!Empty(),"LinkList::UNSAFE_RemoveHead");

        Link *head = mRoot.mNext;
        head->Remove();

        return head;
    }

    Link *UNSAFE_RemoveTail() {
        MCD_ASSERT(!Empty(),"LinkList::RemoveTail");

        Link *tail = mRoot.mPrev;
        tail->Remove();

        return tail;
    }
};



 /***************************************************
	BitArray
 ***************************************************/

class BitArray {
private:
	enum { kDefaultInc = 8 };	// increment in 4-byte words

	MeU32 *mArray;
	unsigned int mSize;	// in bits
	unsigned int mWordSize;	// in 4-byte words
	unsigned int mAlloc;	// in 4-byte words
	unsigned int mInc;	// in 4-byte words

	// re-allocate
	bool SetAlloc(const unsigned int inAlloc) {
		if (inAlloc > mAlloc) {
			MeU32 *newArray = (MeU32 *)MeMemoryAPI.create(sizeof(MeU32)*inAlloc);

			if (!newArray) { return false; }
			unsigned int index = mAlloc;
			while (index--) { newArray[index] = mArray[index]; }
            if(mArray)
                MeMemoryAPI.destroy(mArray);
			mArray = newArray;
			mAlloc = inAlloc;
		}
		return true;
	}

public:
	BitArray() : mSize(0), mWordSize(0), mAlloc(kDefaultInc), mInc(kDefaultInc) 
    {
        mArray = (MeU32 *)MeMemoryAPI.create(mAlloc * sizeof(MeU32));
	}

    BitArray(const unsigned int inSize) : mSize(inSize) 
    {
		mWordSize = (unsigned int)(((((int)mSize)-1)>>5)+1);
		mAlloc = mWordSize;
		mInc = mWordSize;
        mArray = (MeU32 *)MeMemoryAPI.create(mAlloc * sizeof(MeU32));
		MeU32 *word = mArray;
		const MeU32 *stop = word+mWordSize;
		while (word < stop) { *word++ = 0L; }
	}
	
    ~BitArray() 
    { 
        if(mArray) MeMemoryAPI.destroy(mArray); 
    }

	bool SetSize(const unsigned int inSize) 
    {
		unsigned int wordSize = (unsigned int)(((((int)inSize)-1)>>5)+1);
        bool result = true;
        if (wordSize > mAlloc) 
            result = SetAlloc(wordSize);
        if (result) 
        {
		    mSize = inSize;
		    mWordSize = wordSize;
        }
		return result;
	}

	void SetZero() 
    {
		unsigned int wordN = (unsigned int)(mSize>>5);
		if (mSize&31) mArray[wordN--] &= 0xFFFFFFFF<<(mSize&31);
		while (wordN) { mArray[--wordN] = 0L; }
	}

	void SetOne() 
    {
		unsigned int wordN = (unsigned int)(mSize>>5);
		if (mSize&31) mArray[wordN--] |= ~(0xFFFFFFFF<<(mSize&31));
		while (wordN) { mArray[--wordN] = 0xFFFFFFFF; }
	}

    void PadZero() 
    {
		unsigned int wordN = (unsigned int)(mSize>>5);
		// don't overwrite memory if mSize%32 == 0
		if (mSize&31) mArray[wordN++] &= ~(0xFFFFFFFF<<(mSize&31));
		while (wordN < mAlloc) { mArray[wordN++] = 0L; }
	}


	void PadOne() 
    {
		unsigned int wordN = (unsigned int)(mSize>>5);
		if (mSize&31) mArray[wordN++] |= 0xFFFFFFFF<<(mSize&31);
		while (wordN < mAlloc) { mArray[wordN++] = 0xFFFFFFFF; }
	}

	unsigned int GetSize() const 
    { 
        return mSize; 
    }

	unsigned int GetWordSize() const {
        return mWordSize; 
    }

	unsigned int GetAlloc() const 
    { 
        return mAlloc; 
    }

	bool operator [] (const int index) const 
    {
		return (mArray[index>>5]&(1<<(index&31))) != 0;
	}

	void Set(const int index, const bool inValue) 
    {
		int wordN = index>>5;
		int bitN = index&31;
		MeU32 word = mArray[wordN];
		word &= ~(1<<bitN);
		word |= ((MeU32)inValue)<<bitN;
		mArray[wordN] = word;
	}

	MeU32* GetArray() const 
    { 
        return mArray; 
    }
};



 /***************************************************
	NibbleArray
 ***************************************************/

class NibbleArray 
{
private:
	MeU8 *mArray;
    unsigned int mSize;

public:
	NibbleArray(const unsigned int inSize) 
    {
		mSize = (inSize>>1)+(inSize&1);
        mArray = (MeU8 *)MeMemoryAPI.create(mSize*sizeof(MeU8));
		MCD_ASSERT(mArray != NULL, "NibbleArray::NibbleArray");
		MeU8 *elem = mArray;
		MeU8 * const stop = mArray+mSize;
		while (elem < stop) { *elem++ = '\0'; }
	}

    ~NibbleArray() 
    { 
        if(mArray) MeMemoryAPI.destroy(mArray); 
    }

	MeU8 operator [] (const int index) const 
    {
		return (mArray[index>>1]>>((index&1)<<2))&0x0F;
	}

	void Set(const int index, const MeU8 inValue) {
		MeU8 &mem = mArray[index>>1];
		const MeU8 shift = (index&1)<<2;
		mem = (mem&(0xF0>>shift))|(inValue<<shift);
	}

	void And(const int index, const MeU8 inValue) {
		const MeU8 shift = (index&1)<<2;
		mArray[index>>1] &= (0xF0>>shift)|(inValue<<shift);
	}

	void Or(const int index, const MeU8 inValue) {
		mArray[index>>1] |= (inValue<<((index&1)<<2));
	}

	void Xor(const int index, const MeU8 inValue) {
		mArray[index>>1] ^= (inValue<<((index&1)<<2));
	}

	MeU8 *GetArray() const { return mArray; }
};



/***************************************************
    Hash table
 ***************************************************/





#endif // ifndef _CxSmallSort_h_
