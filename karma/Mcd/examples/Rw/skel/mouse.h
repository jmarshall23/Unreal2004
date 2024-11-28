#include <rwcore.h>

RwBool rsMouseInit(void);
RwBool rsMouseTerm(void);
void rsMouseAddDelta(RwV2d *delta);
void rsMouseGetPos(RwV2d *pos);
void rsMouseSetPos(RwV2d *pos);
void rsMouseRender(RwCamera *camera);
void rsMouseVisible(RwBool visible);
