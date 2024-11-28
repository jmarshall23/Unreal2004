#ifndef PLATFORM_H
#define PLATFORM_H

#include <rwcore.h>
#include "vecfont.h"

extern void psWindowSetText(const RwChar *text);
extern void psErrorMessage(const RwChar *text);
extern void psWarningMessage(const RwChar *text);
extern void psDebugMessageHandler(RwDebugType type, const RwChar *str);

extern RwUInt32 psTimer(void);

extern RwChar *psPathnameCreate(const RwChar *srcBuffer);
extern void psPathnameDestroy(RwChar *buffer);
extern RwChar psPathGetSeparator(void);

extern RwBool psInitialise(void);
extern void   psTerminate(void);
extern RwBool psAlwaysOnTop( RwBool AlwaysOnTop );
extern void psCameraShowRaster(RwCamera *camera);
extern void psMouseSetVisibility(RwBool visible);
extern void psMouseSetPos(RwV2d *pos);

extern RwBool psSelectDevice(void);

/* return memory function overload if required */
extern RwMemoryFunctions *psGetMemoryFunctions(void);

/* install the platform specific file system */
extern RwBool psInstallFileSystem(void);

/* Render platform specific metrics */
extern void psMetricsRender(VecFont *vecFont, RwV2d *pos, RwMetrics *metrics);

/* Handle native texture support */
extern RwBool psNativeTextureSupport(void);

#endif
