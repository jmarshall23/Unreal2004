/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.16.6.2 $

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

#include <MdtCheckMacros.h>
#include <MdtContactParams.h>
#include <MdtContact.h>

/**
 * Initialises Contact Parameters structure.
 *
 * Initialises to default values the contact parameters structure. These are
 * the values that usually remain constant over a contacts lifetime.
 *
 * Default values:
 *
 * @arg friction type = MdtContactTypeFrictionZero
 * @arg friction model1 & model2 = MdtFrictionModelBox
 * @arg friction (primary & secondary) = INFINITY
 * @arg friction coefficient (primary & secondary) = 0 
 * @arg restitution = 0
 * @arg velocity threshold = 0.001
 * @arg softness = 0
 * @arg stickiness = 0
 * @arg slip1 = slip2 = 0
 * @arg slide1 = slide2 = 0
 *
 * @param p MdtContactParamsID to reset
 */
void MEAPI MdtContactParamsReset(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsReset");

    /* zero friction is the default */
    p->type = MdtContactTypeFrictionZero;

    /* use box friction by default */
    p->model1 = MdtFrictionModelBox;
    p->model2 = MdtFrictionModelBox;
    p->options = 0;

    /* initialise contact parameters */
    p->restitution = 0;
    p->velThreshold = (MeReal)(0.001);
    p->softness = 0;
    p->max_adhesive_force = 0;

    p->friction1 = MEINFINITY;
    p->frictioncoeff1 = 0;
    p->slip1 = 0;
    p->friction2 = MEINFINITY;
    p->frictioncoeff2 = 0;
    p->slip2 = 0;
    p->slide1 = 0;
    p->slide2 = 0;
}

/*
  Accessors
*/

/**
 * Returns the contact type.
 */
MdtContactType MEAPI MdtContactParamsGetType(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetType");
    return p->type;
}

/**
 * Returns the friction model along the primary direction.
 */
MdtFrictionModel MEAPI MdtContactParamsGetPrimaryFrictionModel(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetPrimaryFrictionModel");
    return p->model1;
}

/**
 * Returns the friction model along the secondary direction.
 */
MdtFrictionModel MEAPI MdtContactParamsGetSecondaryFrictionModel(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetSecondaryFrictionModel");
    return p->model2;
}

/**
 * Returns the current restitution of this contact.
 */
MeReal MEAPI MdtContactParamsGetRestitution(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetRestitution");
    return p->restitution;
}

/**
 * Returns the current impact velocity threshold to use restitution.
 */
MeReal MEAPI MdtContactParamsGetRestitutionThreshold(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetRestitutionThreshold");
    return p->velThreshold;
}

/**
 * Returns the softness (indicates amount of allowed
 * penetration) of this contact.
 */
MeReal MEAPI MdtContactParamsGetSoftness(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetSoftness");
    return p->softness;
}

/**
 * Returns the max_adhesive_force at this contact.
 */
MeReal MEAPI MdtContactParamsGetMaxAdhesiveForce(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetMaxAdhesiveForce");
    return p->max_adhesive_force;
}

/**
 * Returns friction in the primary direction.
 */
MeReal MEAPI MdtContactParamsGetPrimaryFriction(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetPrimaryFriction");
    return p->friction1;
}

/**
 * Returns friction coeffecient in the primary direction.
 */
MeReal MEAPI MdtContactParamsGetPrimaryFrictionCoeffecient(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetPrimaryFrictionCoeffecient");
    return p->frictioncoeff1;    
}


/**
 * Returns slip in the primary direction.
 */
MeReal MEAPI MdtContactParamsGetPrimarySlip(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetPrimarySlip");
    return p->slip1;
}

/**
 * Returns slide in the primary direction.
 */
MeReal MEAPI MdtContactParamsGetPrimarySlide(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetPrimarySlide");
    return p->slide1;
}

/**
 * Returns friction in the secondary direction.
 */
MeReal MEAPI MdtContactParamsGetSecondaryFriction(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetSecondaryFriction");
    return p->friction2;
}

/**
 * Returns friction coeffecient in the secondary direction.
 */
MeReal MEAPI MdtContactParamsGetSecondaryFrictionCoeffecient(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetSecondaryFrictionCoeffecient");
    return p->frictioncoeff2;    
}

/**
 * Returns slip in the secondary direction.
 */
MeReal MEAPI MdtContactParamsGetSecondarySlip(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetSecondarySlip");
    return p->slip2;
}

/**
 * Returns slide in the secondary direction.
 */
MeReal MEAPI MdtContactParamsGetSecondarySlide(const MdtContactParamsID p)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsGetSecondarySlide");
    return p->slide2;
}

/*
  Mutators
*/

/**
 * Sets the type of this contact.
 */
void MEAPI MdtContactParamsSetType(const MdtContactParamsID p, MdtContactType t)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetType");
    switch (t)
    {

    case MdtContactTypeFrictionZero:
        p->type = MdtContactTypeFrictionZero;
        break;

    case MdtContactTypeFriction1D:
        p->type = MdtContactTypeFriction1D;
        break;

    case MdtContactTypeFriction2D:
        p->type = MdtContactTypeFriction2D;
        break;

    default:
        MeWarning(12,
            "Unknown Contact Type, defaulting to zero friction.");
        p->type = MdtContactTypeFrictionZero;
    }
}

/**
 * Sets the friction model to use along the primary direction.
 * @see MdtContactSetDirection
 */
void MEAPI MdtContactParamsSetPrimaryFrictionModel(const MdtContactParamsID p,
                                             MdtFrictionModel m)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetPrimaryFrictionModel");
    switch (m)
    {

    case MdtFrictionModelBox:
        p->model1 = MdtFrictionModelBox;
        break;

    case MdtFrictionModelNormalForce:
        p->model1 = MdtFrictionModelNormalForce;
        break;

    default:
        MeWarning(12,
            "Unknown Friction Model, defaulting to box friction.");
        p->model1 = MdtFrictionModelBox;
    }
}

/**
 * Sets the friction model to use along the secondary direction.
 * @see MdtContactSetDirection 
 */
void MEAPI MdtContactParamsSetSecondaryFrictionModel(const MdtContactParamsID p,
                                               MdtFrictionModel m)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSecondaryFrictionModel");
    switch (m)
    {

    case MdtFrictionModelBox:
        p->model2 = MdtFrictionModelBox;
        break;

    case MdtFrictionModelNormalForce:
        p->model2 = MdtFrictionModelNormalForce;
        break;

    default:
        MeWarning(12,
            "Unknown Friction Model, defaulting to box friction.");
        p->model2 = MdtFrictionModelBox;
    }
}

/**
 * Set the primary and friction models.
 * @see MdtContactParamsSetPrimaryFrictionModel
 * @see MdtContactParamsSetSecondaryFrictionModel
 */
void MEAPI MdtContactParamsSetFrictionModel(const MdtContactParamsID p,
                                            MdtFrictionModel m)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetFrictionModel");
    MdtContactParamsSetPrimaryFrictionModel(p, m);
    MdtContactParamsSetSecondaryFrictionModel(p, m);    
}

/**
 * Sets the amount of restitution at the contact.
 */
void MEAPI MdtContactParamsSetRestitution(const MdtContactParamsID p, MeReal r)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetRestitution");
    if(r<0)
    {
        MeWarning(12, "MdtContactParamsSetRestitution: "
            "Negative restitution not valid - setting to zero.");
        r=0;
    }

    p->restitution = r;
    if (!ME_IS_ZERO(r))
        p->options = p->options | MdtBclContactOptionBounce;
    else
        p->options &= ~MdtBclContactOptionBounce;
}

/**
 * Sets the current impact velocity threshold to use restitution.
 */
void MEAPI MdtContactParamsSetRestitutionThreshold(const MdtContactParamsID p,
                                             MeReal v)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetRestitutionThreshold");
    p->velThreshold = v;
}

/** Sets the softness for this contact. */
void MEAPI MdtContactParamsSetSoftness(const MdtContactParamsID p,  MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSoftness");
    if(s<0)
    {
        MeWarning(12, "MdtContactParamsSetSoftness: "
            "Negative softness not valid - setting to zero.");
        s=0;
    }
    p->softness = s;
    if (!ME_IS_ZERO(s))
        p->options = p->options | MdtBclContactOptionSoft;
    else
        p->options &= ~MdtBclContactOptionSoft;
}

/** Sets the max_adhesive_force for this contact. */
void MEAPI MdtContactParamsSetMaxAdhesiveForce(const MdtContactParamsID p, MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetMaxAdhesiveForce");
    p->max_adhesive_force = s;
    if (!ME_IS_ZERO(s))
        p->options = p->options | MdtBclContactOptionAdhesive;
    else
        p->options &= ~MdtBclContactOptionAdhesive;
}

/** 
 * Sets the maximum friction force applied in the primary direction. 
 * Only used when MdtFrictionModel is set to MdtFrictionModelBox. 
 */
void MEAPI MdtContactParamsSetPrimaryFriction(const MdtContactParamsID p, MeReal f)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetPrimaryFriction");
    p->friction1 = f;
}

/** 
 * Sets the friction coeffecient in the primary direction. 
 * Only used when MdtFrictionModel is set to MdtFrictionModelNormalForce.  
 */
void MEAPI MdtContactParamsSetPrimaryFrictionCoeffecient(const MdtContactParamsID p, 
                                                           MeReal f)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetPrimaryFrictionCoeffecient");
    p->frictioncoeff1 = f;
}

/** Sets the amount of slip on the primary direction. */
void MEAPI MdtContactParamsSetPrimarySlip(const MdtContactParamsID p, MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetPrimarySlip");
    if(s<0)
    {
        MeWarning(12, "MdtContactParamsSetPrimarySlip: "
            "Negative slip not valid - setting to zero.");
        s=0;
    }

    p->slip1 = s;
    if (!ME_IS_ZERO(s))
        p->options = p->options | MdtBclContactOptionSlip1;
    else
        p->options &= ~MdtBclContactOptionSlip1;
}

/** Sets the amount of slide in the primary direction. */
void MEAPI MdtContactParamsSetPrimarySlide(const MdtContactParamsID p, MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetPrimarySlide");
    p->slide1 = s;
    if (!ME_IS_ZERO(s))
        p->options = p->options | MdtBclContactOptionSlide1;
    else
        p->options &= ~MdtBclContactOptionSlide1;
}

/** 
 * Sets the maximum friction force applied in the secondary direction. 
 * Only used when MdtFrictionModel is set to MdtFrictionModelBox. 
 */
void MEAPI MdtContactParamsSetSecondaryFriction(const MdtContactParamsID p, 
               MeReal f)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSecondaryFriction");
    p->friction2 = f;
}

/** 
 * Sets the friction coeffecient in the secondary direction. 
 * Only used when MdtFrictionModel is set to MdtFrictionModelNormalForce.  
 */
void MEAPI MdtContactParamsSetSecondaryFrictionCoeffecient(const MdtContactParamsID p, 
               MeReal f)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSecondaryFrictionCoeffecient");
    p->frictioncoeff2 = f;
}

/** Sets the amount of slip on the secondary direction */
void MEAPI MdtContactParamsSetSecondarySlip(const MdtContactParamsID p, MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSecondarySlip");
    if(s<0)
    {
        MeWarning(12, "MdtContactParamsSetSecondarySlip: "
            "Negative slip not valid - setting to zero.");
        s=0;
    }

    p->slip2 = s;
    if (!ME_IS_ZERO(s))
        p->options = p->options | MdtBclContactOptionSlip2;
    else
        p->options &= ~MdtBclContactOptionSlip2;
}

/** Sets the amount of slide on the secondary direction. */
void MEAPI MdtContactParamsSetSecondarySlide(const MdtContactParamsID p, MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSecondarySlide");
    p->slide2 = s;
    if (!ME_IS_ZERO(s))
        p->options = p->options | MdtBclContactOptionSlide2;
    else
        p->options &= ~MdtBclContactOptionSlide2;
}

/** Sets the amount of velocity slip in the primary & secondary direction. */
void MEAPI MdtContactParamsSetSlip(const MdtContactParamsID p, MeReal s)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetSlip");
    MdtContactParamsSetPrimarySlip(p, s);
    MdtContactParamsSetSecondarySlip(p, s);
}

/**
 * Set the maximum frictional force applied in the primary and secondary direction.
 * Only used when MdtFrictionModel is set to MdtFrictionModelBox.
 *
 * @see MdtContactParamsSetPrimaryFriction
 * @see MdtContactParamsSetSecondaryFriction
 */
void MEAPI MdtContactParamsSetFriction(const MdtContactParamsID p, MeReal f)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetFriction");
    if(f<0)
    {
        MeWarning(12, "MdtContactParamsSetFriction: "
            "Negative friction not valid - setting to zero.");
        f=0;
    }
    p->friction1 = f;
    p->friction2 = f;
}

/**
 * Sets the friction coeffecient in the primary and secondary direction.
 * Only used when MdtFrictionModel is set to MdtFrictionModelNormalForce.
 *
 * @see MdtContactParamsSetPrimaryFrictionCoeffecient
 * @see MdtContactParamsSetSecondaryFrictionCoeffecient
 */
void MEAPI MdtContactParamsSetFrictionCoeffecient(const MdtContactParamsID p, 
               MeReal f)
{
    MdtCHECKCONTACTPARAMS(p,"MdtContactParamsSetFrictionCoeffecient");
    if(f<0)
    {
        MeWarning(12, "MdtContactParamsSetPrimarySlip: "
            "Negative friction not valid - setting to zero.");
        f=0;
    }
    p->frictioncoeff1 = f;
    p->frictioncoeff2 = f;
}
