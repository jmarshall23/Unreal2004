
#ifdef __cplusplus
extern "C"
{
#endif

#include "MdtKea.h"

/*
   writeKeaInputToFile
   -------------------

   Writes the entire input to kea as a text file
   This is used for debugging
*/

void writeKeaInputToFile(const MdtKeaConstraints  constraints,
						 const MdtKeaParameters   parameters,
                         const MdtKeaBody *const  blist[], 
						 int                      num_bodies);

void readKeaInputFromFile(MdtKeaConstraints * constraints,
					  	  MdtKeaParameters *  parameters,
                          MdtKeaBody *        blist[], 
						  int *               num_bodies);

void writeLambdaToFile(const char * filename,
					   const MeReal lambda[],
					   int          num_elts);

void writeFloatArrayToFile(int            file,
						   const char *   desc,
						   int            num_elts,
						   const MeReal * array);

void writeIntArrayToFile(int            file,
	  			         const char *   desc,
						 int            num_elts,
						 const int * array);

void writeIntToFile(int            file,
					const char *   desc,
					int            value);

void keaCloseDebugDataFile(int file);

#ifdef __cplusplus
}
#endif
