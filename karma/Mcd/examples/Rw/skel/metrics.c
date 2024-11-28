/****************************************************************************
 *
 * File :     metrics.c
 *
 * Abstract : 
 *
 ****************************************************************************
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
 ****************************************************************************/

/*--- Include files ---*/
#include <stdio.h>
#include <rwcore.h>
#include "platform.h"
#include "vecfont.h"

static VecFont      *vecFont;
static RWIM2DVERTEX box[4];

void 
RsMetricsRender(void)
{
    /* Get the stats, and print them */
    RwMetrics *metrics = RwEngineGetMetrics();

    if (vecFont && metrics)
    {
        RwChar message[200];
        RwV2d pos = {(RwReal)(10.0f), (RwReal)(10.0f)};

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);

        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, &box, 4);

        sprintf(message, "numTriangles = %08d", metrics->numTriangles);
        RsVecFontPrint(vecFont, &pos, message);
        pos.y += (RwReal)(10.0f);

        sprintf(message, "numProcTriangles = %08d", metrics->numProcTriangles);
        RsVecFontPrint(vecFont, &pos, message);
        pos.y += (RwReal)(10.0f);

        sprintf(message, "numVertices = %08d", metrics->numVertices);
        RsVecFontPrint(vecFont, &pos, message);
        pos.y += (RwReal)(10.0f);

        sprintf(message, "numResourceAllocs = %08d", metrics->numResourceAllocs);
        RsVecFontPrint(vecFont, &pos, message);
        pos.y += (RwReal)(10.0f);

        sprintf(message, "numTextureUploads = %08d", metrics->numTextureUploads);
        RsVecFontPrint(vecFont, &pos, message);
        pos.y += (RwReal)(10.0f);

        /* Now the device specific ones */
        psMetricsRender(vecFont, &pos, metrics);

        /* Feed the size back into the box, so we get it right next frame */
        RWIM2DVERTEXSetScreenY(&box[1], pos.y + (RwReal)(7.0f));
        RWIM2DVERTEXSetScreenY(&box[3], pos.y + (RwReal)(7.0f));

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE);
    }
}

void 
RsMetricsClose(void)
{
    if (vecFont)
    {
        RsVecFontDestroy(vecFont);
    }

    RsVecFontClose();
}

void 
RsMetricsOpen(const RwCamera *camera)
{
    RwV2d  size = {(RwReal)(10.0f), (RwReal)(10.0f)};
    RwRGBA color = {255, 255, 255, 255};

    if (RsVecFontOpen())
    {
        vecFont = RsVecFontCreate(camera, &color, &size);
    }

    /* Might want to grow this dynamically to contain the number of metric
     * lines pertaining to this platform.
     */
    RWIM2DVERTEXSetScreenX(&box[0], (RwReal)(0.0f));
    RWIM2DVERTEXSetScreenY(&box[0], (RwReal)(0.0f));
    RWIM2DVERTEXSetScreenZ(&box[0], RwIm2DGetNearScreenZ());
    RWIM2DVERTEXSetRecipCameraZ(&box[0], (RwReal)(1.0f)/RwCameraGetNearClipPlane(camera));
    RWIM2DVERTEXSetU(&box[0], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetV(&box[0], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetIntRGBA(&box[0], 0, 0, 0, 128);

    RWIM2DVERTEXSetScreenX(&box[1], (RwReal)(0.0f));
    RWIM2DVERTEXSetScreenY(&box[1], (RwReal)(0.0f));
    RWIM2DVERTEXSetScreenZ(&box[1], RwIm2DGetNearScreenZ());
    RWIM2DVERTEXSetRecipCameraZ(&box[1], (RwReal)(1.0f)/RwCameraGetNearClipPlane(camera));
    RWIM2DVERTEXSetU(&box[1], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetV(&box[1], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetIntRGBA(&box[1], 0, 0, 0, 128);

    RWIM2DVERTEXSetScreenX(&box[2], (RwReal)(320.0f));
    RWIM2DVERTEXSetScreenY(&box[2], (RwReal)(0.0f));
    RWIM2DVERTEXSetScreenZ(&box[2], RwIm2DGetNearScreenZ());
    RWIM2DVERTEXSetRecipCameraZ(&box[2], (RwReal)(1.0f)/RwCameraGetNearClipPlane(camera));
    RWIM2DVERTEXSetU(&box[2], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetV(&box[2], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetIntRGBA(&box[2], 0, 0, 0, 128);

    RWIM2DVERTEXSetScreenX(&box[3], (RwReal)(320.0f));
    RWIM2DVERTEXSetScreenY(&box[3], (RwReal)(0.0f));
    RWIM2DVERTEXSetScreenZ(&box[3], RwIm2DGetNearScreenZ());
    RWIM2DVERTEXSetRecipCameraZ(&box[3], (RwReal)(1.0f)/RwCameraGetNearClipPlane(camera));
    RWIM2DVERTEXSetU(&box[3], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetV(&box[3], (RwReal)(0.0f), (RwReal)(1.0f));
    RWIM2DVERTEXSetIntRGBA(&box[3], 0, 0, 0, 128);
}
