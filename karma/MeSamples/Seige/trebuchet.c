#include "Trebuchet.h"

//////////////////////////////////////////////////////////
// need to calc density for use in mstmodandbodcreate.
///////////////////////////////////////////////////////////

/* Create A Trebuchet */
Trebuchet* Trebuchet_create(MstUniverseID u, RRender* rc, TrebuchetProto* p) {

	// Allocate memory for the structures
    Trebuchet* t = MeMemoryAPI.create(sizeof(Trebuchet));
	TrebuchetProto_newInstance(p);
	t->proto = p;

	t->co = CompoundObject_create(u, rc,
		p->bodyList,	p->modelList,	p->jointList,	p->aggList,
		p->numBodies,	p->numModels,	p->numJoints,	p->numAggs,
		p->numAggElements);



	// set the tether etc!!!!!!!!!!!!!!!!!!!!!!!1
	t->bBS1 = t->co->joint[2];
	t->tether = t->co->joint[5];

	return t;
}

/* Reset Trebuchet to initial positions */
void Trebuchet_reset(Trebuchet* t)
{
	CompoundObject_reset(t->co);
}


void MEAPI Trebuchet_fire(Trebuchet* t)
{
	MdtConstraintDisable(t->tether);
}

void Trebuchet_releaseBucket(Trebuchet* t) {

	// Check to see if there is little force exerted on the BUCKET.
	MeVector3 force;

	// Make sure that the trebuchet is not still tethered
	if ((MdtConstraintIsEnabled(t->bBS1))&&(!MdtConstraintIsEnabled(t->tether))){
		// get the force on the BUCKET exerted bu the rope
		MdtConstraintGetForce(t->bBS1,0,force);
		// release if the force is pulling down and back
		if ((force[0] < 0.0f)&&(force[1] < 0.0f))
			{ MdtConstraintDisable(t->bBS1);}
	}
}

void Trebuchet_destroy(Trebuchet* t)
{
	/* Destroy the compound object (containing models joint and bodies */
    CompoundObject_destroy(t->co);
	/* Tell the proto that we've removed an instance*/
	TrebuchetProto_removeInstance(t->proto);
    MeMemoryAPI.destroy(t);
}

