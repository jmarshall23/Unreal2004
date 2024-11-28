/****************************************************************************
 *
 * File :     events.h
 *
 * Abstract : Handles pc mouse and keyboard and sky pad events
 *
 ****************************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#include "skeleton.h"

#ifdef    __cplusplus
extern "C"
{
#endif

extern RsEventStatus KeyboardHandler(RsEvent event, void *param);
extern RsEventStatus MouseHandler(RsEvent event, void *param);
extern RsEventStatus PadHandler(RsEvent event, void *param);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* EVENTS_H */
