//############################################################################
//##                                                                        ##
//##  Miles Sound System                                                    ##
//##                                                                        ##
//##  MSS.H: Miles Sound System main header file                            ##
//##                                                                        ##
//##  Version 1.00 of 15-Feb-95: Initial, derived from AIL.H V3.02          ##
//##          1.01 of 19-Jun-95: Added various functions for V3.03 release  ##
//##          1.02 of 22-Nov-95: C++ typedef problem fixed, declspecs added ##
//##          1.03 of 15-Feb-96: Changes for 16 bit callbacks and multiple  ##
//##                             16 bit DLL loads (JKR)                     ##
//##          1.04 of  2-Nov-97: Changes made to handle DLS in future       ##
//##                             versions                                   ##
//##          1.05 of  1-Jan-98: Massive changes for version 4.0            ##
//##          1.06 of 17-Sep-98: Massive changes for version 5.0            ##
//##          1.07 of  2-Feb-99: Changes for new input API                  ##
//##          1.08 of  8-Feb-99: Changes for new filter helper functions    ##
//##          1.09 of  8-Feb-03: Changes for xbox and linux                 ##
//##                                                                        ##
//##  Author: John Miles                                                    ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Contact RAD Game Tools at 425-893-4300 for technical support.         ##
//##                                                                        ##
//############################################################################

#ifndef MSS_H
#define MSS_H

// IS_DOS for DOS
// IS_WINDOWS for Windows or Win32
// IS_WIN32 for Win32
// IS_WIN16 for Windows
// IS_32 for 32-bit DOS or Win32
// IS_16 for 16-bit Windows
// IS_LE for little endian (PCs)
// IS_BE for big endian (Macs)
// IS_X86 for Intel
// IS_MAC for Mac
// IS_PPC for PPC Mac
// IS_68K for 68K Mac
// IS_LINUX for Linux
// IS_XBOX for Xbox
// IS_STATIC for static versions (DOS, Xbox, GameCube)


#ifdef IS_DOS
#undef IS_DOS
#endif

#ifdef IS_WINDOWS
#undef IS_WINDOWS
#endif

#ifdef IS_WIN32
#undef IS_WIN32
#endif

#ifdef IS_WIN16
#undef IS_WIN16
#endif

#ifdef IS_32
#undef IS_32
#endif

#ifdef IS_16
#undef IS_16
#endif

#ifdef IS_LE
#undef IS_LE
#endif

#ifdef IS_BE
#undef IS_BE
#endif

#ifdef IS_X86
#undef IS_X86
#endif

#ifdef IS_MAC
#undef IS_MAC
#endif

#ifdef IS_PPC
#undef IS_PPC
#endif

#ifdef IS_68K
#undef IS_68K
#endif

#ifdef IS_LINUX
#undef IS_LINUX
#endif

#ifdef IS_STATIC
#undef IS_STATIC
#endif

#ifdef IS_XBOX
#undef IS_XBOX
#endif

#ifdef __DOS__
  #define IS_DOS
  #define IS_32
  #define IS_LE
  #define IS_X86
  #define IS_STATIC
#else
  #ifdef _XBOX
    #define IS_WIN32API
    #define IS_32
    #define IS_LE
    #define IS_X86
    #define IS_XBOX
    #define IS_STATIC
  #else
    #if defined(_WIN32) || defined(WIN32) || defined(__NT__) || defined(__WIN32__)
      #define IS_WIN32API
      #define IS_WINDOWS
      #define IS_WIN32
      #define IS_32
      #define IS_LE
      #define IS_X86
    #else
      #ifdef _WINDOWS
        #define IS_WINDOWS
        #define IS_WIN16
        #define IS_16
        #define IS_LE
        #define IS_X86
      #else
        #if defined(_WINDLL) || defined(WINDOWS) || defined(__WINDOWS__) || defined(_Windows)
          #define IS_WINDOWS
          #define IS_WIN16
          #define IS_16
          #define IS_LE
          #define IS_X86
        #else
          #if defined(macintosh) || defined(__powerc) || defined(powerc) || defined(__POWERPC__) || defined(__MC68K__)
            #define IS_MAC
            #if TARGET_API_MAC_CARBON
              #define IS_CARBON
            #endif
            #define IS_32
            #define IS_BE
            #if defined(__powerc) || defined(powerc) || defined(__POWERPC__)
              #define IS_PPC
            #else
              #if defined(__MC68K__)
                #define IS_68K
              #endif
            #endif
          #else
            #ifdef linux
              #define IS_LINUX
              #define IS_32
              #define IS_LE
              #ifdef i386
                #define IS_X86
              #endif
            #endif
          #endif
        #endif
      #endif
    #endif
  #endif
#endif

#if (!defined(IS_LE) && !defined(IS_BE))
  #error MSS.H did not detect your platform.  Define __DOS__, _WINDOWS, WIN32, or macintosh.
#endif


#pragma pack(push,1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IS_LINUX
#define HIWORD(ptr) (((U32)ptr)>>16)
#define LOWORD(ptr) ((U16)((U32)ptr))
#define FOURCC U32
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
              ((U32)(U8)(ch0) | ((U32)(U8)(ch1) << 8) |   \
              ((U32)(U8)(ch2) << 16) | ((U32)(U8)(ch3) << 24 ))
#endif
#define mmioFOURCC(w,x,y,z) MAKEFOURCC(w,x,y,z)
#endif

//
// General type definitions for portability
//

#ifndef C8
#define C8 char
#endif

#ifndef U8
#define U8 unsigned char
#endif

#ifndef S8
#define S8 signed char
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef S16
#define S16 signed short
#endif

#ifndef U32
#define U32 unsigned int
#endif

#ifndef S32
#define S32 signed int
#endif

#ifndef F32
#define F32 float
#endif

#ifndef F64
#define F64 double
#endif

#ifndef REALFAR
#define REALFAR unsigned long
#endif

#define AIL_MAX_FILE_HEADER_SIZE 4096

typedef S32 HASISTREAM;

typedef U32 HATTRIB;

typedef S32 (* AILASIFETCHCB) (U32       user,            // User value passed to ASI_open_stream()
                                            void *dest,            // Location to which stream data should be copied by app
                                               S32       bytes_requested, // # of bytes requested by ASI codec
                                               S32       offset);         // If not -1, application should seek to this point in stream


#define AIL_ptr_add(ptr,off) ((void*)(((U8*)(ptr))+(off)))

//
// ASI result codes
//

typedef S32 ASIRESULT;

#define ASI_NOERR                   0   // Success -- no error
#define ASI_NOT_ENABLED             1   // ASI not enabled
#define ASI_ALREADY_STARTED         2   // ASI already started
#define ASI_INVALID_PARAM           3   // Invalid parameters used
#define ASI_INTERNAL_ERR            4   // Internal error in ASI driver
#define ASI_OUT_OF_MEM              5   // Out of system RAM
#define ASI_ERR_NOT_IMPLEMENTED     6   // Feature not implemented
#define ASI_NOT_FOUND               7   // ASI supported device not found
#define ASI_NOT_INIT                8   // ASI not initialized
#define ASI_CLOSE_ERR               9   // ASI not closed correctly

#ifdef __cplusplus
}
#endif


#pragma pack(pop)

#endif
