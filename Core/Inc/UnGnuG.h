/*=============================================================================
	UnGnuG.h: Unreal definitions for Gnu G++. Unfinished. Unsupported.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

/*----------------------------------------------------------------------------
	Platform compiler definitions.
----------------------------------------------------------------------------*/
#ifndef _INCL_UNGNUG_H_
#define _INCL_UNGNUG_H_

#if ((defined __LINUX_X86__) || (defined __FreeBSD__))
	#define __UNIX__  1
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#if __ICC
		#undef ASM
		#undef ASM3DNOW
		#undef ASMKNI
		#define ASMLINUX 1
		#define COMPILER "Compiled with Intel C++"
	#else
		#undef ASM
		#undef ASM3DNOW
		#undef ASMKNI
		#define ASMLINUX 1
		#define COMPILER "Compiled with GNU g++ ("__VERSION__")"
	#endif
#elif (defined MACOSX)
	#define __UNIX__  1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
	#define COMPILER "Compiled with MacOS X GNU g++ ("__VERSION__")"
#elif __PSX2_EE__
	#define __UNIX__ 1
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
	#undef ASMLINUX
	#define ASMPSX2 1
	#define COMPILER "Compiled with PSX2-EE g++ ("__VERSION__")"
	// Force use single precision
	#define tanh(x) tanhf(x)
	#define ceil(x) ceilf(x)
	#define fabs(x) fabsf(x)
	#define floor(x) floorf(x)
	#define cosh(x) coshf(x)
	#define sinh(x) sinhf(x)
	#define exp(x) expf(x)
	#define ldexp(x,y) ldexpf(x,y)
	#define log(x) logf(x)
	#define log10(x) log10f(x)
	#define pow(x,y) powf(x,y)
	#define sqrt(x) sqrtf(x)
	#define fmod(x,y) fmodf(x,y)
	#define sin( x ) sinf( x )
	#define cos( x ) cosf( x )
	#define tan( x ) tanf( x )
	#define asin( x ) asinf( x )
	#define acos( x ) acosf( x )
	#define atan( x ) atanf( x )
	#define atan2( x, y ) atan2f( x, y )
#elif __GCN__
	#define __UNIX__ 1
	#define __LINUX__ 1
	#define __INTEL__ 1
	#define __INTEL_BYTE_ORDER__ 1
	#undef ASM
	#undef ASM3DNOW
	#undef ASMKNI
	#undef ASMLINUX
	#undef ASMPSX2
	#define COMPILER "Compiled for GCN ("__VERSION__")"
#else
	#error Unsupported platform.
#endif

// alloca
#include <stdlib.h>

// va_list
#include <stdarg.h>

// FLT_MAX
#include <float.h>


/*----------------------------------------------------------------------------
	Platform specifics types and defines.
----------------------------------------------------------------------------*/

// Disable some inlines that confuse the linker on gcc but not Visual Studio.
#ifndef BUGGYINLINE
#define BUGGYINLINE
#endif

// This...is scary.  --ryan.
#if ASMLINUX
#  ifdef __x86_64__
#    define UREG_ESI "%%rsi"
#    define UREG_EDI "%%rdi"
#  else
#    define UREG_ESI "%%esi"
#    define UREG_EDI "%%edi"
#  endif
#endif


// Undo any Windows defines.
#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT
#undef VOID
#undef CDECL

// Make sure HANDLE is defined.
#define HANDLE DWORD
#define HINSTANCE void*

// Sizes.
enum {DEFAULT_ALIGNMENT = 8 }; // Default boundary to align memory allocations on.
enum {CACHE_LINE_SIZE   = 32}; // Cache line size.

#if __GNUG__
#define GCC_PACK(n)  __attribute__((packed,aligned(n)))
#define GCC_ALIGN(n) __attribute__((aligned(n)))
#define GCC_MOVE_ALIGN(n) 
#else
#define GCC_PACK(n) 
#define GCC_ALIGN(n) 
#define GCC_MOVE_ALIGN(n) 
#endif

//#define GCC_MOVE_ALIGN(n) __attribute__((aligned(n))) __attribute__((section (".bss")))

// Optimization macros
#define DISABLE_OPTIMIZATION  
#define ENABLE_OPTIMIZATION  

// Function type macros.
#define DLL_IMPORT
#if defined(__PSX2_EE__) || defined(__GCN__)
#define DLL_EXPORT
#else
#define DLL_EXPORT			extern "C"
#endif
#define DLL_EXPORT_CLASS
#define VARARGS
#define CDECL
#define STDCALL
#define FORCEINLINE inline
#define ZEROARRAY 0 /* Zero-length arrays in structs */
#define __cdecl

#define __forceinline inline

#if defined(NO_UNICODE_OS_SUPPORT) || !defined(UNICODE)
#define VSNPRINTF vsnprintf
#else
#define VSNPRINTF wvsnprintf
#endif

// Variable arguments.
#define GET_VARARGS(msg,len,lastarg,fmt)	\
{	\
	va_list ArgPtr;	\
	va_start( ArgPtr, lastarg );	\
	VSNPRINTF( msg, len, fmt, ArgPtr );	\
	va_end( ArgPtr );	\
}

#define GET_VARARGS_RESULT(msg,len,lastarg,fmt,result)	\
{	\
	va_list ArgPtr;	\
	va_start( ArgPtr, lastarg );	\
	result = VSNPRINTF( msg, len, fmt, ArgPtr );	\
	va_end( ArgPtr );	\
}

// Unsigned base types.
typedef unsigned char		BYTE;		// 8-bit  unsigned.
typedef unsigned short		_WORD;		// 16-bit unsigned.
typedef unsigned int		DWORD;		// 32-bit unsigned.
typedef unsigned long long	QWORD;		// 64-bit unsigned.
#if __GNUC__
//!!vogel: ???
//typedef unsigned int		OWORD __attribute__ ((mode (TI)));
#endif

// Signed base types.
typedef	signed char			SBYTE;		// 8-bit  signed.
typedef signed short		SHORT;		// 16-bit signed.
typedef signed short		SWORD;		// 16-bit signed.
typedef signed int  		INT;		// 32-bit signed.
typedef unsigned int  		UINT;		// 32-bit unsigned.
typedef signed long long	SQWORD;		// 64-bit signed.

// Character types.
typedef char			    ANSICHAR;	// An ANSI character.
typedef unsigned char		ANSICHARU;	// An ANSI character.

#if defined(NO_UNICODE_OS_SUPPORT) || !defined(UNICODE)
typedef unsigned short      UNICHAR;	// A unicode character.
typedef unsigned short      UNICHARU;	// A unicode character.
#else
typedef wchar_t             UNICHAR;	// A unicode character.
typedef wchar_t             UNICHARU;	// A unicode character.
#endif

// Other base types.
typedef signed int			UBOOL;		// Boolean 0 (false) or 1 (true).
typedef float				FLOAT;		// 32-bit IEEE floating point.
#ifdef __PSX2_EE__
typedef float				DOUBLE;		// 32-bit IEEE floating point.
#else
typedef double				DOUBLE;		// 64-bit IEEE double.
#endif
typedef unsigned int        SIZE_T;     // Corresponds to C SIZE_T.

// Bitfield type.
#if FORCE_EXTREME_PACKING
typedef BYTE                BITFIELD;	// For bitfields.
#else
typedef unsigned int		BITFIELD;	// For bitfields.
#endif

// !!! FIXME: This is currently true on all supported 64 and 32 bit platforms,
// !!! FIXME:  but it may not be universal. PTRINT should be an integer that
// !!! FIXME:  is the same size as a pointer, so casting can be (ahem) safely
// !!! FIXME:  done between them.  --ryan.
typedef unsigned long PTRINT;

// !!! FIXME: This should really just include a header.
#if ((!defined MACOSX) && (!defined __x86_64__))
typedef unsigned int size_t;
#endif

// Make sure characters are signed.
#ifdef __CHAR_UNSIGNED__
	#error "Bad compiler option: Characters must be signed"
#endif

// Strings.
#if __UNIX__
#define LINE_TERMINATOR TEXT("\n")
#define PATH_SEPARATOR TEXT("/")
#define DLLEXT TEXT(".so")
#else
#define LINE_TERMINATOR TEXT("\r\n")
#define PATH_SEPARATOR TEXT("\\")
#define DLLEXT TEXT(".dll")
#endif

// NULL.
#undef NULL
#define NULL 0

// Package implementation.
#define IMPLEMENT_PACKAGE_PLATFORM(pkgname) \
	BYTE GLoaded##pkgname;

// Platform support options.
#define PLATFORM_NEEDS_ARRAY_NEW 1
#define FORCE_ANSI_LOG           0

#if defined(NO_UNICODE_OS_SUPPORT) || !defined(UNICODE)
#define TCHAR_CALL_OS(funcW,funcA) (funcA)
#define TCHAR_TO_ANSI(str) str
#define ANSI_TO_TCHAR(str) str

#else

    CORE_API ANSICHAR* unixToANSI( ANSICHAR* ACh, const UNICHAR* InUCh );
    CORE_API INT unixGetSizeANSI( const UNICHAR* InUCh );
    CORE_API UNICHAR* unixToUNICODE( UNICHAR* UCh, const ANSICHAR* InACh );
    CORE_API INT unixGetSizeUNICODE( const ANSICHAR* InACh );
    CORE_API UNICHAR* unixANSIToUNICODE(char* str);
    CORE_API INT unixDetectUNICODE( void );
    #define UNICODE_BY_HAND 1
    #define _UNICODE 1

    #if defined(NO_ANSI_OS_SUPPORT)
	    #define TCHAR_CALL_OS(funcW,funcA) (funcW)
        #define TCHAR_TO_ANSI(str) str
	    #define ANSI_TO_TCHAR(str) str
    #else
	    #define TCHAR_CALL_OS(funcW,funcA) (GUnicodeOS ? (funcW) : (funcA))
    	#define TCHAR_TO_ANSI(str) unixToANSI((ANSICHAR*)appAlloca(unixGetSizeANSI(str)),str)
    	#define ANSI_TO_TCHAR(str) unixToUNICODE((TCHAR*)appAlloca(unixGetSizeUNICODE(str)),str)
    #endif

#endif


#if UNICODE_BY_HAND

CORE_API void unicode_str_to_stdout(const UNICHAR *str);

// These are implemented as portable C in Core/Src/UnUnix.cpp. You should
//  use these if your platform doesn't supply them, or your platform does
//  something dumb like make you use four byte unicode (like GNU).
//  Chances are that the versions supplied with your platform are going to
//  be much, much more optimized. You have been warned.
CORE_API UNICHAR* wcscpy( UNICHAR* Dest, const UNICHAR* Src);
CORE_API UNICHAR* wcsncpy( UNICHAR* Dest, const UNICHAR* Src, INT MaxLen );
CORE_API UNICHAR* wcscat( UNICHAR* String, const UNICHAR *Add );
CORE_API INT wcslen( const UNICHAR* String );
CORE_API INT wcscmp( const UNICHAR* Str1, const UNICHAR *Str2 );
CORE_API INT wcsncmp( const UNICHAR* Str1, const UNICHAR *Str2, INT max );
CORE_API UNICHAR* wcschr( const UNICHAR* String, const UNICHAR Find );
CORE_API UNICHAR* wcsstr( const UNICHAR* String, const UNICHAR* Find );
CORE_API INT _wcsicmp( const UNICHAR* String1, const UNICHAR* String2 );
CORE_API UNICHAR* _wcsupr( UNICHAR* String );
CORE_API INT wcstoul( const UNICHAR* Start, UNICHAR** End, INT Base );
CORE_API INT _wtoi( const UNICHAR* Str );
CORE_API INT _wcsnicmp( const UNICHAR* Str1, const UNICHAR *Str2, INT max );
CORE_API INT wprintf( const UNICHAR* fmt, ... );
CORE_API INT swscanf( const UNICHAR* fmt, ... );
CORE_API INT wvsnprintf( UNICHAR *buf, INT max, const UNICHAR *fmt, va_list args );
CORE_API INT iswspace( UNICHAR ch  );
CORE_API UNICHAR *_itow( const INT Num, UNICHAR *Buffer, const INT BufSize );
CORE_API QWORD _wcstoui64( const UNICHAR* Start, UNICHAR** End, INT Base );
CORE_API UNICHAR* _ui64tow( QWORD Num, UNICHAR *Buffer, INT Base );
CORE_API UNICHAR* _i64tow( SQWORD Num, UNICHAR *Buffer, INT Base );
CORE_API FLOAT _wtof( const UNICHAR* Str );
#endif

// Memory
#define appAlloca(size) ((size==0) ? 0 : alloca((size+7)&~7))
//#define appAlloca(size) ((size==0) ? 0 : malloc((size+7)&~7))

// System identification.
extern "C"
{
	extern HINSTANCE      hInstance;

    #ifdef __x86_64__  // Hammer always has these instruction sets.
	#define GIsMMX (1)
	#define GIsSSE (1)
	#define GIsAltivec (0)
    #elif MACOSX  // Mac never has Intel opcodes.
	#define GIsMMX (0)
	#define GIsSSE (0)
	extern CORE_API UBOOL GIsAltivec;
    #else
	extern CORE_API UBOOL GIsMMX;
	extern CORE_API UBOOL GIsSSE;
	extern CORE_API UBOOL GIsAltivec;
    #endif
}

// Module name
extern ANSICHAR GModule[32];
extern CORE_API DOUBLE GSecondsPerCycle;

char *strlwr(char *str);

CORE_API DOUBLE appSecondsSlow();


// Math intrinsics.

#include <math.h>

// Sanjay Patel from Apple says:
// "The math.h functions are all wrapped with app* functions in UnAnsi.cpp.
//  This causes an extra level of indirection because gcc doesn't do
//  cross-file inlining. I commented out the code in this file and put
//  #define's in Core.h to work around this problem (for example #define
//  appExp exp). This bought a little less than 1%." ... I cleaned this up
//  slightly.  --ryan.

#define DEFINED_appMathIntrinsics 1
#define appExp(x) exp(x)
#define appLoge(x) log(x)
#define appFmod(x,y) fmod(x,y)
#define appSin(x) sin(x)
#define appAsin(x) asin(x)
#define appCos(x) cos(x)
#define appAcos(x) acos(x)
#define appTan(x) tan(x)
#define appAtan(x) atan(x)
#define appAtan2(x,y) atan2(x,y)
#define appPow(x,y) pow(x,y)
#define appIsNan(x) isnan(x)
#define appRand() rand()
#define appRandInit(x) srand(x)
#define appFrand() ((FLOAT) (rand() / (FLOAT) RAND_MAX))
#define appCeil(x) ((INT)ceil(x))
#define appRound(x) ((INT)floor(x + 0.5f))
#define appFractional(x) (x - appFloor(x))

#ifdef __PSX2_EE__
#define appSqrt(x) Float_Sqrt_VU0(x)
#define appRecipSqrt(x) (1.0f/Float_Sqrt_VU0(x))
#elif MACOSX
#define SQRT_DO_NEWTON_RAPHSON 1
static inline FLOAT appSqrt(FLOAT num)
{
    register FLOAT t;

    if (num == 0.0f)
        return(0.0f);

    __asm__ __volatile__ (
        "frsqrte %0, %1  \n\t"
            : "=f" (t) : "f" (num)
    );

#if SQRT_DO_NEWTON_RAPHSON
{
    register FLOAT hx = num * 0.5f;
    t = t * (1.5f - hx * t * t);
    t = t * (1.5f - hx * t * t);
}
#endif

    return(t * num);
}

static inline DOUBLE appSqrt(DOUBLE num)
{
    register DOUBLE t;

    if (num == 0.0)
        return(0.0);

    __asm__ __volatile__ (
        "frsqrte %0, %1  \n\t"
            : "=f" (t) : "f" (num)
    );

#if SQRT_DO_NEWTON_RAPHSON
{
    register DOUBLE hx = num * 0.5;
    t = t * (1.5 - hx * t * t);
    t = t * (1.5 - hx * t * t);
}
#endif

    return(t * num);
}


static inline FLOAT appRecipSqrt(FLOAT num)
{
    register FLOAT t;

    if (num == 0.0f)
        return(0.0f);

    __asm__ __volatile__ (
        "frsqrte %0, %1  \n\t"
            : "=f" (t) : "f" (num)
    );

#if SQRT_DO_NEWTON_RAPHSON
{
    register FLOAT hx = num * 0.5f;
    t = t * (1.5f - hx * t * t);
    t = t * (1.5f - hx * t * t);
}
#endif

    return(t);
}

static inline DOUBLE appRecipSqrt(DOUBLE num)
{
    register DOUBLE t;

    if (num == 0.0)
        return(0.0);

    __asm__ __volatile__ (
        "frsqrte %0, %1  \n\t"
            : "=f" (t) : "f" (num)
    );

#if SQRT_DO_NEWTON_RAPHSON
{
    register DOUBLE hx = num * 0.5;
    t = t * (1.5 - hx * t * t);
    t = t * (1.5 - hx * t * t);
}
#endif

    return(t);
}

#else
#define appSqrt(x) sqrt(x)
#define appRecipSqrt(x) (1.0/sqrt(x))
#endif


#if 0  // !!! FIXME: Slower than just casting to INT on gcc-x86!
#if ASMLINUX
#define FAST_FTOL 1
FORCEINLINE INT appTrunc( const FLOAT F )
{
	INT Result;
	if( 1 )
	{
		__asm__ __volatile__ (
			"cvttss2sil (%%esi), %%eax  \n\t"
                : "=a" (Result)
                : "S" (&F)
                /* : no clobbers. */
        );
	}
	else
	{
		__asm__ __volatile__ (
			"flds (%%edi)  \n\t"
			"fistpl (%%esi)  \n\t"
                : /* no outputs...write to Result directly. */
                : "S" (&Result), "D" (&F)
                : "memory"
        );
	}
	return Result;
}
#endif
#endif

// Sanjay Patel from Apple says this gives a tiny performance boost. --ryan.
#if MACOSX
inline CORE_API INT appFloor( FLOAT Value )
{
	float zero = Value - Value;
	if(Value < zero)
	{
		// negative values need to handled properly
		return (INT)floor(Value);
	}
	else
	{
		// positive values just get rounded down to the nearest integer
		return (INT)(Value);
	}
}
#else
#define appFloor(x) ((INT)floor(x))
#endif

#define DEFINED_appArgv0 1
CORE_API void appArgv0(const char *argv0);  // defined in UnUnix.cpp ...

//
// CPU cycles, related to GSecondsPerCycle.
//
#if ASMLINUX
#define DEFINED_appCycles 1
inline DWORD appCycles()
{
	DWORD r;
    // !!! FIXME: WHY DOESN'T THIS WORK?!
	//asm("rdtsc" : "=a" (r) : /* no inputs */ : "edx");
	asm("rdtsc" : "=a" (r) : "d" (r));
	return r;
}
#elif MACOSX
#define DEFINED_appCycles 1
// This version of appCycles will NOT work on a PowerPC 601, since it
//  doesn't have the Time Base Register. You could possibly fake it with the
//  Real Time Clock Register on the 601, though. All other known PPC chips
//  make available the 603's TBR.
inline DWORD appCycles(void)
{
    DWORD r;

    // We only use the lower time base register, since it's 32 bits.
    //  There is a high-order 32-bits that can be retrieved with the
    //  "mftbu" instruction. That register increments whenever the low
    //  register overflows.
    __asm__ __volatile__( "mftb %0" : "=r" (r) );
    return(r);
}
#endif

//
// Seconds, arbitrarily based.
//

#if 0 //ASMLINUX
// Intel SpeedStep and AMD Cool & Quiet CPUs automatically adjust their frequency so RDTSC 
// shouldn't be used for game relevant inter- frame timing. The code will default to using
// gettimeofday() instead.
#define DEFINED_appSeconds 1
inline DOUBLE appSeconds()
{
	DWORD L,H;
	asm("rdtsc" : "=a" (L), "=d" (H));
	//!!vogel: add big number to make bugs apparent.
	return ((DOUBLE)L +  4294967296.0 * (DOUBLE)H) * GSecondsPerCycle + 16777216.0;
}
#elif MACOSX
#define DEFINED_appSeconds 1
// This version of appSeconds will NOT work on a PowerPC 601, since it
//  doesn't have the Time Base Register. You could possibly fake it with the
//  Real Time Clock Register on the 601, though. All other known PPC chips
//  make available the 603's TBR.
inline DOUBLE appSeconds()
{
	DWORD L,H,T;
    do
    {
		__asm__ __volatile__( "mftbu %0" : "=r" (T) );
		__asm__ __volatile__( "mftb %0" : "=r" (L) );
		__asm__ __volatile__( "mftbu %0" : "=r" (H) );
    } while (T != H);  // in case high register incremented during read.

	//!!vogel: add big number to make bugs apparent.
	return ((DOUBLE)L +  4294967296.0 * (DOUBLE)H) * GSecondsPerCycle + 16777216.0;
}
#endif


//
// Memory copy.
//
#define DEFINED_appMemcpy 1
inline void appMemcpy( void* Dest, const void* Src, INT Count )
{
	//!!vogel: TODO
	memcpy( Dest, Src, Count );
}

//
// Memory zero.
//
#define DEFINED_appMemzero 1
inline void appMemzero( void* Dest, INT Count )
{
	//!!vogel: TODO
	memset( Dest, 0, Count );
}

#if ASMLINUX
inline DWORD _rotr(DWORD val, INT shift)
{
	DWORD retval;
    asm("rorl %%cl, %%eax" : "=a" (retval) : "a" (val), "c" (shift));
	return retval;
}
#elif MACOSX
#include <stdio.h>
inline DWORD _rotr(DWORD val, INT shift)
{
	DWORD retval;

fprintf(stderr, "%s:%d ... This is probably wrong.\n", __FILE__, __LINE__);

    shift %= 32;
    while (shift > 0)
    {
        // I can't believe it...there's no ROTATE RIGHT opcode...just left! --ryan.
        __asm__ __volatile__("rlwinm %0, %1, 31, 0, 0" : "=r" (retval) : "r" (val));
        shift--;
    }
	return retval;
}
#else
// !!! FIXME: A C fallback would be nice, too. --ryan.
#   error Please define for your platform.
#endif


#if ASMLINUX
inline DWORD _rotl(DWORD val, INT shift)
{
	DWORD retval;
    asm("roll %%cl, %%eax" : "=a" (retval) : "a" (val), "c" (shift));
	return retval;
}
#elif MACOSX
inline DWORD _rotl(DWORD val, INT shift)
{
	DWORD retval;

fprintf(stderr, "%s:%d ... This is probably wrong.\n", __FILE__, __LINE__);

    shift %= 32;
    while (shift > 0)
    {
        // I can't believe it...there's no ROTATE RIGHT opcode...just left! --ryan.
        __asm__ __volatile__("rlwinm %0, %1, 1, 0, 0" : "=r" (retval) : "r" (val));
        shift--;
    }

	return retval;
}
#else
// !!! FIXME: A C fallback would be nice, too. --ryan.
#   error Please define for your platform.
#endif

extern CORE_API QWORD GBaseCycles;

#if ASMLINUX
#define DEFINED_appResetTimer 1
inline void appResetTimer(void)
{
	__asm__ __volatile__
	(
        "rdtsc  \n\t"
            : "=a" ( ((DWORD *) &GBaseCycles)[0] ),
              "=d" ( ((DWORD *) &GBaseCycles)[1] )
    );
}
#elif MACOSX
#define DEFINED_appResetTimer 1
inline void appResetTimer(void)
{
	DWORD L,H,T;
    do
    {
		__asm__ __volatile__( "mftbu %0" : "=r" (T) );
		__asm__ __volatile__( "mftb %0" : "=r" (L) );
		__asm__ __volatile__( "mftbu %0" : "=r" (H) );
    } while (T != H);  // in case high register incremented during read.

    ((DWORD *) &GBaseCycles)[0] = L;
    ((DWORD *) &GBaseCycles)[1] = H;
}

#else
#    error please fill this in for your platform.
#endif

#endif  // include-once blocker.

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/

