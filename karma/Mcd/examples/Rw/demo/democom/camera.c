#include <rwcore.h>

#include "camera.h"

void
CameraSize(RwCamera * camera, RwRect * rect,
           RwReal viewWindow, RwReal aspectRatio)
{
    if (camera)
    {
        RwV2d               vw;
        RwVideoMode         videoMode;
        RwRect              r;
        RwRaster           *subRaster;

        RwEngineGetVideoModeInfo(&videoMode,
                                 RwEngineGetCurrentVideoMode());

        if (!rect)
        {
            /* rect not specified - derive for full screen device or
             * simply reuse current values
             */
            if (videoMode.flags & rwVIDEOMODEEXCLUSIVE)
            {
                r.w = videoMode.width;
                r.h = videoMode.height;
            }
            else
            {
                r.w = RwRasterGetWidth(RwCameraGetRaster(camera));
                r.h = RwRasterGetHeight(RwCameraGetRaster(camera));
            }
            r.x = r.y = 0;
            rect = &r;
        }

        if (videoMode.flags & rwVIDEOMODEEXCLUSIVE)
        {
            /* derive ratio from aspect ratio */
            vw.x = viewWindow;
            vw.y = viewWindow / aspectRatio;
        }
        else
        {
            /* derive from pixel ratios */

            if (rect->w > rect->h)
            {
                vw.x = viewWindow;
                vw.y = (rect->h * viewWindow) / rect->w;
            }
            else
            {
                vw.x = (rect->w * viewWindow) / rect->h;
                vw.y = viewWindow;
            }
        }
        subRaster = RwCameraGetRaster(camera);
        if (subRaster)
        {
            RwRasterSubRaster(subRaster,
                              RwRasterGetParent(subRaster), rect);
        }

        subRaster = RwCameraGetZRaster(camera);
        if (subRaster)
        {
            RwRasterSubRaster(subRaster, RwRasterGetParent(subRaster),
                              rect);
        }
        RwCameraSetViewWindow(camera, &vw);
    }

    return;
}

void
CameraDestroy(RwCamera * camera)
{
    RwRaster           *raster, *tmpRaster;

    if (camera)
    {
        if (RwCameraGetFrame(camera))
        {
            RwFrameDestroy(RwCameraGetFrame(camera));
        }

        raster = RwCameraGetRaster(camera);
        if (raster)
        {
            tmpRaster = RwRasterGetParent(raster);

            RwRasterDestroy(raster);

            if ((tmpRaster != NULL) && (tmpRaster != raster))
            {
                RwRasterDestroy(tmpRaster);
            }
        }

        raster = RwCameraGetZRaster(camera);
        if (raster)
        {
            tmpRaster = RwRasterGetParent(raster);

            RwRasterDestroy(raster);

            if ((tmpRaster != NULL) && (tmpRaster != raster))
            {
                RwRasterDestroy(tmpRaster);
            }
        }
        RwCameraDestroy(camera);
    }

    return;
}

RwCamera           *
CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer)
{
    RwCamera           *camera;
    RwRect              rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;

    camera = RwCameraCreate();

    if (camera)
    {
        RwCameraSetFrame(camera, RwFrameCreate());
        RwCameraSetRaster(camera,
                          RwRasterCreate(0, 0, 0, rwRASTERTYPECAMERA));

        RwRasterSubRaster(RwCameraGetRaster(camera),
                          RwRasterCreate(width, height,
                                         0, rwRASTERTYPECAMERA), &rect);

        if (zBuffer)
        {
            RwCameraSetZRaster(camera,
                               RwRasterCreate(0, 0, 0,
                                              rwRASTERTYPEZBUFFER));

            RwRasterSubRaster(RwCameraGetZRaster(camera),
                              RwRasterCreate(width, height,
                                             0, rwRASTERTYPEZBUFFER),
                              &rect);
        }

        /* now check that everything is valid */
        if (RwCameraGetFrame(camera) &&
            RwCameraGetRaster(camera) &&
            RwRasterGetParent(RwCameraGetRaster(camera)) &&
            (!zBuffer || (RwCameraGetZRaster(camera) &&
                          RwRasterGetParent(RwCameraGetZRaster
                                            (camera)))))
        {
            /* everything OK */
            return (camera);
        }
    }

    /* if we're here then an error must have occurred so clean up */

    CameraDestroy(camera);
    return (NULL);
}

void
CameraMove(RwCamera * cam, RwV3d * v)
{
    RwV3d               offset;

    RwMatrix           *cameraMatrix =

        RwFrameGetMatrix(RwCameraGetFrame(cam));

    RwV3d              *at = RwMatrixGetAt(cameraMatrix);
    RwV3d              *up = RwMatrixGetUp(cameraMatrix);
    RwV3d              *right = RwMatrixGetRight(cameraMatrix);

    offset.x = v->x * right->x + v->y * up->x + v->z * at->x;
    offset.y = v->x * right->y + v->y * up->y + v->z * at->y;
    offset.z = v->x * right->z + v->y * up->z + v->z * at->z;

    /* Translate the camera back to its new position in the world */
    RwFrameTranslate(RwCameraGetFrame(cam), &offset,
                     rwCOMBINEPOSTCONCAT);

    return;
}

void
CameraPan(RwCamera * cam, const RwV3d * pos, RwReal angle)
{
    RwV3d               invCamPos;
    RwFrame            *cameraFrame = RwCameraGetFrame(cam);
    RwMatrix           *cameraMatrix = RwFrameGetMatrix(cameraFrame);
    RwV3d               camPos;

    camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

    RwV3dScale(&invCamPos, &camPos, -1.0f);

    /* Translate the camera back to the rotation origin. */
    RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

    /* Get the cameras Up vector and use this as the axis of rotation */

    RwMatrixRotate(cameraMatrix, RwMatrixGetUp(cameraMatrix),
                   angle, rwCOMBINEPOSTCONCAT);

    /* Translate the camera back to its original position */
    RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

    return;
}

void
CameraTilt(RwCamera * cam, const RwV3d * pos, RwReal angle)
{
    RwV3d               invCamPos;
    RwFrame            *cameraFrame = RwCameraGetFrame(cam);
    RwMatrix           *cameraMatrix = RwFrameGetMatrix(cameraFrame);
    RwV3d               camPos;

    camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

    RwV3dScale(&invCamPos, &camPos, -1.0f);

    /* Translate the camera back to the rotation origin. */
    RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

    /* Get the cameras Right vector and use this as the axis of rotation */

    RwMatrixRotate(cameraMatrix, RwMatrixGetRight(cameraMatrix),
                   angle, rwCOMBINEPOSTCONCAT);

    /* Translate the camera back to its original position */
    RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

    return;
}

void
CameraRotate(RwCamera * cam, const RwV3d * pos, RwReal angle)
{
    RwV3d               invCamPos;
    RwFrame            *cameraFrame = RwCameraGetFrame(cam);
    RwMatrix           *cameraMatrix = RwFrameGetMatrix(cameraFrame);
    RwV3d               camPos;

    camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

    RwV3dScale(&invCamPos, &camPos, -1.0f);

    /* Translate the camera back to the rotation origin. */
    RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

    /* Get the cameras At vector and use this as the axis of rotation */

    RwMatrixRotate(cameraMatrix, RwMatrixGetAt(cameraMatrix),
                   angle, rwCOMBINEPOSTCONCAT);

    /* Translate the camera back to its original position */
    RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

    return;
}
