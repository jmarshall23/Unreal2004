#ifndef _trebuchet_h
#define _trebuchet_h

#include <MeMath.h>
#include <Mcd.h>
#include <Mst.h>
#include <Mdt.h>
#include <MeViewer.h>

#include "CompoundObject.h"
#include "TrebuchetProto.h"


typedef struct
{
	TrebuchetProto*		proto;
	CompoundObject*		co;
	MdtConstraintID		bBS1;
	MdtConstraintID		tether;

} Trebuchet;








/* Create A Trebuchet */
Trebuchet* Trebuchet_create(MstUniverseID u, RRender* rc, TrebuchetProto* p);
/* Delete a Trebuchet */
void Trebuchet_destroy(Trebuchet* t);
/* Setup functions */

/* Actions */
void Trebuchet_reset(Trebuchet* t);
void MEAPI Trebuchet_fire(Trebuchet* t);
void Trebuchet_releaseBucket(Trebuchet* t);


#endif

