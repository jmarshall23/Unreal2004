/* included from qhull's user.h (which is included from qhull_a.h)
   Needed to compile qhull properly without compiler flags
*/
#include <MePrecision.h>
#ifndef _ME_API_DOUBLE
#define REALfloat 1 /*define QHull real as float*/
#else
#define REALfloat 0 /*define QHull real as double*/
#endif
