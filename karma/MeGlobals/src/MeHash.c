/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/05/17 14:44:57 $ - Revision: $Revision: 1.21.2.1.4.1 $

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

#include <MeCall.h>
#include <MeHash.h>
#include <MeMessage.h>
#include <MeMemory.h>

/* EPIC CHANGE 12/29/2003
 *  Need MeUIntPtr typedef.  --ryan.
 */
#include <MePrecision.h>


/**
    Default hashing function. Only suitable for strings. NOT suitable for pointers.
*/
int MEAPI MeHashString(const void *string,int size)
{
    unsigned ret_val = 0;
    char *s;
    for (s = (char*)string; *s; s++)
        ret_val = ret_val * 33 + *s;

    return ret_val % size;
}

/**
    Hashing function suitable for ints and pointers.
*/
int MEAPI MeHashInt(const void *i, int size)
{
    /* EPIC CHANGE 12/29/2003
     *  Use MeUintPtr when casting from a pointer to an int...  --ryan.
     */
    return (int) (((MeUintPtr)i) % size);
}

/**
    Default compare key function.
*/
int MEAPI MeHashStringCompare(const void *k1, const void *k2)
{
    return strcmp((char*)k1,(char*)k2);
}

/**
    Alternative comparison function.
*/
int MEAPI MeHashIntCompare(const void *k1, const void *k2)
{
    return !(k1 == k2);
}

/**
    Creates a hash table with 'size' ordinals. The default table is
    suitable for strings.
*/
MeHash *MEAPI MeHashCreate(int size)
{
  MeHash *table = (MeHash*)MeMemoryAPI.create(sizeof(MeHash));
  table->population = 0;
  table->hash = MeHashString;
  table->compare = MeHashStringCompare;
  table->freeDatum = 0;
  table->freeKey = 0;
  table->collisions = 0;

  if ((size > (unsigned int)(-1) / sizeof(MeHashBucket *)) || size == 0) {
    table->size = 0;
    return (MeHash *)NULL;
  }

  table->table = (MeHashBucket **)MeMemoryAPI.create(size * sizeof(MeHashBucket *));

  if (table->table == NULL) {
    table->size = 0;
    return (MeHash *)NULL;
  }

  table->size = size;
  while (size)
    table->table[--size] = (MeHashBucket *)NULL;

  return table;
}

/**
    Deletes the given hash table and frees memory used by it.
*/
void MEAPI MeHashDestroy(MeHash *table)
{
    MeHashReset(table);
    MeMemoryAPI.destroy(table->table);
    MeMemoryAPI.destroy(table);
}

/**
    Resets the hash table. This leaves the hash table in the same state as
    it is in after calling MeHashCreate().
*/
void MEAPI MeHashReset(MeHash *table)
{
    MeHashBucket *bucket;
    int size = table->size;

    while (--size != -1) {
        while ((bucket = table->table[size])) {
            table->table[size] = bucket->next;
            if (table->freeDatum)
                table->freeDatum(bucket->datum);
            if (table->freeKey)
                table->freeKey(bucket->key);
            MeMemoryAPI.destroy(bucket);
        }
    }
    table->population = 0;
}

/**
    Overrides default hashing function.
*/
void MEAPI MeHashSetHashFunc(MeHash *table,MeHashFunc hash)
{
    table->hash = hash;
}

/**
    Overrides default key comparison function.
*/
void MEAPI MeHashSetKeyCompareFunc(MeHash *table,MeHashCompareFunc compare)
{
    table->compare = compare;
}

/**
    Sets the function to be called on the key when the hash table is destroyed.
*/
void MEAPI MeHashSetKeyFreeFunc(MeHash *table,MeHashFreeFunc freeKey)
{
    table->freeKey = freeKey;
}

/**
    Sets the function to be called on the datum when the hash table is destroyed.
*/
void MEAPI MeHashSetDatumFreeFunc(MeHash *table,MeHashFreeFunc freeDatum)
{
    table->freeDatum = freeDatum;
}

/**
    Inserts a key into a hash table. Returns pointer to inserted datum or
    a pointer to the existing datum if it is there already, or NULL
    for failure.
*/
void *MEAPI MeHashInsert(const void *key,const void *datum,MeHash *table)
{
  int hashVal = table->hash(key,table->size);
  MeHashBucket *bucket;
  int cmp;

  for (bucket = table->table[hashVal]; bucket; bucket = bucket->next) {
    cmp = table->compare(key, bucket->key);

    /* This key is already in the table; return existing datum */
    if (cmp == 0)
      return bucket->datum;

      table->collisions++;

    /* This key needs to be inserted at the beginning of the list of buckets */
    if (cmp < 0) {
      MeHashBucket *newBucket;
      if (!(newBucket = (MeHashBucket *)MeMemoryAPI.create(sizeof(MeHashBucket))))
        return NULL;
      newBucket->key = (void *)key;
      newBucket->datum = (void *)datum;
      newBucket->next = bucket;
      table->table[hashVal] = newBucket;
      table->population++;
      return (void *)datum;
    }

    /* This key needs to be inserted after the current bucket */
    else if (bucket->next == NULL ||
             table->compare(key, bucket->next->key) < 0) {
      MeHashBucket *newBucket;
      if (!(newBucket = (MeHashBucket *)MeMemoryAPI.create(sizeof(MeHashBucket))))
        return NULL;
      newBucket->key = (void *)key;
      newBucket->datum = (void *)datum;
      newBucket->next = bucket->next;
      bucket->next = newBucket;
      table->population++;
      return (void *)datum;
    }
  }

  /*
   * This cell of the hash table hasn't been used yet.  We simply
   * allocate space for the new bucket and point the table cell at it.
   */

  if (!(bucket = (MeHashBucket *)MeMemoryAPI.create(sizeof(MeHashBucket))))
    return NULL;

  bucket->key = (void *)key;
  bucket->datum = (void *)datum;
  bucket->next = (MeHashBucket *)NULL;
  table->table[hashVal] = bucket;
  table->population++;
  return (void *)datum;
}


/**
    Returns the datum associated with a given key. Returns NULL if the
    key doesn't exist.
*/
void *MEAPI MeHashLookup(const void *key, const MeHash *table)
{
  MeHashBucket *bucket;
  int cmp;

  for (bucket = table->table[table->hash(key,table->size)];
       bucket; bucket = bucket->next) {
    cmp = table->compare(key, bucket->key);
    if (cmp == 0)
      return bucket->datum;
    else if (cmp < 0 || bucket->next == NULL)
      return NULL;
  }

  return NULL;
}


/**
    Deletes an entry from the hash table and frees any memory.
    Returns NULL if key not found.
*/
void *MEAPI MeHashDelete(const void *key, MeHash *table)
{
    int hashVal = table->hash(key,table->size);
    MeHashBucket *bucket, *last;
    void *datum;
    int cmp;

    for (bucket = table->table[hashVal], last = (MeHashBucket *)NULL; bucket;
         last = bucket, bucket = bucket->next) {
        cmp = table->compare(key, bucket->key);
        if (cmp == 0) {
            if (last)
                last->next = bucket->next;
            else
                table->table[hashVal] = bucket->next;
            datum = bucket->datum;

            if (table->freeDatum) table->freeDatum(datum);
            if (table->freeKey) table->freeKey(bucket->key);

            MeMemoryAPI.destroy(bucket);
            table->population--;
            return datum;
        }
        else if (cmp < 0) {
            return NULL;
        }
    }
    return NULL;
}


/**
    Returns the number of buckets currently in the hash table.
*/
int MEAPI MeHashPopulation(const MeHash *table)
{
    return table->population;
}



/**
 * Initializes an iterator for use with a given table.
 */
MeHashIterator *MEAPI MeHashInitIterator(MeHashIterator *i, const MeHash *table)
{
    if (i) {
        i->bucket = (MeHashBucket *)NULL;
        i->index = 0;
        i->table = (MeHash *)table;
    }
    return i;
}


/**
    Returns a datum which has not yet been returned by the given iterator
    or NULL if all data for the iterator's hash table has already been returned.
*/
void *MEAPI MeHashGetDatum(MeHashIterator *i)
{
    if (i->bucket) {

      /*
       * If we're not yet at the end of the list, find the next bucket in the
       * current cell's list of buckets
       */

        if (i->bucket->next) {
            i->bucket = i->bucket->next;
            return i->bucket->datum;
        }

      /*
       * There are no buckets left in the current cell's list.  Increment the
       * hash table array index until we find the next cell that contains a
       * bucket.
       */

        for (++i->index; i->index < i->table->size; i->index++) {
            if (i->table->table[i->index]) {
                i->bucket = i->table->table[i->index];
                return i->bucket->datum;
            }
        }
    }

    /*
     * This iterator is freshly initialized.  Start at the first hash table array
     * index and iterate through them until we find the first cell that contains
     * a bucket.
     */

    else {
        for (i->index = 0; i->index < i->table->size; i->index++) {
            if (i->table->table[i->index]) {
                i->bucket = i->table->table[i->index];
                return i->bucket->datum;
            }
        }
    }
    /*
     * No cells containing buckets were found.  Reset the iterator and return
     * NULL.
     */

    i->index = 0;
    i->bucket = (MeHashBucket *)NULL;
    return NULL;
}
