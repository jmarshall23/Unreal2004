#include "CompoundObject.h"


//////////////////////////////////////////////////////////
// need to calc density for use in mstmodandbodcreate.
///////////////////////////////////////////////////////////

/* Create A CompoundObject */
CompoundObject* CompoundObject_create(MstUniverseID u, RRender* rc,
									  bodyData* bl,	modelData* ml,	jointData* jl,	aggData*	al,
									  int numB,		int numM,		int numJ,		int numA,
									  int numAE)
{

	// Allocate memory for the structures
    CompoundObject* t = MeMemoryAPI.create(sizeof(CompoundObject));

	t->numBodies = numB;
	t->numModels = numM;
	t->numJoints = numJ;
	t->numAggs	 = numA;
	t->numAggElements = numAE;

	t->bl = bl;
	t->ml = ml;
	t->jl = jl;
	t->al = al;

    t->body = (MdtBodyID*)MeMemoryAPI.create(t->numBodies * sizeof(MdtBodyID));
    t->bodyG = (RGraphic**)MeMemoryAPI.create(t->numBodies * sizeof(RGraphic*));

    t->model = (McdModelID*)MeMemoryAPI.create(t->numModels * sizeof(McdModelID));
    t->modelG = (RGraphic**)MeMemoryAPI.create(t->numModels * sizeof(RGraphic*));

    t->joint = (MdtConstraintID*)MeMemoryAPI.create(t->numJoints * sizeof(MdtConstraintID));
    t->jointTM = (MeMatrix4*)MeMemoryAPI.createAligned(t->numJoints * sizeof(MeMatrix4), 16);


    t->aggModel = (McdModelID*)MeMemoryAPI.create(t->numAggs * sizeof(McdModelID));
	// Count the numbner of graphics necc for all the aggregates
    t->aggG = (RGraphic**)MeMemoryAPI.create(t->numAggElements * sizeof(RGraphic*));
	t->aggTransMatrix = (MeMatrix4*)MeMemoryAPI.createAligned(t->numAggElements * sizeof(MeMatrix4), 16);

	/* Initialise all the structures */
	CompoundObject_setupBodies(u,rc,t);
	CompoundObject_setupModels(u,rc,t);
	CompoundObject_setupAggregates(u,rc,t);
	CompoundObject_setupJoints(u,rc,t);

	return t;
}

/* Initialise all the dynamics only bodies */
void CompoundObject_setupBodies(MstUniverseID u, RRender* rc, CompoundObject* t)
{
    int i;
	MeMatrix3 R;

    for(i=0; i<t->numBodies; i++)
    {
        t->body[i] = MdtBodyCreate(u->world);

        MdtBodySetPosition(t->body[i],
            t->bl[i].elem.pos[0], t->bl[i].elem.pos[1], t->bl[i].elem.pos[2]);


        MeMatrix3FromEulerAngles(R,
            t->bl[i].elem.rot[0], t->bl[i].elem.rot[1], t->bl[i].elem.rot[2]);

        MdtBodySetOrientation(t->body[i], R);

		CompoundObject_generateGraphics(rc, t->bodyG[i], t->bl[i].elem, MdtBodyGetTransformPtr(t->body[i]));

        MdtBodySetAngularVelocityDamping(t->body[i], 0.5f);
        MdtBodySetLinearVelocityDamping(t->body[i], 0.5f);

        MdtBodySetAngularVelocity(t->body[i], 0.0f,0.0f,0.0f);
        MdtBodySetLinearVelocity(t->body[i], 0.0f,0.0f,0.0f);

        MdtBodyEnable(t->body[i]);
    }
}

/* Initialise all the collision models (and associated bodies */
void CompoundObject_setupModels(MstUniverseID u, RRender* rc, CompoundObject* t){

    int i;
	MeMatrix3 R;

	
    for(i=0; i<t->numModels; i++)
    {

        t->model[i] = MstModelAndBodyCreate(u,t->ml[i].geom,t->ml[i].density);
        MdtBodySetMass(McdModelGetBody(t->model[i]),t->ml[i].density);

        MdtBodySetPosition(McdModelGetBody(t->model[i]),
            t->ml[i].elem.pos[0], t->ml[i].elem.pos[1], t->ml[i].elem.pos[2]);

        MeMatrix3FromEulerAngles(R,
            t->ml[i].elem.rot[0], t->ml[i].elem.rot[1], t->ml[i].elem.rot[2]);

        MdtBodySetOrientation(McdModelGetBody(t->model[i]), R);

		CompoundObject_generateGraphics(rc, t->modelG[i], t->ml[i].elem, McdModelGetTransformPtr(t->model[i]));

//        MdtBodySetAngularVelocityDamping(McdModelGetBody(t->model[i]), 0.5f);
//        MdtBodySetLinearVelocityDamping(McdModelGetBody(t->model[i]), 0.5f);
        MdtBodySetAngularVelocity(McdModelGetBody(t->model[i]),0.0f,0.0f,0.0f);
        MdtBodySetLinearVelocity(McdModelGetBody(t->model[i]), 0.0f,0.0f,0.0f);

        MdtBodyEnable(McdModelGetBody(t->model[i]));
    }
		
}

/* Initialise all the Joints */
void CompoundObject_setupJoints(MstUniverseID u, RRender* rc, CompoundObject* t){

    int i,j;
	MdtBodyID body[2];
	

    for(i=0; i<t->numJoints; i++)
    {
        MeMatrix4TMMakeIdentity(t->jointTM[i]);

#if DRAW_JOINTS
        RGraphicBoxCreate(rc, 0.1, 0.1, 0.1, jointColor, t->jointTM[i]);
#endif

		/* Get the body indices of the attached bodies */
		for(j=0; j<2; j++){
			/* check to see if index is set to -1, this implies attached to the world */
			if (t->jl[i].bodies[j]==-1){
				body[j]=0;
			}else{
				/* decide whether in body or model list */
				switch (t->jl[i].ModOrBod[j]){
				case isModel:
					body[j]=McdModelGetBody(t->model[t->jl[i].bodies[j]]);
					break;
				case isBody:
					body[j]=t->body[t->jl[i].bodies[j]];
					break;
				case isAggregate:
					body[j]=McdModelGetBody(t->aggModel[t->jl[i].bodies[j]]);
					break;
				}
			}
		}
		switch (t->jl[i].type) {

		case HingeJoint:
			{

//            MdtLimitID limit;

            MdtHingeID hinge = MdtHingeCreate(u->world);

			/* set the bodies first */

            MdtHingeSetBodies(hinge, body[0], body[1]);

            MdtHingeSetPosition(hinge, t->jl[i].pos[0], t->jl[i].pos[1], t->jl[i].pos[2]);
            MdtHingeSetAxis(hinge, t->jl[i].axis[0], t->jl[i].axis[1], t->jl[i].axis[2]);

			/* for sorting out the stiffness and damping 
            if(t->jl[i].Param[i][0] > 0.01 || t->jl[i]Param[i][1] > 0.01)
            {
                limit = MdtHingeGetLimit(hinge);
                MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), t->jl[i]Param[i][0]);
                MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), t->jl[i]Param[i][0]);
                MdtSingleLimitSetDamping(MdtLimitGetUpperLimit(limit), t->jl[i]Param[i][1]);
                MdtSingleLimitSetDamping(MdtLimitGetLowerLimit(limit), t->jl[i]Param[i][1]);
                MdtLimitActivateLimits(limit, 1);
            }*/

			/* Cast into Constraint to keep in array */
            t->joint[i] = MdtHingeQuaConstraint(hinge);
			}
			break;

		case BSJoint:
			{
            MdtBSJointID bs = MdtBSJointCreate(u->world);

			// set the bodies first
            MdtBSJointSetBodies(bs, body[0], body[1]);

            MdtBSJointSetPosition(bs, t->jl[i].pos[0], t->jl[i].pos[1], t->jl[i].pos[2]);

            t->joint[i] = MdtBSJointQuaConstraint(bs);
			}
			break;
		
		case RPROJoint:
			{
            MdtRPROJointID rpro = MdtRPROJointCreate(u->world);

			// set the bodies first
            MdtRPROJointSetBodies(rpro, body[0], body[1]);

            MdtRPROJointSetAttachmentPosition(rpro, t->jl[i].pos[0], t->jl[i].pos[1], t->jl[i].pos[2],0);
            MdtRPROJointSetAttachmentPosition(rpro, t->jl[i].axis[0], t->jl[i].axis[1], t->jl[i].axis[2],1);
			MdtRPROJointSetLinearStrength(rpro,100.0f,100.0f,100.0f);

            t->joint[i] = MdtRPROJointQuaConstraint(rpro);
			}
			break;
		}

        if(t->jl[i].isEnabled) {MdtConstraintEnable(t->joint[i]);}
    }

	// Setup the references to the breaking joints. Much better to do this in the treb structure - but hey what the hell
	// Coule put the enums from setup into treb struct - this would vbe much better ;-)
//	t->bBS1 = t->joint[2];
//	t->tether = t->joint[5];

}

/* Initialise all the Aggregates */
void CompoundObject_setupAggregates(MstUniverseID u, RRender* rc, CompoundObject* t){

    int i,j,totalComponentCount;
	McdGeometryInstanceID geomInst;

	totalComponentCount=0;
    for(i=0; i<t->numAggs; i++)
    {

        t->aggModel[i] = MstModelAndBodyCreate(u,t->al[i].geom,t->al[i].density);
		/* Hold a pointer to the geometry instance */
		geomInst =	McdModelGetGeometryInstance(t->aggModel[i]);
		/* Create the graphics */
	    for(j=0; j<t->al[i].numElements; j++)
		{
			// set a pointer to transformation matrix
			McdGeometryInstanceSetTransformPtr(
				// get the jth child and set the pointer
				McdGeometryInstanceGetChild(geomInst,j), 
				t->aggTransMatrix[totalComponentCount]);
			CompoundObject_generateGraphics(rc,
				t->aggG[totalComponentCount],
				t->al[i].element[j].elem, 
				t->aggTransMatrix[totalComponentCount]);
			totalComponentCount++;
		}
	}

		
}

void CompoundObject_generateGraphics(RRender* rc, RGraphic* g, elemData e, MeMatrix4Ptr t){
	switch (e.type){
		case box:
            g = RGraphicBoxCreate(rc,
                e.dim[0], e.dim[1], e.dim[2],
                e.col, t);
			break;

		case cylinder:
            g = RGraphicCylinderCreate(rc,
                e.dim[0]*0.5f, e.dim[2],
                e.col, t);
			break;

		case sphere:
			g = RGraphicSphereCreate(rc,
                e.dim[0]*0.5f,
                e.col, t);
			break;

		default:
			/* Don't know this geometry - so show a bounding box */
			g = RGraphicBoxCreate(rc,
                e.dim[0], e.dim[1], e.dim[2],
                e.col, t);

		}
}

/* Initialise all the dynamics only bodies */
void CompoundObject_resetBodies(CompoundObject* t)
{
    int i;
	MeMatrix3 R;

    for(i=0; i<t->numBodies; i++)
    {

        MdtBodySetPosition(t->body[i],
            t->bl[i].elem.pos[0], t->bl[i].elem.pos[1], t->bl[i].elem.pos[2]);


        MeMatrix3FromEulerAngles(R,
            t->bl[i].elem.rot[0], t->bl[i].elem.rot[1], t->bl[i].elem.rot[2]);

        MdtBodySetOrientation(t->body[i], R);

        MdtBodySetAngularVelocityDamping(t->body[i], 0.5f);
        MdtBodySetLinearVelocityDamping(t->body[i], 0.5f);

        MdtBodyEnable(t->body[i]);
    }
}

/* Initialise all the collision models (and associated bodies */
void CompoundObject_resetModels(CompoundObject* t){

    int i;
	MeMatrix3 R;

	
    for(i=0; i<t->numModels; i++)
    {

//#error "This will not work - using density not density for creation"

        MdtBodySetPosition(McdModelGetBody(t->model[i]),
            t->ml[i].elem.pos[0], t->ml[i].elem.pos[1], t->ml[i].elem.pos[2]);

        MeMatrix3FromEulerAngles(R,
            t->ml[i].elem.rot[0], t->ml[i].elem.rot[1], t->ml[i].elem.rot[2]);

        MdtBodySetOrientation(McdModelGetBody(t->model[i]), R);

//        MdtBodySetAngularVelocityDamping(McdModelGetBody(t->model[i]), 0.5f);
//        MdtBodySetLinearVelocityDamping(McdModelGetBody(t->model[i]), 0.5f);

        MdtBodyEnable(McdModelGetBody(t->model[i]));
    }
		

}

/* Initialise all the Joints */
void CompoundObject_resetJoints(CompoundObject* t){

	int i;

    for(i=0; i<t->numJoints; i++) {
        if(t->jl[i].isEnabled) {MdtConstraintEnable(t->joint[i]);}
    }
}



/* Initialise all the collision models (and associated bodies */
void CompoundObject_resetAggs(CompoundObject* t){

    int i;
	MeMatrix3 R;

	
    for(i=0; i<t->numAggs; i++)
    {

        MdtBodySetPosition(McdModelGetBody(t->aggModel[i]),
            t->al[i].elem.pos[0], t->al[i].elem.pos[1], t->al[i].elem.pos[2]);

        MeMatrix3FromEulerAngles(R,
            t->al[i].elem.rot[0], t->al[i].elem.rot[1], t->al[i].elem.rot[2]);

        MdtBodySetOrientation(McdModelGetBody(t->aggModel[i]), R);

//        MdtBodySetAngularVelocityDamping(McdModelGetBody(t->model[i]), 0.5f);
//        MdtBodySetLinearVelocityDamping(McdModelGetBody(t->model[i]), 0.5f);

        MdtBodyEnable(McdModelGetBody(t->aggModel[i]));
    }
		

}





/* Reset CompoundObject to initial positions */
//void CompoundObjectReset(RRender* rc, void* userData)
void CompoundObject_reset(CompoundObject* t)
{
	CompoundObject_resetBodies(t);
	CompoundObject_resetModels(t);
	CompoundObject_resetJoints(t);
	CompoundObject_resetAggs(t);
}



void CompoundObject_destroy(CompoundObject* t)
{
    MeMemoryAPI.destroy(t->body);
    MeMemoryAPI.destroy(t->bodyG);
    MeMemoryAPI.destroy(t->model);
    MeMemoryAPI.destroy(t->modelG);
    MeMemoryAPI.destroy(t->joint);
    MeMemoryAPI.destroyAligned(t->jointTM);
    MeMemoryAPI.destroy(t->aggModel);
    MeMemoryAPI.destroy(t->aggG);
	MeMemoryAPI.destroyAligned(t->aggTransMatrix);
    MeMemoryAPI.destroy(t);
}

