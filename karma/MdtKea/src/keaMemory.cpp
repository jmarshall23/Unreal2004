/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/05 11:39:46 $ - Revision: $Revision: 1.25.2.6.4.1 $

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

#include <MeMessage.h>
#include <keaMemory.hpp>
#include <MeMath.h>
#include <keaInternal.hpp>
#include <keaFunctions.hpp>
#include <keaDebug.h>

#ifdef PS2
#include <eekernel.h>
#include <keaEeDefs.hpp>
#endif

extern void *pool_ptr;
extern void *pool_max;
extern int poolstack_ptr;
extern void *poolstack[3];

#define PRINT_MEMORY_USAGE 0

void keaPushPoolFrame()
{
#if PRINT_MEMORY_USAGE
    printf("push pool frame\n");
#endif
    poolstack[poolstack_ptr] = pool_ptr;
    poolstack_ptr++;
}

void keaPopPoolFrame()
{
#if PRINT_MEMORY_USAGE
    printf("pop pool frame\n");
#endif
    pool_ptr = poolstack[poolstack_ptr-1];
    poolstack_ptr--;
}

void keaFunctions :: initPool(void *ptr, int size)
{
#if PRINT_MEMORY_USAGE
    MeInfo(0,"User supplied memory pool at addresses [%08x..%08x) (size %d)",ptr,((unsigned int)ptr)+size,size);
#endif

    pool_ptr = ptr;
    poolstack_ptr = 0;
    pool_max = (void *) (((MeI8 *) pool_ptr) + size);
}

void *keaPoolAlloc(int size,const char *name)
{
    /*
      Pool allocator works only if user always allocates chunks whose size is
      a multiple of 16.
    */

#if PRINT_MEMORY_USAGE
    MeInfo(0,"pool_ptr=%08x allocating %20s of size %d pool_ptr=%08x",pool_ptr,name,size,(void *) (((MeI8 *) pool_ptr) + size));
#endif

    if((size % 16) != 0)
    {
        MeFatalError(3,
            "keaPoolAlloc: allocating %s of size %d. "
            "Size must be a multiple of 16 bytes.\n",
            name,size);
    }

    /* Check that there is enough memory in pool */

    if(((void *) (((MeI8 *) pool_ptr) + size)) > pool_max)
    {
        MeFatalError(3,
            "Kea error:Memory pool size exceeded when allocating %s of size %d\n"
            "Pool ends at %08x\n"
            "If memory were allocated, pool would end at %08x",
            name,size,pool_max,((void *) (((MeI8 *) pool_ptr) + size)));
    }

    void *addr = pool_ptr;
    pool_ptr = (void *) (((MeI8 *) pool_ptr) + size);
    return addr;
}

void vanillaAllocateMemory(
    keaTempMemory *   mem,
    MdtKeaConstraints constraints,
    int               num_bodies)
{
    int partition;
    int total_strips_inc_padding = 0;
    int total_strips_exc_padding = 0;

    for (partition = 0;
         partition != constraints.num_partitions;
         partition++)
    {
        if (constraints.num_constraints_partition[partition] > 0)
        {
            int num_rows = constraints.num_rows_exc_padding_partition[partition];
            total_strips_inc_padding = total_strips_inc_padding + MeMathCEIL12(num_rows) / 4;
            total_strips_exc_padding = total_strips_exc_padding + MeMathCEIL4(num_rows) / 4;            
        }
    }

    mem->invIworld         = (MdtKeaInverseMassMatrix *) keaPoolAlloc(MeMathCEIL64(num_bodies                           * sizeof(MdtKeaInverseMassMatrix)) + 64,"invIworld");
    mem->vhmf              = (MdtKeaVelocity *)         keaPoolAlloc(MeMathCEIL64(num_bodies                           * sizeof(MdtKeaVelocity)) + 64,"vhmf");
    mem->jm                = (MdtKeaJBlockPair *)       keaPoolAlloc(MeMathCEIL64(total_strips_exc_padding*MAXPERROW/2 * sizeof(MdtKeaJBlockPair)) + 64,"jm");
    mem->rhs               = (MeReal *)                 keaPoolAlloc(MeMathCEIL64(total_strips_exc_padding*4           * sizeof(MeReal)) + 64,"rhs");
    mem->jlen_12padded     = (int *)                    keaPoolAlloc(MeMathCEIL64(total_strips_inc_padding             * sizeof(int)) + 64,"jlen_12padded");
    mem->jlen              = (int *)                    keaPoolAlloc(MeMathCEIL64(total_strips_exc_padding             * sizeof(int)) + 64,"jlen");
    mem->bl2cbody          = (MdtKeaBl2CBodyRow *)      keaPoolAlloc(MeMathCEIL64(total_strips_exc_padding             * sizeof(MdtKeaBl2CBodyRow)) + 64,"bl2cbody");
    mem->bl2body_12padded  = (MdtKeaBl2BodyRow *)       keaPoolAlloc(MeMathCEIL64(total_strips_inc_padding             * sizeof(MdtKeaBl2BodyRow)) + 64,"bl2body_12padded");
    mem->bl2body           = (MdtKeaBl2BodyRow *)       keaPoolAlloc(MeMathCEIL64(total_strips_exc_padding             * sizeof(MdtKeaBl2BodyRow)) + 64,"bl2body");

#ifdef PS2
    mem->invIworld         = (MdtKeaInverseMassMatrix*) (
                             (((MeU32)mem->invIworld)     & 0xffffffc0) + 64);
    mem->vhmf              = (MdtKeaVelocity *) (
                             (((MeU32)mem->vhmf)          & 0xffffffc0) + 64);
    mem->jm                = (MdtKeaJBlockPair *) (
                             ((((MeU32)mem->jm)           & 0xffffffc0) + 64);
    mem->rhs               = (MeReal *) (               
                             (((MeU32)mem->rhs)           & 0xffffffc0) + 64);
    mem->jlen_12padded     = (int *) (                  
                             (((MeU32)mem->jlen_12padded) & 0xffffffc0) + 64);
    mem->jlen              = (int *) (                  
                             (((MeU32)mem->jlen)          & 0xffffffc0) + 64);
    mem->bl2cbody          = (MdtKeaBl2CBodyRow *) (    
                             (((MeU32)mem->bl2cbody)      & 0xffffffc0) + 64);
    mem->bl2body_12padded  = (MdtKeaBl2BodyRow *) (     
                             (((MeU32)mem->bl2body_12padded)&0xffffffc0) + 64);
    mem->bl2body           = (MdtKeaBl2BodyRow *) (
                             (((MeU32)mem->bl2body)       & 0xffffffc0) + 64);
#endif
}

void keaFunctions_Vanilla :: allocateMemory(
    keaTempMemory *   mem,
    MdtKeaConstraints constraints,
    int               num_bodies)
{
    vanillaAllocateMemory(mem,constraints,num_bodies);
}

#ifndef PS2

#ifndef _BUILD_VANILLA
void keaFunctions_SSE :: allocateMemory(
    keaTempMemory *   mem,
    MdtKeaConstraints constraints,
    int               num_bodies)
{
    vanillaAllocateMemory(mem,constraints,num_bodies);
}
#endif

#else
void keaFunctions_PS2 :: allocateMemory(
    keaTempMemory *   mem,
    MdtKeaConstraints constraints,
    int               num_bodies)
{
    vanillaAllocateMemory(mem,constraints,num_bodies);

#if 1
    mem->invIworld         = (MdtKeaInverseMassMatrix *)(UNCACHED(mem->invIworld));
    mem->vhmf              = (MdtKeaVelocity *)         (UNCACHED(mem->vhmf));
    mem->jm                = (MdtKeaJBlockPair *)       (UNCACHED(mem->jm));
    mem->rhs               = (MeReal *)                 (UNCACHED(mem->rhs));               
    mem->jlen_12padded     = (int *)                    (UNCACHED(mem->jlen_12padded));             
    mem->jlen              = (int *)                    (UNCACHED(mem->jlen));    
    mem->bl2cbody          = (MdtKeaBl2CBodyRow *)      (UNCACHED(mem->bl2cbody));
    mem->bl2body_12padded  = (MdtKeaBl2BodyRow *)       (UNCACHED(mem->bl2body_12padded));          
    mem->bl2body           = (MdtKeaBl2BodyRow *)       (UNCACHED(mem->bl2body)); 
#endif
}
#endif

/*
   A function the user can call to find out how big a memory pool they should
   give us.

   Some of them have 64 bytes added, because they have to be aligned to 64 byte boundaries
*/
#if (defined PS2) && !(defined PS2_VANILLA)
int MEAPI MdtKeaMemoryRequired(const int num_rows_exc_padding[],
                               int num_partitions,
                               int max_rows,
                               int max_bodies)
{
    int i,total_strips_inc_padding,total_strips_exc_padding;

    int c12size       = MeMathCEIL12(max_rows);
    int c16c12size    = MeMathCEIL16(MeMathCEIL12(max_rows));

    int num_12_blocks = c12size/12;

    total_strips_inc_padding = 0;
    total_strips_exc_padding = 0;

    for (i=0; i!=num_partitions; i++)
    {
        int num_rows = num_rows_exc_padding[i];
        total_strips_inc_padding = total_strips_inc_padding + MeMathCEIL12(num_rows) / 4;
        total_strips_exc_padding = total_strips_exc_padding + MeMathCEIL4(num_rows) / 4;            
    }

    int invIworld         = MeMathCEIL64(max_bodies                    * sizeof(MdtKeaInverseMassMatrix)) + 64;
    int vhmf              = MeMathCEIL64(max_bodies * 8                * sizeof(MeReal)) + 64;
    int jm                = MeMathCEIL64(total_strips_exc_padding * MAXPERROW/2 * sizeof(MdtKeaJBlockPair) ) + 64;
    int rhs               = MeMathCEIL64(total_strips_exc_padding * 4           * sizeof(MeReal)           ) + 64;
    int jlen_12padded     = MeMathCEIL64(total_strips_inc_padding               * sizeof(int)              ) + 64;
    int jlen              = MeMathCEIL64(total_strips_exc_padding               * sizeof(int)              ) + 64;
    int bl2cbody          = MeMathCEIL64(total_strips_exc_padding               * sizeof(MdtKeaBl2CBodyRow)) + 64;
    int bl2body_12padded  = MeMathCEIL64(total_strips_inc_padding               * sizeof(MdtKeaBl2BodyRow) ) + 64;
    int bl2body           = MeMathCEIL64(total_strips_exc_padding               * sizeof(MdtKeaBl2BodyRow) ) + 64;
    
    int Amatrix           = MeMathCEIL64(c12size * c12size             * sizeof(MeReal) ) + 64;
    int Achol             = MeMathCEIL64(c12size * c12size             * sizeof(MeReal) ) + 64;
    int Arlist            = MeMathCEIL64(num_12_blocks * num_12_blocks * sizeof(int)    ) + 64;
    int Allist            = MeMathCEIL64(num_12_blocks * num_12_blocks * sizeof(int)    ) + 64;
    int Arlist_len        = MeMathCEIL64(num_12_blocks                 * sizeof(int)    ) + 64;
    int Allist_len        = MeMathCEIL64(num_12_blocks                 * sizeof(int)    ) + 64;
    int AzeroBlock        = MeMathCEIL64(12*12                         * sizeof(MeReal) ) + 64;
    int AnzPairs          = MeMathCEIL64(2*num_12_blocks               * sizeof(MeReal*)) + 64;
    int Allist_len_copy   = MeMathCEIL64(num_12_blocks                 * sizeof(int)    ) + 64;

    int cached            = MeMathCEIL64(max_rows                      * sizeof(MeReal) ) + 64;
    int x                 = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;
    int w                 = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;
    int upper             = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;
    int lower             = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;
    int initialsolve      = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;
    int clampedvalues     = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;
    int Ainv              = MeMathCEIL64(c16c12size * c16c12size       * sizeof(MeReal) ) + 64;
    int Qrhs              = MeMathCEIL64(c16c12size                    * sizeof(MeReal) ) + 64;

    int Qmatrix           = MeMathCEIL64(c16c12size * c16c12size       * sizeof(MeReal) ) + 64;
    int Qchol             = MeMathCEIL64(c16c12size * c16c12size       * sizeof(MeReal) ) + 64;
    int Qrlist            = MeMathCEIL64(num_12_blocks * num_12_blocks * sizeof(int)    ) + 64;
    int Qllist            = MeMathCEIL64(num_12_blocks * num_12_blocks * sizeof(int)    ) + 64;
    int Qrlist_len        = MeMathCEIL64(num_12_blocks                 * sizeof(int)    ) + 64;
    int Qllist_len        = MeMathCEIL64(num_12_blocks                 * sizeof(int)    ) + 64;
    int QzeroBlock        = MeMathCEIL64(12*12                         * sizeof(MeReal) ) + 64;
    int QnzPairs          = MeMathCEIL64(2*num_12_blocks               * sizeof(MeReal*)) + 64;
    int Qllist_len_copy   = MeMathCEIL64(num_12_blocks                 * sizeof(int)    ) + 64;

    int total = invIworld +
                vhmf +
                jm +  
                rhs +      
                jlen_12padded +  
                jlen +  
                bl2cbody +  
                bl2body_12padded +
                bl2body +
                Amatrix +
                Achol +
                Arlist +
                Allist + 
                Arlist_len + 
                Allist_len +     
                AzeroBlock +
                AnzPairs +
                Allist_len_copy +
                cached +
                x +      
                w +      
                upper +
                lower +
                initialsolve +
                clampedvalues +
                Ainv +
                Qrhs +    
                Qmatrix +        
                Qchol +        
                Qrlist +
                Qllist + 
                Qrlist_len + 
                Qllist_len +
                QzeroBlock +
                QnzPairs +
                Qllist_len_copy;

#if PRINT_MEMORY_USAGE
    printf("Max memory calculated by MdtKeaMemoryRequired\n");
    printf("---------------------------------------------\n");
    printf("invIworld         %10d\n", invIworld);
    printf("vhmf              %10d\n", vhmf);
    printf("jm                %10d\n", jm);  
    printf("rhs               %10d\n", rhs);      
    printf("jlen_12padded     %10d\n", jlen_12padded);  
    printf("jlen              %10d\n", jlen);  
    printf("bl2cbody          %10d\n", bl2cbody);  
    printf("bl2body_12padded  %10d\n", bl2body_12padded);  
    printf("bl2body           %10d\n", bl2body);
    printf("Amatrix           %10d\n", Amatrix);
    printf("Achol             %10d\n", Achol);
    printf("Arlist            %10d\n", Arlist);
    printf("Allist            %10d\n", Allist); 
    printf("Arlist_len        %10d\n", Arlist_len); 
    printf("Allist_len        %10d\n", Allist_len);     
    printf("AzeroBlock        %10d\n", AzeroBlock);     
    printf("AnzPairs          %10d\n", AnzPairs);     
    printf("Allist_len_copy   %10d\n", Allist_len_copy);     
    printf("cached            %10d\n", cached);
    printf("x                 %10d\n", x);      
    printf("w                 %10d\n", w);      
    printf("upper             %10d\n", upper);
    printf("lower             %10d\n", lower);
    printf("initialsolve      %10d\n", initialsolve);
    printf("clampedvalues     %10d\n", clampedvalues);
    printf("Ainv              %10d\n", Ainv);
    printf("Qrhs              %10d\n", Qrhs);    
    printf("Qmatrix           %10d\n", Qmatrix);        
    printf("Qchol             %10d\n", Qchol);        
    printf("QnzPairs          %10d\n", QnzPairs);        
    printf("Qllist_len_copy   %10d\n", Qllist_len_copy);     

#endif

    /* Just for testing */

    return total;
}

void MdtFlushCache(int a)
{
    FlushCache(a);
}
void MdtSyncDCache(void *a, void *b)
{
    SyncDCache(a,b);
}
void MdtInvalidDCache(void *a, void *b)
{
    InvalidDCache(a,b);
}
#else

int MEAPI MdtKeaMemoryRequired(const int num_rows_exc_padding[],
                               int num_partitions,
                               int max_rows,
                               int max_bodies)
{
    int i,total_strips_inc_padding,total_strips_exc_padding;

    int c4size = MeMathCEIL4(max_rows);

    int c16c12size        = MeMathCEIL16(MeMathCEIL12(max_rows));

    total_strips_inc_padding = 0;
    total_strips_exc_padding = 0;

    for (i=0; i!=num_partitions; i++)
    {
        int num_rows = num_rows_exc_padding[i];
        total_strips_inc_padding = total_strips_inc_padding + MeMathCEIL12(num_rows) / 4;
        total_strips_exc_padding = total_strips_exc_padding + MeMathCEIL4(num_rows) / 4;            
    }

    int invIworld         = MeMathCEIL64(max_bodies * sizeof(MdtKeaInverseMassMatrix)) + 64;
    int vhmf              = MeMathCEIL64(max_bodies * 8 * sizeof(MeReal)) + 64;
    int jm                = MeMathCEIL64(total_strips_exc_padding * MAXPERROW/2 * sizeof(MdtKeaJBlockPair) ) + 64;
    int rhs               = MeMathCEIL64(total_strips_exc_padding * 4           * sizeof(MeReal)           ) + 64;
    int jlen_12padded     = MeMathCEIL64(total_strips_inc_padding               * sizeof(int)              ) + 64;
    int jlen              = MeMathCEIL64(total_strips_exc_padding               * sizeof(int)              ) + 64;
    int bl2cbody          = MeMathCEIL64(total_strips_exc_padding               * sizeof(MdtKeaBl2CBodyRow)) + 64;
    int bl2body_12padded  = MeMathCEIL64(total_strips_inc_padding               * sizeof(MdtKeaBl2BodyRow) ) + 64;
    int bl2body           = MeMathCEIL64(total_strips_exc_padding               * sizeof(MdtKeaBl2BodyRow) ) + 64;

    int ArsD              = MeMathCEIL16(c4size * sizeof(MeReal)) + 16;
    int Arhs              = c4size * sizeof(MeReal) + 16;
    int A                 = c4size * c4size * sizeof (MeReal) + 16;
    int AChol             = c4size * c4size * sizeof (MeReal) + 16;
    
    int ANAZ              = MeMathCEIL16(c4size*c4size/16) * sizeof(MeReal*);
    int ANCZ              = MeMathCEIL16(c4size*c4size/16) * sizeof(MeReal*);
    int ANR               = MeMathCEIL16(c4size/4) * sizeof(int);
    int ANC               = MeMathCEIL16(c4size/4) * sizeof(int);

    int cached            = MeMathCEIL64(c16c12size * sizeof(int)) + 64;
    int x                 = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;
    int w                 = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;
    int upper             = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;
    int lower             = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;
    int initialSolve      = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;
    int clampedValues     = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;
    int Ainv              = MeMathCEIL64(c16c12size * c16c12size * sizeof(MeReal)) + 64;
    int Qrhs              = MeMathCEIL64(c16c12size * sizeof(MeReal)) + 64;

    int QrsD              = MeMathCEIL16(c4size * sizeof(MeReal)) + 16;
    int Q                 = c4size * c4size * sizeof (MeReal) + 16;
    int QChol             = c4size * c4size * sizeof (MeReal) + 16;    
    int QNAZ              = MeMathCEIL16(c4size*c4size/16) * sizeof(MeReal*);
    int QNCZ              = MeMathCEIL16(c4size*c4size/16) * sizeof(MeReal*);
    int QNR               = MeMathCEIL16(c4size/4) * sizeof(int);
    int QNC               = MeMathCEIL16(c4size/4) * sizeof(int);


#if PRINT_MEMORY_USAGE
    printf("Max memory calculated by MdtKeaMemoryRequired\n");
    printf("---------------------------------------------\n");    
    printf("invIworld         %10d\n", invIworld);
    printf("vhmf              %10d\n", vhmf);
    printf("jm                %10d\n", jm);  
    printf("rhs               %10d\n", rhs);      
    printf("jlen_12padded     %10d\n", jlen_12padded);  
    printf("jlen              %10d\n", jlen);  
    printf("bl2cbody          %10d\n", bl2cbody);  
    printf("bl2body_12padded  %10d\n", bl2body_12padded);
    printf("bl2body           %10d\n", bl2body);    
    printf("ArsD              %10d\n", ArsD);         
    printf("Arhs              %10d\n", Arhs);         
    printf("A                 %10d\n", A);            
    printf("AChol             %10d\n", AChol);        
    printf("ANAZ              %10d\n", ANAZ);         
    printf("ANCZ              %10d\n", ANCZ);         
    printf("ANR               %10d\n", ANR);          
    printf("ANC               %10d\n", ANC);          
    printf("cached            %10d\n", cached);       
    printf("x                 %10d\n", x);            
    printf("w                 %10d\n", w);            
    printf("upper             %10d\n", upper);        
    printf("lower             %10d\n", lower);        
    printf("initialSolve      %10d\n", initialSolve); 
    printf("clampedValues     %10d\n", clampedValues);
    printf("Ainv              %10d\n", Ainv);         
    printf("Qrhs              %10d\n", Qrhs);
    printf("QrsD              %10d\n", QrsD); 
    printf("Q                 %10d\n", Q);    
    printf("QChol             %10d\n", QChol);
    printf("QNAZ              %10d\n", QNAZ); 
    printf("QNCZ              %10d\n", QNCZ); 
    printf("QNR               %10d\n", QNR);  
    printf("QNC               %10d\n", QNC);  

#endif

    int total = invIworld + 
                vhmf +
                jm +
                rhs +
                jlen_12padded + 
                jlen + 
                bl2cbody + 
                bl2body_12padded + 
                bl2body +
                ArsD +        
                Arhs +                        
                A +           
                AChol +       
                ANAZ +        
                ANCZ +        
                ANR +         
                ANC +         
                cached +      
                x +           
                w +           
                upper +       
                lower +       
                initialSolve +
                clampedValues +
                Ainv +         
                Qrhs +    
                QrsD +
                Q +   
                QChol +
                QNAZ +
                QNCZ +
                QNR + 
                QNC;      

    return total;
}

void MdtFlushCache(int a)
{
}
void MdtSyncDCache(void *a, void *b)
{
}
void MdtInvalidDCache(void *a, void *b)
{
}

#endif
