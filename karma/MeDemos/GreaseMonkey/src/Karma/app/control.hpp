/***********************************************************************************************
*		
*	$Id: control.hpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*	
************************************************************************************************/
#ifndef _CONTROL_H
#define _CONTROL_H

//#ifdef __cplusplus
//extern "C"
//{
//#endif

typedef enum {
	CTRL_KEYBOARD = 0,
	CTRL_MOUSE,
	CTRL_JOYSTICK,
	CTRL_JOYPAD
} CTRL_INPUT_METHOD;



extern void xToggleScreenInfo(void);
extern void xTogglePause(void);
extern void xReset(int);
extern void xCameraRight(void);
extern void xCameraLeft(void);
extern void xCameraDown(void);
extern void xCameraUp(void);
extern void xCameraIn(void);
extern void xCameraOut(void);
extern void xAlterSteering(MeReal inc);
extern void xAlterThrottle(MeReal inc);
extern void xFlipCar(void);
extern void xUpshift(void);
extern void xDownshift(void);
extern void DecaySteering(void);
extern void DecayThrottle(void);
extern void xSetTranny(int type);
extern void xTakeControlOfCar(int id);

extern void xNextCar(void);
extern void xViewPlayer(void);
extern void xToggleLOD(void);

#ifdef __cplusplus
extern "C"
{
#endif

extern CTRL_INPUT_METHOD input_control_method;

extern void SetPlayerSteering(MeReal);
extern void SetPlayerThrottle(MeReal);
//extern void SetPlayerRightThrottle(MeReal);
//extern void SetPlayerLeftThrottle(MeReal);

#ifdef __cplusplus
}
#endif
//#ifdef __cplusplus
//}
//#endif

#endif
