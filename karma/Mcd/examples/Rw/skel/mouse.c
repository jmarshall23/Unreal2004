#ifdef RWMOUSE

#include <stdio.h>             /* TBD - remove */

#include "mouse.h"
#include "mousedat.h"
#include "platform.h"

static RwV2d        MousePos = { 0.0f, 0.0f };
static RwRaster    *CursorRaster = NULL;
static RwBool       DrawMouse = TRUE;

void
rsMouseVisible(RwBool visible)
{
    DrawMouse = visible;
}

void
rsMouseAddDelta(RwV2d * delta)
{
    RwV2dAdd(&MousePos, &MousePos, delta);
}

void
rsMouseGetPos(RwV2d * pos)
{
    *pos = MousePos;
}

void
rsMouseSetPos(RwV2d * pos)
{
    MousePos = *pos;
}

void
rsMouseRender(RwCamera * camera)
{
    /* clamp mouse to camera limits */

    if (MousePos.x < 0)
    {
        MousePos.x = 0;
    }
    if (MousePos.y < 0)
    {
        MousePos.y = 0;
    }
    if (MousePos.x > RwRasterGetWidth(RwCameraGetRaster(camera)))
    {
        MousePos.x =
            (RwReal) RwRasterGetWidth(RwCameraGetRaster(camera));
    }
    if (MousePos.y > RwRasterGetHeight(RwCameraGetRaster(camera)))
    {
        MousePos.y =
            (RwReal) RwRasterGetHeight(RwCameraGetRaster(camera));
    }

    if (CursorRaster && DrawMouse)
    {
        if (RwRasterPushContext(RwCameraGetRaster(camera)))
        {
            RwRasterRender(CursorRaster,
                           (RwInt32) MousePos.x, (RwInt32) MousePos.y);
            RwRasterPopContext();
        }
    }
}

RwBool
rsMouseTerm(void)
{
    if (CursorRaster)
    {
        RwRasterDestroy(CursorRaster);
        CursorRaster = NULL;
        return (FALSE);
    }

    return (TRUE);
}

RwBool
rsMouseInit(void)
{
    RwInt32             rasterHeight;
    RwInt32             rasterWidth;
    RwInt32             rasterDepth;

    RwInt32             rasterFlags;
    RwImage            *image;

    image = RwImageCreate(11, 19, 32);
    RwImageSetStride(image, 11 * 4);
    RwImageSetPixels(image, mousedat);

    RwImageFindRasterFormat(image, rwRASTERTYPENORMAL,
                            &rasterWidth, &rasterHeight,
                            &rasterDepth, &rasterFlags);

    /* Gamma correct the image */
    RwImageGammaCorrect(image);

    /* Create a raster */
    CursorRaster =
        RwRasterCreate(rasterWidth, rasterHeight, rasterDepth,
                       rasterFlags);
    if (!CursorRaster)
    {
        RwImageDestroy(image);
        return (FALSE);
    }

    /* Convert the image into the raster */
    RwRasterSetFromImage(CursorRaster, image);
    RwImageDestroy(image);

    /* If platform already has mouse support then turn it off */
    psMouseSetVisibility(FALSE);

    return (TRUE);
}

#endif /* RWMOUSE */
