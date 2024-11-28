/**********************************************************************
 *
 * File :     smoke.h
 *
 * Abstract : Smoke.
 *
 **********************************************************************
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
 ************************************************************************/

/*--- Include Files ---*/

#include <rwcore.h>

/*--- Global Variables ---*/


typedef struct ParticleTag
{
    RwV3d   pos;
    RwV3d   velocity;
    RwReal  size;
    RwReal  life;
}
Particle;

typedef struct SmokeTag
{
//    RwInt32         numParticles;
//    RwV3d           pos;
    Particle        *particles;
    RWIM3DVERTEX    *verts;
    RWIMVERTEXINDEX *indices;
}
Smoke;

//extern RwBool SmokeAnim;
//extern RwBool SmokeShow;

//extern Smoke *smoke_sys;
/****************************************************************************
 Function prototypes
 */
#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwBool InitialiseSmoke(void);

extern RwBool SmokeParticleCreate(RwV3d *pos, RwV3d *vel);

extern void SmokeDestroy(void);

extern RwBool SmokeUpdate(RwReal delta_t);

extern RwBool SmokeRender(void);

//extern RwBool _ParticlesSetRandomLife(Particle *particle);

//extern RwBool _ParticleInit(Particle *particle, RwV3d *pos);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */
