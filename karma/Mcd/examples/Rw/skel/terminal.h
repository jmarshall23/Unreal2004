/**********************************************************************
 *
 * File :     terminal.h
 *
 * Abstract : Functions to manipulate a console within the demo skeleton
 *
 **********************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd. or
 * Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. will not, under any
 * circumstances, be liable for any lost revenue or other damages arising
 * from the use of this file.
 *
 * Copyright (c) 1999 Criterion Software Ltd.
 * All Rights Reserved.
 *
 * RenderWare is a trademark of Canon Inc.
 *
 ************************************************************************/

#ifndef TERMINAL_H
#define TERMINAL_H

/****************************************************************************
 Includes
 */

#include <rwcore.h>
#include <rtcharse.h>

/****************************************************************************
 Defines
 */

/****************************************************************************
 Global Types
 */

typedef struct _RsTerminalCommandDictionaryEntry RsTerminalCommandDictionaryEntry;
typedef struct _RsTerminal RsTerminal;

typedef RwBool (*RsTerminalDisplayCallBack)(RsTerminal *term, void *pData);

struct _RsTerminalCommandDictionaryEntry
{
    const RwChar *command;
    void (*fp)(RwChar *args);
};

enum _RsTerminalState
{
    NARSTERMINALSTATE,
    RSTERMINALSTATEOFF,
    RSTERMINALSTATEACTIVE,
    RSTERMINALSTATEDISPLAYONLY,
    NARSTERMINALSTATELAST
};
typedef enum _RsTerminalState RsTerminalState;

struct _RsTerminal
{
    RsTerminalState state;
    
    RwInt32 blinkCount;
    RwInt32 blinkWait;
    
    RwInt32 cursorX;
    RwInt32 cursorY;
    
    RwInt32 width;
    RwInt32 height;
    RwInt32 lockedLines;
    RwInt32 position;
    
    RtCharset *font;
    
    RsTerminalDisplayCallBack display;
    void *displayData;
    
    RwChar  *screen;
    
    RwInt32  historyLength;
    RwInt32  historyWrite;
    RwInt32  historyRead;
    RwChar  *history;
    
    RwInt32  numCommands;
    RsTerminalCommandDictionaryEntry *commands;
};

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */


/* Default command functions */
    extern void
    do_quit(RwChar *args __RWUNUSED__);
    extern void
    do_help(RwChar *args __RWUNUSED__);

/* Creating and destroying */

    extern RsTerminal *
    RsTerminalCreate(RwInt32 width, RwInt32 height, RwRGBA *fore, RwRGBA *back,
                     RsTerminalDisplayCallBack display, void *data);
    extern void
    RsTerminalDestroy(RsTerminal *term);
    extern RwBool
    RsTerminalHistoryCreate(RsTerminal *term, RwInt32 length);
    extern void
    RsTerminalHistoryDestroy(RsTerminal *term);

    extern RwBool
    RsGenerateCompletionDictionary(RsTerminal *term);
    extern RwBool
    RsTerminalTabCompletion(RsTerminal *term);
    
    extern RwBool
    RsTerminalAddCommand(RsTerminal *term, RsTerminalCommandDictionaryEntry *command);
    extern RwBool
    RsTerminalRemoveCommand(RsTerminal *term, RwChar *command);

/* Updating */

    extern void
    RsTerminalScroll(RsTerminal *term, RwInt32 lines);
    extern void
    RsTerminalHistoryScrollUp(RsTerminal *term);
    extern void
    RsTerminalHistoryScrollDown(RsTerminal *term);
    extern void
    RsTerminalClear(RsTerminal *term);
    extern void
    RsTerminalClearLine(RsTerminal *term, RwInt32 line);
    extern void
    RsTerminalClearRestOfLine(RsTerminal *term, RwInt32 line);
    extern RwBool
    RsTerminalLockLines(RsTerminal *term, RwInt32 lines);
    extern RwInt32
    RsTerminalGetLockedLines(RsTerminal *term);
    extern RsTerminalState
    RsTerminalGetState(RsTerminal *term);
    extern void
    RsTerminalSetState(RsTerminal *term, RsTerminalState nState);

/* Printing */

    extern void
    RsTerminalPrintChar(RsTerminal *term, RwUInt8 c);
    extern void
    RsTerminalPrint(RsTerminal *term, const RwChar *text);
    extern RwBool
    RsTerminalPrintOnLine(RsTerminal *term, const RwChar *text, RwInt32 line);
    extern void
    RsTerminalPrintProtect(RsTerminal *term, const RwChar *text);
    extern void
    RsTerminalPrintf(RsTerminal *term, const RwChar *sFormat,...);

/* Updating from keyboard input */

    extern RwChar *
    RsTerminalUpdate(RsTerminal *term, RwUInt8 key, RwRaster *raster);
    extern RwBool
    RsTerminalDispatchCommand(RsTerminal *term, RwChar *buffer);

/* Displaying */

    extern void
    RsTerminalSetFontColor(RsTerminal *term, RwRGBA *fore, RwRGBA *back);
    extern RwBool
    RsTerminalDisplay(RsTerminal *term);
    extern RwBool
    RsTerminalRender(RsTerminal *term, void *data __RWUNUSED__);
    extern void
    RsTerminalRenderPosition(RsTerminal *term, RwInt32 x, RwInt32 y);
    extern void
    RsTerminalGetPixelSize(RsTerminal *term, RwInt32 *width, RwInt32 *height);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* TERMINAL_H */
