/*=============================================================================
	UnMisc.cpp: Various core platform-independent functions.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

// Core includes.
#include "CorePrivate.h"
#include "FConfigCacheIni.h"

CORE_API FCriticalSection *gethostbyname_mutex = NULL;

/*-----------------------------------------------------------------------------
	FOutputDevice implementation.
-----------------------------------------------------------------------------*/

void FOutputDevice::Log( EName Event, const TCHAR* Str )
{
	if( !FName::SafeSuppressed(Event)&&(GSys||(Event==NAME_Log||Event==NAME_Init)) )
		Serialize( Str, Event );
}
void FOutputDevice::Log( const TCHAR* Str )
{
	if( !FName::SafeSuppressed(NAME_Log) )
		Serialize( Str, NAME_Log );
}
void FOutputDevice::Log( const FString& S )
{
	if( !FName::SafeSuppressed(NAME_Log) )
		Serialize( *S, NAME_Log );
}
void FOutputDevice::Log( enum EName Type, const FString& S )
{
	if( !FName::SafeSuppressed(Type)&&(GSys||(Type==NAME_Log||Type==NAME_Init)) )
		Serialize( *S, Type );
}
void FOutputDevice::Logf( EName Event, const TCHAR* Fmt, ... )
{
	if( !FName::SafeSuppressed(Event)&&(GSys||(Event==NAME_Log||Event==NAME_Init)) )
	{
		TCHAR TempStr[4096];
		GET_VARARGS(TempStr,ARRAY_COUNT(TempStr),Fmt,Fmt);
		Serialize( TempStr, Event );
	}
}
void FOutputDevice::Logf( const TCHAR* Fmt, ... )
{
	if( !FName::SafeSuppressed(NAME_Log) )
	{
		TCHAR TempStr[4096];
		GET_VARARGS(TempStr,ARRAY_COUNT(TempStr),Fmt,Fmt);
		Serialize( TempStr, NAME_Log );
	}
}


/*-----------------------------------------------------------------------------
	FArray implementation.
-----------------------------------------------------------------------------*/

void FArray::Realloc( INT ElementSize )
{
	guard(FArray::Realloc);
	Data = appRealloc( Data, ArrayMax*ElementSize, TEXT("FArray") );
	unguardf(( TEXT("%i*%i"), ArrayMax, ElementSize ));
}

void FArray::Remove( INT Index, INT Count, INT ElementSize )
{
	guardSlow(FArray::Remove);
	if( Count )
	{
		appMemmove
		(
			(BYTE*)Data + (Index      ) * ElementSize,
			(BYTE*)Data + (Index+Count) * ElementSize,
			(ArrayNum - Index - Count ) * ElementSize
		);
		ArrayNum -= Count;
		if
		(	(3*ArrayNum<2*ArrayMax || (ArrayMax-ArrayNum)*ElementSize>=16384)
		&&	(ArrayMax-ArrayNum>64 || ArrayNum==0) )
		{
			ArrayMax = ArrayNum;
			Realloc( ElementSize );
		}
	}
	checkSlow(ArrayNum>=0);
	checkSlow(ArrayMax>=ArrayNum);
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FString implementation.
-----------------------------------------------------------------------------*/

FString FString::Chr( TCHAR Ch )
{
	guardSlow(FString::Chr);
	TCHAR Temp[2]={Ch,0};
	return FString(Temp);
	unguardSlow;
}

FString FString::LeftPad( INT ChCount )
{
	guardSlow(FString::LeftPad);
	INT Pad = ChCount - Len();
	if( Pad > 0 )
	{
		TCHAR* Ch = (TCHAR*)appAlloca((Pad+1)*sizeof(TCHAR));
		INT i;
		for( i=0; i<Pad; i++ )
			Ch[i] = ' ';
		Ch[i] = 0;
		return FString(Ch) + *this;
	}
	else return *this;
	unguardSlow;
}
FString FString::RightPad( INT ChCount )
{
	guardSlow(FString::RightPad);
	INT Pad = ChCount - Len();
	if( Pad > 0 )
	{
		TCHAR* Ch = (TCHAR*)appAlloca((Pad+1)*sizeof(TCHAR));
		INT i;
		for( i=0; i<Pad; i++ )
			Ch[i] = ' ';
		Ch[i] = 0;
		return *this + FString(Ch);
	}
	else return *this;
	unguardSlow;
}

FString FString::ExtractFilename()
{
	guardSlow(FString::ExtractFilename);

	INT Pos = InStr(PATH_SEPARATOR, 1);
	if( Pos != -1 )
		return Mid( Pos+1, Len()-Pos-1 );

	Pos = InStr(TEXT("/"), 1);
	if( Pos != -1 )
		return Mid( Pos+1, Len()-Pos-1 );
	
	Pos = InStr(TEXT(":"), 1);
	if( Pos != -1 )
		return Mid( Pos+1, Len()-Pos-1 );

	return *this;

	unguardSlow;
}

UBOOL FString::IsNumeric()
{
	guardSlow(FString::IsNumeric);

	if ( Len() == 0 )
		return 0;

	TCHAR c;
	bool dot = false, dash = false;
	for ( INT i = 0; i < Len(); i++ )
	{
		c = (*this)(i);
		if ( c == '.' )
		{
			if ( dot )
				return 0;
			else dot = true;
		}
		else if ( c == '-' )
		{
			if ( dash )
				return 0;
			else dash = true;
		}
		else if ( !appIsDigit(c) )
			return 0;
	}

	return 1;

	unguardSlow;
}

UBOOL FString::EndsInDigit() //amb
{
	guardSlow(FString::EndsInDigit);
    if (Len() == 0)
        return 0;
    else 
        return (appIsDigit ((*this)(Len()-1))); // gam
	unguardSlow;
}
FString::FString( BYTE Arg, INT Digits )
: TArray<TCHAR>()
{
}
FString::FString( SBYTE Arg, INT Digits )
: TArray<TCHAR>()
{
}
FString::FString( _WORD Arg, INT Digits )
: TArray<TCHAR>()
{
}
FString::FString( SWORD Arg, INT Digits )
: TArray<TCHAR>()
{
}
FString::FString( INT Arg, INT Digits )
: TArray<TCHAR>()
{
}
FString::FString( DWORD Arg, INT Digits )
: TArray<TCHAR>()
{
}
FString::FString( FLOAT Arg, INT Digits, INT RightDigits, UBOOL LeadZero )
: TArray<TCHAR>()
{
}
#ifndef __PSX2_EE__
FString::FString( DOUBLE Arg, INT Digits, INT RightDigits, UBOOL LeadZero )
: TArray<TCHAR>()
{
}
#endif
FString FString::Printf( const TCHAR* Fmt, ... )
{
	TCHAR TempStr[4096];
	GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), Fmt, Fmt );
	return FString(TempStr);
}
CORE_API FArchive& operator<<( FArchive& Ar, FString& A )
{
	guard(FString<<);
	A.CountBytes( Ar );
	INT SaveNum = appIsPureAnsi(*A) ? A.Num() : -A.Num();
	Ar << AR_INDEX(SaveNum);
	if( Ar.IsLoading() )
	{
		// Protect against network packets allocating too much memory
		if( Ar.ArMaxSerializeSize && Abs(SaveNum) > Ar.ArMaxSerializeSize )
		{
			Ar.ArIsError = 1;
			Ar.ArIsCriticalError = 1;
			return Ar;
		}

		A.ArrayMax = A.ArrayNum = Abs(SaveNum);
		A.Realloc( sizeof(TCHAR) );
		if( SaveNum>=0 )
			for( INT i=0; i<A.Num(); i++ )
				{ANSICHAR ACh; Ar << *(BYTE*)&ACh; A(i)=FromAnsi(ACh);}
		else
			for( INT i=0; i<A.Num(); i++ )
				{UNICHAR UCh; Ar << UCh; A(i)=FromUnicode(UCh);}
		if( Ar.IsLoading() && A.Num()==1 )
			A.Empty();
	}
	else
	{
		if( SaveNum>=0 )
			for( INT i=0; i<A.Num(); i++ )
				{ANSICHAR ACh=ToAnsi(A(i)); Ar << *(BYTE*)&ACh;}
		else
			for( INT i=0; i<A.Num(); i++ )
				{UNICHAR UCh=ToUnicode(A(i)); Ar << UCh;}
	}
	return Ar;
	unguard;
}

/*-----------------------------------------------------------------------------
	String functions.
-----------------------------------------------------------------------------*/

//
// Returns whether the string is pure ANSI.
//
CORE_API UBOOL appIsPureAnsi( const TCHAR* Str )
{
#if UNICODE
	for( ; *Str; Str++ )
		if( *Str<0 || *Str>0xff )
			return 0;
#endif
	return 1;
}

//
// Failed assertion handler.
//warning: May be called at library startup time.
//
CORE_API void VARARGS appFailAssert( const ANSICHAR* Expr, const ANSICHAR* File, INT Line )
{
	appErrorf( TEXT("Assertion failed: %s [File:%s] [Line: %i]"), appFromAnsi(Expr), appFromAnsi(File), Line );
}

//
// Gets the extension of a file, such as "PCX".  Returns NULL if none.
// string if there's no extension.
//
CORE_API const TCHAR* appFExt( const TCHAR* fname )
{
	guard(appFExt);

	if( appStrstr(fname,TEXT(":")) )
		fname = appStrstr(fname,TEXT(":"))+1;

	while( appStrstr(fname,TEXT("/")) )
		fname = appStrstr(fname,TEXT("/"))+1;

	while( appStrstr(fname,TEXT(".")) )
		fname = appStrstr(fname,TEXT("."))+1;

	return fname;
	unguard;
}

//
// Get a static result string.
//
CORE_API TCHAR* appStaticString1024()
{
	guard(appStaticString1024);
	static TCHAR Results[256][STATICSTRINGLENGTH];
	static INT Count=0;
	TCHAR* Result = Results[Count++ & 255];
	*Result = 0;
	return Result;
	unguard;
}

//
// Get an ANSI static string.
//
CORE_API ANSICHAR* appAnsiStaticString1024()
{
	return (ANSICHAR*)appStaticString1024();
}

//
// Find string in string, case insensitive, requires non-alphanumeric lead-in.
//
const TCHAR* appStrfind( const TCHAR* Str, const TCHAR* Find )
{
	guard(appStrfind);	
	UBOOL Alnum  = 0;
	TCHAR f      = (*Find<'a' || *Find>'z') ? (*Find) : (*Find+'A'-'a');
	INT   Length = appStrlen(Find++)-1;
	TCHAR c      = *Str++;
	while( c )
	{
		if( c>='a' && c<='z' )
			c += 'A'-'a';
		if( !Alnum && c==f && !appStrnicmp(Str,Find,Length) )
			return Str-1;
		Alnum = (c>='A' && c<='Z') || (c>='0' && c<='9');
		c = *Str++;
	}
	return NULL;
	unguard;
}

//
// Returns a certain number of spaces.
// Only one return value is valid at a time.
//
const TCHAR* appSpc( INT Num )
{
	guard(spc);
	static TCHAR Spacing[256];
	static INT OldNum=-1;
	if( Num != OldNum )
	{
		for( OldNum=0; OldNum<Num; OldNum++ )
			Spacing[OldNum] = ' ';
		Spacing[Num] = 0;
	}
	return Spacing;
	unguard;
}

// 
// Trim spaces from an asciiz string by zeroing them.
//
void appTrimSpaces( ANSICHAR* String )
{		
	guard(appTrimSpaces);
	// Find 0 terminator.
	INT t=0;
	while( (String[t]!=0 ) && (t< 1024) ) t++;
	if (t>0) t--;
	// Zero trailing spaces.
	while( (String[t]==32) && (t>0) )
	{
		String[t]=0;
		t--;
	}
	unguard;
}

// gam ---
INT appStrPrefix( const TCHAR* Str, const TCHAR* Prefix )
{
    for(;;)
    {
        if( !*Prefix )
            return 0;
            
        TCHAR A = appToUpper(*Str);
        TCHAR B = appToUpper(*Prefix);
        
        if( A != B )
            return( B - A );

        Str++;
        Prefix++;
    }
}
// --- gam

/*-----------------------------------------------------------------------------
	Memory functions.
-----------------------------------------------------------------------------*/

//
// Memory functions.
//
CORE_API void appMemswap( void* Ptr1, void* Ptr2, DWORD Size )
{
	void* Temp = appAlloca(Size);
	appMemcpy( Temp, Ptr1, Size );
	appMemcpy( Ptr1, Ptr2, Size );
	appMemcpy( Ptr2, Temp, Size );
}

/*-----------------------------------------------------------------------------
	CRC functions.
-----------------------------------------------------------------------------*/

// CRC 32 polynomial.
#define CRC32_POLY 0x04c11db7
CORE_API DWORD GCRCTable[256];

//
// CRC32 computer based on CRC32_POLY.
//
DWORD appMemCrc( const void* InData, INT Length, DWORD CRC )
{
	guardSlow(appMemCrc);
	BYTE* Data = (BYTE*)InData;
	CRC = ~CRC;
	for( INT i=0; i<Length; i++ )
		CRC = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ Data[i]];
	return ~CRC;
	unguardSlow;
}

//
// String CRC.
//
DWORD appStrCrc( const TCHAR* Data )
{
	guardSlow(appStrCrc);
	INT Length = appStrlen( Data );
	DWORD CRC = 0xFFFFFFFF;
	for( INT i=0; i<Length; i++ )
	{
		TCHAR C   = Data[i];
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CL];;
#if UNICODE
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CH];;
#endif
	}
	return ~CRC;
	unguardSlow;
}

//
// String CRC, case insensitive.
//
DWORD appStrCrcCaps( const TCHAR* Data )
{
	guardSlow(appStrCrcCaps);
	INT Length = appStrlen( Data );
	DWORD CRC = 0xFFFFFFFF;
	for( INT i=0; i<Length; i++ )
	{
		TCHAR C   = appToUpper(Data[i]);
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CL];
#if UNICODE
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CH];
#endif
	}
	return ~CRC;
	unguardSlow;
}

//
// Returns smallest N such that (1<<N)>=Arg.
// Note: appCeilLogTwo(0)=0 because (1<<0)=1 >= 0.
//
static BYTE GLogs[257];
CORE_API BYTE appCeilLogTwo( DWORD Arg )
{
	if( --Arg == MAXDWORD )
		return 0;
	BYTE Shift = Arg<=0x10000 ? (Arg<=0x100?0:8) : (Arg<=0x1000000?16:24);
	return Shift + GLogs[Arg>>Shift];
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

//
// MD5 initialization.  Begins an MD5 operation, writing a new context.
//
CORE_API void appMD5Init( FMD5Context* context )
{
	context->count[0] = context->count[1] = 0;
	// Load magic initialization constants.
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
    appMemzero(context->buffer, sizeof (context->buffer));
}

//
// MD5 block update operation.  Continues an MD5 message-digest operation,
// processing another message block, and updating the context.
//
CORE_API void appMD5Update( FMD5Context* context, BYTE* input, INT inputLen )
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
		appMemcpy( &context->buffer[index], input, partLen );
		appMD5Transform( context->state, context->buffer );
		for (i = partLen; i + 63 < inputLen; i += 64)
			appMD5Transform( context->state, &input[i] );
		index = 0;
	} else
		i = 0;

	// Buffer remaining input.
	appMemcpy( &context->buffer[index], &input[i], inputLen-i );
}

//
// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
// Digest is 16 BYTEs.
//
CORE_API void appMD5Final ( BYTE* digest, FMD5Context* context )
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
	appMemset( context, 0, sizeof(*context) );
}

//
// MD5 basic transformation. Transforms state based on block.
//
CORE_API void appMD5Transform( DWORD* state, BYTE* block )
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
	appMemset( x, 0, sizeof(x) );
}

//
// Encodes input (DWORD) into output (BYTE).
// Assumes len is a multiple of 4.
//
CORE_API void appMD5Encode( BYTE* output, DWORD* input, INT len )
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
CORE_API void appMD5Decode( DWORD* output, BYTE* input, INT len )
{
	INT i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((DWORD)input[j]) | (((DWORD)input[j+1]) << 8) |
		(((DWORD)input[j+2]) << 16) | (((DWORD)input[j+3]) << 24);
}

/*-----------------------------------------------------------------------------
	Exceptions.
-----------------------------------------------------------------------------*/

//
// Throw a string exception with a message.
//
CORE_API void VARARGS appThrowf( const TCHAR* Fmt, ... )
{
	static TCHAR TempStr[4096];
	GET_VARARGS(TempStr,ARRAY_COUNT(TempStr),Fmt,Fmt);
	throw( TempStr );
}

/*-----------------------------------------------------------------------------
	Parameter parsing.
-----------------------------------------------------------------------------*/

//
// Get a string from a text string.
//
CORE_API UBOOL Parse
(
	const TCHAR* Stream, 
	const TCHAR* Match,
	TCHAR*		 Value,
	INT			 MaxLen,
	TCHAR*		 AllowedChars
)
{
	guard(ParseString);

	const TCHAR* Found = appStrfind(Stream,Match);
	const TCHAR* Start;

	if( Found )
	{
		Start = Found + appStrlen(Match);
		if( *Start == '\x22' )
		{
			// Quoted string with spaces.		
			const TCHAR* Pos = Start + 1;
			INT i;
			for( i=0;i<MaxLen-1 && *Pos; i++,Pos++ )
			{
				if( *Pos == '\x22' )
					break;
				if( *Pos == '\\' && (*(Pos+1)=='\\' || *(Pos+1)=='\x22') )
				{
                    Pos++;
					if( !*Pos )
						break;
				}
				Value[i] = *Pos;
			}
			Value[i]=0;
		}
		else
		{
			// Non-quoted string without spaces.
			appStrncpy( Value, Start, MaxLen );
			Value[MaxLen-1]=0;
			TCHAR* Temp;
			if ( AllowedChars )
			{
				Temp = appStrfind( AllowedChars, TEXT(" ") ) == NULL ? appStrstr( Value, TEXT(" ")) : NULL; if (Temp) *Temp = 0;
				Temp = appStrfind( AllowedChars, TEXT("\r") ) == NULL ? appStrstr( Value, TEXT("\r") ) : NULL; if( Temp ) *Temp=0;
				Temp = appStrfind( AllowedChars, TEXT("\n") ) == NULL ? appStrstr( Value, TEXT("\n") ) : NULL; if( Temp ) *Temp=0;
				Temp = appStrfind( AllowedChars, TEXT("\t") ) == NULL ? appStrstr( Value, TEXT("\t") ) : NULL; if( Temp ) *Temp=0;
				Temp = appStrfind( AllowedChars, TEXT(",") ) == NULL ? appStrstr( Value, TEXT(",") ) : NULL; if( Temp ) *Temp=0;
			}
			else
			{
				Temp = appStrstr( Value, TEXT(" ") ); if( Temp ) *Temp=0;
				Temp = appStrstr( Value, TEXT("\r") ); if( Temp ) *Temp=0;
				Temp = appStrstr( Value, TEXT("\n") ); if( Temp ) *Temp=0;
				Temp = appStrstr( Value, TEXT("\t") ); if( Temp ) *Temp=0;
				Temp = appStrstr( Value, TEXT(",")  ); if( Temp ) *Temp=0;
			}
		}
		return 1;
	}
	else return 0;
	unguard;
}

//
// See if a command-line parameter exists in the stream.
//
UBOOL CORE_API ParseParam( const TCHAR* Stream, const TCHAR* Param )
{
	guard(GetParam);
	const TCHAR* Start = Stream;
	if( *Stream )
		while( (Start=appStrfind(Start+1,Param)) != NULL )
			if( Start>Stream && (Start[-1]=='-' || Start[-1]=='/') )
				return 1;
	return 0;
	unguard;
}

// 
// Parse a string.
//
UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, FString& Value, TCHAR* AllowedChars )
{
	guard(FString::Parse);
	TCHAR Temp[4096]=TEXT("");
	if( ::Parse( Stream, Match, Temp, ARRAY_COUNT(Temp), AllowedChars ) )
	{
		Value = Temp;
		return 1;
	}
	else return 0;
	unguard;
}

//
// Parse a quadword.
//
UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, QWORD& Value )
{
	guard(ParseQWORD);
	return Parse( Stream, Match, *(SQWORD*)&Value );
	unguard;
}

//
// Parse a signed quadword.
//
UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, SQWORD& Value )
{
	guard(ParseSQWORD);
	TCHAR Temp[4096]=TEXT(""), *Ptr=Temp;
	if( ::Parse( Stream, Match, Temp, ARRAY_COUNT(Temp) ) )
	{
		Value = 0;
		UBOOL Negative = (*Ptr=='-');
		Ptr += Negative;
		while( *Ptr>='0' && *Ptr<='9' )
			Value = Value*10 + *Ptr++ - '0';
		if( Negative )
			Value = -Value;
		return 1;
	}
	else return 0;
	unguard;
}

//
// Get an object from a text stream.
//
CORE_API UBOOL ParseObject( const TCHAR* Stream, const TCHAR* Match, UClass* Class, UObject*& DestRes, UObject* InParent )
{
	guard(ParseUObject);
	TCHAR TempStr[256];
	if( !Parse( Stream, Match, TempStr, NAME_SIZE ) )
	{
		return 0;
	}
	else if( appStricmp(TempStr,TEXT("NONE"))==0 )
	{
		DestRes = NULL;
		return 1;
	}
	else
	{
		// Look this object up.
		UObject* Res;
		Res = UObject::StaticFindObject( Class, InParent, TempStr );
		if( !Res )
			return 0;
		DestRes = Res;
		return 1;
	}
	unguard;
}

//
// Get a name.
//
CORE_API UBOOL Parse
(
	const TCHAR* Stream, 
	const TCHAR* Match, 
	FName& Name
)
{
	guard(ParseFName);
	TCHAR TempStr[NAME_SIZE];

	if( !Parse(Stream,Match,TempStr,NAME_SIZE) )
		return 0;
	Name = FName( TempStr );

	return 1;
	unguard;
}

//
// Get a DWORD.
//
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, DWORD& Value )
{
	guard(ParseDWORD);

	const TCHAR* Temp = appStrfind(Stream,Match);
	TCHAR* End;
	if( Temp==NULL )
		return 0;
	Value = appStrtoi( Temp + appStrlen(Match), &End, 10 );

	return 1;
	unguard;
}

//
// Get a byte.
//
UBOOL CORE_API Parse( const TCHAR* Stream, const TCHAR* Match, BYTE& Value )
{
	guard(ParseBYTE);

	const TCHAR* Temp = appStrfind(Stream,Match);
	if( Temp==NULL )
		return 0;
	Temp += appStrlen( Match );
	Value = (BYTE)appAtoi( Temp );
	return Value!=0 || appIsDigit(Temp[0]);

	unguard;
}

//
// Get a signed byte.
//
UBOOL CORE_API Parse( const TCHAR* Stream, const TCHAR* Match, SBYTE& Value )
{
	guard(ParseCHAR);
	const TCHAR* Temp = appStrfind(Stream,Match);
	if( Temp==NULL )
		return 0;
	Temp += appStrlen( Match );
	Value = appAtoi( Temp );
	return Value!=0 || appIsDigit(Temp[0]);
	unguard;
}

//
// Get a word.
//
UBOOL CORE_API Parse( const TCHAR* Stream, const TCHAR* Match, _WORD& Value )
{
	guard(ParseWORD);
	const TCHAR* Temp = appStrfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Temp += appStrlen( Match );
	Value = (_WORD)appAtoi( Temp );
	return Value!=0 || appIsDigit(Temp[0]);
	unguard;
}

//
// Get a signed word.
//
UBOOL CORE_API Parse( const TCHAR* Stream, const TCHAR* Match, SWORD& Value )
{
	guard(ParseSWORD);
	const TCHAR* Temp = appStrfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Temp += appStrlen( Match );
	Value = (SWORD)appAtoi( Temp );
	return Value!=0 || appIsDigit(Temp[0]);
	unguard;
}

//
// Get a floating-point number.
//
UBOOL CORE_API Parse( const TCHAR* Stream, const TCHAR* Match, FLOAT& Value )
{
	guard(ParseFLOAT);
	const TCHAR* Temp = appStrfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Value = appAtof( Temp+appStrlen(Match) );
	return 1;
	unguard;
}

//
// Get a signed double word.
//
UBOOL CORE_API Parse( const TCHAR* Stream, const TCHAR* Match, INT& Value )
{
	guard(ParseINT);
	const TCHAR* Temp = appStrfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Value = appAtoi( Temp + appStrlen(Match) );
	return 1;
	unguard;
}

//
// Get a boolean value.
//
UBOOL CORE_API ParseUBOOL( const TCHAR* Stream, const TCHAR* Match, UBOOL& OnOff )
{
	guard(ParseUBOOL);
	TCHAR TempStr[16];
	if( Parse( Stream, Match, TempStr, 16 ) )
	{
		OnOff
		=	!appStricmp(TempStr,TEXT("On"))
		||	!appStricmp(TempStr,TEXT("True"))
		||	!appStricmp(TempStr,GTrue)
		||	!appStricmp(TempStr,TEXT("1"));
		return 1;
	}
	else return 0;
	unguard;
}

//
// Get a globally unique identifier.
//
CORE_API UBOOL Parse( const TCHAR* Stream, const TCHAR* Match, class FGuid& Guid )
{
	guard(ParseGUID);

	TCHAR Temp[256];
	if( !Parse( Stream, Match, Temp, ARRAY_COUNT(Temp) ) )
		return 0;

	Guid.A = Guid.B = Guid.C = Guid.D = 0;
	if( appStrlen(Temp)==32 )
	{
		TCHAR* End;
		Guid.D = appStrtoi( Temp+24, &End, 16 ); Temp[24]=0;
		Guid.C = appStrtoi( Temp+16, &End, 16 ); Temp[16]=0;
		Guid.B = appStrtoi( Temp+8,  &End, 16 ); Temp[8 ]=0;
		Guid.A = appStrtoi( Temp+0,  &End, 16 ); Temp[0 ]=0;
	}
	return 1;

	unguard;
}

//
// Sees if Stream starts with the named command.  If it does,
// skips through the command and blanks past it.  Returns 1 of match,
// 0 if not.
//
CORE_API UBOOL ParseCommand
(
	const TCHAR** Stream, 
	const TCHAR*  Match
)
{
	guard(ParseCommand);

	while( (**Stream==' ')||(**Stream==9) )
		(*Stream)++;

	if( appStrnicmp(*Stream,Match,appStrlen(Match))==0 )
	{
		*Stream += appStrlen(Match);
		if( !appIsAlnum(**Stream) )
		{
			while ((**Stream==' ')||(**Stream==9)) (*Stream)++;
			return 1; // Success.
		}
		else
		{
			*Stream -= appStrlen(Match);
			return 0; // Only found partial match.
		}
	}
	else return 0; // No match.
	unguard;
}

//
// Get next command.  Skips past comments and cr's.
//
CORE_API void ParseNext( const TCHAR** Stream )
{
	guard(ParseNext);

	// Skip over spaces, tabs, cr's, and linefeeds.
	SkipJunk:
	while( **Stream==' ' || **Stream==9 || **Stream==13 || **Stream==10 )
		++*Stream;

	if( **Stream==';' )
	{
		// Skip past comments.
		while( **Stream!=0 && **Stream!=10 && **Stream!=13 )
			++*Stream;
		goto SkipJunk;
	}

	// Upon exit, *Stream either points to valid Stream or a nul.
	unguard;
}

//
// Grab the next space-delimited string from the input stream.
// If quoted, gets entire quoted string.
//
CORE_API UBOOL ParseToken( const TCHAR*& Str, TCHAR* Result, INT MaxLen, UBOOL UseEscape )
{
	guard(ParseToken);
	INT Len=0;

	// Skip spaces and tabs.
	while( *Str==' ' || *Str==9 )
		Str++;
	if( *Str == 34 )
	{
		// Get quoted string.
		Str++;
		while( *Str && *Str!=34 && (Len+1)<MaxLen )
		{
			TCHAR c = *Str++;
			if( c=='\\' && UseEscape )
			{
				// Get escape.
				c = *Str++;
				if( !c )
					break;
			}
			if( (Len+1)<MaxLen )
				Result[Len++] = c;
		}
		if( *Str==34 )
			Str++;
	}
	else
	{
		// Get unquoted string.
		for( ; *Str && *Str!=' ' && *Str!=9; Str++ )
			if( (Len+1)<MaxLen )
				Result[Len++] = *Str;
	}
	Result[Len]=0;
	return Len!=0;
	unguard;
}
CORE_API UBOOL ParseToken( const TCHAR*& Str, FString& Arg, UBOOL UseEscape )
{
	TCHAR Buffer[1024];
	if( ParseToken( Str, Buffer, ARRAY_COUNT(Buffer), UseEscape ) )
	{
		Arg = Buffer;
		return 1;
	}
	return 0;
}
CORE_API FString ParseToken( const TCHAR*& Str, UBOOL UseEscape )
{
	TCHAR Buffer[1024];
	if( ParseToken( Str, Buffer, ARRAY_COUNT(Buffer), UseEscape ) )
		return Buffer;
	else
		return TEXT("");
}

//
// Get a line of Stream (everything up to, but not including, CR/LF.
// Returns 0 if ok, nonzero if at end of stream and returned 0-length string.
//
CORE_API UBOOL ParseLine
(
	const TCHAR**	Stream,
	TCHAR*			Result,
	INT				MaxLen,
	UBOOL			Exact
)
{
	guard(ParseLine);
	UBOOL GotStream=0;
	UBOOL IsQuoted=0;
	UBOOL Ignore=0;

	checkSlow(*Stream);

	*Result=0;
	while( **Stream!=0 && **Stream!=10 && **Stream!=13 && --MaxLen>0 )
	{
		// Start of comments.
		if( !IsQuoted && !Exact && (*Stream)[0]=='/' && (*Stream)[1]=='/' )
			Ignore = 1;
		
		// Command chaining.
		if( !IsQuoted && !Exact && **Stream=='|' )
			break;

		// Check quoting.
		IsQuoted = IsQuoted ^ (**Stream==34);
		GotStream=1;

		// Got stuff.
		if( !Ignore )
			*(Result++) = *((*Stream)++);
		else
			(*Stream)++;
	}
	if( Exact )
	{
		// Eat up exactly one CR/LF.
		if( **Stream == 13 )
			(*Stream)++;
		if( **Stream == 10 )
			(*Stream)++;
	}
	else
	{
		// Eat up all CR/LF's.
		while( **Stream==10 || **Stream==13 || **Stream=='|' )
			(*Stream)++;
	}
	*Result=0;
	return **Stream!=0 || GotStream;
	unguard;
}
CORE_API UBOOL ParseLine
(
	const TCHAR**	Stream,
	FString&		Result,
	UBOOL			Exact
)
{
	guard(ParseLine);
	TCHAR Temp[4096]=TEXT("");
	UBOOL Success = ParseLine( Stream, Temp, ARRAY_COUNT(Temp), Exact );
	Result = Temp;
	return Success;
	unguard;
}

/*----------------------------------------------------------------------------
	String substitution.
----------------------------------------------------------------------------*/

CORE_API FString appFormat( FString Src, const TMultiMap<FString,FString>& Map )
{
	guard(appFormat);
	FString Result;
	for( INT Toggle=0; ; Toggle^=1 )
	{
		INT Pos=Src.InStr(TEXT("%")), NewPos=Pos>=0 ? Pos : Src.Len();
		FString Str = Src.Left( NewPos );
		if( Toggle )
		{
			const FString* Ptr = Map.Find( Str );
			if( Ptr )
				Result += *Ptr;
			else if( NewPos!=Src.Len() )
				Result += US + TEXT("%") + Str + TEXT("%");
			else
				Result += US + TEXT("%") + Str;
		}
		else Result += Str;
		Src = Src.Mid( NewPos+1 );
		if( Pos<0 )
			break;
	}
	return Result;
	unguard;
}

/*----------------------------------------------------------------------------
	Localization.
----------------------------------------------------------------------------*/

CORE_API const TCHAR* Localize( const TCHAR* Section, const TCHAR* Key, const TCHAR* Package, const TCHAR* LangExt, UBOOL bQuiet )
{
	guard(Localize);

	TCHAR* Result = appStaticString1024();
	*Result=0;
	if( !GIsStarted || !GConfig )
	{
		appStrcpy( Result, Key );
		return Result;
	}
	TCHAR Filename[256];
	LangExt = LangExt ? LangExt : UObject::GetLanguage();

	appSprintf( Filename, TEXT("%s.%s"), Package, LangExt );
	if( !GConfig->GetString( Section, Key, Result, STATICSTRINGLENGTH, Filename ) )
	{
		if ( appStricmp(LangExt, TEXT("int")) )
		{
			appSprintf( Filename, TEXT("%s.int"), Package );

			// If it doesn't exist in the english version either, just ignore it -- rjp
			if ( !GConfig->GetString( Section, Key, Result, STATICSTRINGLENGTH, Filename ) )
				return Result;
		}

		if( !bQuiet )
			debugf( NAME_Localization, TEXT("No localization: %s.%s.%s (%s)"), Package, Section, Key, LangExt );

		if ( ParseParam(appCmdLine(),TEXT("SHOWLANGERROR")) )
            appSprintf( Result, TEXT("<?%s?%s.%s.%s?>"), LangExt, Package, Section, Key );
	}
	return Result;
	unguard;
}
// rjp -- To help with loading large localized arrays of structs
// This code should only be called for loading properties from localization files
CORE_API void LocalizeBig( const TCHAR* Section, const TCHAR* Key, const TCHAR* Package, const TCHAR* LangExt, UBOOL bQuiet, TCHAR* Result, INT Size )
{
	guard(Localize);
	if( !GIsStarted || !GConfig )
	{
		appStrcpy( Result, Key );
		return;
	}
	TCHAR Filename[256];
	LangExt = LangExt ? LangExt : UObject::GetLanguage();

	appSprintf( Filename, TEXT("%s.%s"), Package, LangExt );
	if( !GConfig->GetString( Section, Key, Result, Size, Filename ) )
	{
		// If we're using english, just ignore it
		if ( appStricmp(LangExt, TEXT("int")) == 0 )
			return;

		appSprintf( Filename, TEXT("%s.int"), Package );

		// If it doesn't exist in the english version either, just ignore it -- rjp
		if ( !GConfig->GetString( Section, Key, Result, Size, Filename ) )
			return;

		if( !bQuiet )
			debugf( NAME_Localization, TEXT("No localization: %s.%s.%s (%s)"), Package, Section, Key, LangExt );

		if ( ParseParam(appCmdLine(),TEXT("SHOWLANGERROR")) )
			appSprintf( Result, TEXT("<?%s?%s.%s.%s?>"), LangExt, Package, Section, Key );
	}
	unguard;
}

CORE_API const TCHAR* LocalizeError( const TCHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return Localize( TEXT("Errors"), Key, Package, LangExt );
}
CORE_API const TCHAR* LocalizeProgress( const TCHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return Localize( TEXT("Progress"), Key, Package, LangExt );
}
CORE_API const TCHAR* LocalizeQuery( const TCHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return Localize( TEXT("Query"), Key, Package, LangExt );
}
CORE_API const TCHAR* LocalizeGeneral( const TCHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return Localize( TEXT("General"), Key, Package, LangExt );
}

#if UNICODE
CORE_API const TCHAR* Localize( const ANSICHAR* Section, const ANSICHAR* Key, const TCHAR* Package, const TCHAR* LangExt, UBOOL bQuiet )
{
	return Localize( appFromAnsi(Section), appFromAnsi(Key), Package, LangExt, bQuiet );
}
CORE_API void LocalizeBig( const ANSICHAR* Section, const ANSICHAR* Key, const TCHAR* Package, const TCHAR* LangExt, UBOOL bQuiet, TCHAR* Result, INT Size )
{
	LocalizeBig( appFromAnsi(Section), appFromAnsi(Key), Package, LangExt, bQuiet, Result, Size );
}
CORE_API const TCHAR* LocalizeError( const ANSICHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return LocalizeError( appFromAnsi(Key), Package, LangExt );
}
CORE_API const TCHAR* LocalizeProgress( const ANSICHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return LocalizeProgress( appFromAnsi(Key), Package, LangExt );
}
CORE_API const TCHAR* LocalizeQuery( const ANSICHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return LocalizeQuery( appFromAnsi(Key), Package, LangExt );
}
CORE_API const TCHAR* LocalizeGeneral( const ANSICHAR* Key, const TCHAR* Package, const TCHAR* LangExt )
{
	return LocalizeGeneral( appFromAnsi(Key), Package, LangExt );
}
#endif

// gam ---
CORE_API UBOOL appIsOnConsole()
{
    if( appStricmp( UObject::GetLanguage(), TEXT("uce") ) == 0 ) // Unreal Championship English
        return( 1 );
    
    return( 0 );
}
// --- gam

/*-----------------------------------------------------------------------------
	High level file functions.
-----------------------------------------------------------------------------*/

//
// Update file modification time.
//
CORE_API UBOOL appUpdateFileModTime( TCHAR* Filename )
{
	guard(appUpdateFileModTime);
	FArchive* Ar = GFileManager->CreateFileWriter(Filename,FILEWRITE_Append,GNull);
	if( Ar )
	{
		delete Ar;
		return 1;
	}
	return 0;
	unguard;
}

//
// Load a binary file to a dynamic array.
//
CORE_API UBOOL appLoadFileToArray( TArray<BYTE>& Result, const TCHAR* Filename, FFileManager* FileManager, DWORD ReadFlags, UBOOL bLogErrors )
{
	guard(appLoadFileToArray);
	FArchive* Reader = FileManager->CreateFileReader( Filename, ReadFlags, bLogErrors ? GLog : NULL );
	if( !Reader )
		return 0;
	Result.Empty();
	Result.Add( Reader->TotalSize() );
	Reader->Serialize( &Result(0), Result.Num() );
	UBOOL Success = Reader->Close();
	delete Reader;
	return Success;
	unguard;
}

//
// Load a text file to an FString.
// Supports all combination of ANSI/Unicode files and platforms.
//
CORE_API UBOOL appLoadFileToString( FString& Result, const TCHAR* Filename, FFileManager* FileManager, DWORD ReadFlags, UBOOL bLogErrors )
{
	guard(appLoadFileToString);
	FArchive* Reader = FileManager->CreateFileReader( Filename, ReadFlags, bLogErrors ? GLog : NULL );
	if( !Reader )
		return 0;
	INT Size = Reader->TotalSize();
	TArray<ANSICHAR> Ch( Size+2 );
	Reader->Serialize( &Ch(0), Size );
	UBOOL Success = Reader->Close();
	delete Reader;
	Ch( Size+0 )=0;
	Ch( Size+1 )=0;
	TArray<TCHAR>& ResultArray = Result.GetCharArray();
	ResultArray.Empty();
	if( Size>=2 && !(Size&1) && (BYTE)Ch(0)==0xff && (BYTE)Ch(1)==0xfe )
	{
		// Unicode Intel byte order.
		ResultArray.Add( Size/sizeof(TCHAR) );
		for( INT i=0; i<ResultArray.Num()-1; i++ )
			ResultArray( i ) = FromUnicode( (_WORD)(ANSICHARU)Ch(i*2+2) + (_WORD)(ANSICHARU)Ch(i*2+3)*256 );
	}
	else if( Size>=2 && !(Size&1) && (BYTE)Ch(0)==0xfe && (BYTE)Ch(1)==0xff )
	{
		// Unicode non-Intel byte order.
		ResultArray.Add( Size/sizeof(TCHAR) );
		for( INT i=0; i<ResultArray.Num()-1; i++ )
			ResultArray( i ) = FromUnicode( (_WORD)(ANSICHARU)Ch(i*2+3) + (_WORD)(ANSICHARU)Ch(i*2+2)*256 );
	}
	else
	{
		// ANSI.
		ResultArray.Add( Size+1 );
		for( INT i=0; i<ResultArray.Num()-1; i++ )
			ResultArray( i ) = FromAnsi( Ch(i) );
	}
	ResultArray.Last() = 0;
	return Success;
	unguard;
}

//
// Save a binary array to a file.
//
CORE_API UBOOL appSaveArrayToFile( const TArray<BYTE>& Array, const TCHAR* Filename, FFileManager* FileManager, DWORD WriteFlags, UBOOL bLogErrors )
{
	guard(appSaveArrayToFile);
	FArchive* Ar = FileManager->CreateFileWriter( Filename, WriteFlags, bLogErrors ? GLog : NULL );
	if( !Ar )
		return 0;
	Ar->Serialize( const_cast<BYTE*>(&Array(0)), Array.Num() );
	delete Ar;
	return 1;
	unguard;
}

//
// Write the FString to a file.
// Supports all combination of ANSI/Unicode files and platforms.
//
CORE_API UBOOL appSaveStringToFile( const FString& String, const TCHAR* Filename, FFileManager* FileManager, DWORD WriteFlags, UBOOL bLogErrors )
{
	guard(appSaveStringToFile);
	if( !String.Len() )
		return 0;
	FArchive* Ar = FileManager->CreateFileWriter( Filename, WriteFlags, bLogErrors ? GLog : NULL );
	if( !Ar )
		return 0;
	UBOOL SaveAsUnicode=0, Success=1;
#if UNICODE
	for( INT i=0; i<String.Len(); i++ )
	{
		if( (*String)[i] != (TCHAR)(ANSICHARU)ToAnsi((*String)[i]) )
		{
			UNICHAR BOM = UNICODE_BOM;
			Ar->Serialize( &BOM, sizeof(BOM) );
			SaveAsUnicode = 1;
			break;
		}
	}
#endif
	if( SaveAsUnicode || sizeof(TCHAR)==1 )
	{
		Ar->Serialize( const_cast<TCHAR*>(*String), String.Len()*sizeof(TCHAR) );
	}
	else
	{
		TArray<ANSICHAR> AnsiBuffer(String.Len());
		for( INT i=0; i<String.Len(); i++ )
			AnsiBuffer(i) = ToAnsi((*String)[i]);
		Ar->Serialize( const_cast<ANSICHAR*>(&AnsiBuffer(0)), String.Len() );
	}
	delete Ar;
	if( !Success )
		GFileManager->Delete( Filename );
	return Success;
	unguard;
}

INT appCreateBitmap( const TCHAR* Pattern, INT Width, INT Height, DWORD* Data, FFileManager* FileManager )
{
	TCHAR File[1024];
	INT i=0;

	appSprintf( File, TEXT("%s.bmp"), Pattern);

	/*
	for( i=0; i<65536; i++ )
	{
		appSprintf( File, TEXT("%s%05i.bmp"), Pattern, i );
		if( FileManager->FileSize(File) < 0 )
			break;
	}
*/	
//	if( FileManager->FileSize(File)<0 )
	//{
		FArchive* Ar = FileManager->CreateFileWriter( File );
		if( Ar )
		{
			// Types.
			#if SUPPORTS_PRAGMA_PACK
			#pragma pack (push,1)
			#endif
			struct BITMAPFILEHEADER
			{
				_WORD   bfType GCC_PACK(1);
				DWORD   bfSize GCC_PACK(1);
				_WORD   bfReserved1 GCC_PACK(1); 
				_WORD   bfReserved2 GCC_PACK(1);
				DWORD   bfOffBits GCC_PACK(1);
			} FH; 
			struct BITMAPINFOHEADER
			{
				DWORD  biSize GCC_PACK(1); 
				INT    biWidth GCC_PACK(1);
				INT    biHeight GCC_PACK(1);
				_WORD  biPlanes GCC_PACK(1);
				_WORD  biBitCount GCC_PACK(1);
				DWORD  biCompression GCC_PACK(1);
				DWORD  biSizeImage GCC_PACK(1);
				INT    biXPelsPerMeter GCC_PACK(1); 
				INT    biYPelsPerMeter GCC_PACK(1);
				DWORD  biClrUsed GCC_PACK(1);
				DWORD  biClrImportant GCC_PACK(1); 
			} IH;
			#if SUPPORTS_PRAGMA_PACK
			#pragma pack (pop)
			#endif

			// File header.
			FH.bfType       = INTEL_ORDER16((_WORD) ('B' + 256*'M'));
			FH.bfSize       = INTEL_ORDER32((DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 3 * Width * Height));
			FH.bfReserved1  = INTEL_ORDER16((_WORD) 0);
			FH.bfReserved2  = INTEL_ORDER16((_WORD) 0);
			FH.bfOffBits    = INTEL_ORDER32((DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)));
			Ar->Serialize( &FH, sizeof(FH) );

			// Info header.
			IH.biSize               = INTEL_ORDER32((DWORD) sizeof(BITMAPINFOHEADER));
			IH.biWidth              = INTEL_ORDER32((DWORD) Width);
			IH.biHeight             = INTEL_ORDER32((DWORD) Height);
			IH.biPlanes             = INTEL_ORDER16((_WORD) 1);
			IH.biBitCount           = INTEL_ORDER16((_WORD) 24);
			IH.biCompression        = INTEL_ORDER32((DWORD) 0); //BI_RGB
			IH.biSizeImage          = INTEL_ORDER32((DWORD) Width * Height * 3);
			IH.biXPelsPerMeter      = INTEL_ORDER32((DWORD) 0);
			IH.biYPelsPerMeter      = INTEL_ORDER32((DWORD) 0);
			IH.biClrUsed            = INTEL_ORDER32((DWORD) 0);
			IH.biClrImportant       = INTEL_ORDER32((DWORD) 0);
			Ar->Serialize( &IH, sizeof(IH) );

			// Colors.
			for( INT i=Height-1; i>=0; i-- )
			{
				for( INT j=0; j<Width; j++ )
					Ar->Serialize( &Data[i*Width+j], 3 );
			}

			// Success.
			delete Ar;
		}
		else 
			return -1;
//	}
	//else 
		//return -1;

	// Success.
	return i;
}


/*-----------------------------------------------------------------------------
	Files.
-----------------------------------------------------------------------------*/

//
// Check a file's guid
//
UBOOL appCheckPackageFileGUID( const TCHAR* InFilename, const FGuid* InGuid )
{
	guard(appCheckPackageFileGUID);

    FArchive* Ar = GFileManager->CreateFileReader( InFilename, 0, GError );
	if( !Ar )
		return 0;

	// Read summary from file.
	FPackageFileSummary Summary;
	*Ar << Summary;

	Ar->Close();
	delete Ar;

	return Summary.Tag==PACKAGE_FILE_TAG && Summary.Guid==*InGuid;

	unguard;
}

DWORD appGetPackageFileFlags( const TCHAR* InFilename )
{
	FArchive* Ar = GFileManager->CreateFileReader( InFilename, 0, GError );
	if( !Ar )
		return 0;

	// Read summary from file.
	FPackageFileSummary Summary;
	*Ar << Summary;

	Ar->Close();
	delete Ar;

	return Summary.Tag==PACKAGE_FILE_TAG ? Summary.PackageFlags : 0;
}

static UBOOL CheckPotentialFileGuid( const TCHAR* InFilename, const FGuid* InGuid )
{
	guard(CheckGuid)

	// if we don't care about the guid, always go with this file	
	if( !InGuid )
		return 1;

	// if it's a .u file, we always go with the file we found.
	if( FString(InFilename).Right(2).Caps() == TEXT(".U") )
		return 1;

	// Read summary from to check package flags and GUID
	FArchive* Ar = GFileManager->CreateFileReader( InFilename, 0, GError );
	if( !Ar )
		return 1;
	FPackageFileSummary Summary;
	*Ar << Summary;
	Ar->Close();
	delete Ar;
	if( Summary.Tag!=PACKAGE_FILE_TAG )
		return 1;

	// if the file we found was 'official', go with the file we found.
	if( Summary.PackageFlags & PKG_Official )
		return 1;

	// otherwise, only go with this file if the guids match.
	return Summary.Guid==*InGuid;

	unguard;
}


//
// Find a file.
//
UBOOL appFindPackageFile( const TCHAR* InIn, const FGuid* Guid, TCHAR* Out, INT GenerationLevel )
{
	guard(appFindPackageFile);
	TCHAR Temp[256];
	TCHAR In[200];
	appStrncpy( In, InIn, ARRAY_COUNT(In) );	// only 200 chars, allow room for extra path stuff appened to filename.

	// Don't return it if it's a library.
	if( appStrlen(In)>appStrlen(DLLEXT) && appStricmp( In + appStrlen(In)-appStrlen(DLLEXT), DLLEXT )==0 )
		return 0;

	// If using non-default language, search for internationalized version.
	UBOOL International = (appStricmp(UObject::GetLanguage(),TEXT("int"))!=0);

	// Try file as specified.
	appStrcpy( Out, In );
	if( appStrstr(Out, TEXT("."))!= NULL && GFileManager->FileSize( Out ) >= 0 && CheckPotentialFileGuid(Out, Guid) )
		return 1;

	// Try all of the predefined paths.
	INT DoCd;
	for( DoCd=0; DoCd<(1+(GCdPath[0]!=0)); DoCd++ )
	{
		for( INT i=DoCd; i<GSys->Paths.Num()+(Guid!=NULL); i++ )
		{
			for( INT j=0; j<International+1; j++ )
			{
				// Get directory only.
				const TCHAR* Ext;
				*Temp = 0;
				if( DoCd )
				{
					appStrcat( Temp, GCdPath );
					appStrcat( Temp, TEXT("System") );
                    appStrcat( Temp, PATH_SEPARATOR );
				}
				if( i<GSys->Paths.Num() )
				{
					appStrcat( Temp, *GSys->Paths(i) );
					TCHAR* Ext2 = appStrstr(Temp,TEXT("*"));
					if( Ext2 )
						*Ext2++ = 0;
					Ext = Ext2;
					appStrcpy( Out, Temp );
					appStrcat( Out, In );
				}
				else
				{
					appStrcat( Temp, *GSys->CachePath );
					appStrcat( Temp, PATH_SEPARATOR );
					Ext = *GSys->CacheExt;
					appStrcpy( Out, Temp );
					appStrcat( Out, Guid->String() );

					if (GenerationLevel>0)
					{
						appStrcat(Out,TEXT("-"));
						appStrcat(Out,appItoa(GenerationLevel));
					}

				}
				// Check for file which includes the extension.
				UBOOL Found = 0;
				Found = (
							(!Ext||appStrlen(Out)>appStrlen(Ext)) &&
							(!Ext||!appStricmp(Ext, &Out[appStrlen(Out)-appStrlen(Ext)])) &&
							 GFileManager->FileSize(Out)>=0 &&
							 CheckPotentialFileGuid(Out, Guid)
						);
				if( !Found && Ext )
				{
					appStrcat( Out, TEXT(".") );
					if( International-j )
					{
						appStrcat( Out, UObject::GetLanguage() );
						appStrcat( Out, TEXT("_") );
					}
					appStrcat( Out, Ext+1 );
					Found = (GFileManager->FileSize( Out )>=0 && CheckPotentialFileGuid(Out, Guid));
				}
				if( Found )
				{
					if( i==GSys->Paths.Num() )
						appUpdateFileModTime( Out );
					return 1;
				}
			}
		}
	}

	// Try case-insensitive search.
	for( DoCd=0; DoCd<(1+(GCdPath[0]!=0)); DoCd++ )
	{
		for( INT i=0; i<GSys->Paths.Num()+(Guid!=NULL); i++ )
		{
			// Get directory only.
			const TCHAR* Ext;
			*Temp = 0;
			if( DoCd )
			{
				appStrcat( Temp, GCdPath );
				appStrcat( Temp, TEXT("System") );
                appStrcat( Temp, PATH_SEPARATOR );
			}
			if( i<GSys->Paths.Num() )
			{
				appStrcat( Temp, *GSys->Paths(i) );
				TCHAR* Ext2 = appStrstr(Temp,TEXT("*"));
				if( Ext2 )
					*Ext2++ = 0;
				Ext = Ext2;
				appStrcpy( Out, Temp );
				appStrcat( Out, In );
			}
			else
			{
				appStrcat( Temp, *GSys->CachePath );
				appStrcat( Temp, PATH_SEPARATOR );
				Ext = *GSys->CacheExt;
				appStrcpy( Out, Temp );
				appStrcat( Out, Guid->String() );
				appStrcat(Out,TEXT("-"));
				appStrcat(Out,appItoa(GenerationLevel));
			}

			// Find files.
			TCHAR Spec[256];
			*Spec = 0;
			TArray<FString> Files;
			appStrcpy( Spec, Temp );
			appStrcat( Spec, TEXT("*") );
			if( Ext )
				appStrcat( Spec, Ext );
			Files = GFileManager->FindFiles( Spec, 1, 0 );

			// Check for match.
			UBOOL Found = 0;
			TCHAR InExt[256];
			*InExt = 0;
			if( Ext )
			{
				appStrcpy( InExt, In );
				appStrcat( InExt, Ext );
			}
			for( INT j=0; Files.IsValidIndex(j); j++ )
			{
				if( (appStricmp( *(Files(j)), In )==0) ||
					(appStricmp( *(Files(j)), InExt)==0) )
				{
					appStrcpy( Out, Temp );
					appStrcat( Out, *(Files(j)));
					Found = (GFileManager->FileSize( Out )>=0 && CheckPotentialFileGuid(Out, Guid));
				}
			}
			if( Found )
			{
				debugf( TEXT("Case-insensitive search: %s -> %s"), In, Out );
				if( i==GSys->Paths.Num() )
					appUpdateFileModTime( Out );
				return 1;
			}
		}
	}

	// Not found.
	return 0;
	unguard;
}

//
// Create a temporary file.
//
CORE_API void appCreateTempFilename( const TCHAR* Path, TCHAR* Result256 )
{
	guard(appCreateTempFilename);
	static INT i=0;
	do
		appSprintf( Result256, TEXT("%s%04X.tmp"), Path, i++ );
	while( GFileManager->FileSize(Result256)>0 );
	unguard;
}

/*-----------------------------------------------------------------------------
	Init and Exit.
-----------------------------------------------------------------------------*/

//
// General initialization.
//
TCHAR GCmdLine[4096]=TEXT("");
CORE_API const TCHAR* appCmdLine()
{
	return GCmdLine;
}

CORE_API void appInit( const TCHAR* InPackage, const TCHAR* InCmdLine, FMalloc* InMalloc, FOutputDevice* InLog, FOutputDeviceError* InError, FFeedbackContext* InWarn, FFileManager* InFileManager, FConfigCache*(*ConfigFactory)(), UBOOL RequireConfig )
{
	guard(appInit);
	GFileManager = InFileManager;

	// Init CRC table.
    for( DWORD iCRC=0; iCRC<256; iCRC++ )
		for( DWORD c=iCRC<<24, j=8; j!=0; j-- )
			GCRCTable[iCRC] = c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);

	// Init log table.
	{for( INT i=0,e=-1,c=0; i<=256; i++ )
	{
		GLogs[i] = e+1;
		if( !i || ++c>=(1<<e) )
			c=0, e++;
	}}

	// Command line.
    #ifdef _XBOX // sjs - don't eat the cmdline

	#elif _MSC_VER
		if( *InCmdLine=='\"' )
		{
			InCmdLine++;
			while( *InCmdLine && *InCmdLine!='\"' )
				InCmdLine++;
			if( *InCmdLine )
				InCmdLine++;
		}
		while( *InCmdLine && *InCmdLine!=' ' )
			InCmdLine++;
		if( *InCmdLine )
			InCmdLine++;
	#endif

	appStrcpy( GCmdLine, InCmdLine );

	// Error history.
	appStrcpy( GErrorHist, TEXT("General protection fault!\r\n\r\nHistory: ") );

	// Subsystems.
	GLog         = InLog;
	GError       = InError;
	GWarn        = InWarn;

	// Memory allocator.
	GMalloc = InMalloc;
	GMalloc->Init();

	// Switch into executable's directory.
	GFileManager->Init(1);
	GFileManager->SetDefaultDirectory( appBaseDir() );

	// Handle command files.  Gets around Win9x command line length limitation. :(
	TCHAR	CommandFile[1024];
	if(Parse(appCmdLine(),TEXT("COMMAND="),CommandFile,ARRAY_COUNT(CommandFile)))
	{
		FString	Command;
		verify(appLoadFileToString(Command,CommandFile,GFileManager));
		appStrcpy(GCmdLine,*Command);
		GFileManager->Delete(CommandFile);
	}

	// Init names.
	FName::StaticInit();

	// Platform specific pre-init.
	appPlatformPreInit();

	// Command line.
	debugf( NAME_Init, TEXT("Version: %i (%i.%i)"), ENGINE_VERSION, PACKAGE_FILE_VERSION, PACKAGE_FILE_VERSION_LICENSEE ); // gam
	debugf( NAME_Init, TEXT("Compiled: %s %s"), appFromAnsi(__DATE__), appFromAnsi(__TIME__) );
	debugf( NAME_Init, TEXT("Command line: %s"), appCmdLine() );
	// Parameters.
	GIsStrict = ParseParam( appCmdLine(), TEXT("STRICT") );

	// make sure logfile notes if this is a platform-specific patch over the usual revision... --ryan.
	#ifdef PLATFORM_VERSION_BUMP
		#if MACOSX
		const TCHAR *platform = TEXT("MacOSX");
		#elif FREEBSD
		const TCHAR *platform = TEXT("FreeBSD");
		#elif LINUX && PLATFORM_64BITS
		const TCHAR *platform = TEXT("Linux64");
		#elif LINUX
		const TCHAR *platform = TEXT("Linux");
		#elif WIN64
		const TCHAR *platform = TEXT("Win64");
		#else
		#error Fill in your platform.
		#endif
		debugf( NAME_Init, TEXT("(This is %s patch version %d.%d)"), platform, ENGINE_VERSION, PLATFORM_VERSION_BUMP );
	#endif

    if( ParseParam( appCmdLine(), TEXT("MAKENAMES") ) ) // sjs
    {
        GTransientNaming = 1;
        debugf( NAME_Init, TEXT("Transient objects will be uniquely named."));
    }
    
    if( ParseParam( appCmdLine(), TEXT("SOAKING") ) ) // sjs
        GIsSoaking = 1;

    if( ParseParam( appCmdLine(), TEXT("SHIP") ) ) // gam
        GShowBuildLabel = 0;
	
	if ( !Parse(appCmdLine(), TEXT("-MOD="),GModName,256) )
	{
		// Ini.
		if( !Parse( appCmdLine(), TEXT("INI="), GIni, 1024 ) )
			appSprintf( GIni, TEXT("%s.ini"), InPackage );

		if( GFileManager->FileSize(GIni)<0 && RequireConfig )
		{
			// Create Package.ini from default.ini.
			FString S;
			if( !appLoadFileToString( S, TEXT("Default.ini"), GFileManager ) )
				appErrorf( LocalizeError(TEXT("MissingIni"),TEXT("Core")), "Default.ini" );
			appSaveStringToFile( S, GIni );
		}

		// User Ini.
		if( !Parse( appCmdLine(), TEXT("USERINI="), GUserIni, 1024 ) )
			appStrcpy( GUserIni, TEXT("User.ini") );
		if( GFileManager->FileSize(GUserIni)<0 && RequireConfig )
		{
			// Create User.ini from DefUser.ini.
			FString S;
			if( !appLoadFileToString( S, TEXT("DefUser.ini"), GFileManager ) )
				appErrorf( LocalizeError(TEXT("MisingIni"),TEXT("Core")), "DefUser.ini" );
			appSaveStringToFile( S, GUserIni );
		}
	}
	else
	{
		appSprintf(GModPath,TEXT("..\\%s"),GModName);
		appSprintf(GModDef,TEXT("%s\\UT2K4MOD.ini"),GModPath);

		if( GFileManager->FileSize(GModDef)<0)
			appErrorf( LocalizeError(TEXT("BadMod"), TEXT("Core")), GModName);

		appSprintf(GIni,TEXT("%s\\system\\%s.ini"),GModPath,GModName);

		if( GFileManager->FileSize(GIni)<0 && RequireConfig )
		{
			FConfigFile TmpINI;
			TmpINI.Read(TEXT("Default.ini"));
			TmpINI.Combine(*FString::Printf(TEXT("%s\\system\\Default.ini"),GModPath));
			TmpINI.Dirty=true;
			TmpINI.Write(GIni);		
		}
		
		appSprintf(GUserIni,TEXT("%s\\system\\%sUser.ini"),GModPath,GModName);

		if( GFileManager->FileSize(GUserIni)<0 && RequireConfig )
		{
			FConfigFile TmpINI;
			TmpINI.Read(TEXT("DefUser.ini"));
			TmpINI.Combine(*FString::Printf(TEXT("%s\\system\\DefUser.ini"),GModPath));
			TmpINI.Dirty=true;
			TmpINI.Write(GUserIni);		
		}
	}

	debugf( NAME_Init, TEXT("Character set: %s"), sizeof(TCHAR)==1 ? TEXT("ANSI") : TEXT("Unicode") );
	debugf( NAME_Init, TEXT("Base directory: %s"), appBaseDir() );
	debugf( NAME_Init, TEXT("Ini:%s   UserIni:%s"), GIni, GUserIni );

	// Init config.
	GConfig = ConfigFactory();
	GConfig->Init( GIni, GUserIni, RequireConfig );

	// Language.
	TCHAR Temp[256];
	if( GConfig->GetString( TEXT("Engine.Engine"), TEXT("Language"), Temp, ARRAY_COUNT(Temp) ) )
		UObject::SetLanguage( Temp );

    // gam ---
    if( !GConfig->GetString(TEXT("BuildVersion"), TEXT("Label"), Temp, ARRAY_COUNT(Temp), TEXT("Build.ini")) ) //amb
	    debugf( NAME_Warning, TEXT("Build label absent!") );
    else
    {
        if( !GIsEditor )
            appSprintf( GBuildLabel, TEXT("%s Build %s"), LocalizeGeneral( TEXT("Abbreviation"), appPackage() ), Temp );
        else
            appSprintf( GBuildLabel, TEXT("Build %s"), Temp );

    	debugf( NAME_Init, TEXT("Build label: %s"), GBuildLabel );
    }
    // --- gam

	// Script profiling.
	if( !Parse( appCmdLine(), TEXT("PROFILINGSTACKDEPTH="), GMaxScriptProfilingStackDepth ) )
		GMaxScriptProfilingStackDepth = 1000000;

	// Object initialization.
	UObject::StaticInit();

	// Memory initalization.
	GMem.Init( 65536 );

	// Platform specific init.
	appPlatformInit();

	check(gethostbyname_mutex == NULL);
	gethostbyname_mutex = new FCriticalSection;

	unguard;
}

//
// Pre-shutdown.
// Called from within guarded exit code, only during non-error exits.
//
CORE_API void appPreExit()
{
	guard(appPreExit);

	debugf( NAME_Exit, TEXT("Preparing to exit.") );
	appPlatformPreExit();
	GMem.Exit();
	UObject::StaticExit();

	unguard;
}

//
// Shutdown.
// Called outside guarded exit code, during all exits (including error exits).
//
void appExit()
{
	guard(appExit);

	debugf( NAME_Exit, TEXT("Exiting.") );

	INT GByte =  GFileManagerBytesRead / 1024 / 1024 / 1024,
		MByte =	(GFileManagerBytesRead - GByte * 1024 * 1024 * 1024) / 1024 / 1024,
		KByte = (GFileManagerBytesRead - GByte * 1024 * 1024 * 1024 - MByte * 1024 * 1024 ) / 1024,
		Bytes =  GFileManagerBytesRead - GByte * 1024 * 1024 * 1024 - MByte * 1024 * 1024 - KByte * 1024;
	
	debugf( NAME_Log, TEXT("FileManager: Reading %i GByte %i MByte %i KByte %i Bytes from HD took %f seconds (%f reading, %f seeking)."), GByte, MByte, KByte, Bytes, GFileManagerLoadingTime + GFileManagerSeekTime, GFileManagerLoadingTime, GFileManagerSeekTime );
	debugf( NAME_Log, TEXT("FileManager: %f seconds spent with misc. duties"), GFileManagerMiscTime );

	appPlatformExit();
	if( GConfig )
	{
		GConfig->Exit();
		delete GConfig;
		GConfig = NULL;
	}
	FName::StaticExit();
#ifndef _XBOX
	if( !GIsCriticalError )
		GMalloc->DumpAllocs();
#endif

	delete gethostbyname_mutex;

	unguard;
}

/*-----------------------------------------------------------------------------
	String conversion.
-----------------------------------------------------------------------------*/
// TCHAR to ANSICHAR.
CORE_API const ANSICHAR* appToAnsi( const TCHAR* Str, ANSICHAR* ACh, INT maxlen )
{
#if UNICODE
	guard(appToAnsi);
	if( !Str )
		return NULL;
	ANSICHAR* InCh = ACh;
	if( !ACh )
	{
		ACh = appAnsiStaticString1024();
		maxlen = Min(maxlen, STATICSTRINGLENGTH);
	}

	INT Count;
	for( Count=0; ( (InCh && maxlen == STATICSTRINGLENGTH) || Count < maxlen-1) && Str[Count]; Count++ )
		ACh[Count] = ToAnsi( Str[Count] );
	ACh[Count] = 0;
	return ACh;
	unguard;
#else
	return Str;
#endif
}

// TCHAR to UNICHAR.
CORE_API const UNICHAR* appToUnicode( const TCHAR* Str, UNICHAR* UCh, INT maxlen )
{
#if UNICODE
	return Str;
#else
	guard(appToUnicode);
	if( !Str )
		return NULL;
	static UNICHAR StaticUCh[STATICSTRINGLENGTH];
	UNICHAR* InCh = UCh;
	if( !UCh )
	{
		UCh = StaticUCh;
		maxlen = Min(maxlen, STATICSTRINGLENGTH);
	}
	INT Count;
	for( Count=0; ( (InCh && maxlen == STATICSTRINGLENGTH) || Count < maxlen-1) && Str[Count]; Count++ )
		UCh[Count] = ToUnicode( Str[Count] );
	UCh[Count] = 0;
	return UCh;
	unguard;
#endif
}

// ANSICHAR to TCHAR.
CORE_API const TCHAR* appFromAnsi( const ANSICHAR* ACh, TCHAR* Ch, INT maxlen )
{
#if UNICODE
	guard(appFromAnsi);
	if( !ACh )
		return NULL;
	TCHAR* InCh = Ch;
	if( !Ch )
	{
		Ch = appStaticString1024();
		maxlen = Min(maxlen, STATICSTRINGLENGTH);
	}
	INT Count;
	for( Count=0; ( (InCh && maxlen == STATICSTRINGLENGTH) || Count<maxlen-1) && ACh[Count]; Count++ )
		Ch[Count] = FromAnsi( ACh[Count] );
	Ch[Count] = 0;
	return Ch;
	unguard;
#else
	return ACh;
#endif
}

// UNICHAR to TCHAR.
CORE_API const TCHAR* appFromUnicode( const UNICHAR* UCh, TCHAR* Ch, INT maxlen )
{
#if UNICODE
	return UCh;
#else
	guard(appFromUnicode);
	if( !UCh )
		return NULL;
	TCHAR* InCh = Ch;
	if( !Ch )
	{
		Ch = appStaticString1024();
		maxlen = Min(maxlen, STATICSTRINGLENGTH);
	}
	INT Count;
	for( Count=0; ( (InCh && maxlen == STATICSTRINGLENGTH) || Count<maxlen-1) && UCh[Count]; Count++ )
		Ch[Count] = FromUnicode( UCh[Count] );
	Ch[Count] = 0;
	return Ch;
	unguard;
#endif
}

/*-----------------------------------------------------------------------------
	Error handling.
-----------------------------------------------------------------------------*/

//
// Unwind the stack.
//
CORE_API void VARARGS appUnwindf( const TCHAR* Fmt, ... )
{
	GIsCriticalError = 1;

	TCHAR TempStr[4096];
	GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), Fmt, Fmt );

	static INT Count=0;
	if( Count++ )
		appStrncat( GErrorHist, TEXT(" <- "), ARRAY_COUNT(GErrorHist) );
	appStrncat( GErrorHist, TempStr, ARRAY_COUNT(GErrorHist) );

	debugf( NAME_Critical, TempStr );
}


#if DO_PORTABLE_GUARD_BLOCKS
INT UnGuardBlock::GuardIndex = 0;
const char *UnGuardBlock::GuardTexts[MAX_PORTABLE_GUARD_BLOCKS];

void UnGuardBlock::GetBackTrace(FString &str)
{
    INT i = MAX_PORTABLE_GUARD_BLOCKS;
    if (GuardIndex < i)
        i = GuardIndex;

    if (!i)
        str = "(no backtrace available.)";
    else
    {
        str = GuardTexts[i];
        while ( --i > 0 )
        {
            str += TEXT(" <- ");
            str += GuardTexts[i];
        }
    }
}
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

