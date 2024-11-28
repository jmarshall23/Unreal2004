/***********************************************************************************************
*
*	$Id: platxtra.h,v 1.1.2.2 2002/03/13 10:31:05 richardm Exp $
*
************************************************************************************************/
#ifndef _PLATEXTRA_H
#define _PLATEXTRA_H


/* Extra platform specific functions used across all platforms	*/
#ifdef __cplusplus
extern "C" {
#endif

    extern void InitialisePerformanceTimer(void);
    extern void ResetPerformanceTimer(void);
    extern float ReadPerformanceTimer(void);

#ifdef __cplusplus
}
#endif

#endif //_PLATEXTRA_H
