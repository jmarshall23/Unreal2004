/*********************************************************
  hash.h
  Written by Bryan Galdrikian
  (c) 1999 MathEngine, inc.
 *********************************************************/

#ifndef _hash_h_
#define _hash_h_

/*********************************************************
  A good, fast hashing function.
 *********************************************************/

extern const MeU32 kgCRCHashTable[];

/**********************************************************************************
  "Generalized CRC Hash" -- from Dr. Dobb's Journal, Sep. 1997, p. 107
 **********************************************************************************/

inline MeU32 Hash(MeI8 *inKey, int inLen) {
  MeU32 hash = (MeU32)inLen;
  while (inLen-- > 0) {
    hash = (hash<<8)^kgCRCHashTable[(hash>>24)^((MeU8)*inKey++)];
  }
  return hash;
}

inline MeU32 Hash(MeI8 *inCStr) {
  MeI8 *str = inCStr;
  while (*str) { str++; }
  return Hash(inCStr,(int)(str-inCStr));
}

inline MeU32 HashLong(const MeU32 inKey) {
  MeU32 hash = (0x400)^kgCRCHashTable[(inKey>>24)&0xFF];
  hash = (hash<<8)^kgCRCHashTable[((hash>>24)^(inKey>>16))&0xFF];
  hash = (hash<<8)^kgCRCHashTable[((hash>>24)^(inKey>>8))&0xFF];
  return (hash<<8)^kgCRCHashTable[((hash>>24)^inKey)&0xFF];
}

#endif // ifndef _hash_h_
