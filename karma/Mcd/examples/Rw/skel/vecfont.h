#if (!defined(_VECFONT_H))
#define _VECFONT_H

/****************************************************************************
 Global types
 */

typedef struct
{
    RwV2d         size;
    RwRGBA        color;
    RwReal        recipZ;
    RWIM2DVERTEX *lineVertBuffer;
    RwUInt32      lineVertBufferSize;
}
VecFont;

/****************************************************************************
 Function prototypes
 */
#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwBool RsVecFontOpen(void);
extern void RsVecFontClose(void);

extern VecFont *RsVecFontCreate(const RwCamera *camera, const RwRGBA *color, const RwV2d *size);
extern RwBool RsVecFontDestroy(VecFont *font);

extern void RsVecFontPrint(VecFont *font, const RwV2d *pos, const RwChar *string);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* (!defined(_VECFONT_H)) */
