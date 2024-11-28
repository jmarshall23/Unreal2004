/* -*- mode: C; -*- */

/*
     Copyright (c) 1997-2002 MathEngine PLC

     $Name: t-stevet-RWSpre-030110 $

     Date: $Date: 2002/04/18 14:17:27 $ - Revision: $Revision: 1.14.2.11 $

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

/** @file
 * MathEngine message handling API implementation.
 *
 * These functions support both message output and error
 * signaling. It is highly configurable: the message handler functions
 * can be redefined, even dynamically, and so can the function that
 * print messages in the default message handlers.
 *
 * A message can be shown to the user/a condition signaled by invoking
 * one of four entry types, for four types of messages/conditions:
 * information, warning, fatal error and debug.
 *
 * The default action is to print a message to the standard error
 * stream, and, in the case of fatal errors, to abort the program.
 *
 * The user can redefine the output functions to, for example, show
 * a message panel under Windows, or the handler functions to, for
 * example, enter debug mode when a warning message is requested.
 */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <MeMessage.h>
#include <MeString.h>

#if (WIN32)
#   ifndef _XBOX
#       include "windows.h"
#       if(_MSC_VER)
#           include <crtdbg.h>
#       endif
#   endif
#endif

#ifdef _XBOX
#    include <xtl.h>
#endif

#if (NGC)
#    include <dolphin/os.h>
#endif

#ifdef NEED_SNPRINTF
    /* These are now in 'MeString.c' */
    extern int vsnprintf(char *const string,const size_t n,
        const char *const format, va_list ap);

    extern int snprintf(char *const string,const size_t n,
        const char *const format,...);

# if (MeMAXMESSAGE < 4000)
#   error Maximum message size must be at least 4000
# endif
#endif

/* The default messaging function handlers below will only output their
     messages (using the 'MeShowStdout' or 'MeShowStderr' functions
     pointers above as opportune) if their 'level' argument is less than
     or equal to the value of the corresponding global level cutoff
     below. The default is '0' for all, so only level '0' messages
     will be output; the default is the opposite in debug mode.

     Despite 'MeFatalErrorLevel' all fatal errors will be output; the level
     will just be passed to '(*MeFatalErrorShow)'.

     Only error levels up to 127 are guaranteed to be available, and
     that should be enough. */

#ifdef NDEBUG
# define levelDEFAULT           0
#else
# define levelDEFAULT           64
#endif

int MeInfoLevel                 = levelDEFAULT;
int MeWarningLevel              = levelDEFAULT;
int MeFatalErrorLevel           = levelDEFAULT;
int MeDebugLevel                = levelDEFAULT;

/** @var MeInfoShow
 * Default information message handler output function.
 */
/** @var MeWarningShow
 * Default warning message handler output function.
 */
/** @var MeFatalErrorShow
 * Default fatal error message handler output function.
 */
/** @var MeDebugShow
 * Default debug message handler output function.
 */
/* For each type of message you can specify the output function.
     This is passed the level of the message (so that for example
     different levels of message can go to different destinations,
     e.g. 'syslog'  or 'stderr'), and the string to print.

     All these pointers default to 'MeShowStderr' or equivalent. */

MeShow MeInfoShow             = MeShowStderr;
MeShow MeWarningShow          = MeShowStderr;
MeShow MeLogShow              = MeShowStderr;

#if (WIN32 && _MSC_VER)
    MeShow MeFatalErrorShow     = MeShowDialog;
    MeShow MeDebugShow          = MeShowCrtWarn;
#else
    MeShow MeFatalErrorShow     = MeShowStderr;
    MeShow MeDebugShow          = MeShowStderr;
#endif

/* For each type of message you can specify the handler function
     itself. This is passed the level of the message (so that for example
     different levels of message can go to different destinations,
     e.g. 'syslog' or 'stderr'), and the format and argument list. */

MeHandler MeInfoHandler         = MeHandlerInfo;
MeHandler MeLogHandler          = MeHandlerLog;
MeHandler MeWarningHandler      = MeHandlerWarning;
MeHandler MeFatalErrorHandler   = MeHandlerFatalError;
MeHandler MeDebugHandler        = MeHandlerDebug;

/*
    Default ``show'' (print/display) functions
*/

/**
 * Print a string to stdout if the level is right.
 *
 * This could be a default messaging output function.
 */
void MEAPI MeShowStdout(const int level,const char *const string)
{
    (void) level;

#if (defined PS2 || defined MEEE)
    (void) printf("%s\n",string);
#elif (defined NGC)
    (void) OSReport("%s\n",string);
#else
    (void) fputs(string,stdout);
    (void) fputs("\n",stderr);
#endif
}

/**
 * Print a string to stderr if the level is right.
 *
 * This is the default messaging output function.
 */
void MEAPI MeShowStderr(const int level,const char *const string)
{
    (void) level;

#if (defined PS2 || defined MEEE)
    /* Unfortunately the PS2 RT does not distinguish between
         'stdout' and 'stderr'. */
    (void) printf("%s\n",string);
#elif (defined _XBOX)
    OutputDebugStringA(string);
    OutputDebugStringA("\n");
#elif (defined NGC)
    (void) OSReport("%s\n",string);
#else
    (void) fputs(string,stderr);
    (void) fputs("\n",stderr);
#endif
}

#if (WIN32 && _MSC_VER)
/**
 * Print a string to WIN32 CRT debug warning if the level is right.
 *
 * This is the default messaging output function for warnings.
 */
void MEAPI MeShowCrtWarn(const int level,const char *const string)
{
    (void) level;

#if (defined _XBOX)
    OutputDebugStringA(string);
#else
    _RPT0(_CRT_WARN,string);
#endif
}

/**
 * Print a string to WIN32 CRT debug error if the level is right.
 *
 * This is the default messaging output function for errors.
 */
void MEAPI MeShowCrtErr(const int level,const char *const string)
{
    (void) level;

#if (defined _XBOX)
    OutputDebugStringA(string);
#else
    _RPT0(_CRT_ERROR,string);
#endif
}

/**
 * Print a string to a WIN32 message box if the level is right.
 *
 * This is the default messaging output function in some cases.
 */
void MEAPI MeShowDialog(const int level,const char *const string)
{
#ifndef _XBOX
    (void) level;

    MessageBox(0, string, "MathEngine Toolkit",
        MB_SETFOREGROUND | MB_TASKMODAL | MB_ICONEXCLAMATION);
#else
    OutputDebugStringA(string);
#endif
}
#endif

/*
    Default handlers
*/

/**
 * Default handler for information messages.
 *
 * Does nothing if the level of the message is greater than the current
 * information message level. Otherwise formats it and prints it using
 * @c *MeInfoShow.
 */
void MEAPI MeHandlerInfo(const int level,
    const char *const format,va_list ap)
{
    if (level <= MeInfoLevel)
    {
        char format2[MeMAXMESSAGE];
        char message[MeMAXMESSAGE];

        (void) snprintf(format2,sizeof format2,"MeInfo{%d}: %s",level,format);
        (void) vsnprintf(message,sizeof message,format2,ap);

        (*MeInfoShow)(level,message);
    }
}

/**
 * Default handler for information messages.
 *
 * Does nothing if the level of the message is greater than the current
 * information message level. Otherwise formats it and prints it using
 * @c *MeInfoShow.
 */
void MEAPI MeHandlerLog(const int level, const char *const format,va_list ap)
{
    if (level > MeInfoLevel)
        return;

    {
        char format2[MeMAXMESSAGE];
        char message[MeMAXMESSAGE];

        (void) snprintf(format2,sizeof format2,"MeLog{%d}: %s",level,format);
        (void) vsnprintf(message,sizeof message,format2,ap);

        (*MeLogShow)(level,message);
    }
}

/**
 * Default handler for warning messages.
 *
 * Does nothing if the level of the message is greater than the current
 * information message level. Otherwise formats it and prints it using
 * @c *MeWarningShow.
 */
void MEAPI MeHandlerWarning(const int level,
    const char *const format,va_list ap)
{
    if (level <= MeWarningLevel)
    {
        char format2[MeMAXMESSAGE];
        char message[MeMAXMESSAGE];

        (void) snprintf(format2,sizeof format2,
            "MeWarning{%d}: %s",level,format);
        (void) vsnprintf(message,sizeof message,format2,ap);

        (*MeWarningShow)(level,message);
    }
}

/**
 * Default handler for fatal error messages.
 *
 * Does nothing if the level of the message is greater than the current
 * information message level. Otherwise formats it and prints it using
 * @c *MeFatalErrorShow, and then aborts the program.
 */
void MEAPI MeHandlerFatalError(const int level,
    const char *const format,va_list ap)
{
    {
        char format2[MeMAXMESSAGE];
        char message[MeMAXMESSAGE];

        (void) snprintf(format2,sizeof format2,"MeFatalError{%d}: %s",
            level,format);
        (void) vsnprintf(message,sizeof message,format2,ap);

        (*MeFatalErrorShow)(level,message);
    }

#if (defined NGC)
    OSHalt("Terminating because of fatal error above...\n");
#elif (defined _DEBUG)
    abort();
#else
    exit(1);
#endif
}

/**
 * Default handler for debug messages.
 *
 * Does nothing if the level of the message is greater than the current
 * information message level. Otherwise formats it and prints it using
 * @c *MeDebugShow.
 */
void MEAPI MeHandlerDebug(const int level,
    const char *const format,va_list ap)
{
    if (level <= MeDebugLevel)
    {
        char format2[MeMAXMESSAGE];
        char message[MeMAXMESSAGE];

        (void) snprintf(format2,sizeof format2,"MeDebug{%d}: %s",level,format);
        (void) vsnprintf(message,sizeof message,format,ap);

        (*MeDebugShow)(level,message);
    }
}

/*
    Set the print function entry point, returning the previous value
*/

/**
 * Set the default informational message handler print function, returning
 * the previous value.
 *
 * @param n a print function of type @c MeShow.
 */
MeShow MEAPI MeSetInfoShow(MeShow n)
{
    const MeShow current = MeInfoShow;

    MeInfoShow = n;

    return current;
}

/**
 * Set the default logging handler print function, returning
 * the previous value.
 *
 * @param n a print function of type @c MeShow.
 */
MeShow MEAPI MeSetLogShow(MeShow n)
{
    const MeShow current = MeLogShow;

    MeLogShow = n;

    return current;
}

/**
 * Set the default warning message handler print function, returning
 * the previous value.
 *
 * @param n a print function of type @c MeShow.
 */
MeShow MEAPI MeSetWarningShow(MeShow n)
{
    const MeShow current = MeWarningShow;

    MeWarningShow = n;

    return current;
}

/**
 * Set the default fatal error message handler print function, returning
 * the previous value.
 *
 * @param n a print function of type @c MeShow.
 */
MeShow MEAPI MeSetFatalErrorShow(MeShow n)
{
    const MeShow current = MeFatalErrorShow;

    MeFatalErrorShow = n;

    return current;
}

/**
 * Set the default debug message handler print function, returning
 * the previous value.
 *
 * @param n a print function of type @c MeShow.
 */
MeShow MEAPI MeSetDebugShow(MeShow n)
{
    const MeShow current = MeDebugShow;

    MeDebugShow = n;

    return current;
}

/* Set the handler entry point, returning the current value. */

/**
 * Set the default informational message handler function, returning
 * the previous value.
 *
 * @param n handler function.
 * @return the previous handler function.
 */
MeHandler MEAPI MeSetInfoHandler(MeHandler n)
{
    const MeHandler current = MeInfoHandler;

    MeInfoHandler = n;

    return current;
}

/**
 * Set the logging message handler function, returning
 * the previous value.
 *
 * @param n handler function.
 * @return the previous handler function.
 */
MeHandler MEAPI MeSetLogHandler(MeHandler n)
{
    const MeHandler current = MeLogHandler;

    MeLogHandler = n;

    return current;
}

/**
 * Set the default warning message handler function, returning
 * the previous value.
 *
 * @param n handler function.
 * @return the previous handler function.
 */
MeHandler MEAPI MeSetWarningHandler(MeHandler n)
{
    const MeHandler current = MeWarningHandler;

    MeWarningHandler = n;

    return current;
}

/**
 * Set the default fatal error message handler function, returning
 * the previous value.
 *
 * @param n handler function.
 * @return the previous handler function.
 */

MeHandler MEAPI MeSetFatalErrorHandler(MeHandler n)
{
    const MeHandler current = MeFatalErrorHandler;

    MeFatalErrorHandler = n;

    return current;
}

/**
 * Set the default debug message handler function, returning
 * the previous value.
 *
 * @param n handler function.
 * @return the previous handler function.
 */
MeHandler MEAPI MeSetDebugHandler(MeHandler n)
{
    const MeHandler current = MeDebugHandler;

    MeDebugHandler = n;

    return current;
}

/* The real entry points. They just call the handlers, that are by
     default the functions above. */

/**
 * The informational message function.
 *
 * Calls a message handler, and the default ones will then call a
 * message print function.
 *
 * @param level The level of the message.
 * @param format A @c printf style format string.
 * @param ... The list of parameters for the @a format string.
 */
void MEAPI MeInfo(const int level, const char *const format,...)
{
    va_list ap;

    va_start(ap,format);
    (*MeInfoHandler)(level,format,ap);
    va_end(ap);
}

/**
 * The logging message function.
 *
 * Calls a message handler, and the default ones will then call a
 * message print function.
 *
 * @param level The level of the message.
 * @param format A @c printf style format string.
 * @param ... The list of parameters for the @a format string.
 */
void MEAPI MeLog(const int level, const char *const format,...)
{
    va_list ap;

    va_start(ap,format);
    (*MeLogHandler)(level,format,ap);
    va_end(ap);
}

/**
 * The warning message function.
 *
 * Calls a message handler, and the default ones will then call a
 * message print function.
 *
 * @param level The level of the message.
 * @param format A @c printf style format string.
 * @param ... The list of parameters for the @a format string.
 */
void MEAPI MeWarning(const int level, const char *const format,...)
{
    va_list ap;

    va_start(ap,format);
    (*MeWarningHandler)(level,format,ap);
    va_end(ap);
}

/**
 * The fatal error message function.
 *
 * Calls a message handler, and the default ones will then call a
 * message print function. It will then by default abort the program.
 *
 * @param level The level of the message.
 * @param format A @c printf style format string.
 * @param ... The list of parameters for the @a format string.
 */
void MEAPI MeFatalError(const int level,const char *const format,...)
{
    va_list ap;

    va_start(ap,format);
    (*MeFatalErrorHandler)(level,format,ap);
    va_end(ap);
}

/**
 * The debug message function.
 *
 * Calls a message handler, and the default ones will then call a
 * message print function.
 *
 * @param level The level of the message.
 * @param format A @c printf style format string.
 * @param ... The list of parameters for the @a format string.
 */
void MEAPI MeDebug(const int level,const char *const format,...)
{
    va_list ap;

    va_start(ap,format);
    (*MeDebugHandler)(level,format,ap);
    va_end(ap);
}

/**
 * The debug message function, special cased for QHull, to
 * look like a call to @c fprintf.
 *
 * Calls a message handler, and the default ones will then call a
 * message print function.
 *
 * @param f A @c FILE pointer, ignored
 * @param format A @c printf style format string.
 * @param ... The list of parameters for the @a format string.
 */
void MeDebugF(const FILE *const f,const char *const format,...)
{
    va_list ap;

    (void) f;

    va_start(ap,format);
    (*MeDebugHandler)(1,format,ap);
    va_end(ap);
}

/**
 * Shortens the full path of a filename to just the name.  This is for
 * use in warnings and error messages. Takes into account the fact that
 * WIN32 delimiter is \ and Unix is /.
 */
char * MEAPI MeShortenPath(char *path)
{
    char *ptr,*ptr2;
#ifdef WIN32
    int delimiter = '\\';
#else
    int delimiter = '/';
#endif

    ptr = strchr(path,delimiter);

    if (!ptr)
        return path;

    while(ptr)
    {
        ptr++;
        ptr2 = ptr;
        ptr = strchr(ptr2,delimiter);
    }
    return ptr2;
}
