
/**************************************************************************
 *
 * File :     terminal.c
 *
 * Abstract : Functions to manipulate a console within the demo skeleton
 *
 **************************************************************************
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
 **************************************************************************/

/**************************************************************************
 Includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rwcore.h>
#include <rpworld.h>
#include <rtcharse.h>

/* This file */

#include "skeleton.h"
#include "terminal.h"

/****************************************************************************
 Local Types
 */

/****************************************************************************
 Local (Static) Prototypes
 */

/****************************************************************************
 Local Defines
 */

#define RSTERMINALDEFAULTBLINK    10
#define RSTERMINALCHARACTERWIDTH   8
#define RSTERMINALCHARACTERHEIGHT 14

/****************************************************************************
 Globals (across program)
 */

/****************************************************************************
 Local (static) Globals
 */

/* Here are the default commands that terminal have */
RsTerminal         *gCurrentTerminal = NULL;

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                           Terminal interface

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* terminal_help() and terminal_quit() are default command functions given to each Terminal */

/****************************************************************************
 terminal_help
 
 Print out a list of the registered commands for the current terminal (see
 gCurrentTerminal in RsTerminalDispatchCommand
 
 */

static void
terminal_help(RwChar *args __RWUNUSED__)
{
    int                 i, tabwidth, col;

    if (!gCurrentTerminal)
        return;

    /* pass one, compute a convenient tab width */
    tabwidth = 0;
    i = 0;
    while (gCurrentTerminal->commands[i].fp)
    {
        if (rwstrlen(gCurrentTerminal->commands[i].command) >
            (unsigned int) tabwidth)
            tabwidth = rwstrlen(gCurrentTerminal->commands[i].command);
        i++;
    }
    tabwidth += 4;
    tabwidth &= ~3;

    /* pass two, dump out the commands */
    i = 0;
    col = 0;
    while (gCurrentTerminal->commands[i].fp)
    {
        RsTerminalPrintf(gCurrentTerminal, RWSTRING("%-*s"), tabwidth,
                         gCurrentTerminal->commands[i].command);
        col += tabwidth;
        if (col + tabwidth >= (gCurrentTerminal->width - 2))
        {
            RsTerminalPrintf(gCurrentTerminal, RWSTRING("\n"));
            col = 0;
        }
        i++;
    }
    RsTerminalPrintf(gCurrentTerminal, RWSTRING("\n"));
}

/****************************************************************************
 terminal_quit - Get the skeleton to quit the app.
 */

static void
terminal_quit(RwChar *args __RWUNUSED__)
{
    RsEventHandler(rsQUITAPP, NULL);

    return;
}

static RsTerminalCommandDictionaryEntry *
GetDefaultCommands(RwInt32 * numDefaultCommands)
{
    static RsTerminalCommandDictionaryEntry defaultCommands[] =
    { 
        {RWSTRING("help"), terminal_help},
        {RWSTRING("quit"), terminal_quit},
        {RWSTRING(""),     NULL}
    };

    if (numDefaultCommands)
    {
        *numDefaultCommands =
            (sizeof(defaultCommands) / sizeof(defaultCommands[0])) - 1;
    }

    return defaultCommands;
}

/***********************************************************************************
 RsTerminalTabCompletion
 
 Complete the command line's current word from the terminal's completion dictionary
 
 On entry: Terminal
 On Exit : TRUE if we found a word to complete, FALSE otherwise
 */
RwBool
RsTerminalTabCompletion(RsTerminal * term)
{
    if (term)
    {
        if ((term->cursorX > 0) && term->commands)
        {
            RwChar             *line, *word, *match;
            RwInt32             i;

            term->screen[term->cursorY * term->width + term->cursorX] =
                '\0';

            line =
                (RwChar *) term->screen + (term->cursorY * term->width);
            word = rwstrrchr(line, ' ');

            /* might be first word on line */
            if (!word)
            {
                if ((RwInt32) rwstrlen(line) == term->cursorX)
                    word = line;
            }
            else
            {
                /* skip space */
                word++;
            }

            /* if we've got a incomplete word, attempt to find best match */
            if (word)
            {
                i = 0;
                while (term->commands[i].fp)
                {
                    match = rwstrstr(term->commands[i].command, word);
                    if (match == term->commands[i].command)
                    {
                        rwstrcpy(word, term->commands[i].command);
                        term->cursorX = rwstrlen(line);
                        return (TRUE);
                    }
                    i++;
                }
            }
        }
    }
    return (FALSE);
}

/****************************************************************************
 RsGetTerminalState

 On entry   : Terminal
 On exit    : the current state of the terminal
 */

RsTerminalState
RsTerminalGetState(RsTerminal * term)
{
    return (term->state);
}

/****************************************************************************
 RsSetTerminalState

 On entry   : Terminal
            : State wanted
 On exit    :
 */

void
RsTerminalSetState(RsTerminal * term, RsTerminalState state)
{
    /* RsTerminalClear(term); */
    term->state = state;

    return;
}

/****************************************************************************
 RsTerminalSetFontColor

 On entry   : Terminal
            : Foreground & Background colours
 On exit    :
 */

void
RsTerminalSetFontColor(RsTerminal * term, RwRGBA * fore, RwRGBA * back)
{
    RtCharsetSetColors(term->font, fore, back);

    return;
}

/****************************************************************************
 RsTerminalPrintChar

 On entry   : Terminal
            : Char to print (unsigned so scancode comparisons work hitch-free)
 On exit    :
 */

void
RsTerminalPrintChar(RsTerminal * term, RwUInt8 c)
{
    switch (c)
    {
        case 0xa:
            {
                /* LF - ignore ... */
                /* break; */
            }
        case 0xd:
            {
                /* CR */

                term->screen[term->cursorY * term->width +
                            term->cursorX] = '\0';

                term->cursorX = 0;
                term->cursorY++;

                while (term->cursorY >= term->height)
                {
                    RsTerminalScroll(term, term->height);
                    term->cursorY--;
                }
                break;
            }
        case 0x9:
            {
                /* Tab */
                if (!RsTerminalTabCompletion(term))
                {
                    term->cursorX = (term->cursorX + 8) & (~7);

                    if (term->cursorX >= term->width)
                    {
                        term->cursorX = 0;
                    }
                }
                break;
            }
        case 0x7f:
            {
                /* Del */

                break;
            }
        case 0x8:
            {
                /* backspace */

                if (term->cursorX)
                {
                    term->cursorX--;
                }

                term->screen[term->cursorY * term->width +
                            term->cursorX] = ' ';

                break;
            }
        default:
            {
                term->screen[term->cursorY * term->width +
                            term->cursorX] = (c);

                /* Move the cursor */

                term->cursorX++;

                if (term->cursorX >= term->width)
                {
                    term->cursorY++;
                    term->cursorX = 0;
                }

                while (term->cursorY >= term->height)
                {
                    RsTerminalScroll(term, term->height);

                    term->cursorY--;
                }
            }
    }

    return;
}

/****************************************************************************
 RsTerminalScroll

 Scroll the terminal up by a specified number of lines.

 On entry   : Terminal
            : lines - nbumber of lines to scroll this terminal up
 On exit    :
 */

void
RsTerminalScroll(RsTerminal * term, RwInt32 lines)
{
    RwInt32             i;
    RwChar             *src, *dst;

    src = term->screen + term->width * (term->lockedLines + 1);
    dst = term->screen + term->width * (term->lockedLines);

    for (i = 0; i < lines - 1 - term->lockedLines; i++)
    {
        memcpy(dst, src, term->width * sizeof(RwChar));

        src += term->width;
        dst += term->width;
    }

    memset(dst, 0x00, term->width * sizeof(RwChar));

    return;
}

/****************************************************************************
 RsTerminalHistoryScrollUp

 On entry   : Terminal
 On exit    :
 */

void
RsTerminalHistoryScrollUp(RsTerminal * term)
{
    if (term->history)
    {
        RwChar             *line;
        RwInt32             i;

        i = term->historyRead - 1;
        if (i < 0)
            i = term->historyLength - 1;
        line = (RwChar *) term->history + (i * term->width);
        if (rwstrlen(line) || i == term->historyWrite)
        {
            rwstrncpy((RwChar *) term->screen +
                      (term->cursorY) * term->width, line,
                      term->width - 1);
            term->cursorX = rwstrlen(line);
            term->historyRead = i;
        }
    }

    return;
}

/****************************************************************************
 RsTerminalHistoryScrollDown

 On entry   : Terminal
 On exit    :
 */

void
RsTerminalHistoryScrollDown(RsTerminal * term)
{
    if (term->history)
    {
        RwChar             *line;
        RwInt32             i;

        i = term->historyRead + 1;
        if (i >= term->historyLength)
            i = 0;
        line = (RwChar *) term->history + (i * term->width);
        if (rwstrlen(line) || i == term->historyWrite)
        {
            rwstrncpy((RwChar *) term->screen +
                      (term->cursorY) * term->width, line,
                      term->width - 1);
            term->cursorX = rwstrlen(line);
            term->historyRead = i;
        }
    }

    return;
}

/****************************************************************************
 TerminalClear

 On entry   : Terminal
 On exit    :
 */

void
RsTerminalClear(RsTerminal * term)
{
    memset(term->screen, 0x00, term->width * term->height * sizeof(RwChar));

    term->cursorX = 0;
    term->cursorY = term->height - 1;

    return;
}

/****************************************************************************
 RsTerminalClearLine

 On entry   : Terminal
            : line to clear
 On exit    :
 */

void
RsTerminalClearLine(RsTerminal * term, RwInt32 line)
{
    if ((line < term->height) && (line >= 0))
    {
        memset(((RwChar *)term->screen) + term->width * line, 0x00, term->width * sizeof(RwChar));
    }

    return;
}

/****************************************************************************
 RsTerminalClearRestOfLine

 Clear the rest of the specified line from the X position of the cursor

 On entry   : Terminal
            : line to clear end of
 On exit    :
 */

void
RsTerminalClearRestOfLine(RsTerminal * term, RwInt32 line)
{
    if ((line < term->height) && (line >= 0))
    {
        if (term->width - term->cursorX != 0)
        {
            memset(((RwChar *)term->screen) + term->width * line + term->cursorX,
                   0x00, (term->width - term->cursorX) * sizeof(RwChar));
        }
    }

    return;
}

/****************************************************************************
 RsTerminalLockLines

 On entry   : Terminal
            : Number of lines to lock - [0,last-2]
 On exit    : FALSE if too many or negative number. Won't lock _any_ if out of bounds
 */

RwBool
RsTerminalLockLines(RsTerminal * term, RwInt32 lines)
{
    /* Don't go beyond last-2 line */
    if ((lines > (term->height - 2)) || (lines < 0))
    {
        return (FALSE);
    }
    else
    {
        term->lockedLines = lines;
        if (term->cursorY < lines)
        {
            term->cursorY = lines;
        }
        return (TRUE);
    }

}

/****************************************************************************
 RsTerminalGetLockedLines

 On entry   : Terminal
 On exit    : Number of lines locked
 */

RwBool
RsTerminalGetLockedLines(RsTerminal * term)
{
    return (term->lockedLines);
}

/****************************************************************************
 TerminalPrint

 On entry   : Terminal
            : String to print
 On exit    :
 */

void
RsTerminalPrint(RsTerminal * term, const RwChar * text)
{
    while (*text)
    {
        RsTerminalPrintChar(term, (RwUInt8)(*text));

        /* Next char */

        text++;
    }

    return;
}

/****************************************************************************
 RsTerminalPrintOnLine

 On entry   : Terminal
            : String to print
            : Line to print string on
 On exit    : FALSE if line out of bounds (max penultimate)
 */

RwBool
RsTerminalPrintOnLine(RsTerminal * term, const RwChar * text, RwInt32 line)
{
    RwInt32             nX, nY;

    /* Don't go beyond penultimate line */
    if ((line > term->height) || (line < 0))
    {
        return FALSE;
    }

    RsTerminalClearLine(term, line);

    /* Save present pos */
    nX = term->cursorX;
    nY = term->cursorY;

    /* Output on line */
    term->cursorX = 0;
    term->cursorY = line;

    while (*text)
    {
        RsTerminalPrintChar(term, (RwUInt8)(*text));

        /* Next char */

        text++;
    }

    term->cursorX = nX;
    term->cursorY = nY;

    return TRUE;
}

/****************************************************************************
 RsTerminalPrintProtect

 On entry   : Terminal
            : String to print
 On exit    :
 */

void
RsTerminalPrintProtect(RsTerminal * term, const RwChar * text)
{
    RwInt32             x;

    /* make space on penultimate line */
    RsTerminalScroll(term, term->height - 1);
    x = term->cursorX;

    /* force output on penultimate line */
    term->cursorX = 0;
    term->cursorY = term->height - 2;
    term->height--;

    while (*text)
    {
        RsTerminalPrintChar(term, (RwUInt8)(*text));

        /* Next char */

        text++;
    }

    /* force back to last line */
    term->height++;
    term->cursorX = x;
    term->cursorY = term->height - 1;

    return;
}

/****************************************************************************
 RsTerminalPrintf

 On entry   : Terminal
            : String to print
 On exit    :
 */

void
RsTerminalPrintf(RsTerminal * term, const RwChar * sFormat, ...)
{
    RwChar              sBuffer[128];
    va_list             ap;
    long                args[12];
    RwInt32             i;

    va_start(ap, sFormat);
    for (i = 0; i < 12; i++)
        args[i] = va_arg(ap, long);

    rwsprintf(sBuffer, sFormat, args[0], args[1], args[2], args[3],
              args[4], args[5], args[6], args[7],
              args[8], args[9], args[10], args[11]);
    va_end(ap);
    RsTerminalPrint(term, sBuffer);

    return;
}

/****************************************************************************
 RsTerminalUpdate

 Read keyboard state and act accordingly
 If raster is passed in (i.e you want it to render the terminal) you must be
 inbetween a RwCameraBeing/EndUpdate() pair.

 On entry   : Terminal
            : key pressed (unsigned char so scancode comparisons work hitch-free)
            : raster to render to - if NULL, don't update visuals
 On exit    : string just ENTERed or NULL
 */

#if 0
void
showhistory(RsTerminal * term)
{
    int                 i;
    RwChar              buffer[80], *line;

    for (i = 0; i < term->historyLength; i++)
    {
        line = term->history + (i * term->width);
        if (line[0] == '\0')
            line = "NULL";
        sprintf(buffer, "%c%c:%s\n",
                ((i == term->historyRead) ? 'R' : ' '),
                ((i == term->historyWrite) ? 'W' : ' '), line);
        PsDisplayMessage(buffer);
    }
}

#endif

RwChar *
RsTerminalUpdate(RsTerminal * term, RwUInt8 key, RwRaster * raster)
{
    RwChar             *line = NULL;

    if (term)
    {
        RwInt32  lockedLines = RsTerminalGetLockedLines(term);
        RwUInt32 state       = RsTerminalGetState(term);

        /* Make sure the cursor is on a valid line */
        if (lockedLines > term->cursorY)
            term->cursorY = lockedLines;

        /* Read the keyboard if the terminal's active */
        if (state == RSTERMINALSTATEACTIVE)
        {
            /* restore from history file */
            if (key == rsUP)
                RsTerminalHistoryScrollUp(term);
            if (key == rsDOWN)
                RsTerminalHistoryScrollDown(term);

            /* is printable */
            if (key < 0x80)
            {
                RsTerminalPrintChar(term, key);
            }

            if (key == 0xd)
            {
                line =
                    (RwChar *) term->screen + (term->cursorY -
                                               1) * term->width;

                /* save in history file */
                if (!term->history)
                    RsTerminalHistoryCreate(term, 8);
                if (term->history && line[0])
                {
                    rwstrncpy((RwChar *) term->history +
                              (term->historyWrite * term->width), line,
                              term->width - 1);

                    term->historyWrite++;
                    if (term->historyWrite >= term->historyLength)
                        term->historyWrite = 0;

                    /* overwrite any entry */
                    term->history[term->historyWrite * term->width] =
                        '\0';
                    term->historyRead = term->historyWrite;
                }

                RsTerminalDispatchCommand(term, line);
            }
        }

        /* Move the terminal onto/off the screen */
        if (raster)
        {
            RwInt32             newPos;
            RwInt32             width, height;

            RsTerminalGetPixelSize(term, &width, &height);

            if (state == RSTERMINALSTATEOFF)
            {
                if (term->position > -height)
                {
                    newPos = (term->position + (-height)) >> 1;

                    (newPos ==
                     term->position) ? (term->
                                        position--) : (term->position =
                                                       newPos);
                }
            }
            else
            {
                if (term->position < 0)
                {
                    newPos = term->position >> 1;

                    (newPos ==
                     term->position) ? (term->
                                        position++) : (term->position =
                                                       newPos);
                }
            }

            /* Try to display the terminal */
            if (term->position > -height)
            {
                RsTerminalDisplay(term);
            }
        }
    }

    return (line);
}

/****************************************************************************
 RsTerminalDispatchCommand

 Takes an entered string and tries to dispatch it to a registered command handler

 On entry   : Terminal
            : string to try and fob off on a command handler
 On exit    :
 */

RwBool
RsTerminalDispatchCommand(RsTerminal * term, RwChar * buffer)
{
    RwChar             *command;
    RwInt32             i;

    /* So default func terminal_help knows which console to print to (yuck!) */
    gCurrentTerminal = term;

    /* dispatch command with any arguments */
    command = rwstrtok(buffer, RWSTRING(" "));
    if (command)
    {
        i = 0;
        while (term->commands[i].fp)
        {
            if (rwstrcmp(command, term->commands[i].command) == 0)
            {
                term->commands[i].fp(rwstrtok(NULL, RWSTRING("\0")));
                gCurrentTerminal = NULL;
                return (TRUE);
            }
            i++;
        }
        RsTerminalPrintf(term, RWSTRING("Unknown: <%s>\n"), command);
    }

    gCurrentTerminal = NULL;
    return (FALSE);
}

/****************************************************************************
 RsTerminalRender

 Renders the terminals display into the current context (so we should be between
 a RwRasterPush/PopContext() pair)

 On entry   : Terminal
 On exit    :
 */

RwBool
RsTerminalRender(RsTerminal * term, void *data __RWUNUSED__)
{
    if (term)
    {
        RwInt32             width, height, x;

        RsTerminalGetPixelSize(term, &width, &height);
        x =
            (RwRasterGetWidth
             (RwCameraGetRaster(RwCameraGetCurrentCamera())) -
             width) >> 1;

        RsTerminalRenderPosition(term, x, term->position);
        return (TRUE);
    }

    return (FALSE);
}

/****************************************************************************
 RsTerminalGetPixelSize

 On entry   : Terminal
            : Width (in pixels)
            : Height (in pixels)
 On exit    :
 */

void
RsTerminalGetPixelSize(RsTerminal * term, RwInt32 * width,
                       RwInt32 * height)
{
    if (term && term->font)
    {
        (*width) = term->width * RSTERMINALCHARACTERWIDTH;
        (*height) = term->height * RSTERMINALCHARACTERHEIGHT;
    }

    return;
}

/****************************************************************************
 RsTerminalDisplay

 Uses callback to display the terminals image
 If the default callback is in use, this assumes we're between a
 RwRasterPush/PopContext() pair.

 On entry   : Terminal
 On exit    : TRUE or FALSE
 */

RwBool
RsTerminalDisplay(RsTerminal * term)
{
    return (term->display(term, term->displayData));
}

/****************************************************************************
 RsTerminalRenderPosition

 Renders the terminals display into the current context (so we should be
 between a RwRasterPush/PopContext() pair), with the top-left corner at a
 specified location

 On entry   : Terminal
            : X Position
            : Y Position
 On exit    :
 */

void
RsTerminalRenderPosition(RsTerminal *term, RwInt32 x, RwInt32 y)
{
    RwChar             *cursor = term->screen;
    RwInt32             i;
    RwChar              tmpChar[2] = { '\0', '\0' };
    RwChar             *tmpString = RwMalloc(sizeof(RwChar) * (term->width + 1));

    if (tmpString)
    {
        for (i = 0; i < term->height * RSTERMINALCHARACTERHEIGHT;
             i += RSTERMINALCHARACTERHEIGHT)
        {
            if (tmpString)
            {
                memcpy(tmpString, cursor, term->width * sizeof(RwChar));
                tmpChar[term->width] = '\0';

                RtCharsetPrint(term->font, tmpString, x, i + y);

                /* Next line */
                cursor += term->width;
            }
        }
        RwFree(tmpString);
    }

    /* Blinking cursor */
    term->blinkCount++;

    if (term->blinkCount > (term->blinkWait >> 1))
    {
        tmpChar[0] = '_';

        RtCharsetPrint(term->font, tmpChar,
                       term->cursorX * RSTERMINALCHARACTERWIDTH + x,
                       term->cursorY * RSTERMINALCHARACTERHEIGHT + y);
    }

    if (term->blinkCount > term->blinkWait)
    {
        term->blinkCount = 0;
    }

    return;
}

/****************************************************************************
 RsTerminalRemoveCommand

 On entry   : Terminal
			: name of command to remove
 On exit    : TRUE or FALSE
 */
RwBool
RsTerminalRemoveCommand(RsTerminal * term, RwChar * command)
{
    if (term && command)
    {
        RwInt32             i;

        for (i = 0; i < term->numCommands; i++)
        {
            RsTerminalCommandDictionaryEntry *const target =
                &term->commands[i];

            if (!rwstrcmp(command, target->command))
            {
                RsTerminalCommandDictionaryEntry *const source =
                    &term->commands[term->numCommands - 1];

                *target = *source;
                *source = term->commands[term->numCommands--];
                break;

            }
        }
        if (i < term->numCommands)
            return (TRUE);
    }
    return (FALSE);
}

/****************************************************************************
 RsTerminalAddCommand

 On entry   : Terminal
			: CommandEntry to add
 On exit    : TRUE or FALSE
 */
RwBool
RsTerminalAddCommand(RsTerminal * term,
                     RsTerminalCommandDictionaryEntry * command)
{
    if (term && command && command->command && command->fp)
    {
        RsTerminalCommandDictionaryEntry *enlarged;
        RwInt32             bytes;

        bytes =
            (term->numCommands + 2) *
            sizeof(RsTerminalCommandDictionaryEntry);

        enlarged = RwRealloc(term->commands, bytes);

        if (enlarged)
        {
            RsTerminalCommandDictionaryEntry *const source =
                &enlarged[term->numCommands++];

            RsTerminalCommandDictionaryEntry *const target =

                &enlarged[term->numCommands];

            *target = *source;
            *source = *command;

            source->command = 
                RwMalloc((1 + rwstrlen(command->command)) * sizeof(RwChar));

            rwstrcpy((RwChar *)(source->command), 
                     command->command);

            term->commands = enlarged;

            return (TRUE);
        }
    }
    return (FALSE);
}

/****************************************************************************
 RsTerminalHistoryDestroy
 
 On entry   : Terminal
 On exit    :
 */
void
RsTerminalHistoryDestroy(RsTerminal * term)
{
    if (term->history)
    {
        RwFree(term->history);
        term->history = NULL;

        term->historyLength = 0;
        term->historyWrite = 0;
        term->historyRead = 0;
    }

    return;
}

/****************************************************************************
 RsTerminalHistoryCreate
 
 The history is a cyclic buffer of strings used to recall previously entered commands.
 
 On entry   : Terminal
			: history length
 On exit    :
 */
RwBool
RsTerminalHistoryCreate(RsTerminal * term, RwInt32 nLength)
{
    term->history = (RwChar *) RwMalloc(term->width * nLength * sizeof(RwChar));
    if (term->history)
    {
        memset(term->history, 0x00, term->width * nLength * sizeof(RwChar));
        term->historyLength = nLength;
        term->historyWrite = 0;
        term->historyRead = 0;
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

/****************************************************************************
 RsTerminalDestroy

 On entry   : Terminal
 On exit    :
 */

void
RsTerminalDestroy(RsTerminal * term)
{
    if (term)
    {
        RsTerminalHistoryDestroy(term);
        RtCharsetDestroy(term->font);

        if (term->commands)
        {
            RsTerminalCommandDictionaryEntry *command = term->commands;

            while (command->fp)
            {
                RwFree((void *)command->command);
                command->command = NULL;

                command++;
            }
            RwFree((void *)command->command);
            command->command = NULL;

            RwFree(term->commands);
            term->commands = NULL;
        }

        RwFree(term);
        term = NULL;
    }

    return;
}

/****************************************************************************
 RsTerminalCreate

 On entry   : Width
            : Height
            : Foreground and background colour
            : Display callback
            : Data for the display callback (if callback is NULL this should be
              the raster of the current camera)
 On exit    : Terminal (else null)
 */

RsTerminal         *
RsTerminalCreate(RwInt32 width, RwInt32 height, RwRGBA * fCol,
                 RwRGBA * bCol, RsTerminalDisplayCallBack display,
                 void *data)
{
    static RwRGBA       fore = { 0, 0xff, 0xff, 0xff };
    static RwRGBA       back = { 0, 0, 0, 0 };
    RsTerminal         *term;
    RwInt32             numDefaultCommands;
    RsTerminalCommandDictionaryEntry *DefaultCommands;

    DefaultCommands = GetDefaultCommands(&numDefaultCommands);

    if (fCol)
        fore = *fCol;
    if (bCol)
        back = *bCol;

    term = RwMalloc(sizeof(RsTerminal) + width * height * sizeof(RwChar));

    if (!term)
    {
        return NULL;
    }

    /* Start with the console off so it can scroll on at the start if the app wants it
     * (rather than having to scroll off if it doesn't) */
    term->state = RSTERMINALSTATEOFF;

    term->historyLength = 0;
    term->history = NULL;

    term->screen = (RwChar *) (term + 1);

    term->width = width;
    term->height = height;
    term->lockedLines = 0;

    term->blinkWait = RSTERMINALDEFAULTBLINK;
    term->blinkCount = 0;

    term->cursorX = 0;
    term->cursorY = term->height - 1;

    /* Set the callback */
    term->display = display ? display : RsTerminalRender;
    term->displayData = data;

    /* Clear the terminal initially */
    RsTerminalClear(term);

    /* Create the font */
    term->font = RtCharsetCreate(&fore, &back);

    /* Make the terminal start off screen */
    {
        RwInt32             nW, nH;

        RsTerminalGetPixelSize(term, &nW, &nH);
        term->position = -nH;
    }

    if (!term->font)
    {
        RwFree(term);
        term = NULL;

        return term;
    }

    /* Set some default commands */
    term->numCommands = numDefaultCommands;
    term->commands =
        RwMalloc(sizeof(RsTerminalCommandDictionaryEntry) *
                 (numDefaultCommands + 1));
    if (term->commands)
    {
        RwInt32             i, length;

        /* Copy the command strings */
        for (i = 0; i <= numDefaultCommands; i++)
        {
            term->commands[i].fp = DefaultCommands[i].fp;
            length = rwstrlen(DefaultCommands[i].command);

            term->commands[i].command = 
                RwMalloc((1 + length) * sizeof(RwChar));
            rwstrcpy((RwChar *)(term->commands[i].command),
                     DefaultCommands[i].command);
        }
    }
    else
    {
        RtCharsetDestroy(term->font);

        RwFree(term);
        term = NULL;

        return (term);
    }

    /* All done */
    return (term);
}
