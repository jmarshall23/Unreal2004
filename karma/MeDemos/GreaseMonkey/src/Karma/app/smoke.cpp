
/*--- Include Files ---*/
#include <malloc.h>
#include <stddef.h>
#include <rwcore.h>
#include <rpworld.h>
//#include <rprandom.h>

#include "skeleton.h"
#include "McdFrame.h"

#include "MdtCar.hpp"
#include "carAPI.hpp"

#include "smoke.hpp"
#include "utils.hpp"

#include "car.hpp"
#include "MeMemory.h"

#define INIT_SIZE    (0.25f)

#define INITVELOCITYY   (5.0f)

#define VELOCITYDECAYY  (0.5f)
#define VELOCITYDECAYXZ  (0.05f)
#define DLIFE           (0.008f)
#define DSIZE           (2.0f)

#define MAX_SMOKE (50)
#define SMOKE_MAX_LIFE (0.2f)

/*--- Static Variables ---*/
static RwTexture    *SmokeTex = (RwTexture *)NULL;
static RwRaster     *SmokeRas = (RwRaster *)NULL;

static Smoke *smoke_sys = (Smoke *)NULL;

/************************************************************************
 *
 *      Function:       _ParticlesSetRandomLife()
 *
 *      Description:    Sets a random life for a particle in the
 *                      range [0...1].
 *
 *      Parameters:     particle - The particle whose life to set.
 *
 *      Return Value:   True on success else false.
 *
 ************************************************************************/
RwBool
_ParticlesSetRandomLife(Particle *particle)
{
    if (!particle)
    {
        return (FALSE);
    }

    particle->life = SMOKE_MAX_LIFE; //(RwReal)(RpRandom() & 1023) / 1023.0f;

    return (TRUE);
}

/************************************************************************
 *
 *      Function:       _ParticleInit()
 *
 *      Description:    Initializes a particle.
 *
 *      Parameters:     particle - The particle to be initialized.
 *                      pos - The position of the particle in world space.
 *                      ivel - initial velocity vector
 *
 *      Return Value:   True on success else false.
 *
 ************************************************************************/
RwBool
_ParticleInit(Particle *particle, RwV3d *pos, RwV3d *vel)
{
    if (!particle)
    {
        return (FALSE);
    }

    particle->pos.x = pos->x;
    particle->pos.y = pos->y;
    particle->pos.z = pos->z;

    particle->velocity.x = vel->x; //0.0005f - (0.01f * (RwReal)rand()) / (RwReal)RAND_MAX;
    particle->velocity.y = vel->y; //INITVELOCITYY;
    particle->velocity.z = vel->z; //0.0005f + (0.01f * (RwReal)rand()) / (RwReal)RAND_MAX;

    particle->size = INIT_SIZE;

    particle->life = SMOKE_MAX_LIFE;

    return (TRUE);
}

/************************************************************************
 *
 *      Function:       SmokeOpen()
 *
 *      Description:    Loads the smoke texture to be used.
 *
 *      Parameters:
 *
 *      Return Value:   True on success else false.
 *
 ************************************************************************/
RwBool
InitialiseSmoke(void)
{
    RwChar *path = RsPathnameCreate(RWSTRING(game_vars.texture_path));
    RwImageSetPath(path);

    SmokeTex = RwTextureRead(RWSTRING("smoke"), RWSTRING("smokem"));
    RsPathnameDestroy(path);
    if (!SmokeTex)
    {
        return (RwBool)(FALSE);
    }

    SmokeRas = RwTextureGetRaster(SmokeTex);


    smoke_sys = (Smoke *)RwMalloc(sizeof(Smoke));
    if (smoke_sys)
    {
        RwInt32         i;
        RWIM3DVERTEX    *vert;

        smoke_sys->particles = (Particle *)RwMalloc((sizeof(Particle) * MAX_SMOKE));
        if (!smoke_sys->particles)
        {
            RwFree(smoke_sys);
            smoke_sys = (SmokeTag *)NULL;
            return (RwBool)(FALSE);
        }

        smoke_sys->verts = (RWIM3DVERTEX *)RwMalloc((sizeof(RWIM3DVERTEX) * MAX_SMOKE * 4));
        if (!smoke_sys->verts)
        {
            RwFree(smoke_sys->particles);
            RwFree(smoke_sys);
            smoke_sys = (SmokeTag *)NULL;
            return (RwBool)(FALSE);
        }

        /*
         * Initialize the particles color and texture coordinates
         */

        vert = smoke_sys->verts;
        for (i = 0; i < MAX_SMOKE; i++)
        {
            RWIM3DVERTEXSetRGBA(vert, 0xFF, 0xFF, 0xFF, 0xFF);
            RWIM3DVERTEXSetU(vert, 0.0f);
            RWIM3DVERTEXSetV(vert, 0.0f);
            vert++;

            RWIM3DVERTEXSetRGBA(vert, 0xFF, 0xFF, 0xFF, 0xFF);
            RWIM3DVERTEXSetU(vert, 0.0f);
            RWIM3DVERTEXSetV(vert, 1.0f);
            vert++;

            RWIM3DVERTEXSetRGBA(vert, 0xFF, 0xFF, 0xFF, 0xFF);
            RWIM3DVERTEXSetU(vert, 1.0f);
            RWIM3DVERTEXSetV(vert, 1.0f);
            vert++;

            RWIM3DVERTEXSetRGBA(vert, 0xFF, 0xFF, 0xFF, 0xFF);
            RWIM3DVERTEXSetU(vert, 1.0f);
            RWIM3DVERTEXSetV(vert, 0.0f);
            vert++;
        }

        smoke_sys->indices = (RWIMVERTEXINDEX *)RwMalloc((sizeof(RWIMVERTEXINDEX) * MAX_SMOKE * 3 * 2));
        if (!smoke_sys->indices)
        {
            RwFree(smoke_sys->verts);
            RwFree(smoke_sys->particles);
            RwFree(smoke_sys);
            smoke_sys = (SmokeTag *)NULL;
            return (RwBool)(FALSE);
        }

        /*
         * Initialize the index list
         */

        for (i = 0; i < MAX_SMOKE; i++)
        {
            smoke_sys->indices[(i * 6) + 0] = (i * 4) + 0;
            smoke_sys->indices[(i * 6) + 1] = (i * 4) + 1;
            smoke_sys->indices[(i * 6) + 2] = (i * 4) + 2;
            smoke_sys->indices[(i * 6) + 3] = (i * 4) + 0;
            smoke_sys->indices[(i * 6) + 4] = (i * 4) + 2;
            smoke_sys->indices[(i * 6) + 5] = (i * 4) + 3;
        }
    }

    return (RwBool)(TRUE);
}


/************************************************************************
 *
 *      Function:       SmokeCreate()
 *
 *      Description:    Creates smoke.
 *
 *      Parameters:     numParticles - The number of particles to create
 *                                     for the smoke.
 *                      pos - The positions of the smoke in world space.
 *
 *      Return Value:   The smoke pointer on success else NULL.
 *
 ************************************************************************/
RwBool
SmokeParticleCreate(RwV3d *pos, RwV3d *vel)
{
    int i;
    RwUInt8 alpha;

    if(!smoke_sys) return FALSE;

    for(i=0;i<MAX_SMOKE;i++)
    {
        if(smoke_sys->particles[i].life <= 0.0f)
        {
            _ParticleInit(&(smoke_sys->particles[i]), pos, vel);
            //_ParticlesSetRandomLife(&(smoke_sys->particles[i]));

            alpha = (unsigned char)(smoke_sys->particles[i].life/SMOKE_MAX_LIFE * 255);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 0]),
                0xFF, 0xFF, 0xFF, alpha);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 1]),
                0xFF, 0xFF, 0xFF, alpha);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 2]),
                0xFF, 0xFF, 0xFF, alpha);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 3]),
                0xFF, 0xFF, 0xFF, alpha);

            return(TRUE);
        }
    }

    return (FALSE);
}

void
SmokeClose(void)
{
    if (SmokeTex)
    {
        RwTextureDestroy(SmokeTex);
        SmokeTex = NULL;
    }

    return;
}

/************************************************************************
 *
 *      Function:       SmokeDestroy()
 *
 *      Description:    Destroys smoke.
 *
 *      Parameters:     smoke - The smoke to be destroyed.
 *
 *      Return Value:   The smoke pointer on success else NULL.
 *
 ************************************************************************/

void
SmokeDestroy(void)
{

    if (!smoke_sys)
    {
        return;
    }

    if (smoke_sys->indices)
    {
        RwFree(smoke_sys->indices);
    }

    if (smoke_sys->verts)
    {
        RwFree(smoke_sys->verts);
    }

    if (smoke_sys->particles)
    {
        RwFree(smoke_sys->particles);
    }

    if (SmokeTex)
    {
        RwTextureDestroy(SmokeTex);
        SmokeTex = (RwTexture *)NULL;
    }


    RwFree(smoke_sys);

    smoke_sys = (SmokeTag *)NULL;

    return;
}

/************************************************************************
 *
 *      Function:       SmokeUpdate()
 *
 *      Description:    Updates the smoke.
 *
 *      Parameters:     smoke - the smoke to be updated.
 *
 *      Return Value:   True on success else false.
 *
 ************************************************************************/
RwBool
SmokeUpdate(RwReal delta_t)
{
    RwInt32 i;

    if (!smoke_sys)
    {
        return (FALSE);
    }

    for (i = 0; i < MAX_SMOKE; i++)
    {
        RwUInt8 alpha;

        /*
         * Are we still alive?
         */

        if (smoke_sys->particles[i].life > 0.0f)
        {
            /*
             * YES, time to party
             */

            smoke_sys->particles[i].pos.x += smoke_sys->particles[i].velocity.x*delta_t;
            smoke_sys->particles[i].pos.y += smoke_sys->particles[i].velocity.y*delta_t;
            smoke_sys->particles[i].pos.z += smoke_sys->particles[i].velocity.z*delta_t;

            smoke_sys->particles[i].velocity.x *= VELOCITYDECAYXZ;
            smoke_sys->particles[i].velocity.y *= VELOCITYDECAYY;
            smoke_sys->particles[i].velocity.z *= VELOCITYDECAYXZ;
            smoke_sys->particles[i].size *= (1.0f + (DSIZE*delta_t));

            alpha = (unsigned char)(smoke_sys->particles[i].life/SMOKE_MAX_LIFE * 255);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 0]),
                0xFF, 0xFF, 0xFF, alpha);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 1]),
                0xFF, 0xFF, 0xFF, alpha);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 2]),
                0xFF, 0xFF, 0xFF, alpha);
            RWIM3DVERTEXSetRGBA(&(smoke_sys->verts[(i * 4) + 3]),
                0xFF, 0xFF, 0xFF, alpha);

            smoke_sys->particles[i].life -= delta_t;
        }
    }

    return (TRUE);
}

/************************************************************************
 *
 *      Function:       SmokeRender()
 *
 *      Description:    Renders the smoke.
 *
 *      Parameters:     smoke - the smoke to be rendered.
 *
 *      Return Value:   True on success else false.
 *
 ************************************************************************/
RwBool
SmokeRender()
{
    RwFrame         *camFrame;
    RwMatrix        *camMatrix;
    RwV3d           up, right;
    RwInt32         srcBlend, dstBlend;
    RwInt32         i, num_to_render=0;
    RWIM3DVERTEX    *vert;

    if (!smoke_sys)
    {
        return (FALSE);
    }

    camFrame = RwCameraGetFrame(game_cam.cam);
    camMatrix = RwFrameGetMatrix(camFrame);

    RwRenderStateSet(rwRENDERSTATEALPHAPRIMITIVEBUFFER, (void *)FALSE);

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);

    /* get the old render states */
    RwRenderStateGet(rwRENDERSTATESRCBLEND,  &srcBlend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND, &dstBlend);

    /* New states for additive alpha */
    RwRenderStateSet(rwRENDERSTATESRCBLEND,  (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDONE);
    /* use correct texture */
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     SmokeRas);

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);

    vert = smoke_sys->verts;
    for (i = 0; i < MAX_SMOKE; i++)
    {
        if(smoke_sys->particles[i].life > 0) { //particle still alive
            num_to_render++;
            up = *RwMatrixGetUp(camMatrix);
            right = *RwMatrixGetRight(camMatrix);

            RwV3dScale(&up, &up, smoke_sys->particles[i].size);
            RwV3dScale(&right, &right, smoke_sys->particles[i].size);

            /* Top Left */
            RWIM3DVERTEXSetPos(vert,
                               smoke_sys->particles[i].pos.x + right.x + up.x,
                               smoke_sys->particles[i].pos.y + right.y + up.y,
                               smoke_sys->particles[i].pos.z + right.z + up.z);
            vert++;

            /* Bottom Left */
            RWIM3DVERTEXSetPos(vert,
                               smoke_sys->particles[i].pos.x + right.x - up.x,
                               smoke_sys->particles[i].pos.y + right.y - up.y,
                               smoke_sys->particles[i].pos.z + right.z - up.z);
            vert++;

            /* Bottom Right */
            RWIM3DVERTEXSetPos(vert,
                               smoke_sys->particles[i].pos.x - right.x - up.x,
                               smoke_sys->particles[i].pos.y - right.y - up.y,
                               smoke_sys->particles[i].pos.z - right.z - up.z);
            vert++;

            /* Top Right */
            RWIM3DVERTEXSetPos(vert,
                               smoke_sys->particles[i].pos.x - right.x + up.x,
                               smoke_sys->particles[i].pos.y - right.y + up.y,
                               smoke_sys->particles[i].pos.z - right.z + up.z);
            vert++;
        }
    }

	if(num_to_render > 0)
	{
		if (RwIm3DTransform(smoke_sys->verts, num_to_render * 4,
                        (RwMatrix *)NULL, rwIM3D_VERTEXUV))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST,
										 smoke_sys->indices,
										 num_to_render * 6);
			RwIm3DEnd();
		}
	}

    RwRenderStateSet(rwRENDERSTATEALPHAPRIMITIVEBUFFER, (void *)TRUE);

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      (void *)TRUE);

    /*
     * Reset old render states
     */

    RwRenderStateSet(rwRENDERSTATESRCBLEND,  (void *)srcBlend);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)dstBlend);

    return (TRUE);
}

