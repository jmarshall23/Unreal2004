#include <stdio.h>
#include "MdtKea.h"
#include "MeSimpleFile.h"
#include "MeStream.h"
#include "ReadWriteKeaInputToFile.h"
#include "MeMemory.h"
#include "MeMessage.h"
#include <keaFunctions.hpp>
#include "keaDebug.h"

extern MdtKeaDebugDataRequest * gDebug;

void writeIntArrayToFile(int            file,
						 const char *   desc,
						 int            num_elts,
						 const int *    array)
{
	int i,count;
	char buf[256];

	count = sprintf(buf,"%s\n",desc);
	MeWrite(file,buf,count);
	for(i=0;i!=num_elts;i++)
	{
		count = sprintf(buf,"%d\n",array[i]);
		MeWrite(file,buf,count);		
	}
}

void writeMdtKeaBodyIndexPairArrayToFile(int                            file,
						                 const char *                   desc,
						                 int                            num_elts,
						                 const MdtKeaBodyIndexPair *    array)
{
    writeIntArrayToFile(file,
						desc,
						num_elts*2,
						(const int *)array);
}

void writeFloatArrayToFile(int            file,
						   const char *   desc,
						   int            num_elts,
						   const MeReal * array)
{
	int i,count;
	char buf[256];

	count = sprintf(buf,"%s\n",desc);
	MeWrite(file,buf,count);
	for(i=0;i!=num_elts;i++)
	{
		count = sprintf(buf,"%08x\n", *((unsigned int *)(array+i)) );
		MeWrite(file,buf,count);		
	}
}
void writeFloatVec4ToFile(int            file,
						  const char *   desc,
						  const MeReal * array)
{
	int i,count;
	char buf[256];

	count = sprintf(buf,"%s\n",desc);
	MeWrite(file,buf,count);
	for(i=0;i!=4;i++)
	{
		count = sprintf(buf,"%08x\n",*((unsigned int *)(array+i)));
		MeWrite(file,buf,count);		
	}
}
void writeFloatToFile(int            file,
					  const char *   desc,
					  MeReal         value)
{
	int count;
	char buf[256];

	count = sprintf(buf,"%s\n",desc);
	MeWrite(file,buf,count);
	count = sprintf(buf,"%08x\n",*((unsigned int *)(&value)));
	MeWrite(file,buf,count);		
}
void writeIntToFile(int            file,
					const char *   desc,
					int            value)
{
	int count;
	char buf[256];

	count = sprintf(buf,"%s\n",desc);
	MeWrite(file,buf,count);
	count = sprintf(buf,"%d\n",value);
	MeWrite(file,buf,count);		
}
/*
   Functions for reading data from a file
*/
void readIntArrayFromFile(MeStream       stream,
						  const char *   desc,
						  int            num_elts,
						  int *          array)
{
	char buf[256];
	int i;

	/* Read the description string */
	MeStreamReadLine(buf,256,stream);

	for(i=0;i!=num_elts;i++)
	{
		/* Read the float */
		MeStreamReadLine(buf,256,stream);
		sscanf(buf,"%d\n",array+i);
	}
}

void readMdtKeaBodyIndexPairArrayFromFile(MeStream               stream,
						                  const char *           desc,
						                  int                    num_elts,
						                  MdtKeaBodyIndexPair *  array)
{
    readIntArrayFromFile(stream,
						 desc,
						 num_elts*2,
						 (int *)array);
}

void readFloatArrayFromFile(MeStream       stream,
						    const char *   desc,
						    int            num_elts,
						    const MeReal * array)
{
	char buf[256];
	int i;

	/* Read the description string */
	MeStreamReadLine(buf,256,stream);

	for(i=0;i!=num_elts;i++)
	{
		/* Read the float */
		MeStreamReadLine(buf,256,stream);
		sscanf(buf,"%08x\n",(unsigned int *)(array+i));
	}
}
void readFloatVec4FromFile(MeStream       stream,
						   const char *   desc,
						   const MeReal * array)
{
	char buf[256];
	int i;

	/* Read the description string */
	MeStreamReadLine(buf,256,stream);

	for(i=0;i!=4;i++)
	{
		/* Read the float */
		MeStreamReadLine(buf,256,stream);
		sscanf(buf,"%08x\n",(unsigned int *)(array+i) );
	}
}
void readFloatFromFile(MeStream       stream,
					   const char *   desc,
					   MeReal *       value)
{
	char buf[256];

	/* Read the description string */
	MeStreamReadLine(buf,256,stream);

	/* Read the float */
	MeStreamReadLine(buf,256,stream);
	sscanf(buf,"%08x\n",(unsigned int *)value);
}
void readIntFromFile(MeStream       stream,
				  	 const char *   desc,
					 int *          value)
{
	char buf[256];

	/* Read the description string */
	MeStreamReadLine(buf,256,stream);

	/* Read the float */
	MeStreamReadLine(buf,256,stream);
	sscanf(buf,"%d\n",value);
}
/*
   writeKeaInputToFile
   -------------------

   Writes the entire input to kea as a text file
   This is used for debugging
*/
void writeKeaInputToFile(const MdtKeaConstraints  constraints,
						 const MdtKeaParameters   parameters,
                         const MdtKeaBody *const  blist[], 
						 int                      num_bodies)
{
    int i,j,k;
    int count;
    int file;
    char buf[256];
    
    file = MeOpen(gDebug->writeKeaInputDataFilename,kMeOpenModeRDWR);
    printf("-- writing kea input to file %s\n",
        gDebug->writeKeaInputDataFilename);
    
    writeFloatToFile(file,"gamma",parameters.gamma);
    writeFloatToFile(file,"epsilon",parameters.epsilon);
    writeFloatToFile(file,"stepsize",parameters.stepsize);
    writeIntToFile(file,"max_iterations",parameters.max_iterations);
    
    writeIntToFile(file,"num_partitions",constraints.num_partitions);
    writeIntToFile(file,"max_partitions",constraints.max_partitions);
    
    writeIntArrayToFile(file,"num_rows_exc_padding_partition",
        constraints.num_partitions,constraints.num_rows_exc_padding_partition);
    
    writeIntArrayToFile(file,"num_rows_inc_padding_partition",
        constraints.num_partitions,constraints.num_rows_inc_padding_partition);
    
    writeIntArrayToFile(file,"num_constraints_partition",
        constraints.num_partitions,constraints.num_constraints_partition);
    
    writeIntToFile(file,"num_rows_inc_padding",
        constraints.num_rows_inc_padding);
    
    writeIntToFile(file,"num_rows_exc_padding",
        constraints.num_rows_exc_padding);
    
    writeIntToFile(file,"max_rows_inc_padding",
        constraints.max_rows_inc_padding);
    
    writeIntToFile(file,"num_constraints",
        constraints.num_constraints);
    
    writeIntToFile(file,"max_constraints",
        constraints.max_constraints);
    
    count = sprintf(buf,"Jstore\n");
    MeWrite(file,buf,count);
    for (i = 0; i != constraints.num_rows_inc_padding / 4; i++)
    {
        for (j = 0; j != 4; j++)
        {
            for (k = 0; k != 6; k++)
            {
                count = sprintf(buf,"%08x; ",*((unsigned int *)(&constraints.Jstore[i][0].col[k][j])) );
                MeWrite(file,buf,count);
            }

            for (k = 0; k != 6; k++)
            {
                count = sprintf(buf,"%08x; ",*((unsigned int *)(&constraints.Jstore[i][1].col[k][j])) );
                MeWrite(file,buf,count);
            }

            count = sprintf(buf,"\n");
            MeWrite(file,buf,count);
        }
        count = sprintf(buf,"\n");
        MeWrite(file,buf,count);
    }
    
    writeFloatArrayToFile(file,"xi",
        constraints.num_rows_exc_padding,constraints.xi);
    
    writeFloatArrayToFile(file,"c",
        constraints.num_rows_exc_padding,constraints.c);
    
    writeFloatArrayToFile(file,"lo",
        constraints.num_rows_exc_padding,constraints.lo);
    
    writeFloatArrayToFile(file,"hi",
        constraints.num_rows_exc_padding,constraints.hi);
    
    //writeFloatArrayToFile(file,"lambda",
    //	                  constraints->num_rows_exc_padding,constraints->lambda);
    
    //writeFloatArrayToFile(file,"force",
    //	                  constraints->num_rows_exc_padding,constraints->force);
    
    writeFloatArrayToFile(file,"slipfactor",
        constraints.num_rows_exc_padding,constraints.slipfactor);
    
    writeFloatArrayToFile(file,"xgamma",
        constraints.num_rows_exc_padding,constraints.xgamma);
    
    writeIntArrayToFile(file,"Jsize",
        constraints.num_constraints,constraints.Jsize);
    
    writeIntArrayToFile(file,"Jofs",
        constraints.num_constraints,constraints.Jofs);
    
    writeMdtKeaBodyIndexPairArrayToFile(file,"Jbody",
        constraints.num_constraints,constraints.Jbody);
    
    writeIntToFile(file,"num_bodies",num_bodies);
    
    for(i=0;i!=num_bodies;i++)
    {
        count = sprintf(buf,"body %d\n",i);
        MeWrite(file,buf,count);
        
        writeFloatToFile(file,"invmass",blist[i]->invmass);
        writeIntToFile(file,"flags",blist[i]->flags);
        
        writeFloatVec4ToFile(file,"force",
            blist[i]->force);
        writeFloatVec4ToFile(file,"torque",
            blist[i]->torque);
        writeFloatVec4ToFile(file,"invI0",
            blist[i]->invI0);
        writeFloatVec4ToFile(file,"invI1",
            blist[i]->invI1);
        writeFloatVec4ToFile(file,"invI2",
            blist[i]->invI2);
        writeFloatVec4ToFile(file,"I0",
            blist[i]->I0);
        writeFloatVec4ToFile(file,"I1",
            blist[i]->I1);
        writeFloatVec4ToFile(file,"I2",
            blist[i]->I2);
        writeFloatVec4ToFile(file,"vel",
            blist[i]->vel);
        writeFloatVec4ToFile(file,"velrot",
            blist[i]->velrot);
        writeFloatVec4ToFile(file,"qrot",
            blist[i]->qrot);
        writeFloatVec4ToFile(file,"accel",
            blist[i]->accel);
        writeFloatVec4ToFile(file,"fastSpinAxis",
            blist[i]->fastSpinAxis);
    }
    
    MeClose(file);
}

void readKeaInputFromFile(MdtKeaConstraints * constraints,
					  	  MdtKeaParameters *  parameters,
                          MdtKeaBody *        blist[],
						  int *               num_bodies)
{
    int i,j;
    char buf[256];
    
    MeStream stream;
    
    stream = MeStreamOpen(gDebug->readKeaInputDataFilename,
        kMeOpenModeRDONLY);
    
    if(stream!=0)
    {
        printf("-- reading kea input from file %s\n",
            gDebug->readKeaInputDataFilename);
        readFloatFromFile(stream,"gamma",&parameters->gamma);
        readFloatFromFile(stream,"epsilon",&parameters->epsilon);
        readFloatFromFile(stream,"stepsize",&parameters->stepsize);
        readIntFromFile(stream,"max_iterations",&parameters->max_iterations);
        
        readIntFromFile(stream,"num_partitions",&constraints->num_partitions);
        readIntFromFile(stream,"max_partitions",&constraints->max_partitions);
        
        readIntArrayFromFile(stream,"num_rows_exc_padding_partition",
            constraints->num_partitions,constraints->num_rows_exc_padding_partition);
        
        readIntArrayFromFile(stream,"num_rows_inc_padding_partition",
            constraints->num_partitions,constraints->num_rows_inc_padding_partition);
        
        readIntArrayFromFile(stream,"num_constraints_partition",
            constraints->num_partitions,constraints->num_constraints_partition);
        
        readIntFromFile(stream,"num_rows_inc_padding",
            &constraints->num_rows_inc_padding);
        
        readIntFromFile(stream,"num_rows_exc_padding",
            &constraints->num_rows_exc_padding);
        
        readIntFromFile(stream,"max_rows_inc_padding",
            &constraints->max_rows_inc_padding);
        
        readIntFromFile(stream,"num_constraints",
            &constraints->num_constraints);
        
        readIntFromFile(stream,"max_constraints",
            &constraints->max_constraints);
        
        /* Read the Jstore description string */
        MeStreamReadLine(buf,256,stream);
        
        for (i = 0; i != constraints->num_rows_inc_padding / 4; i++)
        {
            for (j = 0; j != 4; j++)
            {
                MeStreamReadLine(buf,256,stream);
                sscanf(buf,"%08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x; %08x;\n", 
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  0*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  1*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  2*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  3*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  4*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  5*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  6*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  7*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  8*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 +  9*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 + 10*4 ]),
                    (unsigned int *)&(constraints->Jstore[j + i * 48 + 11*4 ]));
            }
            MeStreamReadLine(buf,256,stream);
        }
        
        readFloatArrayFromFile(stream,"xi",
            constraints->num_rows_exc_padding,constraints->xi);
        
        readFloatArrayFromFile(stream,"c",
            constraints->num_rows_exc_padding,constraints->c);
        
        readFloatArrayFromFile(stream,"lo",
            constraints->num_rows_exc_padding,constraints->lo);
        
        readFloatArrayFromFile(stream,"hi",
            constraints->num_rows_exc_padding,constraints->hi);
        
        //readFloatArrayFromFile(stream,"lambda",
        //	                  constraints->num_rows_exc_padding,constraints->lambda);
        
        //readFloatArrayFromFile(stream,"force",
        //	                  constraints->num_rows_exc_padding,constraints->force);
        
        readFloatArrayFromFile(stream,"slipfactor",
            constraints->num_rows_exc_padding,constraints->slipfactor);
        
        readFloatArrayFromFile(stream,"xgamma",
            constraints->num_rows_exc_padding,constraints->xgamma);
        
        readIntArrayFromFile(stream,"Jsize",
            constraints->num_constraints,constraints->Jsize);
        
        readIntArrayFromFile(stream,"Jofs",
            constraints->num_constraints,constraints->Jofs);
        
        readMdtKeaBodyIndexPairArrayFromFile(stream,"Jbody",
            constraints->num_constraints,constraints->Jbody);
        
        readIntFromFile(stream,"num_bodies",num_bodies);
        
        for(i=0;i!=*num_bodies;i++)
        {
            blist[i] = (MdtKeaBody *)MeMemoryAPI.create(sizeof(MdtKeaBody));
            
            /* Read body identifier */
            MeStreamReadLine(buf,256,stream);
            
            readFloatFromFile(stream,"invmass",&blist[i]->invmass);
            readIntFromFile(stream,"flags",&blist[i]->flags);
            
            readFloatVec4FromFile(stream,"force",
                blist[i]->force);
            readFloatVec4FromFile(stream,"torque",
                blist[i]->torque);
            readFloatVec4FromFile(stream,"invI0",
                blist[i]->invI0);
            readFloatVec4FromFile(stream,"invI1",
                blist[i]->invI1);
            readFloatVec4FromFile(stream,"invI2",
                blist[i]->invI2);
            readFloatVec4FromFile(stream,"I0",
                blist[i]->I0);
            readFloatVec4FromFile(stream,"I1",
                blist[i]->I1);
            readFloatVec4FromFile(stream,"I2",
                blist[i]->I2);
            readFloatVec4FromFile(stream,"vel",
                blist[i]->vel);
            readFloatVec4FromFile(stream,"velrot",
                blist[i]->velrot);
            readFloatVec4FromFile(stream,"qrot",
                blist[i]->qrot);
            readFloatVec4FromFile(stream,"accel",
                blist[i]->accel);
            readFloatVec4FromFile(stream,"fastSpinAxis",
                blist[i]->fastSpinAxis);
        }
        
        MeStreamClose(stream);
        }
        else
        {
            MeFatalError(0,"file %s could not be opened",
                gDebug->readKeaInputDataFilename);
        }
	
}
void writeLambdaToFile(const char * filename,
					   const MeReal lambda[],
					   int          num_elts)
{
	int file;

	file = MeOpen(filename,kMeOpenModeWRONLY);
	printf("-- writing kea output data (lambda) to file %s\n",
        gDebug->writeKeaOutputDataFilename);
	writeFloatArrayToFile(file,"lambda",num_elts,lambda);
	MeClose(file);
}
void keaFunctions :: keaCloseDebugDataFile(int file)
{
    //printf("keaCloseDebugDataFile not implemented\n");
}

int keaFunctions :: checkPrintDebugInput(
        const MdtKeaConstraints  constraints,
		const MdtKeaParameters   parameters,
        const MdtKeaBody *const  blist[], 
		int                      num_bodies)
{
    if(gDebug->writeKeaInputData && gDebug->frame==gDebug->badFrame)
    {
        writeKeaInputToFile(
            constraints,
			parameters,
            blist, 
			num_bodies);
    }

#if PRINT_KEA_INPUT
    printKeaInput(constraints,
                  parameters,
                  blist,
                  num_bodies);
#endif

    return 0;
}