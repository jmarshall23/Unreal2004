/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:40 $ - Revision: $Revision: 1.17.8.3 $

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

#include <MeViewer.h>
#include <RMenu.h>
#include <MeMemory.h>

const MeReal menuOrigin[2]       = {200, 30};
const MeReal menuEntrySpacing[2] = {0, 25};

const MeReal valueOffset         = 300;
const MeReal toggleStatusOffset  = 300;

char         helpStateWas        = 0;

const char   *menuHelp[] = {
                              "Select:",
                              "  $UP",
                              "  $DOWN",
                              " ",
                              "Modify value:",
                              "  $LEFT",
                              "  $RIGHT",
                              " ",
                              "Submenu:",
                              "  $LEFT",
                              "  $RIGHT",
                              "\0"
                           };

/************************** INTERNAL UTILITIES ****************************/
RMenuBaseEntry* FindLastMenuEntry(RMenu* menu)
{
    RMenuBaseEntry* lastEntry = menu->entryHead;

    /* If there is at least one thing in the list... */
    if(lastEntry)
    {
        while(lastEntry->next)
            lastEntry = lastEntry->next;

        return lastEntry;
    }
    else
        return 0;
}

void AddEntryToMenuEnd(RMenuBaseEntry* newEntry, RMenu* menu)
{
    /* Find last entry in list. */
    RMenuBaseEntry* lastEntry = FindLastMenuEntry(menu);

    if(lastEntry)
    {
        /* And add this node after it. */
        lastEntry->next = newEntry;
        newEntry->prev = lastEntry;
        newEntry->next = 0;
    }
    else /* Otherwise - easy! */
    {
        menu->entryHead = newEntry;
        newEntry->prev = 0;
        newEntry->next = 0;
    }
}

void MEAPI HighlightCurrent(RMenu* rm)
{
    if(rm->entryCurrent)
        rm->entryCurrent->highlight(rm->entryCurrent);
}

void MEAPI UnhighlightCurrent(RMenu* rm)
{
    if(rm->entryCurrent)
        rm->entryCurrent->unhighlight(rm->entryCurrent);
}

/************************** BASE ENTRY FUNCTIONS ****************************/

void MEAPI BaseEntryInit(RMenuBaseEntry* entry, RMenu* menu, const char* name)
{
    strcpy(entry->name, name);
    entry->nameGraphic = 0;

    entry->execute1 = 0;
    entry->execute2 = 0;
    entry->display = 0;
    entry->undisplay = 0;
    entry->highlight = 0;
    entry->unhighlight = 0;

    entry->menu = menu;

    entry->next = 0;
    entry->prev = 0;
}

/************************** TOGGLE FUNCTIONS ********************************/

RGraphic* CreateToggleStatus(RRender* rc, MeBool status, MeReal x, MeReal y,
              const float color[4])
{
    char statusText[50];
    if(status)
        strcpy(statusText, "YES");
    else
        strcpy(statusText, "NO");

    return RGraphicTextCreate(rc, statusText, x, y, color);
}

void MEAPI ToggleDisplay(RMenuBaseEntry* entry, MeReal x, MeReal y)
{
    RMenuToggleEntry* toggleEntry = (RMenuToggleEntry*)entry;


    if(entry->nameGraphic)
        return; /* already displaying! */

    entry->nameGraphic = RGraphicTextCreate(entry->menu->rc,
        entry->name, x, y, entry->menu->textColor);

    toggleEntry->statusPos[0] = x+toggleStatusOffset;
    toggleEntry->statusPos[1] = y;

    toggleEntry->statusGraphic = CreateToggleStatus(entry->menu->rc,
        toggleEntry->toggleValue,
        toggleEntry->statusPos[0], toggleEntry->statusPos[1],
        entry->menu->textColor);
}

void MEAPI ToggleUndisplay(RMenuBaseEntry* entry)
{
    RMenuToggleEntry* toggleEntry = (RMenuToggleEntry*)entry;
    if(entry->nameGraphic)
    {
        RGraphicDelete(entry->menu->rc, entry->nameGraphic, 1);
        entry->nameGraphic = 0;

        RGraphicDelete(entry->menu->rc, toggleEntry->statusGraphic, 1);
        toggleEntry->statusGraphic = 0;
    }
}

void MEAPI ToggleHighlight(RMenuBaseEntry* entry)
{
    /* RMenuToggleEntry* toggleEntry = (RMenuToggleEntry*)entry; */
    if(entry->nameGraphic)
        RGraphicSetColor(entry->nameGraphic, entry->menu->highlightColor);
}

void MEAPI ToggleUnhighlight(RMenuBaseEntry* entry)
{
    /* RMenuToggleEntry* toggleEntry = (RMenuToggleEntry*)entry; */
    if(entry->nameGraphic)
        RGraphicSetColor(entry->nameGraphic, entry->menu->textColor);
}

void MEAPI ToggleExecute(RMenuBaseEntry* entry)
{
    RMenuToggleEntry* toggleEntry = (RMenuToggleEntry*)entry;

    toggleEntry->toggleValue = ! toggleEntry->toggleValue;

    if(toggleEntry->toggleCallback)
        toggleEntry->toggleCallback(toggleEntry->toggleValue);

    RGraphicDelete(entry->menu->rc, toggleEntry->statusGraphic, 1);

    toggleEntry->statusGraphic = CreateToggleStatus(entry->menu->rc,
        toggleEntry->toggleValue,
        toggleEntry->statusPos[0], toggleEntry->statusPos[1],
        entry->menu->textColor);
}

/**
   Add a `toggle' entry to a menu.

   This is an entry type that has two states, on and off, which are toggled
   by pressing the button when the entry is highlighted. The callback is
   called with the new value for each change of state.

   @param rm The menu to which the entry will be added.
   @param name The text to be displayed for this menu entry.
   @param func The function that will be called when this menu entry is
   selected.
   @param defaultValue The initial value for this toggle when it is created.
*/
void MEAPI RMenuAddToggleEntry(RMenu* rm, const char * name,
               RMenuToggleCallback func, MeBool defaultValue)
{
    RMenuToggleEntry* toggleEntry = (RMenuToggleEntry*)
        MeMemoryAPI.create(sizeof(RMenuToggleEntry));

    BaseEntryInit((RMenuBaseEntry*)toggleEntry, rm, name);

    toggleEntry->base.execute1 = ToggleExecute;
    toggleEntry->base.execute2 = ToggleExecute;
    toggleEntry->base.display = ToggleDisplay;
    toggleEntry->base.undisplay = ToggleUndisplay;
    toggleEntry->base.highlight = ToggleHighlight;
    toggleEntry->base.unhighlight = ToggleUnhighlight;

    toggleEntry->statusGraphic = 0;
    toggleEntry->toggleValue = defaultValue;
    toggleEntry->toggleCallback = func;

    AddEntryToMenuEnd((RMenuBaseEntry*)toggleEntry, rm);
}

/************************** VALUE FUNCTIONS ********************************/

RGraphic* CreateValueText(RRender* rc, MeReal value, MeReal x, MeReal y,
              const float color[4])
{
    char valueText[50];

    sprintf(valueText, "% 2.2f", value);

    return RGraphicTextCreate(rc, valueText, x, y, color);
}

void MEAPI ValueDisplay(RMenuBaseEntry* entry, MeReal x, MeReal y)
{
    RMenuValueEntry* valueEntry = (RMenuValueEntry*)entry;

    if(entry->nameGraphic)
        return;

    entry->nameGraphic = RGraphicTextCreate(entry->menu->rc,
        entry->name, x, y, entry->menu->textColor);

    valueEntry->valuePos[0] = x + valueOffset;
    valueEntry->valuePos[1] = y;

    valueEntry->valueGraphic = CreateValueText(entry->menu->rc,
        valueEntry->value,
        valueEntry->valuePos[0], valueEntry->valuePos[1],
        entry->menu->textColor);
}

void MEAPI ValueUndisplay(RMenuBaseEntry* entry)
{
    RMenuValueEntry* valueEntry = (RMenuValueEntry*)entry;

    if(entry->nameGraphic)
    {
        RGraphicDelete(entry->menu->rc, entry->nameGraphic, 1);
        entry->nameGraphic = 0;

        RGraphicDelete(entry->menu->rc, valueEntry->valueGraphic, 1);
        valueEntry->valueGraphic = 0;
    }
}

void MEAPI ValueHighlight(RMenuBaseEntry* entry)
{
    /* RMenuValueEntry* valueEntry = (RMenuValueEntry*)entry; */
    if(entry->nameGraphic)
        RGraphicSetColor(entry->nameGraphic, entry->menu->highlightColor);
}

void MEAPI ValueUnhighlight(RMenuBaseEntry* entry)
{
    /* RMenuValueEntry* valueEntry = (RMenuValueEntry*)entry; */
    if(entry->nameGraphic)
        RGraphicSetColor(entry->nameGraphic, entry->menu->textColor);
}

void MEAPI ValueExecute1(RMenuBaseEntry* entry)
{
    RMenuValueEntry* valueEntry = (RMenuValueEntry*)entry;

    valueEntry->value += valueEntry->increment;

    if(valueEntry->value > valueEntry->hi)
        valueEntry->value = valueEntry->hi;

    if(valueEntry->valueCallback)
        valueEntry->valueCallback(valueEntry->value);

    RGraphicDelete(entry->menu->rc, valueEntry->valueGraphic, 1);

    valueEntry->valueGraphic = CreateValueText(entry->menu->rc,
        valueEntry->value,
        valueEntry->valuePos[0], valueEntry->valuePos[1],
        entry->menu->textColor);
}

void MEAPI ValueExecute2(RMenuBaseEntry* entry)
{
    RMenuValueEntry* valueEntry = (RMenuValueEntry*)entry;


    valueEntry->value -= valueEntry->increment;

    if(valueEntry->value < valueEntry->lo)
        valueEntry->value = valueEntry->lo;

    if(valueEntry->valueCallback)
        valueEntry->valueCallback(valueEntry->value);

    RGraphicDelete(entry->menu->rc, valueEntry->valueGraphic, 1);

    valueEntry->valueGraphic = CreateValueText(entry->menu->rc,
        valueEntry->value,
        valueEntry->valuePos[0], valueEntry->valuePos[1],
        entry->menu->textColor);
}

/**
   Add a `value' entry to a menu.

   This is a menu entry type which provides a variable which can be modified
   by a specified stepsize between a minimum and maximum value by pressing
   the appropriate buttons when the entry is highlighted. The callback is
   called with the new value for each change of state.

   @param rm The menu to which the entry will be added.
   @param name The text to be displayed for this menu entry.
   @param func The function to be called when the value is changed.
   @param hi Maximum value.
   @param lo Minimum value.
   @param increment Amount by which the value is changed for each button
   press.
   @param defaultValue The starting point for the value.

*/
void MEAPI RMenuAddValueEntry(RMenu* rm, const char * name,
               RMenuValueCallback func, MeReal hi, MeReal lo,
               MeReal increment, MeReal defaultValue)
{
    RMenuValueEntry* valueEntry = (RMenuValueEntry*)
        MeMemoryAPI.create(sizeof(RMenuValueEntry));

    BaseEntryInit((RMenuBaseEntry*)valueEntry, rm, name);

    valueEntry->base.execute1 = ValueExecute1;
    valueEntry->base.execute2 = ValueExecute2;
    valueEntry->base.display = ValueDisplay;
    valueEntry->base.undisplay = ValueUndisplay;
    valueEntry->base.highlight = ValueHighlight;
    valueEntry->base.unhighlight = ValueUnhighlight;

    valueEntry->hi = hi;
    valueEntry->lo = lo;
    valueEntry->increment = increment;

    if(defaultValue > valueEntry->hi)
        valueEntry->value = valueEntry->hi;
    else if(defaultValue < valueEntry->lo)
        valueEntry->value = valueEntry->lo;
    else
        valueEntry->value = defaultValue;

    valueEntry->valueGraphic = 0;
    valueEntry->valueCallback = func;

    AddEntryToMenuEnd((RMenuBaseEntry*)valueEntry, rm);
}

/************************** SUBMENU FUNCTIONS ********************************/
void MEAPI SubmenuDisplay(RMenuBaseEntry* entry, MeReal x, MeReal y)
{
    /* RMenuValueEntry* submenuEntry = (RMenuValueEntry*)entry; */

    if(entry->nameGraphic)
        return; /* already displaying! */

    entry->nameGraphic = RGraphicTextCreate(entry->menu->rc,
        entry->name, x, y, entry->menu->textColor);
}

void MEAPI SubmenuUndisplay(RMenuBaseEntry* entry)
{
    /* RMenuValueEntry* submenuEntry = (RMenuValueEntry*)entry; */
    if(entry->nameGraphic)
    {
        RGraphicDelete(entry->menu->rc, entry->nameGraphic, 1);
        entry->nameGraphic = 0;
    }
}

void MEAPI SubmenuHighlight(RMenuBaseEntry* entry)
{
    /* RMenuValueEntry* submenuEntry = (RMenuValueEntry*)entry; */
    if(entry->nameGraphic)
        RGraphicSetColor(entry->nameGraphic, entry->menu->highlightColor);
}

void MEAPI SubmenuUnhighlight(RMenuBaseEntry* entry)
{
    /* RMenuValueEntry* submenuEntry = (RMenuValueEntry*)entry; */
    if(entry->nameGraphic)
        RGraphicSetColor(entry->nameGraphic, entry->menu->textColor);
}

void MEAPI SubmenuExecute(RMenuBaseEntry* entry)
{
    RMenuSubmenuEntry* submenuEntry = (RMenuSubmenuEntry*)entry;
    int savedHelpState = helpStateWas;

    RRenderHideCurrentMenu(entry->menu->rc);

    if(submenuEntry->subMenu) {
        RMenuDisplay(submenuEntry->subMenu);
    }

    helpStateWas = savedHelpState;
}

/**
   Add a `sub-menu' entry to a menu.

   This is an entry type that, when selected, opens another menu.

   @param rm The menu to which this submenu entry will be added.
   @param name The text to be displayed that represents this menu entry.
   @param submenu The menu that will be displayed when this entry is selected.
*/
void MEAPI RMenuAddSubmenuEntry(RMenu* rm, const char * name, RMenu* submenu)
{
    RMenuSubmenuEntry* submenuEntry = (RMenuSubmenuEntry*)
        MeMemoryAPI.create(sizeof(RMenuSubmenuEntry));

    BaseEntryInit((RMenuBaseEntry*)submenuEntry, rm, name);

    submenuEntry->base.execute1 = SubmenuExecute;
    submenuEntry->base.execute2 = SubmenuExecute;
    submenuEntry->base.display = SubmenuDisplay;
    submenuEntry->base.undisplay = SubmenuUndisplay;
    submenuEntry->base.highlight = SubmenuHighlight;
    submenuEntry->base.unhighlight = SubmenuUnhighlight;

    submenuEntry->subMenu = submenu;

    AddEntryToMenuEnd((RMenuBaseEntry*)submenuEntry, rm);
}


/************************** MENU FUNCTIONS ****************************/
/**
   Create a new on-screen menu.

   @param rc The render context that will display the menu.
   @param name The title of the new menu.
   @return A pointer to the new menu.
 */
RMenu* MEAPI RMenuCreate(RRender* rc, const char* name)
{
    RMenu* menu = (RMenu*)MeMemoryAPI.create(sizeof(RMenu));

    strcpy(menu->name, name);

    menu->nameGraphic = 0;
    menu->helpGraphic = 0;
    menu->entryHead = 0;
    menu->entryCurrent = 0;
    menu->rc = rc;
    menu->parentMenu = 0;

    menu->textColor[0] = 1;
    menu->textColor[1] = 0;
    menu->textColor[2] = 0;
    menu->textColor[3] = 1;

    menu->highlightColor[0] = 1;
    menu->highlightColor[1] = 1;
    menu->highlightColor[2] = 0;
    menu->highlightColor[3] = 1;

    menu->titleColor[0] = 0;
    menu->titleColor[1] = 0;
    menu->titleColor[2] = 1;
    menu->titleColor[3] = 1;

    return menu;
}

/**
   Destroy a menu.

   @param rm The menu to destroy.
 */
void MEAPI RMenuDestroy(RMenu* rm)
{
    RMenuBaseEntry* entry = rm->entryHead;

    while(entry)
    {
        RMenuBaseEntry* nextEntry = entry->next;
        MeMemoryAPI.destroy(entry);
        entry = nextEntry;
    }

    MeMemoryAPI.destroy(rm);
}


void MEAPI RRenderHideCurrentMenu(RRender* rc)
{
    if(rc->m_MenuCurrent)
    {
        RMenuBaseEntry* entry = rc->m_MenuCurrent->entryHead;

        RGraphicDelete(rc->m_MenuCurrent->rc, rc->m_MenuCurrent->nameGraphic, 1);
        RGraphicDelete(rc->m_MenuCurrent->rc, rc->m_MenuCurrent->helpGraphic, 1);

        /* destroy graphics for the menu. */
        while(entry)
        {
            entry->undisplay(entry);
            entry = entry->next;
        }

        rc->m_MenuCurrent = 0;

        if (helpStateWas) {
          RRenderToggleUserHelp(rc);
        }
    }
}

/**
   Display a menu.

   @param rm The menu to display.

   N.B. The render context in which this menu will be displayed is found out
   from rm.
 */
void MEAPI RMenuDisplay(RMenu* rm)
{

    if(!rm->rc->m_MenuCurrent)
    {
        int i = 0;
        float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        char *text;
        int length = 0;

        RMenuBaseEntry* entry = rm->entryHead;

        rm->nameGraphic = RGraphicTextCreate(rm->rc, rm->name,
            menuOrigin[0], menuOrigin[1], rm->titleColor);

        for( i = 0; i < menuHelp[i][0]; i++ ) {
          length += strlen(menuHelp[i]) + 1;
        }
        length++;

        text = (char *)MeMemoryAPI.create(length);
        text[0] = '\0';
        for( i = 0; i < menuHelp[i][0]; i++ ) {
          strcat(text, menuHelp[i]); strcat(text, "\n");
        }
        rm->helpGraphic = RGraphicTextCreate(rm->rc, text, 0, 0,
                                             white);
        MeMemoryAPI.destroy(text);

        /* create graphics for the menu. */
        i = 0;
        while(entry)
        {
            entry->display(entry, menuOrigin[0] + ((2+i)*menuEntrySpacing[0]),
                menuOrigin[1] + ((2+i)*menuEntrySpacing[1]));

            entry = entry->next;
            i++;
        }

        rm->entryCurrent = rm->entryHead;
        HighlightCurrent(rm);

        helpStateWas = rm->rc->m_isHelpDisplayed;
        if (helpStateWas)
          RRenderToggleUserHelp(rm->rc);

        rm->rc->m_MenuCurrent = rm;

    }
    else
    {
        MeWarning(0, "RMenuDisplay: An RMenu is already being "
            "displayed!");
    }
}

/**
   Make a menu the default menu in a given render context.

   This means that this menu will be the one that appears when the menu key
   is pressed.

   @param rc The render context in which the menu is to be made the default.
   @param menu The menu to make the default.
*/
void MEAPI RRenderSetDefaultMenu(RRender *rc, RMenu* menu)
{
    rc->m_MenuDefault = menu;
}

void MEAPI RRenderDisplayDefaultMenu(RRender *rc)
{
    if(rc->m_MenuDefault)
        RMenuDisplay(rc->m_MenuDefault);
}

RMenu* MEAPI RRenderGetCurrentMenu(RRender* rc)
{
    return rc->m_MenuCurrent;
}

void MEAPI RMenuNextEntry(RRender* rc)
{
    /* If no menu currently displayed, do nothing. */
    if(!rc->m_MenuCurrent)
        return;

    if(rc->m_MenuCurrent->entryCurrent)
    {
        UnhighlightCurrent(rc->m_MenuCurrent);

        if(rc->m_MenuCurrent->entryCurrent->next)
            rc->m_MenuCurrent->entryCurrent = rc->m_MenuCurrent->entryCurrent->next;
        else
            rc->m_MenuCurrent->entryCurrent = rc->m_MenuCurrent->entryHead;

    }
    /* Shouldn't really be, but if nothing is current - pick first. */
    else if(rc->m_MenuCurrent->entryHead)
        rc->m_MenuCurrent->entryCurrent = rc->m_MenuCurrent->entryHead;

    HighlightCurrent(rc->m_MenuCurrent);
}

void MEAPI RMenuPreviousEntry(RRender* rc)
{
    /* If no menu currently displayed, do nothing. */
    if(!rc->m_MenuCurrent)
        return;

    if(rc->m_MenuCurrent->entryCurrent)
    {
        UnhighlightCurrent(rc->m_MenuCurrent);

        if(rc->m_MenuCurrent->entryCurrent->prev)
            rc->m_MenuCurrent->entryCurrent = rc->m_MenuCurrent->entryCurrent->prev;
        else
            rc->m_MenuCurrent->entryCurrent = FindLastMenuEntry(rc->m_MenuCurrent);

    }
    /* Shouldn't reach this, but if nothing is current - pick first. */
    else if(rc->m_MenuCurrent->entryHead)
        rc->m_MenuCurrent->entryCurrent = rc->m_MenuCurrent->entryHead;

    HighlightCurrent(rc->m_MenuCurrent);
}

void MEAPI RMenuExecute1Entry(RRender* rc)
{
    /* If no menu currently displayed, do nothing. */
    if(!rc->m_MenuCurrent)
        return;

    if(rc->m_MenuCurrent->entryCurrent)
        rc->m_MenuCurrent->entryCurrent->execute1(rc->m_MenuCurrent->entryCurrent);
}

void MEAPI RMenuExecute2Entry(RRender* rc)
{
    /* If no menu currently displayed, do nothing. */
    if(!rc->m_MenuCurrent)
        return;

    if(rc->m_MenuCurrent->entryCurrent)
        rc->m_MenuCurrent->entryCurrent->execute2(rc->m_MenuCurrent->entryCurrent);
}
