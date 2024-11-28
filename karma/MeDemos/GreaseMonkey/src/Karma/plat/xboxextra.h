/***********************************************************************************************
*
*	$Id: xboxextra.h,v 1.1.2.1 2002/03/13 10:31:05 richardm Exp $
*
************************************************************************************************/
#ifndef XBOXEXTRA_H
#define XBOXEXTRA_H

/* Extra XBox specific functions, namely Direct X stuff	*/

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL inputInitDirectInput(HINSTANCE hInst, HWND hWnd);
extern DWORD inputProcessDeviceInput(void);
extern void inputCleanupDirectInput(void);

#ifdef __cplusplus
}
#endif

#endif //XBOXEXTRA_H