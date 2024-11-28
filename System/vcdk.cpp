#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define CDKEYBASEMAP ("ABCDEFGHJLKMNPQRTUVWXYZ2346789")
#define CDKEYBASE (strlen(CDKEYBASEMAP))
#define CDKEYBASEMAPINDEX(ch) (INT)((strchr(CDKEYBASEMAP,(ch))-CDKEYBASEMAP))

static char CDKEYAPPENDSTRING[64];

typedef int INT;
typedef unsigned char BYTE;
typedef unsigned long long QWORD;
typedef signed long long SQWORD;
typedef double DOUBLE;
typedef unsigned int DWORD;

static inline void appMD5Encode( BYTE* output, DWORD* input, INT len );
static inline void appMD5Decode( DWORD* output, BYTE* input, INT len );

struct FMD5Context
{
	DWORD state[4];
	DWORD count[2];
	BYTE buffer[64];
};


static inline DWORD INTEL_ORDER32(DWORD val)
{
#if MACOSX
    register DWORD retval;
    __asm__ __volatile__ (
        "lwbrx %0,0,%1"
            : "=r" (retval)
            : "r" (&val)
    );
    return retval;
#else
    return(val);
#endif
}

static inline QWORD INTEL_ORDER64(QWORD x)
{
#if MACOSX
    /* Separate into high and low 32-bit values and swap them */
	DWORD l = (DWORD) (x & 0xFFFFFFFF);
	DWORD h = (DWORD) ((x >> 32) & 0xFFFFFFFF);
	return( (((QWORD) (INTEL_ORDER32(l))) << 32) |
             ((QWORD) (INTEL_ORDER32(h))) );
#else
    return(x);
#endif
}


static inline char* _ui64tow( QWORD Num, char *Buffer, INT Base )
{
    int bufidx = 1;
    for (QWORD x = Num; x >= Base; x /= Base)
        bufidx++;

    Buffer[bufidx] = 0;  /* null terminate where the end will be. */

    while (--bufidx >= 0)
    {
        int val = (int) (Num % Base);
        Buffer[bufidx] = ((val > 9) ? ('a' + (val - 10)) : ('0' + val));
        Num /= Base;
    }

    return(Buffer);
}

static inline char *appQtoa( QWORD Num, INT Base )
{
	static char Buffer[30];
	memset( Buffer, '\0', sizeof(Buffer) );
	return _ui64tow( Num, Buffer, Base );
}



/*-----------------------------------------------------------------------------
	MD5 functions, adapted from MD5 RFC by Brandon Reinhart
-----------------------------------------------------------------------------*/

//
// Constants for MD5 Transform.
//

enum {S11=7};
enum {S12=12};
enum {S13=17};
enum {S14=22};
enum {S21=5};
enum {S22=9};
enum {S23=14};
enum {S24=20};
enum {S31=4};
enum {S32=11};
enum {S33=16};
enum {S34=23};
enum {S41=6};
enum {S42=10};
enum {S43=15};
enum {S44=21};

static BYTE PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

//
// Basic MD5 transformations.
//
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//
// Rotates X left N bits.
//
#define ROTLEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//
// Rounds 1, 2, 3, and 4 MD5 transformations.
// Rotation is seperate from addition to prevent recomputation.
//
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTLEFT ((a), (s)); \
	(a) += (b); \
}

#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTLEFT ((a), (s)); \
	(a) += (b); \
}

#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTLEFT ((a), (s)); \
	(a) += (b); \
}

#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTLEFT ((a), (s)); \
	(a) += (b); \
}

static inline void appMD5Transform( DWORD* state, BYTE* block );

//
// MD5 initialization.  Begins an MD5 operation, writing a new context.
//
static inline void appMD5Init( FMD5Context* context )
{
	context->count[0] = context->count[1] = 0;
	// Load magic initialization constants.
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
    memset(context->buffer, '\0', sizeof (context->buffer));
}

//
// MD5 block update operation.  Continues an MD5 message-digest operation,
// processing another message block, and updating the context.
//
static inline void appMD5Update( FMD5Context* context, BYTE* input, INT inputLen )
{
	INT i, index, partLen;

	// Compute number of bytes mod 64.
	index = (INT)((context->count[0] >> 3) & 0x3F);

	// Update number of bits.
	if ((context->count[0] += ((DWORD)inputLen << 3)) < ((DWORD)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((DWORD)inputLen >> 29);

	partLen = 64 - index;

	// Transform as many times as possible.
	if (inputLen >= partLen) {
		memcpy( &context->buffer[index], input, partLen );
		appMD5Transform( context->state, context->buffer );
		for (i = partLen; i + 63 < inputLen; i += 64)
			appMD5Transform( context->state, &input[i] );
		index = 0;
	} else
		i = 0;

	// Buffer remaining input.
	memcpy( &context->buffer[index], &input[i], inputLen-i );
}

//
// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
// Digest is 16 BYTEs.
//
static inline void appMD5Final ( BYTE* digest, FMD5Context* context )
{
	BYTE bits[8];
	INT index, padLen;

	// Save number of bits.
	appMD5Encode( bits, context->count, 8 );

	// Pad out to 56 mod 64.
	index = (INT)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	appMD5Update( context, PADDING, padLen );

	// Append length (before padding).
	appMD5Update( context, bits, 8 );

	// Store state in digest
	appMD5Encode( digest, context->state, 16 );

	// Zeroize sensitive information.
	memset( context, 0, sizeof(*context) );
}

//
// MD5 basic transformation. Transforms state based on block.
//
static inline void appMD5Transform( DWORD* state, BYTE* block )
{
	DWORD a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	appMD5Decode( x, block, 64 );

	// Round 1
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	// Round 2
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	// Round 3
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	// Round 4
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	// Zeroize sensitive information.
	memset( x, 0, sizeof(x) );
}

//
// Encodes input (DWORD) into output (BYTE).
// Assumes len is a multiple of 4.
//
static inline void appMD5Encode( BYTE* output, DWORD* input, INT len )
{
	INT i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (BYTE)(input[i] & 0xff);
		output[j+1] = (BYTE)((input[i] >> 8) & 0xff);
		output[j+2] = (BYTE)((input[i] >> 16) & 0xff);
		output[j+3] = (BYTE)((input[i] >> 24) & 0xff);
	}
}

//
// Decodes input (BYTE) into output (DWORD).
// Assumes len is a multiple of 4.
//
static inline void appMD5Decode( DWORD* output, BYTE* input, INT len )
{
	INT i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((DWORD)input[j]) | (((DWORD)input[j+1]) << 8) |
		(((DWORD)input[j+2]) << 16) | (((DWORD)input[j+3]) << 24);
}


union FCdKeyMD5Qword
{
	BYTE Digest[16];
	QWORD Q;
};


static inline QWORD __stoui64( const char * Start, char **End, INT Base )
{
    while (*Start == ' ')
        Start++;

    const char *ptr = Start;
    while (1)
    {
        char ch = toupper((char) *ptr);

        if ((ch >= 'A') && (ch <= 'Z'))
        {
            if ((Base <= 10) || (ch >= ('A' + Base)))
                break;
        }

        else if ((ch >= '0') && (ch <= '9'))
        {
            if (ch >= ('0' + Base))
                break;
        }

        else
            break;

        ptr++;
    }

    if (End != NULL)
        *End = const_cast<char *>(ptr);

    QWORD retval = 0;
    QWORD accumulator = 1;
    while (--ptr >= Start)
    {
        char ch = toupper((char) *ptr);
        QWORD val = (((ch >= 'A') && (ch <= 'Z')) ? (ch - 'A') + 10 : ch - '0');
        retval += val * accumulator;
        accumulator *= (QWORD) Base;
    }

    return(retval);
}


static inline char* _i64tow( SQWORD Num, char *Buffer, INT Base )
{
    int bufidx = 1;
    int negative = 0;
    if (Num < 0)
    {
        negative = 1;
        Num = -Num;
        bufidx++;
        Buffer[0] = '-';
    }

    for (QWORD x = Num; x >= Base; x /= Base)
        bufidx++;

    Buffer[bufidx] = 0;  /* null terminate where the end will be. */

    while ((--bufidx - negative) >= 0)
    {
        int val = (int) (Num % Base);
        Buffer[bufidx] = ((val > 9) ? ('a' + (val - 10)) : ('0' + val));
        Num /= Base;
    }

    return(Buffer);
}


static inline char HexToCDKeyMap( char ch )
{
	if( ch >= '0' && ch <='9' )
		return CDKEYBASEMAP[ch-'0'];
	if( ch >= 'A' && ch <='Z' )
		return CDKEYBASEMAP[ch-'A'+10];
	if( ch >= 'a' && ch <='z' )
		return CDKEYBASEMAP[ch-'a'+10];
	return 0;
}

static inline char *HexToCDKeyMap( const char *hex )
{
	static char Map[1024];
    int i;
	for( i=0;hex[i];i++ )
		Map[i] = HexToCDKeyMap(hex[i]);

    Map[i] = '\0';
	return Map;
}

static inline char CDKeyMapToHex( char ch )
{
	INT Index = CDKEYBASEMAPINDEX(ch);
	if( Index < 10 )
		return Index + '0';
	else
		return Index - 10 + 'a';
}

static inline char *CDKeyMapToHex( const char* ch )
{
	static char Map[1024];
    INT i;
	for( i=0;ch[i];i++ )
		Map[i] = CDKeyMapToHex(ch[i]);

    Map[i] = '\0';
	return Map;
}

static inline int ValidateCDKey( const char* CDKey )
{
    char FullKey[21];
    memset(FullKey, '\0', sizeof (FullKey));
    memcpy(&FullKey[0],  &CDKey[12], 5);
    memcpy(&FullKey[5],  &CDKey[6],  5);
    memcpy(&FullKey[10], &CDKey[0],  5);
    memcpy(&FullKey[15], &CDKey[18], 5);

	char Random[15];
    memset(Random, '\0', sizeof (Random));
    memcpy(Random, FullKey, 14);

	QWORD Seed = __stoui64( CDKeyMapToHex(Random), NULL, CDKEYBASE );

	char buf[128];
	char *Check = _i64tow(*((SQWORD *) &Seed), buf, 10);
	strcat(Check, CDKEYAPPENDSTRING);

	FCdKeyMD5Qword md5;

	FMD5Context Context;
	appMD5Init( &Context );
	appMD5Update( &Context, (unsigned char*)Check, strlen(Check) );
	appMD5Final( md5.Digest, &Context );

	char *CheckOutput = HexToCDKeyMap(appQtoa(INTEL_ORDER64(md5.Q), CDKEYBASE));
	CheckOutput[6] = '\0';

	while( strlen(CheckOutput) < 6 )
    {
        memmove(CheckOutput + 1, CheckOutput, strlen(CheckOutput) + 1);
        CheckOutput[0] = CDKEYBASEMAP[0];
    }

	return((strcmp(CheckOutput, FullKey + 14) == 0) ? 1 : 0);
}


static inline int massage_key(const char *argv1, char *keybuf, int keysize)
{
    if ((argv1 == NULL) || (strlen(argv1) > keysize - 1))
        return(0);

    strcpy(keybuf, argv1);

    // trim left spaces...
    char *ptr = keybuf;
    while (*ptr == ' ')
        ptr++;

    if (ptr != keybuf)
        memmove(keybuf, ptr, strlen(ptr) + 1);

    if (strlen(keybuf) == 0)
        return(0);

    // trim right spaces...
    ptr = keybuf + (strlen(keybuf) - 1);
    while ((ptr > keybuf) && (*ptr == ' '))
        ptr--;

    ptr++;
    if (*ptr == ' ')
        *ptr = '\0';

    size_t l = strlen(keybuf);

    // add dashes if needed...
    if (strchr(keybuf, '-') == NULL)
    {
        if (l != 20)
            return(0);

        char tmpbuf[21];
        strcpy(tmpbuf, keybuf);
        memcpy(keybuf + 0,  tmpbuf + 0,  5); keybuf[5]  = '-';
        memcpy(keybuf + 6,  tmpbuf + 5,  5); keybuf[11] = '-';
        memcpy(keybuf + 12, tmpbuf + 10, 5); keybuf[17] = '-';
        memcpy(keybuf + 18, tmpbuf + 15, 5); keybuf[23] = '\0';
    }
    else if (l != 23)
    {
        return(0);
    }

    // uppercase key...
    for (ptr = keybuf; *ptr; ptr++)
        *ptr = (char) toupper(*ptr);

    return(1);
}


static inline void fill_in_string(void)
{
    //#define CDKEYAPPENDSTRING "appDebugfNoInit"
    int vals[] = {
                    98, 113, 113, 69, 102, 99, 118,
                    104, 103, 79, 112, 74, 111, 106, 117, 1, 0
                 };

    char *ptr = CDKEYAPPENDSTRING;
    int i;

    for (i = 0; vals[i]; i++)
        ptr[i] = vals[i] - 1;
}


int main(int argc, char **argv)
{
    int retval = 0;

    // game won't work, but this is a failsafe to get past installation in
    //  case there's a bug in this code.
    if ((argv[1]) && (strcasecmp(argv[1], "skip") == 0))
        retval = 1;
    else
    {
        fill_in_string();
        char keybuf[128];
        if (massage_key(argv[1], keybuf, sizeof (keybuf)))
            retval = ValidateCDKey(keybuf);
    }

    if (!retval)
        sleep(2);

    return(retval);
}

