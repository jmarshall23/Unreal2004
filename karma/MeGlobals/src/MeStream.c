/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 16:25:53 $ - Revision: $Revision: 1.30.2.6 $

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

#include <MeMemory.h>
#include <MeSimpleFile.h>

#include <MeStream.h>

extern const char *const MeFilePrefix;

/**
 *  Opens a stream in the given mode and search either only the first
 *  (null) prefix or all prefixes.
 *
 *  Returns 0 if the stream couldn't be opened.
 */
static MeStream MeStreamOpenPrefixed(const char *const filename,
    const MeOpenMode_enum mode,const unsigned allPrefixes)
{
    MeStream s = (_MeStream *) (*MeMemoryAPI.create)(sizeof (_MeStream));

#if (MeStreamSIMPLE)
    int f;

    f = MeOpenPrefixed(filename,mode,allPrefixes);
    if (f < 0)
    {
        (*MeMemoryAPI.destroy)(s);
        return 0;
    }

    s->handle = f;
    s->whereAmI = 0;
    s->buffer = MeLoadWholeFileHandle(s->handle,&(s->bufferSize));

    MeClose(s->handle);
#endif

#if (MeStreamSTDIO)
    char *m;
    char fullname[FILENAME_MAX];
    unsigned prefixlen;
    const char *prefix2;
    unsigned i;

    s->bUseMemblock = 0;

    switch (mode)
    {
    case kMeOpenModeRDONLY:     m = "r";    break;
    case kMeOpenModeWRONLY:     m = "w";    break;
    case kMeOpenModeRDWR:       m = "a+";   break;
    case kMeOpenModeRDBINARY:   m = "rb";   break;
    }

    if (MeFilePrefix == 0 || MeFilePrefix[0] == '\0')
    {
        fullname[0] = '\0';
        prefixlen = 0;
    }
    else
    {
        strncpy(fullname,MeFilePrefix,sizeof fullname);
        fullname[FILENAME_MAX-1] = '\0';
        prefixlen = strlen(fullname);
    }

    for (i = 0; (prefix2 = MeGetDefaultFileLocation(i)) != 0; i++)
    {
        fullname[prefixlen] = '\0';

        if (strncmp(filename,prefix2,strlen(prefix2)) != 0)
            strncat(fullname,prefix2,sizeof fullname-strlen(fullname));
        strncat(fullname,filename,sizeof fullname-strlen(fullname));

        fullname[FILENAME_MAX-1] = '\0';

        s->handle = (void *) fopen(fullname,m);

        if (s->handle != 0 || !allPrefixes)
            break;
    }

    if (s->handle == 0)
    {
        (*MeMemoryAPI.destroy)(s);
        return 0;
    }
#endif

    s->filename = filename;

    return s;
}

/**
 * Opens a stream in the given mode. See MeSimpleFile.h for the file modes.
 * Returns 0 if the stream couldn't be opened.
 */
MeStream MEAPI
MeStreamOpen(const char *filename,MeOpenMode_enum mode)
{
    return MeStreamOpenPrefixed(filename,mode,0);
}

/**
 *  Opens a stream in the given mode and searches default locations.
 *  Returns 0 if the stream couldn't be opened.
 */
MeStream MEAPI
MeStreamOpenWithSearch(const char *filename,MeOpenMode_enum mode)
{
    return MeStreamOpenPrefixed(filename,mode,1);
}

MeStream MEAPI 
MeStreamOpenAsMemBuffer(unsigned initialSize)
{
#if (MeStreamSIMPLE)
    return 0;
#endif

#if (MeStreamSTDIO)
    MeStream s;

    s = (_MeStream *) (*MeMemoryAPI.create)(sizeof (_MeStream));
    if (s == 0)
        return 0;

    s->filename = 0;
    s->handle = 0;

    s->bUseMemblock = 1;
    s->buffer = (char *) (*MeMemoryAPI.create)(initialSize);
    s->bufSize = initialSize;
    s->bufLength = 0;
    s->curIndex = 0;
    s->bGrowFast = 1;

    return s;
#endif
}

MeStream MEAPI 
MeStreamCreateFromMemBuffer(char *buffer,unsigned bufLength,unsigned bufSize)
{
#if (MeStreamSIMPLE)
    return 0;
#endif

#if (MeStreamSTDIO)
    MeStream s;

    s = (_MeStream *) (*MeMemoryAPI.create)(sizeof (_MeStream));
    if (s == 0)
        return 0;

    s->filename = 0;
    s->handle = 0;

    s->bUseMemblock = 1;
    s->buffer = buffer;
    s->bufSize = bufSize;
    s->bufLength = bufLength;
    s->curIndex = 0;
    s->bGrowFast = 1;

    return s;
#endif
}

void MEAPI 
MeStreamMemBufferFreeSlackSpace(MeStream stream)
{
#if (MeStreamSTDIO)
    char *newBuf;

    if (stream == 0 || stream->bUseMemblock == 0
            || stream->bufLength == stream->bufSize)
        return;

    newBuf = (char *) (*MeMemoryAPI.create)(stream->bufLength);
    memcpy(newBuf,stream->buffer,stream->bufLength);

    (*MeMemoryAPI.destroy)(stream->buffer);

    stream->buffer = newBuf;
    stream->bufSize = stream->bufLength;
    stream->curIndex = stream->bufLength;
#endif
}

void MEAPI 
MeStreamMemBufferUseConservativeGrowth(MeStream stream, MeBool bConsGrowth)
{
#if (MeStreamSTDIO)
    if (stream == 0 || stream->bUseMemblock == 0)
        return;

    stream->bGrowFast = !bConsGrowth;
#endif
}

/**
 *   Close a stream and free associated memory.
 */
void MEAPI
MeStreamClose(MeStream stream)
{
    if (stream == 0)
        return;

#if (MeStreamSIMPLE)
    if (stream->modified != MEFALSE)
        MeSaveWholeFileHandle(stream->handle,
            stream->buffer,stream->bufferSize);
    (*MeMemoryAPI.destroy)(stream->buffer);
#endif

#if (MeStreamSTDIO)
    if (!stream->bUseMemblock)
        (void) fclose((FILE *) stream->handle);
    else
    {
        if (stream->buffer != 0)
            (*MeMemoryAPI.destroy)(stream->buffer);
    }
#endif

    (*MeMemoryAPI.destroy)(stream);
}

/**
 *  Read from a stream.
 */
size_t MEAPI
MeStreamRead(void *buffer,size_t size,size_t count,MeStream stream)
{
    if (stream == 0) 
        return 0;

#if (MeStreamSIMPLE)
    if (stream->whereAmI + count*size < stream->bufferSize)
    {
        memcpy(buffer, stream->buffer + stream->whereAmI, count*size);
        stream->whereAmI += count*size;

        return count;
    }
    else
    {
        int bytes;
        memcpy(buffer,stream->buffer + stream->whereAmI, 
            stream->bufferSize - stream->whereAmI);
        bytes = stream->bufferSize - stream->whereAmI;
        stream->whereAmI = stream->bufferSize;

        return bytes / size;
    }
#endif

#if (MeStreamSTDIO)
    if (!stream->bUseMemblock)
        return fread(buffer,size,count,(FILE*)stream->handle);

    if(stream->curIndex + count*size < stream->bufLength )
    {
        memcpy(buffer, stream->buffer + stream->curIndex, count*size);
        stream->curIndex += count*size;
        return count;
    }
    else
    {
        /* cannot return (count) items */
        MeReal maxCount;
        unsigned retCount;

        maxCount = (MeReal) (stream->bufLength - stream->curIndex)
                        / (MeReal) size;
        retCount = (unsigned)floor(maxCount);

        memcpy(buffer, stream->buffer + stream->curIndex, retCount*size);
        stream->curIndex += retCount*size;
        return retCount;
    }
#endif
}

/**
 * Read a line of data from a stream.
 *
 * n is the maximum line length. Same as fgets().
 */
char *MEAPI MeStreamReadLine(char *string,int n,MeStream stream)
{
#if (MeStreamSIMPLE)
    char *c = string;
    int i = 0;

    if (stream->whereAmI == stream->bufferSize)
        return 0;

    for (i = 0; 
            i != (n-1)
            && *(c-1) != '\n'
            && stream->whereAmI != stream->bufferSize;
            i++)
        *c++ = stream->buffer[stream->whereAmI++];

    *c = '\0';

    return string;
#endif

#if (MeStreamSTDIO)
    unsigned endIndex;

    if (!stream->bUseMemblock)
        return fgets(string,n,stream->handle);

    if (stream->curIndex >= stream->bufLength-1)
        return 0;

    for (endIndex = stream->curIndex;
            endIndex != stream->bufLength-1
            && stream->buffer[endIndex] != '\n' 
            && endIndex-(stream->curIndex) != ((unsigned) n)-2;
            endIndex++)
        continue;

    memcpy(string,stream->buffer+stream->curIndex,
        endIndex-stream->curIndex+1);
    string[(endIndex-stream->curIndex)+1] = '\0';

    stream->curIndex = endIndex+1;

    return string;
#endif
}

/**
 *  Write to a stream.
 */
size_t MEAPI
MeStreamWrite(void *buffer,size_t size,size_t count,MeStream stream)
{
    if (stream == 0)
        return 0;

#if (MeStreamSIMPLE)
    stream->modified = !MEFALSE;

    if (stream->whereAmI + count*size > stream->bufferSize)
    {
        /* Make a new, bigger buffer! */
        char *newbuf = (char *)
            (*MeMemoryAPI.create)(stream->bufferSize + size*count);
        memcpy(newbuf,stream->buffer,stream->whereAmI+1);

        (*MeMemoryAPI.destroy)(stream->buffer);

        stream->buffer = newbuf;
        stream->bufferSize = stream->bufferSize + size*count;
    }

    memcpy(stream->buffer+stream->whereAmI,buffer,count*size);
    stream->whereAmI += count*size;

    return count;
#endif

#if (MeStreamSTDIO)
    if (!stream->bUseMemblock)
        return fwrite(buffer,size,count,(FILE*)stream->handle);

    /* Make sure buffer is big enough */
    if ((stream->curIndex + size*count) > stream->bufSize)
    {
        const unsigned newbufsize =
            (stream->bGrowFast && (stream->bufSize > size*count))
                ? stream->bufSize*2 : stream->bufSize + size*count;
        char *newbuf = (char *) (*MeMemoryAPI.create)(newbufsize);            

        memcpy(newbuf, stream->buffer, stream->bufLength);
        (*MeMemoryAPI.destroy)(stream->buffer);

        stream->buffer = newbuf;
        stream->bufSize = newbufsize;
    }

    /* Overwrite from current index --would an insert mode be useful? */

    memcpy(stream->buffer+stream->curIndex,buffer,count*size);

    {
        const unsigned charsToLength = stream->bufLength-stream->curIndex+1;

        stream->curIndex += count*size;

        if ((count*size) >= charsToLength)
            stream->bufLength += count*size - charsToLength+1;
    }

    return count;
#endif
}

void MEAPI 
MeStreamRewind(MeStream stream)
{
    if (stream == 0)
        return;

#if (MeStreamSIMPLE)
    stream->whereAmI = 0;
#endif

#if (MeStreamSTDIO)
    if (!stream->bUseMemblock)
        fseek((FILE *) stream->handle,0,SEEK_SET);
    else
        stream->curIndex = 0;
#endif
}
