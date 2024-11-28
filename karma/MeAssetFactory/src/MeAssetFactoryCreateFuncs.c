/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/05/17 14:09:42 $

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
#include <MeAssetFactory.h>
#include <MeMath.h>
#include <Mst.h>

/**
 * Function for converting from an MeFAssetPart (and its associated MeFModel
 * and MeFGeometry) to Mdt/Mcd structures. This function can be completely
 * replaced by a user function (@see MeAssetFactorySetModelCreateFunction).
 * Another option is to accept this default and set up a post create callback
 * to further customize the McdModel/MdtBody (@see MeAssetFactorySetModelPostCreateCB).
 * By default MdtBody's are enabled.
 */
McdModelID MEAPI McdModelCreateFromMeFAssetPart(MeFAssetPart *part, McdGeometryID g, MdtWorldID world, MeMatrix4Ptr assetTM)
{
    MeFModel *fmodel = MeFAssetPartGetModel(part);
    MeFModelType type = MeFModelGetType(fmodel);
    McdModelID model = McdModelCreate(g);
    MeMatrix4Ptr modelTM;
    MdtBodyID body = 0;

    if (type == kMeFModelTypeGeometryOnly)
    {
        modelTM = (MeMatrix4Ptr)MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
        McdModelSetTransformPtr(model, modelTM);
    }
    else
    {
        body = MdtBodyCreate(world);
        modelTM = MdtBodyGetTransformPtr(body);
    }

    if(assetTM)
    {
        MeMatrix4MultiplyMatrix(modelTM,
            MeFAssetPartGetTransformPtr(part), assetTM);
    }
    else /* Just create part relative to origin. */
    {
        MeMatrix4Copy(modelTM, MeFAssetPartGetTransformPtr(part));
    }

    if (type == kMeFModelTypeDynamicsAndGeometry || type == kMeFModelTypeDynamicsOnly)
    {
        MdtBodyEnable(body);
        MdtBodySetTransform(body, modelTM);
        MdtBodySetMass(body, MeFModelGetMass(fmodel));
        {
            MeVector3 pos;
            MeFModelGetMassOffset(fmodel, pos);
            MdtBodySetCenterOfMassRelativePosition(body,pos);
        }
        MdtBodyEnableNonSphericalInertia(body);
        MdtBodyEnableCoriolisForce(body);

        {
            MeMatrix3 I;
            MeFModelGetInertiaTensor(fmodel, I);

            /* clamp inertia tensor */
            {
                MeMatrix3 clamped;
                MeReal iMag;

				// Should there be accessors for these?
				iMag = world->params.massScale * world->params.lengthScale * world->params.lengthScale;
				
				MeMatrix3Copy(clamped,I);
				
#ifdef _MECHECK
				/* check that the matrix is symmetrical */
				if (clamped[1][0] != clamped[0][1] ||
					clamped[2][0] != clamped[0][2] ||
					clamped[2][1] != clamped[1][2])
					MeWarning(12, "Inertia tensor matrix is non-symmetric.");
				
				if(clamped[0][0] < iMag / MDTBODY_INERTIA_RANGE_LIMIT ||
					clamped[1][1] < iMag / MDTBODY_INERTIA_RANGE_LIMIT ||
					clamped[2][2] < iMag / MDTBODY_INERTIA_RANGE_LIMIT)
					MeWarning(12, "Inertia of body is below lower range limit. Clamping.");
				
				if(clamped[0][0] > iMag * MDTBODY_INERTIA_RANGE_LIMIT ||
					clamped[1][1] > iMag * MDTBODY_INERTIA_RANGE_LIMIT ||
					clamped[2][2] > iMag * MDTBODY_INERTIA_RANGE_LIMIT)
					MeWarning(12, "Inertia of body is beyond upper range limit. Clamping.");
#endif
				
				clamped[0][0] = MeCLAMP(clamped[0][0], iMag / MDTBODY_INERTIA_RANGE_LIMIT, iMag * MDTBODY_INERTIA_RANGE_LIMIT);
				clamped[1][1] = MeCLAMP(clamped[1][1], iMag / MDTBODY_INERTIA_RANGE_LIMIT, iMag * MDTBODY_INERTIA_RANGE_LIMIT);
				clamped[2][2] = MeCLAMP(clamped[2][2], iMag / MDTBODY_INERTIA_RANGE_LIMIT, iMag * MDTBODY_INERTIA_RANGE_LIMIT);

                MeMatrix3Copy(I,clamped);
            }
            MdtBodySetInertiaTensor(body, I);
        }

        MdtBodySetLinearVelocityDamping(body,
            MeFModelGetLinearVelocityDamping(fmodel));
        MdtBodySetAngularVelocityDamping(body,
            MeFModelGetAngularVelocityDamping(fmodel));

        if (MeFModelIsFastSpinAxisEnabled(fmodel))
        {
            MeVector3 axis;
            MeFModelGetFastSpinAxis(fmodel, axis);
            MdtBodySetFastSpinAxis(body, axis[0], axis[1], axis[2]);
        }
        McdModelSetBody(model, body);
    }

    return model;
}

/**
 * Converts a MeFJoint database structure to an MdtConstraint. Like models, this
 * function can be replaced by a user function (@see MeAssetFactorySetJointCreateFunction),
 * or a post create callback can be set for customization
 * (@see MeAssetFactorySetJointPostCreateCB).
 */
MdtConstraintID MEAPI MdtConstraintCreateFromMeFJoint(MeFJoint *joint, MdtWorldID world, McdModelID m1, McdModelID m2, MeMatrix4Ptr assetTM)
{
    MdtBodyID b1=0, b2=0;
    MeBool swap = 0;
    MdtConstraintID con = 0;
    MdtSkeletalID limb = 0;
    MeVector3 pos, pax, oax;
    int body1_index = 0;
    int body2_index = 1;
    MeFJointType type = MeFJointGetType(joint);

    MEASSERT(joint);
    MEASSERT(world);

    if(m1)
        b1 = McdModelGetBody(m1);

    if(m2)
        b2 = McdModelGetBody(m2);

    if(b1 == 0 && b2 == 0)
    {
        MeWarning(0, "MdtConstraintCreateFromMeFJoint: "
            "Both bodies cannot be NULL.");
        return 0;
    }

    if(b1 == b2)
    {
        MeWarning(0, "MdtConstraintCreateFromMeFJoint: "
            "Both bodies the same.");
        return 0;
    }

    if (b1 == 0 && b2 != 0)
    {
        b1 = b2;
        b2 = 0;
        body1_index = 1;
        body2_index = 0;
    }

    if(type == kMeFJointTypeBallAndSocket)
    {
        MdtBSJointID bs = MdtBSJointCreate(world);
        con = MdtBSJointQuaConstraint(bs);
    }
    else if(type == kMeFJointTypeUniversal)
    {
        MdtUniversalID u = MdtUniversalCreate(world);
        con = MdtUniversalQuaConstraint(u);
    }
    else if(type == kMeFJointTypeRpro)
    {
        MdtRPROJointID rp = MdtRPROJointCreate(world);
        con = MdtRPROJointQuaConstraint(rp);
    }
    else if(type == kMeFJointTypeHinge)
    {
        MdtHingeID h = MdtHingeCreate(world);
        con = MdtHingeQuaConstraint(h);
    }
    else if(type == kMeFJointTypeCarwheel)
    {
        MdtBodyID temp;
        MdtCarWheelID cw = MdtCarWheelCreate(world);
        con = MdtCarWheelQuaConstraint(cw);

        /* flip bodies of car wheel joint because it is not consistent... */
        if(b2 != 0)
        {
            body1_index = !body1_index;
            body2_index = !body2_index;
            temp = b1;
            b1 = b2;
            b2 = temp;
        }
    }
    else if(type == kMeFJointTypeConeLimit)
    {
        MdtConeLimitID cl = MdtConeLimitCreate(world);
        con = MdtConeLimitQuaConstraint(cl);
    }
    else if(type == kMeFJointTypePrismatic)
    {
        MdtPrismaticID h = MdtPrismaticCreate(world);
        con = MdtPrismaticQuaConstraint(h);
    }
    else if(type == kMeFJointTypeSkeletal)
    {
        MdtSkeletalID h = MdtSkeletalCreate(world);
        con = MdtSkeletalQuaConstraint(h);
    }
    else if (type == kMeFJointTypeAngular3)
    {
        MdtAngular3ID a = MdtAngular3Create(world);
        con = MdtAngular3QuaConstraint(a);
    }
    else if (type == kMeFJointTypeSpring6)
    {
        MdtSpring6ID a = MdtSpring6Create(world);
        con = MdtSpring6QuaConstraint(a);
    }
    else
    {
        MeWarning(0, "MdtConstraintCreateFromMeFJoint: "
            "Unknown joint type.");
        return 0;
    }

    MdtConstraintSetBodies(con, b1, b2);

    /*  Set position and axis of constraint relative to
        both bodies reference frames. */

    /*** BODY 1 ***/
    MeFJointGetPosition(joint, body1_index, pos);
    MeFJointGetPrimaryAxis(joint, body1_index, pax);
    MeFJointGetOrthogonalAxis(joint, body1_index, oax);

    if (type == kMeFJointTypeCarwheel)
    {
        MeVector3 vec;
        MeFJointGetPrimaryAxis(joint, body1_index, oax);
        MeFJointGetOrthogonalAxis(joint, body1_index, pax);
        MeVector3Cross(vec, pax, oax);
        MeVector3Copy(pax, vec);
    }

    MdtConstraintBodySetPositionRel(con, 0, pos[0], pos[1], pos[2]);
    MdtConstraintBodySetAxesRel(con, 0,
        pax[0], pax[1], pax[2],
        oax[0], oax[1], oax[2]);


    /*** BODY 2 ***/
    if (b2 == 0) /* joint with the world */
    {
        MeVector3 prim,orth,pos2,outpos;
        MeMatrix4Ptr tm = 0;
        MeFAssetPart *part = MeFJointGetPart(joint, body2_index);
        MeMatrix4 temp;

        if(part)
        {
            /* this is a joint between body 1 and a non-dynamics part
               so we still set position/axes relative to the part */
            MeFJointGetPosition(joint, body2_index, pos2);
            tm = MeFAssetPartGetTransformPtr(part);

            MeFJointGetPrimaryAxis(joint, body2_index, prim);
            MeFJointGetOrthogonalAxis(joint, body2_index, orth);
        }
        else
        {
            /* This is a joint with the world */
            /* we need to set outpos to be world position of joint */
            MeFJointGetPosition(joint, body1_index, pos2);
            part = MeFJointGetPart(joint, body1_index);
            tm = MeFAssetPartGetTransformPtr(part);

            MeFJointGetPrimaryAxis(joint, body1_index, prim);
            MeFJointGetOrthogonalAxis(joint, body1_index, orth);
        }

        if (assetTM)
        {
            MeMatrix4MultiplyMatrix(temp, tm, assetTM);
            tm = temp;
        }

        MeMatrix4TMTransform(outpos, tm, pos2);
        MdtConstraintBodySetPosition(con, 1, outpos[0], outpos[1], outpos[2]);
        MeMatrix4TMRotate(pax, tm, prim);
        MeMatrix4TMRotate(oax, tm, orth);

        MdtConstraintBodySetAxes(con, 1,
            pax[0], pax[1], pax[2],
            oax[0], oax[1], oax[2]);

    }
    else
    {
        MeFJointGetPosition(joint, body2_index, pos);
        MeFJointGetPrimaryAxis(joint, body2_index, pax);
        MeFJointGetOrthogonalAxis(joint, body2_index, oax);

        if (type == kMeFJointTypeCarwheel)
        {
            MeVector3 vec;
            MeFJointGetPrimaryAxis(joint, body2_index, oax);
            MeFJointGetOrthogonalAxis(joint, body2_index, pax);
            MeVector3Cross(vec, pax, oax);
            MeVector3Copy(pax, vec);
        }

        MdtConstraintBodySetPositionRel(con, 1, pos[0], pos[1], pos[2]);
        MdtConstraintBodySetAxesRel(con, 1,
            pax[0], pax[1], pax[2],
            oax[0], oax[1], oax[2]);
    }

    /* Constraint-specific configuration. */
    if(type == kMeFJointTypeBallAndSocket)
    {
        MdtBSJointID bs = MdtConstraintDCastBSJoint(con);
        /* Nothing to set up */
    }
    else if(type == kMeFJointTypeUniversal)
    {
        MdtUniversalID u = MdtConstraintDCastUniversal(con);
        /* Nothing to set up */
    }
    else if(type == kMeFJointTypeRpro)
    {
        MdtRPROJointID r = MdtConstraintDCastRPROJoint(con);

        MeReal p[3] = {0};
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength1, &p[0]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength2, &p[1]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength3, &p[2]);
        MdtRPROJointSetLinearStrength(r, p[0], p[1], p[2]);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength4, &p[0]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength5, &p[1]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength6, &p[2]);
        MdtRPROJointSetAngularStrength(r, p[0], p[1], p[2]);

    }
    else if(type == kMeFJointTypeHinge)
    {
        MdtHingeID h = MdtConstraintDCastHinge(con);
        MdtLimitID l = MdtHingeGetLimit(h);
        MdtSingleLimitID upper = MdtLimitGetUpperLimit(l);
        MdtSingleLimitID lower = MdtLimitGetLowerLimit(l);

        MeReal p = 0, p2 = 0;
        MeBool b = 0;

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop1, &p);
        MdtSingleLimitSetStop(upper, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop2, &p);
        MdtSingleLimitSetStop(lower, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p);
        MdtSingleLimitSetStiffness(upper, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness2, &p);
        MdtSingleLimitSetStiffness(lower, p);

        MeFJointGetProperty1b(joint, kMeFJointPropertyLimited1, &b);
        MdtLimitActivateLimits(l, b);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDesiredVelocity1, &p);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength1, &p2);

        MdtLimitSetLimitedForceMotor(l, p, p2);

        MeFJointGetProperty1b(joint, kMeFJointPropertyMotorized1, &b);
        MdtLimitActivateMotor(l, b);
    }
    else if(type == kMeFJointTypeCarwheel)
    {
        MdtCarWheelID cw = MdtConstraintDCastCarWheel(con);
        MeBool b = 0;
        MeReal p[6] = {0};

        MeFJointGetProperty1b(joint, kMeFJointPropertySpecialBool1, &b);
        MdtCarWheelSetSteeringLock(cw, b);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p[0]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping1, &p[1]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness2, &p[2]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStop2, &p[3]);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStop1, &p[4]);
        MeFJointGetProperty1f(joint, kMeFJointPropertySpecialFloat1, &p[5]);


        MdtCarWheelSetSuspension(cw, p[0], p[1], p[2], p[3], p[4], p[5]);

        /* Don't bother with steering params. Is this right? */
    }
    else if(type == kMeFJointTypeConeLimit)
    {
        MdtConeLimitID cl = MdtConstraintDCastConeLimit(con);
        MeReal p = 0;

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop1, &p);
        MdtConeLimitSetConeHalfAngle(cl, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p);
        MdtConeLimitSetStiffness(cl, p);
    }
    else if(type == kMeFJointTypePrismatic)
    {
        MdtPrismaticID h = MdtConstraintDCastPrismatic(con);
        MdtLimitID l = MdtPrismaticGetLimit(h);

        MdtSingleLimitID upper = MdtLimitGetUpperLimit(l);
        MdtSingleLimitID lower = MdtLimitGetLowerLimit(l);

        MeReal p = 0, p2 = 0;
        MeBool b = 0;

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop1, &p);
        MdtSingleLimitSetStop(upper, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop2, &p);
        MdtSingleLimitSetStop(lower, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p);
        MdtSingleLimitSetStiffness(upper, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness2, &p);
        MdtSingleLimitSetStiffness(lower, p);

        MeFJointGetProperty1b(joint, kMeFJointPropertyLimited1, &b);
        MdtLimitActivateLimits(l, b);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDesiredVelocity1, &p);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStrength1, &p2);

        MdtLimitSetLimitedForceMotor(l, p, p2);

        MeFJointGetProperty1b(joint, kMeFJointPropertyMotorized1, &b);
        MdtLimitActivateMotor(l, b);
    }
    else if(type == kMeFJointTypeSkeletal)
    {
        MdtSkeletalID j = MdtConstraintDCastSkeletal(con);
        MdtSkeletalConeOption co;
        MdtSkeletalTwistOption to;
        int i = 0;
        MeReal p = 0;

        MeFJointGetProperty1i(joint, kMeFJointPropertySpecialInt1, &i);

        switch (i)
        {
        case 0:
            co = MdtSkeletalConeOptionFree;
            break;
        case 1:
            co = MdtSkeletalConeOptionSlot;
            break;
        case 2:
            co = MdtSkeletalConeOptionCone;
            break;
        case 3:
            co = MdtSkeletalConeOptionFixed;
            break;
        }

        MdtSkeletalSetConeOption(j, co);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop1, &p);
        MdtSkeletalSetConePrimaryLimitAngle(j, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop2, &p);
        MdtSkeletalSetConeSecondaryLimitAngle(j, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p);
        MdtSkeletalSetConeStiffness(j, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping1, &p);
        MdtSkeletalSetConeDamping(j, p);

        MeFJointGetProperty1i(joint, kMeFJointPropertySpecialInt2, &i);
        switch (i)
        {
        case 0:
            to = MdtSkeletalTwistOptionFree;
            break;
        case 1:
            to = MdtSkeletalTwistOptionLimited;
            break;
        case 2:
            to = MdtSkeletalTwistOptionFixed;
            break;
        }

        MdtSkeletalSetTwistOption(j, to);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStop3, &p);
        MdtSkeletalSetTwistLimitAngle(j, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness2, &p);
        MdtSkeletalSetTwistStiffness(j, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping2, &p);
        MdtSkeletalSetTwistDamping(j, p);

        /* Here is a large hack for skeletal to the world */
        if(b2 == 0)
        {
            MeMatrix3 I;
            MeReal fudgeFactor = (MeReal)1.1;
            MdtBodyID bID = MdtConstraintGetBody(con, 0);

            MdtBodyGetInertiaTensor(bID, I);
            if(I[0][0] < I[1][1] && I[0][0] < I[2][2])
            {
                I[0][0] = fudgeFactor * MeMIN(I[1][1], I[2][2]);
                MdtBodySetInertiaTensor(bID, I);
                MeWarning(0,"AF: Modified Inertia for Skeletal to World");
            }
        }
    }
    else if (type == kMeFJointTypeAngular3)
    {
        MdtAngular3ID j = MdtConstraintDCastAngular3(con);
        MeBool b = 0;
        MeReal p = 0;

        MeFJointGetProperty1b(joint, kMeFJointPropertySpecialBool1, &b);
        MdtAngular3EnableRotation(j, b);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p);
        MdtAngular3SetStiffness(j, p);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping1, &p);
        MdtAngular3SetDamping(j, p);
    }
    else if (type == kMeFJointTypeSpring6)
    {
        MdtSpring6ID j = MdtConstraintDCastSpring6(con);
        MeReal p1 = 0, p2 = 0, p3 = 0;

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness1, &p1);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness2, &p2);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness3, &p3);
        MdtSpring6SetLinearStiffness(j, 0, p1);
        MdtSpring6SetLinearStiffness(j, 1, p2);
        MdtSpring6SetLinearStiffness(j, 2, p3);

        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness4, &p1);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness5, &p2);
        MeFJointGetProperty1f(joint, kMeFJointPropertyStiffness6, &p3);
        MdtSpring6SetAngularStiffness(j, 0, p1);
        MdtSpring6SetAngularStiffness(j, 1, p2);
        MdtSpring6SetAngularStiffness(j, 2, p3);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping1, &p1);
        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping2, &p2);
        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping3, &p3);
        MdtSpring6SetLinearDamping(j, 0, p1);
        MdtSpring6SetLinearDamping(j, 1, p2);
        MdtSpring6SetLinearDamping(j, 2, p3);

        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping4, &p1);
        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping5, &p2);
        MeFJointGetProperty1f(joint, kMeFJointPropertyDamping6, &p3);
        MdtSpring6SetAngularDamping(j, 0, p1);
        MdtSpring6SetAngularDamping(j, 1, p2);
        MdtSpring6SetAngularDamping(j, 2, p3);
    }

    return con;
}
