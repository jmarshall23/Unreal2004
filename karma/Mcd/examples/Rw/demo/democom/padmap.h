#ifndef PADMAP_H
#define PADMAP_H

#include <rwcore.h>
#include "skeleton.h"

/****************************************************************************
 Function prototypes
 */
#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern void PadMouse(RsEvent event, void *param);
extern void PadCursor(RsEvent event, void *param);
extern void MousePad(RsEvent event, void *param);
extern void KeysPad(RsEvent event, void *param);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif
