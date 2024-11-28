/*
  $Id: events.cpp,v 1.1.2.5 2002/03/10 19:17:12 piercarl Exp $
*/

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include "rwcore.h"
#ifdef RWLOGO
#include "rplogo.h"
#endif
#include "rpworld.h"
#include "rtcharse.h"
#include "rprandom.h"
#include "rpcollis.h"

#ifdef USE_VRML
#include "rpvrml.h"
#endif

#include "../plat/platxtra.h"

static unsigned int control_input = 0;
static unsigned int shift_pressed = 0;
static unsigned int ctrl_pressed = 0;

#if 0
#include "MathEngine.h"
#endif
#include "RwFuncs.hpp"

#if 0
#include "carAPI.hpp"
#endif
#include "McdFrame.h"

#include "MdtCar.hpp"
#include "carAPI.hpp"
#include "utils.hpp"
#include "car.hpp"
#include "control.hpp"
#include "init.hpp"
#include "driver.hpp"
#include "MeMemory.h"

#define LEFT_ARROW  (0x00000001)
#define RIGHT_ARROW (0x00000002)
#define UP_ARROW    (0x00000004)
#define DOWN_ARROW  (0x00000008)
#define CAM_UP      (0x00000010)
#define CAM_DOWN    (0x00000020)
#define CAM_LEFT    (0x00000040)
#define CAM_RIGHT   (0x00000080)
#define CAM_IN      (0x00000100)
#define CAM_OUT     (0x00000200)
/*
 * Platform abstraction interface...
 */
#include "skeleton.h"
#include "camera.h"
//turnoff#include "padmap.h"
#include "mouse.h"

static int last_mouse_x;
static int last_mouse_y;
static RwBool mouse_L_but = false;
static RwBool mouse_R_but = false;

#define DEFAULT_ASPECTRATIO (RwReal)(4.0 / 3.0)

static RwReal  CurrentViewWindow = (RwReal)(0.577);

extern void ToggleSafeTime(void);

/*
 *****************************************************************************
 */
#include "carsound.hpp"

//extern "C" int PsScreenWidth();
//extern "C" int PsScreenHeight();
static RwBool
Initialise(void){
    if( RsInitialize() )    {
        /*RsGlobal.maximumWidth = PsScreenWidth(); //CAMERAWIDTH;
        RsGlobal.maximumHeight = PsScreenHeight(); //CAMERAHEIGHT;
       */ 
		RsGlobal.maximumWidth = 640; //CAMERAWIDTH;
        RsGlobal.maximumHeight = 480; //CAMERAHEIGHT;
        return TRUE;
    }
    return FALSE;
}

/*
int PsScreenWidth()
{
    int width=GetSystemMetrics(SM_CXMAXIMIZED);
    return width>0?width:640;
}
int PsScreenHeight()
{
    int height= GetSystemMetrics(SM_CYMAXIMIZED);
    return height>0?height:480;

}*/

/*
 *****************************************************************************
 */
static RwBool
AttachPlugins(void)
{
#ifdef RWLOGO
    if( !RpLogoPluginAttach() )
    {
        RsErrorMessage(RWSTRING("RpLogoPluginAttach failed."));

        return FALSE;
    }
#endif

    if( !RpWorldPluginAttach() )
    {
        RsErrorMessage(RWSTRING("RpWorldPluginAttach failed."));

        return FALSE;
    }

#ifdef USE_VRML
    if( !RpVrmlPluginAttach() )
    {
        RsErrorMessage(RWSTRING("VRML plugin attach failed."));

        return FALSE;
    }
#endif

#ifdef __KATANA__
    RwKatanaInitWorldPipeline();
#endif /* __KATANA__ */

#ifdef SKY
    RwSkyInitWorldPipeline();
#endif

//    if( !RpRandomPluginAttach() )
//    {
//        RsErrorMessage(RWSTRING("RpRandomPluginAttach failed."));

//        return FALSE;
//    }

	if( !RpCollisionPluginAttach() )
    {
        RsErrorMessage(RWSTRING("RpCollisionPluginAttach failed."));

        return FALSE;
    }

    return TRUE;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleKeyDown(void *param)
{
    RsKeyStatus *keyStatus;
    static int tranny = 0;

    keyStatus = (RsKeyStatus *)param;

    switch( keyStatus->keyCharCode )
    {
        case rsF1:
            xToggleScreenInfo();
            break;
        case rsENTER:
        case rsPADENTER:
            xTakeControlOfCar(game_cam.view_driver);
            break;
        case rsLCTRL:
        case rsRCTRL:
            ctrl_pressed = 1;
            break;
        case rsRSHIFT:
        case rsLSHIFT:
            shift_pressed = 1;
            break;
        case 'r':
        case 'R':
            xReset(game_cam.view_driver);
            break;
        case 'a':
        case 'A':
//          control_input |= CAM_LEFT;
        break;
        case 's':
        case 'S':
			ToggleSafeTime();
            break;
        case 'w':
        case 'W':
//          control_input |= CAM_UP;
            break;
        case 'z':
        case 'Z':
//          control_input |= CAM_DOWN;
            break;
        case 'e':
        case 'E':
//          control_input |= CAM_IN;
            break;
        case 'd':
        case 'D':
            xToggleLOD();
//          control_input |= CAM_OUT;
            break;
        case 'f':
        case 'F':
            xFlipCar();
            break;
        case ' ': //space
            xUpshift();
            break;
        case 'x':
        case 'X':
            xDownshift();
            break;
        case 'p':
        case 'P':
        case rsF10:
            xTogglePause();
            break;
//      case 'l':
//          xSetTranny(0);
//          break;
        case 't':
        case 'T':
            xSetTranny(tranny = 1-tranny);
            break;
        case rsPLUS:
            break;
        case rsMINUS:
            break;
        case rsLEFT:
                control_input |= LEFT_ARROW;
                control_input |= CAM_LEFT;
            break;
        case rsRIGHT:
                control_input |= RIGHT_ARROW;
                control_input |= CAM_RIGHT;
            break;
        case rsUP:
                control_input |= UP_ARROW;
                control_input |= CAM_UP;
                control_input |= CAM_IN;
            break;
        case rsDOWN:
                control_input |= DOWN_ARROW;
                control_input |= CAM_DOWN;
                control_input |= CAM_OUT;
            break;
        case rsTAB:
			if(drivers[game_cam.view_driver]->GetControlMethod() == PLAYER_CONTROL){
				xTakeControlOfCar(game_cam.view_driver);//give last car back to computer
			}
            xNextCar();
            break;
        case 'b':
        case 'B':
            xViewPlayer();
            break;
        case '1':

            if(game_cam.mode == CM_FIXED_TO_CAR) {
                game_cam.index++;
                if(game_cam.index >= NUM_F1_CAMS) game_cam.index = 0;
            } else {
                game_cam.index = 0;
            }

            ResetCamera(CM_FIXED_TO_CAR, game_cam.view_clump);

            break;
        case '2':

            if(game_cam.mode == CM_FIXED_VIEW_POINT) {
                game_cam.index++;
                if(game_cam.index >= NUM_F2_CAMS) game_cam.index = 0;
            } else {
                game_cam.index = 0;
            }
            ResetCamera(CM_FIXED_VIEW_POINT, game_cam.view_clump);
            break;
        case '3':
            ResetCamera(CM_MOVING_VIEW_POINT, game_cam.view_clump);
            break;
        case '4':
            ResetCamera(CM_FOLLOW, game_cam.view_clump);
            break;
        case '5':
            ResetCamera(CM_MOVEABLE, game_cam.view_clump);
            break;
        case '6':
            //xViewPlayer();
			ResetCamera(CM_FIXED_IN_CAR, game_cam.view_clump);
            break;
        case rsESC:		
			RsEventHandler(rsQUITAPP, NULL);

            break;
    }

    return rsEVENTPROCESSED;
}

static RsEventStatus
HandleKeyUp(void *param)
{
    RsKeyStatus *keyStatus;

    keyStatus = (RsKeyStatus *)param;

    switch( keyStatus->keyCharCode )
    {
        case rsLEFT:
            control_input &= ~LEFT_ARROW;
            control_input &= ~CAM_LEFT;
            break;
        case rsRIGHT:
            control_input &= ~RIGHT_ARROW;
            control_input &= ~CAM_RIGHT;
            break;
        case rsUP:
            control_input &= ~UP_ARROW;
            control_input &= ~CAM_UP;
            control_input &= ~CAM_IN;
            break;
        case rsDOWN:
            control_input &= ~DOWN_ARROW;
            control_input &= ~CAM_DOWN;
            control_input &= ~CAM_OUT;
            break;
        case 'a':
        case 'A':
//          control_input &= ~CAM_LEFT;
            break;
        case 's':
        case 'S':
//          control_input &= ~CAM_RIGHT;
            break;
        case 'w':
        case 'W':
//          control_input &= ~CAM_UP;
            break;
        case 'z':
        case 'Z':
//          control_input &= ~CAM_DOWN;
            break;
        case 'e':
        case 'E':
//          control_input &= ~CAM_IN;
            break;
        case 'd':
        case 'D':
//          control_input &= ~CAM_OUT;
            break;
        case rsLCTRL:
        case rsRCTRL:
            ctrl_pressed = 0;
            break;
        case rsRSHIFT:
        case rsLSHIFT:
            shift_pressed = 0;
            break;
        default:
            break;
    }

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonDown(void *param)
{
    mouse_L_but = true;

    control_input |= UP_ARROW;

    return rsEVENTPROCESSED;
}
/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonUp(void)
{
    mouse_L_but = false;

    control_input &= ~UP_ARROW;

    return rsEVENTPROCESSED;
}
/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonDown(void *param)
{
    mouse_R_but = true;

    control_input |= DOWN_ARROW;

    return rsEVENTPROCESSED;
}
/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonUp(void)
{
    mouse_R_but = false;

    control_input &= ~DOWN_ARROW;

    return rsEVENTPROCESSED;
}
/*
 *****************************************************************************
 */
static RsEventStatus
HandleMouseMove(void *param)
{
    RsMouseStatus *ms = (RsMouseStatus *)param;

//  xAlterSteering(-20.0f*(RwReal)ms->delta.x/(RwReal)RsGlobal.maximumWidth);
    SetPlayerSteering(-(2.0f*(RwReal)ms->pos.x/(RwReal)RsGlobal.maximumWidth - 1.0f));

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
RsEventStatus
KeyboardHandler(RsEvent event, void *param)
{
    switch( event )
    {
        case rsKEYDOWN:
        {
            return HandleKeyDown(param);
        }

    case rsKEYUP:
        {
            return HandleKeyUp(param);
        }
        default:
            return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
RsEventStatus
MouseHandler(RsEvent event, void *param)
{
    /*
     * Simulate a mouse on pad platforms...
     */
    //turnoffPadMouse(event, param);

    switch( event )
    {
        case rsLEFTBUTTONDOWN:
        {
            return HandleLeftButtonDown(param);
        }

        case rsLEFTBUTTONUP:
        {
            return HandleLeftButtonUp();
        }

        case rsRIGHTBUTTONDOWN:
        {
            return HandleRightButtonDown(param);
        }

        case rsRIGHTBUTTONUP:
        {
            return HandleRightButtonUp();
        }

        case rsMOUSEMOVE:
        {
            return HandleMouseMove(param);
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
RsEventStatus
PadHandler(RsEvent event, void *param)
{
             RsKeyStatus ks;
         RsPadButtonStatus *pb;


    switch( event )
    {
        case rsPADBUTTONDOWN:
        {
            pb = (RsPadButtonStatus *)param;

        //          printf("padButtons = %d\n",pb->padButtons);

            if( pb->padButtons & rsPADBUTTON1 )
            {
                ks.keyCharCode = rsTAB;

                KeyboardHandler(rsKEYDOWN, &ks);
            }

            if( pb->padButtons & rsPADBUTTON2 )
            {
                ks.keyCharCode = rsUP;

                KeyboardHandler(rsKEYDOWN, &ks);
            }

            if( pb->padButtons & rsPADBUTTON3 )
            {
                ks.keyCharCode = rsDOWN;

                KeyboardHandler(rsKEYDOWN, &ks);
            }

            if( pb->padButtons & rsPADBUTTON4 )
            {
                ks.keyCharCode = 'd';

                KeyboardHandler(rsKEYDOWN, &ks);
            }

            if( pb->padButtons & rsPADBUTTON5 )
            {
                ks.keyCharCode = rsF1;

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            if( pb->padButtons & rsPADBUTTON6 )
            {
                ks.keyCharCode = rsENTER;

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            if( pb->padButtons & rsPADBUTTON7 )
            {
                ks.keyCharCode = 'p';

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            if( pb->padButtons & rsPADBUTTON8 )
            {
                ks.keyCharCode = 'r';

                KeyboardHandler(rsKEYDOWN, &ks);
            }
        if( pb->padButtons & rsPADDPADUP )
            {
                ks.keyCharCode = '6';

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            if( pb->padButtons & rsPADDPADDOWN )
            {
                ks.keyCharCode = '4';

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            if( pb->padButtons & rsPADDPADLEFT )
            {
                ks.keyCharCode = '3';

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            if( pb->padButtons & rsPADDPADRIGHT )
            {
                ks.keyCharCode = '2';

                KeyboardHandler(rsKEYDOWN, &ks);
            }
            return rsEVENTPROCESSED;
        }

        case rsPADBUTTONUP:
        {
            pb = (RsPadButtonStatus *)param;

            if( pb->padButtons & rsPADBUTTON2 )
            {
                ks.keyCharCode = rsUP;

                KeyboardHandler(rsKEYUP, &ks);
            }

            if( pb->padButtons & rsPADBUTTON3 )
            {
                ks.keyCharCode = rsDOWN;

                KeyboardHandler(rsKEYUP, &ks);
            }

            return rsEVENTPROCESSED;
        }

        case rsPADANALOGUELEFT:
        {
            RwV2d *delta = (RwV2d *)param;

            if(delta->y > 0.5f) xUpshift();
            else if(delta->y < -0.5f) xDownshift();

            SetPlayerSteering(delta->x*0.5f);

            return rsEVENTPROCESSED;

            break;
        }

        case rsPADANALOGUERIGHT:
        {
          //            RwV2d *delta = (RwV2d *)param;


            //          SetPlayerThrottle(delta->y);

            return rsEVENTPROCESSED;
            break;
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RwBool
AttachInputDevices(void)
{
    RsInputDeviceAttach(rsKEYBOARD, KeyboardHandler);

    RsInputDeviceAttach(rsMOUSE, MouseHandler);

    RsInputDeviceAttach(rsPAD, PadHandler);

    return TRUE;
}

/*
 *****************************************************************************
 */
static void
HandleIdle(void)
{
    switch(input_control_method) {
    case CTRL_KEYBOARD:
        {
            if(control_input&LEFT_ARROW)
            {
                if(!(shift_pressed || ctrl_pressed)) xAlterSteering(1.5f);
            }
            else if(control_input&RIGHT_ARROW)
            {
                if(!(shift_pressed || ctrl_pressed)) xAlterSteering(-1.5f);
            }
            else
            {
                DecaySteering();
            }

            if(control_input&UP_ARROW)
            {
                if(!(shift_pressed || ctrl_pressed)) xAlterThrottle(4.0f);
            }
            else if(control_input&DOWN_ARROW)
            {
                if(!(shift_pressed || ctrl_pressed)) xAlterThrottle(-4.0f);
            }
            else
            {
                DecayThrottle();
            }
        }
        break;
    case CTRL_JOYPAD:
    case CTRL_MOUSE:
            if(control_input&UP_ARROW)
            {
                if(!(shift_pressed || ctrl_pressed)) xAlterThrottle(4.0f);
            }
            else if(control_input&DOWN_ARROW)
            {
                if(!(shift_pressed || ctrl_pressed)) xAlterThrottle(-4.0f);
            }
            else
            {
                DecayThrottle();
            }
        break;
    case CTRL_JOYSTICK:

    default:
        break;
    }
    if(game_cam.mode == CM_MOVEABLE) {
        if(control_input&CAM_LEFT   && ctrl_pressed)    xCameraLeft();
        if(control_input&CAM_RIGHT  && ctrl_pressed)    xCameraRight();
        if(control_input&CAM_UP     && ctrl_pressed)    xCameraUp();
        if(control_input&CAM_DOWN   && ctrl_pressed)    xCameraDown();
        if(control_input&CAM_IN     && shift_pressed)   xCameraIn();
        if(control_input&CAM_OUT    && shift_pressed)   xCameraOut();
    }
    DoFrame();
}

/*
 *****************************************************************************
 */
RsEventStatus
AppEventHandler(RsEvent event, void *param)
{
    /*
     * Simulate a mouse on pad platforms...
     */
//    PadMouse(event, param);



    switch(event)
    {

        case rsINITIALIZE:
        {
            return Initialise() ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsCAMERASIZE:
        {
            CameraSize(game_cam.cam, (RwRect *)param, CurrentViewWindow, DEFAULT_ASPECTRATIO);

            if(game_vars.rear_view_mirror) ResizeRearViewMirror();

            return rsEVENTPROCESSED;
        }

        case rsRWINITIALIZE:
        {
//            return Initialise3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
            return Init3DAndSimulation(param) ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsRWTERMINATE:
        {
            TerminateCarDemo();

            return rsEVENTPROCESSED;
        }

        case rsPLUGINATTACH:
        {
            return AttachPlugins() ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsINPUTDEVICEATTACH:
        {
            AttachInputDevices();

            return rsEVENTPROCESSED;
        }
        case rsIDLE:
        {
            HandleIdle();

            return rsEVENTPROCESSED;
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

}

/*
 *****************************************************************************
 */
