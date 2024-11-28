/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/05 11:19:47 $ - Revision: $Revision: 1.23.2.2.4.1 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <MdtKeaProfile.h>
#include "keaDebug.h"
#include <MePrecision.h>
#include <MeMath.h>
#include <MdtKea.h>
#include "keaIntegrate.hpp"
#ifndef _BUILD_VANILLA
#include "keaInternal.hpp"
#include "KeaSSEi.h"
#endif

//#include "keaCTicks.h"

#ifndef _BUILD_VANILLA
#if 0
void MEAPI KeaIntegrateSystem_sse(MdtKeaBody *const       blist[],
                                  MdtKeaTransformation *  tlist,
                                  int                     num_bodies,
                                  MdtKeaParameters        parameters)
{

    MeReal _MM_ALIGN16 myw[4];     // omega goes here

    __m128 rw, dq, qr, qt, qu, qs, rz;
    __m128 r1 = _mm_load_ps(MASK_1F);
    __m128 ss = rspread(parameters.stepsize);

    int i;

    for(i=0; i!=num_bodies; i++)
    {
        _mm_store_ps(blist[i]->vel, _mm_add_ps(_mm_load_ps(blist[i]->vel), _mm_mul_ps(_mm_load_ps(blist[i]->accel), ss)));
        _mm_store_ps(blist[i]->velrot, _mm_add_ps(_mm_load_ps(blist[i]->velrot), _mm_mul_ps(_mm_load_ps(blist[i]->accelrot), ss)));
    }
    
    /* do 'pos = pos + h*vel' : compute quaternion derivatives and update position */

    for(i=0; i!=num_bodies; i++)
    {
        //tlist[i].pos[0] += parameters.stepsize*blist[i]->vel[0];
        //tlist[i].pos[1] += parameters.stepsize*blist[i]->vel[1];
        //tlist[i].pos[2] += parameters.stepsize*blist[i]->vel[2];
        __m128 rt = _mm_add_ps(_mm_load_ps(tlist[i].pos), _mm_mul_ps(_mm_load_ps(blist[i]->vel), ss));
        //tlist[i].pos[3] = (MeReal)(1.0);
        _mm_store_ps(tlist[i].pos, _mm_shuffle_ps(rt, _mm_movehl_ps(r1, rt), _MM_SHUFFLE(3,0,1,0) ));

        // for this body, if fastSpin=0 then use the standard infitesimal
        // quaternion update. otherwise use a finite rotation on the fast spin
        // axis followed by an infitesimal update on the rotation axis orthogonal
        // to the fast spin axis. this is a dodgy hack!!
        
        //MeReal myw[3];    // omega goes here
        //myw[0] = blist[i]->velrot[0];
        //myw[1] = blist[i]->velrot[1];
        //myw[2] = blist[i]->velrot[2];
        _mm_store_ps(myw, rw = _mm_load_ps(blist[i]->velrot));
        
        if(blist[i]->flags & MdtKeaBodyFlagUseFastSpin) {
            MeReal rot = MeVector3Dot(myw, blist[i]->fastSpinAxis);
            MeQuaternionFiniteRotation(blist[i]->qrot, blist[i]->fastSpinAxis, rot * parameters.stepsize);
            //myw[0] -= blist[i]->fastSpinAxis[0] * rot;
            //myw[1] -= blist[i]->fastSpinAxis[1] * rot;
            //myw[2] -= blist[i]->fastSpinAxis[2] * rot;
            _mm_store_ps(myw, rw = _mm_sub_ps(rw, _mm_mul_ps(_mm_load_ps(blist[i]->fastSpinAxis), rspread(rot))));

        }
        
        //MeReal dq[4];
        //dq[0] = (MeReal)(0.5)*( -blist[i]->qrot[1]*myw[0] -blist[i]->qrot[2]*myw[1] -blist[i]->qrot[3]*myw[2] );
        //dq[1] = (MeReal)(0.5)*( +blist[i]->qrot[0]*myw[0] +blist[i]->qrot[3]*myw[1] -blist[i]->qrot[2]*myw[2] );
        //dq[2] = (MeReal)(0.5)*( -blist[i]->qrot[3]*myw[0] +blist[i]->qrot[0]*myw[1] +blist[i]->qrot[1]*myw[2] );
        //dq[3] = (MeReal)(0.5)*( +blist[i]->qrot[2]*myw[0] -blist[i]->qrot[1]*myw[1] +blist[i]->qrot[0]*myw[2] );
        qr = _mm_load_ps(blist[i]->qrot);

        dq = _mm_xor_ps(_mm_load_ps((float*)MASK_MPMP), _mm_mul_ps(_mm_shuffle_ps(qr, qr, _MM_SHUFFLE(2,3,0,1)), broadcast(rw, 0)));
        dq = _mm_add_ps(dq, _mm_xor_ps(_mm_load_ps((float*)MASK_MPPM), _mm_mul_ps(_mm_shuffle_ps(qr, qr, _MM_SHUFFLE(1,0,3,2)), broadcast(rw, 1))));
        dq = _mm_add_ps(dq, _mm_xor_ps(_mm_load_ps((float*)MASK_MMPP), _mm_mul_ps(_mm_shuffle_ps(qr, qr, _MM_SHUFFLE(0,1,2,3)), broadcast(rw, 2))));
        dq = _mm_mul_ps(dq, _mm_load_ps(MASK_HALF));

        //blist[i]->qrot[0] += parameters.stepsize * dq[0];
        //blist[i]->qrot[1] += parameters.stepsize * dq[1];
        //blist[i]->qrot[2] += parameters.stepsize * dq[2];
        //blist[i]->qrot[3] += parameters.stepsize * dq[3];
        //_mm_store_ps(blist[i]->qrot,
        qr = _mm_add_ps(qr, _mm_mul_ps(dq, ss));

        // Normalise the quaternion
        //MeReal s=0.0f;
        //for(int j=0; j<4; j++) s += MeSqr(blist[i]->qrot[j]);
        //s = MeRecipSqrt(s);
        //blist[i]->qrot[0] *= s;
        //blist[i]->qrot[1] *= s;
        //blist[i]->qrot[2] *= s;
        //blist[i]->qrot[3] *= s;
        _mm_store_ps(blist[i]->qrot, qr = normalize4(qr));
       
        // update the position auxiliary variables
        // (maintain coupling invariant that qrot and R represent the same rotation)
        tlist[i].R0[0] =   blist[i]->qrot[0]*blist[i]->qrot[0] + blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R0[1] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        tlist[i].R0[2] = -2.0f*blist[i]->qrot[0]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[1]*blist[i]->qrot[3];
        tlist[i].R0[3] =  0;
        tlist[i].R1[0] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] - 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        tlist[i].R1[1] =   blist[i]->qrot[0]*blist[i]->qrot[0] -      blist[i]->qrot[1]*blist[i]->qrot[1] + blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R1[2] =  2.0f*blist[i]->qrot[0]*blist[i]->qrot[1] + 2.0f*blist[i]->qrot[2]*blist[i]->qrot[3];
        tlist[i].R1[3] =  0;
        tlist[i].R2[0] =  2.0f*blist[i]->qrot[0]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[1]*blist[i]->qrot[3];
        tlist[i].R2[1] = -2.0f*blist[i]->qrot[0]*blist[i]->qrot[1] + 2.0f*blist[i]->qrot[2]*blist[i]->qrot[3];
        tlist[i].R2[2] =   blist[i]->qrot[0]*blist[i]->qrot[0] -      blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] + blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R2[3] =  0;
        /*
        //tlist[i].R0[0] =   blist[i]->qrot[0]*blist[i]->qrot[0] + blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        //tlist[i].R1[1] =   blist[i]->qrot[0]*blist[i]->qrot[0] - blist[i]->qrot[1]*blist[i]->qrot[1] + blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        //tlist[i].R2[2] =   blist[i]->qrot[0]*blist[i]->qrot[0] - blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] + blist[i]->qrot[3]*blist[i]->qrot[3];
        qs = broadcast(qr, 0);
        qt = _mm_mul_ps(qs, qs);
        qs = broadcast(qr, 1);
        qt = _mm_add_ps(qt, _mm_xor_ps(_mm_load_ps((float*)MASK_PMMP), _mm_mul_ps(qs, qs)));
        qs = broadcast(qr, 2);
        qt = _mm_add_ps(qt, _mm_xor_ps(_mm_load_ps((float*)MASK_MPMP), _mm_mul_ps(qs, qs)));
        qs = broadcast(qr, 3);
        qt = _mm_add_ps(qt, _mm_xor_ps(_mm_load_ps((float*)MASK_MMPP), _mm_mul_ps(qs, qs)));

        //tlist[i].R0[1] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        //tlist[i].R1[0] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] - 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        //tlist[i].R0[2] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[3] - 2.0f*blist[i]->qrot[0]*blist[i]->qrot[2];
        //tlist[i].R2[0] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[3] + 2.0f*blist[i]->qrot[0]*blist[i]->qrot[2];
        /*
        1*2
        1*3
        0*3
        0*2
        unpacklo
        unpackhi
        mask
        add
        x2
        //
        qs = _mm_shuffle_ps(qr, qr, _MM_SHUFFLE(0,0,1,1));
        qu = _mm_mul_ps(qs, _mm_shuffle_ps(qr, qr, _MM_SHUFFLE(2,3,3,2)));
        qs = _mm_add_ps(_mm_unpacklo_ps(qu, qu), _mm_xor_ps(_mm_load_ps((float*)MASK_PMMP), _mm_unpackhi_ps(qu, qu)));
        qs = _mm_add_ps(qs, qs);

        //tlist[i].R2[1] =  2.0f*blist[i]->qrot[2]*blist[i]->qrot[3] - 2.0f*blist[i]->qrot[0]*blist[i]->qrot[1];
        //tlist[i].R1[2] =  2.0f*blist[i]->qrot[2]*blist[i]->qrot[3] + 2.0f*blist[i]->qrot[0]*blist[i]->qrot[1];
        qu = _mm_shuffle_ps(qr, qr, _MM_SHUFFLE(0,0,2,2));
        qu = _mm_mul_ps(qu, _mm_shuffle_ps(qr, qr, _MM_SHUFFLE(1,1,3,3)));
        qu = _mm_add_ps(qu, _mm_xor_ps(_mm_load_ps((float*)MASK_MPMP), _mm_movehl_ps(qu, qu)));
        qu = _mm_add_ps(qu, qu);

        //swizzle(!)
        /*
        for tlist[i]
        qt:
            R0[0]
            R1[1]
            R2[2]
            BLAH
        qs:
            R0[1]
            R1[0]
            R0[2]
            R2[0]
        qu:
            R2[1]
            R1[2]
            BLAH
            BLAH
        //
        rz = _mm_setzero_ps();
        _mm_store_ps(tlist[i].R0, merge4(qt, qs, _mm_movehl_ps(qs, qs), rz));
        _mm_store_ps(tlist[i].R1, merge4(broadcast(qs, 1), broadcast(qt, 1), broadcast(qt, 1), rz));
        _mm_store_ps(tlist[i].R2, merge4(broadcast(qs, 3), qu, broadcast(qt, 2), rz));
        */
    }
}
#endif
//#if 0
void MEAPI KeaIntegrateSystem_sse(MdtKeaBody *const       blist[],
                                  MdtKeaTransformation *  tlist,
                                  int                     num_bodies,
                                  MdtKeaParameters        parameters)
{
    int i,j;

    for (i=0; i!=num_bodies; i++)
    {
        blist[i]->vel[0] += parameters.stepsize * blist[i]->accel[0];
        blist[i]->vel[1] += parameters.stepsize * blist[i]->accel[1];
        blist[i]->vel[2] += parameters.stepsize * blist[i]->accel[2];
        blist[i]->velrot[0] += parameters.stepsize * blist[i]->accelrot[0];
        blist[i]->velrot[1] += parameters.stepsize * blist[i]->accelrot[1];
        blist[i]->velrot[2] += parameters.stepsize * blist[i]->accelrot[2];
    }
    
    // do 'pos = pos + h*vel' : compute quaternion derivatives and update
    // position
    
    for (i=0; i!=num_bodies; i++)
    {
        tlist[i].pos[0] += parameters.stepsize*blist[i]->vel[0];
        tlist[i].pos[1] += parameters.stepsize*blist[i]->vel[1];
        tlist[i].pos[2] += parameters.stepsize*blist[i]->vel[2];
        tlist[i].pos[3] = (MeReal)(1.0);
        
        // for this body, if fastSpin=0 then use the standard infitesimal
        // quaternion update. otherwise use a finite rotation on the fast spin
        // axis followed by an infitesimal update on the rotation axis orthogonal
        // to the fast spin axis. this is a dodgy hack!!
        MeReal myw[3];    // omega goes here
        myw[0] = blist[i]->velrot[0];
        myw[1] = blist[i]->velrot[1];
        myw[2] = blist[i]->velrot[2];
        
        if (blist[i]->flags & MdtKeaBodyFlagUseFastSpin) {
            MeReal rot = MeVector3Dot (myw,blist[i]->fastSpinAxis);
            MeQuaternionFiniteRotation (blist[i]->qrot,blist[i]->fastSpinAxis,rot * parameters.stepsize);
            myw[0] -= blist[i]->fastSpinAxis[0] * rot;
            myw[1] -= blist[i]->fastSpinAxis[1] * rot;
            myw[2] -= blist[i]->fastSpinAxis[2] * rot;
        }
        
        MeReal dq[4];
        MeReal s=0.0f;
        dq[0] = (MeReal)(0.5)*( -blist[i]->qrot[1]*myw[0] -blist[i]->qrot[2]*myw[1] -blist[i]->qrot[3]*myw[2] );
        dq[1] = (MeReal)(0.5)*( +blist[i]->qrot[0]*myw[0] +blist[i]->qrot[3]*myw[1] -blist[i]->qrot[2]*myw[2] );
        dq[2] = (MeReal)(0.5)*( -blist[i]->qrot[3]*myw[0] +blist[i]->qrot[0]*myw[1] +blist[i]->qrot[1]*myw[2] );
        dq[3] = (MeReal)(0.5)*( +blist[i]->qrot[2]*myw[0] -blist[i]->qrot[1]*myw[1] +blist[i]->qrot[0]*myw[2] );
        
        blist[i]->qrot[0] += parameters.stepsize * dq[0];
        blist[i]->qrot[1] += parameters.stepsize * dq[1];
        blist[i]->qrot[2] += parameters.stepsize * dq[2];
        blist[i]->qrot[3] += parameters.stepsize * dq[3];
        
        // Normalise the quaternion
        for (j=0; j<4; j++) s += MeSqr(blist[i]->qrot[j]);
        s = MeRecipSqrt(s);
        
        // update the position auxiliary variables
        // (maintain coupling invariant that qrot and R represent the same rotation)
        tlist[i].R0[3] = 0;
        tlist[i].R1[3] = 0;
        tlist[i].R2[3] = 0;
        
        blist[i]->qrot[0] *= s;
        blist[i]->qrot[1] *= s;
        blist[i]->qrot[2] *= s;
        blist[i]->qrot[3] *= s;
        
        tlist[i].R0[0] =   blist[i]->qrot[0]*blist[i]->qrot[0] + blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R1[0] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] - 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        tlist[i].R2[0] =  2.0f*blist[i]->qrot[0]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[1]*blist[i]->qrot[3];
        tlist[i].R0[1] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        tlist[i].R1[1] =   blist[i]->qrot[0]*blist[i]->qrot[0] -      blist[i]->qrot[1]*blist[i]->qrot[1] + blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R2[1] = -2.0f*blist[i]->qrot[0]*blist[i]->qrot[1] + 2.0f*blist[i]->qrot[2]*blist[i]->qrot[3];
        tlist[i].R0[2] = -2.0f*blist[i]->qrot[0]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[1]*blist[i]->qrot[3];
        tlist[i].R1[2] =  2.0f*blist[i]->qrot[0]*blist[i]->qrot[1] + 2.0f*blist[i]->qrot[2]*blist[i]->qrot[3];
        tlist[i].R2[2] =   blist[i]->qrot[0]*blist[i]->qrot[0] -      blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] + blist[i]->qrot[3]*blist[i]->qrot[3];
        
    }
}
//#endif
#endif

void MEAPI KeaIntegrateSystem_vanilla(MdtKeaBody *const       blist[],
                                      MdtKeaTransformation *  tlist,
                                      int                     num_bodies,
                                      MdtKeaParameters        parameters)
{
    int i,j;

    for (i=0; i!=num_bodies; i++)
    {
        blist[i]->vel[0] += parameters.stepsize * blist[i]->accel[0];
        blist[i]->vel[1] += parameters.stepsize * blist[i]->accel[1];
        blist[i]->vel[2] += parameters.stepsize * blist[i]->accel[2];
        blist[i]->velrot[0] += parameters.stepsize * blist[i]->accelrot[0];
        blist[i]->velrot[1] += parameters.stepsize * blist[i]->accelrot[1];
        blist[i]->velrot[2] += parameters.stepsize * blist[i]->accelrot[2];
    }
    
    // do 'pos = pos + h*vel' : compute quaternion derivatives and update
    // position
    
    for (i=0; i!=num_bodies; i++)
    {
        tlist[i].pos[0] += parameters.stepsize*blist[i]->vel[0];
        tlist[i].pos[1] += parameters.stepsize*blist[i]->vel[1];
        tlist[i].pos[2] += parameters.stepsize*blist[i]->vel[2];
        tlist[i].pos[3] = (MeReal)(1.0);
        
        // for this body, if fastSpin=0 then use the standard infitesimal
        // quaternion update. otherwise use a finite rotation on the fast spin
        // axis followed by an infitesimal update on the rotation axis orthogonal
        // to the fast spin axis. this is a dodgy hack!!
        MeReal myw[3];    // omega goes here
        myw[0] = blist[i]->velrot[0];
        myw[1] = blist[i]->velrot[1];
        myw[2] = blist[i]->velrot[2];
        
        if (blist[i]->flags & MdtKeaBodyFlagUseFastSpin) {
            MeReal rot = MeVector3Dot (myw,blist[i]->fastSpinAxis);
            MeQuaternionFiniteRotation (blist[i]->qrot,blist[i]->fastSpinAxis,rot * parameters.stepsize);
            myw[0] -= blist[i]->fastSpinAxis[0] * rot;
            myw[1] -= blist[i]->fastSpinAxis[1] * rot;
            myw[2] -= blist[i]->fastSpinAxis[2] * rot;
        }
        
        MeReal dq[4];
        MeReal s=0.0f;
        dq[0] = (MeReal)(0.5)*( -blist[i]->qrot[1]*myw[0] -blist[i]->qrot[2]*myw[1] -blist[i]->qrot[3]*myw[2] );
        dq[1] = (MeReal)(0.5)*( +blist[i]->qrot[0]*myw[0] +blist[i]->qrot[3]*myw[1] -blist[i]->qrot[2]*myw[2] );
        dq[2] = (MeReal)(0.5)*( -blist[i]->qrot[3]*myw[0] +blist[i]->qrot[0]*myw[1] +blist[i]->qrot[1]*myw[2] );
        dq[3] = (MeReal)(0.5)*( +blist[i]->qrot[2]*myw[0] -blist[i]->qrot[1]*myw[1] +blist[i]->qrot[0]*myw[2] );
        
        blist[i]->qrot[0] += parameters.stepsize * dq[0];
        blist[i]->qrot[1] += parameters.stepsize * dq[1];
        blist[i]->qrot[2] += parameters.stepsize * dq[2];
        blist[i]->qrot[3] += parameters.stepsize * dq[3];
        
        // Normalise the quaternion
        for (j=0; j<4; j++) s += MeSqr(blist[i]->qrot[j]);
        s = MeRecipSqrt(s);
        
        // update the position auxiliary variables
        // (maintain coupling invariant that qrot and R represent the same rotation)
        tlist[i].R0[3] = 0;
        tlist[i].R1[3] = 0;
        tlist[i].R2[3] = 0;
        
        blist[i]->qrot[0] *= s;
        blist[i]->qrot[1] *= s;
        blist[i]->qrot[2] *= s;
        blist[i]->qrot[3] *= s;
        
        tlist[i].R0[0] =   blist[i]->qrot[0]*blist[i]->qrot[0] + blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R1[0] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] - 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        tlist[i].R2[0] =  2.0f*blist[i]->qrot[0]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[1]*blist[i]->qrot[3];
        tlist[i].R0[1] =  2.0f*blist[i]->qrot[1]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[0]*blist[i]->qrot[3];
        tlist[i].R1[1] =   blist[i]->qrot[0]*blist[i]->qrot[0] -      blist[i]->qrot[1]*blist[i]->qrot[1] + blist[i]->qrot[2]*blist[i]->qrot[2] - blist[i]->qrot[3]*blist[i]->qrot[3];
        tlist[i].R2[1] = -2.0f*blist[i]->qrot[0]*blist[i]->qrot[1] + 2.0f*blist[i]->qrot[2]*blist[i]->qrot[3];
        tlist[i].R0[2] = -2.0f*blist[i]->qrot[0]*blist[i]->qrot[2] + 2.0f*blist[i]->qrot[1]*blist[i]->qrot[3];
        tlist[i].R1[2] =  2.0f*blist[i]->qrot[0]*blist[i]->qrot[1] + 2.0f*blist[i]->qrot[2]*blist[i]->qrot[3];
        tlist[i].R2[2] =   blist[i]->qrot[0]*blist[i]->qrot[0] -      blist[i]->qrot[1]*blist[i]->qrot[1] - blist[i]->qrot[2]*blist[i]->qrot[2] + blist[i]->qrot[3]*blist[i]->qrot[3];
        
    }
}

void MEAPI MdtKeaIntegrateSystem(MdtKeaBody *const       blist[],
                                 MdtKeaTransformation *  tlist,
                                 int                     num_bodies,
                                 MdtKeaParameters        parameters)
{
#ifndef _NOTIC
  MdtKeaProfileStart("Integrate");
#endif

  //*** Print integrator input for debugging purposes

#if PRINT_INTEGRATOR_INPUT
  int k;

  MeInfo(0,"Integrator input %d bodies:",num_bodies);
  for(int i=0;i!=num_bodies;i++)
  {
    MeInfo(0,"body %3d: ",i);
	MeInfo(0,"% 8.2f ",blist[i]->force[1]); 
  }
#endif

/*  Set up platform/build dependant function table */

  typedef void (MEAPI *pIntegratorFunc)(MdtKeaBody *const       blist[],
                                        MdtKeaTransformation *  tlist,
                                        int                     num_bodies,
                                        MdtKeaParameters        parameters);
  pIntegratorFunc   KeaIntegrateSystem;

#ifdef _BUILD_VANILLA
  KeaIntegrateSystem = KeaIntegrateSystem_vanilla;
#else
  if(parameters.cpu_resources==(MeCPUResources)MeSSE)
      KeaIntegrateSystem = KeaIntegrateSystem_sse;
  else
      KeaIntegrateSystem = KeaIntegrateSystem_vanilla;
#endif

    //ReadTscOverhead();
    //AllClocks = ReadTsc();
    KeaIntegrateSystem(blist, tlist, num_bodies, parameters);
    //Clocks = ReadTsc() - AllClocks - OverClocks;
    //printf(_CTP, "Integrate System", Clocks);
    //printf("\n");

#ifndef _NOTIC
  MdtKeaProfileEnd("Integrate");
#endif

}

  /* MdtKeaInverseMassMatrix *invIworldp = (MdtKeaInverseMassMatrix *)invIworld;*/
  //.........................................................................
  // given lambda, update system state

  // vel(:) = vel(:) + h*inv(M)*(fe(:) + J'*lambda);

  //
  // Input data:
  //
  // size
  // m
  // n
  // oldJstore
  // Jsize
  // lambda
  // Joffs
  // JBody
  // body
  //   accessed:
  //   fe
  //   invmass
  //   invIworldp
  //   vel
  // stepsize
  //
  // Output data:
  //
  // body -
  //    fields written:
  //    fe  - accumulator of external forces on body
  //    vel - center of mass velocity and angular velocity

