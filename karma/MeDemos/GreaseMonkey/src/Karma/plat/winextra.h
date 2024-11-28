/***********************************************************************************************
*
*	$Id: winextra.h,v 1.1.2.1 2002/03/01 19:54:12 richardm Exp $
*
************************************************************************************************/
#ifndef WINEXTRA_H
#define WINEXTRA_H

/* Extra Windows specific functions, namely Direct X stuff	*/

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL inputInitDirectInput(HINSTANCE hInst, HWND hWnd);
extern DWORD inputProcessDeviceInput(void);
extern void inputCleanupDirectInput(void);

#ifdef __cplusplus
}
#endif

#endif //WINEXTRA_H