/* -*- mode: C++; -*- */

/*
Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $
  
    Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.12.2.1 $
    
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

#include "keaCheckCPU_sse.hpp"


/*  Note:
    In what follows we are using Windoze Structured Exception Handling (SEH). This is
    supported by the compiler using Microsoft extensions "__try" etc.
    (rather than plain "try").
*/

#define WIN32_LEAN_AND_MEAN

#ifndef _XBOX
#include <windows.h>
#endif

#include <excpt.h>
#include <string.h>

/* Checking CPUID support. */
static bool SIMDSupport_CPUID(int s)
{
         
    /*  Make sure that the processor supports CPUID, and get the processor
        description string while we're at it.  */
    __try
    {
        __asm
        {
            mov   eax, 0        // First, check processor name
            push  ebx
            cpuid
            mov   eax, s
            mov   [eax],   ebx  // ebx contains "Genu" or "Auth"
            pop   ebx
            mov   [eax+4], edx  // edx contains "ineI" or "enti"
            mov   [eax+8], ecx  // ecx contains "ntel" or "cAMD" 
        }
        /* So '(char*)s' gets "GenuineIntel" or "AuthenticAMD" or some other crap */
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        /* If we get here, an unexpected exception occurred. */
        return false; 
    }

    return true;
}

static int SIMDSupport_CPUIDbitMMX()
{
    /* Check the CPUID bits for MMX SSE support. */
    int Support = 0;
    
    __asm
    {
        mov   eax, 1                    // set 1 to get feature bits from CPUID
        push  ebx
        cpuid
        pop   ebx
        and   edx, 0800000h             // bit 23 flags MMX support.         
        mov   [Support], edx
    }
    
    return Support;
}

static int SIMDSupport_CPUIDbitXMM()
{
    /* Check the CPUID bits for XMM SSE support. */
    int Support = 0;
    
    __asm
    {
        mov   eax, 1                    // set 1 to get feature bits from CPUID
        push  ebx
        cpuid
        pop   ebx
        and   edx, 02000000h            // bit 25 flags XMM support. (AMD?)
        mov   DWORD PTR[Support],edx 
    }
    
    return Support;
}

static bool SIMDSupport_OS()
{
    __try
    {
        _asm  xorps xmm0, xmm0          //Execute a SSE to see if support exists.
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        /*  If an Invalid Opcode exception (ILLEGAL_INSTRUCTION) occurs,
            and you have already checked SIMDSupport_CPUIDbitXMM, then
            XMM SSE is not supported by the OS!  */
        return false;
    }
    
    return true;
}

void CPUResources::DiscoverSIMDAvailablilty ()
{
    char brand[12];
    bool support = true;

    /* Test processor for CPUID support, and get the processor
       description string while we're at it. */
    if(SIMDSupport_CPUID((int)(&brand[0])))
    {
        // Now make sure the processor is "GenuineIntel" or "AuthenticAMD". 
        // note that strncmp returns 0 if the strings match.
        if(strncmp (brand, "GenuineIntel", 12))         
        {
            if(strncmp (brand, "AuthenticAMD", 12))
                support =  false;
            // This is not an Intel or AMD processor!
        }

        //if(!SIMDSupport_CPUIDbitMMX()) support =  false;      //we don't use mmx(?)

        if(!SIMDSupport_CPUIDbitXMM()) support =  false;

        if(!SIMDSupport_OS()) support = false;

    }
    else
        support =  false;
  
    s_kni_available = support;
}

bool CPUResources::s_kni_available;
