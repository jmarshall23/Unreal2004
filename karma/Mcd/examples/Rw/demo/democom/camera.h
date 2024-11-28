#if (!defined(_CAMERA_H))
#define _CAMERA_H

/****************************************************************************
 Function prototypes
 */
#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern void CameraMove(RwCamera *cam,
                       RwV3d *offset);
extern void CameraTilt(RwCamera *cam,
                       const RwV3d *pos,
                       RwReal angle);
extern void CameraPan(RwCamera *cam,
                      const RwV3d *pos,
                      RwReal angle);
extern void CameraRotate(RwCamera *cam,
                         const RwV3d *pos,
                         RwReal angle);
extern void CameraSize(RwCamera *camera,
                       RwRect *rect,
                       RwReal viewWindow,
                       RwReal aspectRatio);
extern void CameraDestroy(RwCamera *camera);
extern RwCamera *CameraCreate(RwInt32 width,
                              RwInt32 height,
                              RwBool zBuffer);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* (!defined(_CAMERA_H)) */
