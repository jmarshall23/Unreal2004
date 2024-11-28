#include <string.h>
#include <MeAssetDBXMLIO.h>
#include <MeIDPool.h>
#define _MECHECK
#include <MeAssert.h>
#include <MeMemory.h>


void DoTest(char *file)
{
    MeAssetDB *db1, *db2;
    MeAssetDBXMLInput *input;
    MeAssetDBXMLOutput *output;

    MeIDPool *IDPool;
//    int i, id;
    MeStream stream;
    MeStream stream1;
    char buf[255];
    char filename[128];
    strtok(file, ".");
    strcpy(filename, file);
    strcat(filename, ".ka");

    db1 = MeAssetDBCreate();

    /* There should be proper test code for this somewhere ... */
    IDPool = MeIDPoolCreate();
#if 0
    for (i = 0; i < 50; i++)
    {
        id = MeIDPoolRequestID(IDPool);
    }

    MeIDPoolReturnID(IDPool, 0);
    MeIDPoolReturnID(IDPool, 3);
    MeIDPoolReturnID(IDPool, 6);
    MeIDPoolReturnID(IDPool, 8);
    MeIDPoolReturnID(IDPool, 31);

    id = MeIDPoolRequestID(IDPool);
    id = MeIDPoolRequestID(IDPool);
    id = MeIDPoolRequestID(IDPool);
    id = MeIDPoolRequestID(IDPool);
    MeIDPoolReturnID(IDPool, 8);
    id = MeIDPoolRequestID(IDPool);
    id = MeIDPoolRequestID(IDPool);
    id = MeIDPoolRequestID(IDPool);
#endif
    input = MeAssetDBXMLInputCreate(db1, IDPool);

    stream1 = MeStreamOpen(filename, kMeOpenModeRDONLY);
    
    /* test membuffer */
    stream = MeStreamOpenAsMemBuffer(100);
    while( MeStreamReadLine(buf, 255, stream1) )
    {
        MeStreamWrite(buf, strlen(buf), 1, stream);
    }
    MeStreamClose(stream1);
    MeStreamRewind(stream);

    /* stream is a memory buffer */
    MeAssetDBXMLInputRead(input, stream);
    
    MeStreamClose(stream);

    printf("Creating assets from file.\n");

    printf("Copying database\n");
    db2 = MeAssetDBCreateCopy(db1);
    output = MeAssetDBXMLOutputCreate(db2);
    MeAssetDBXMLOutputSetFileHeader(output, "\tTest file");

    printf("deleting db1\n");
    MeAssetDBDeleteContents(db1);

    MEASSERT(MeAssetDBIsEmpty(db1));

    stream = MeStreamOpenAsMemBuffer(1000);
    printf("writing db2\n");
    MeAssetDBXMLOutputWrite(output, stream);
    MeStreamRewind(stream);
    
    strcpy(filename, file);
    strcat(filename, "_copy.ka");    
    stream1 = MeStreamOpen(filename, kMeOpenModeWRONLY);
    
    while(MeStreamReadLine(buf, 255, stream) )
    {
        MeStreamWrite(buf, strlen(buf), 1, stream1);
    }
    MeStreamClose(stream);

    MeStreamClose(stream1);

    MeAssetDBXMLInputDestroy(input);
    MeAssetDBXMLOutputDestroy(output);
    printf("deleting db1\n");
    MeAssetDBDestroy(db1);
    printf("deleting db2\n");
    MeAssetDBDestroy(db2);
    MeIDPoolDestroy(IDPool);
}