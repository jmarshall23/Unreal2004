/***********************************************************************************************
*
*	$Id: winextra.c,v 1.1.2.1 2002/03/01 19:54:12 richardm Exp $
*
************************************************************************************************/
#define DIRECTINPUT_VERSION 0x0700

#include <windows.h>
#include <winbase.h>
#include <mmsystem.h>
#include <dinput.h>
#include <stdio.h>

#include "rwcore.h"
#include "rpworld.h"

#include "skeleton.h"
#include "events.h"
#include "MePrecision.h"
#include "control.hpp"
#include "winextra.h"
#include "platxtra.h"
#include "MePrecision.h"

extern void SetPlayerSteering(MeReal);
extern void SetPlayerThrottle(MeReal);
extern void wUpshift(void);
extern void wDownshift(void);

#define RANGE_MIN (-1000)
#define RANGE_MAX (1000)
#define DEADZONE (1000)
#define FF_GAIN (10000)


/* Joystick stuff	*/
// file global variables
static BOOL                 fIsFFDevice  = FALSE;   // does our device support
                                                    // ForceFeedback
static DWORD                dwGain       = FF_GAIN;// gain selection from user
static LPDIRECTINPUT        gpdi         = NULL;    // base DirectInput object
static LPDIRECTINPUTDEVICE2 gpdiJoystick = NULL;    // DirectInputDevice2 objects
                                                    // support ForceFeedback

/*--- Functions ---*/

/*
 * Platform Specific Functionality
 */
static LARGE_INTEGER perf_timer_freq;
static LARGE_INTEGER perf_start_time;
static BOOL perf_timer = 0;


void
InitialisePerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
	perf_timer = 0;

	if(QueryPerformanceFrequency(&perf_timer_freq)) perf_timer = 1;
#endif
}

void
ResetPerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
	QueryPerformanceCounter(&perf_start_time);
#endif
}

float
ReadPerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
	float res = 0.0f;
	LARGE_INTEGER perf_timer_count;

	if(perf_timer && perf_timer_freq.QuadPart)
	{
		QueryPerformanceCounter(&perf_timer_count);
 		return (float)(1000.0f * (double)(perf_timer_count.QuadPart - perf_start_time.QuadPart)/(double)perf_timer_freq.QuadPart); //time in ms
	}
#endif
	return 0.0f;
}


/**** DirectInput Joystick Control Stuff	*****/

//===========================================================================
// inputEnumDeviceProc
//
// Enumerates DirectInput devices of type specified in call to
//  IDirectInput::EnumDevices()
//
// Parameters:
//
// Returns:
//
//===========================================================================
static BOOL CALLBACK inputEnumDeviceProc(LPDIDEVICEINSTANCE pdidi, LPVOID pv)
{
    GUID *pguidDevice = NULL;

    // validate pv
    // BUGBUG

    // report back the instance guid of the device we enumerated
    if(pv)
    {

        pguidDevice = (GUID *)pv;

        *pguidDevice = pdidi->guidInstance;

    }

    // BUGBUG for now, we'll just use the first device that has been found
    return DIENUM_STOP;
	//return DIENUM_CONTINUE;
} //*** end inputEnumDeviceProc()

//===========================================================================
// inputAcquireDevices
//
// Acquires the input device(s).
//
// Parameters:
//
// Returns:
//
//===========================================================================
static BOOL inputAcquireDevices(void)
{
    if(!gpdiJoystick)
    {
        return FALSE;
    }

    // reacquire the device
    if(SUCCEEDED(gpdiJoystick->lpVtbl->Acquire(gpdiJoystick)))
    {


        // DirectInput automatically resets the device whenever
        // ownership changes, so we can assume we've got a device
        // unsullied by its previous owner.

		//inputCreateEffect(EF_BOUNCE | EF_FIRE | EF_EXPLODE);

        return TRUE;
    }
    // if we get here, we did >not< acquire the device
    return FALSE;

} //*** end inputAcquireDevices()


//===========================================================================
// inputPrepareDevice
//
// Performs device preparation by setting the device's parameters (ie
// deadzone).
//
// Parameters:
//
// Returns:
//
//===========================================================================
static BOOL inputPrepareDevice(void)
{
    HRESULT       hRes;
    DIPROPRANGE   dipr;
    DIPROPDWORD   dipdw;


    // quick check to make sure that the object pointer is non-NULL
    if(!gpdiJoystick)
    {
        return FALSE;
    }

    // call Unacquire() on the device
    //
    // SetParameter() will fail if a device is currently acquired, we are
    // doing this here in case we get careless and forget to call this
    // function either before we call Acquire() or after we call Unacquire().
    gpdiJoystick->lpVtbl->Unacquire(gpdiJoystick);

    // set the axis ranges for the device
    //
    // We will use the same range for the X and Y axes.  We are setting them
    // fairly low since we are not concerned with anything other than
    // "left", "right", "forward", "backward" and "centered"
	//* prepare DIPROPRANGE structure
    dipr.diph.dwSize        = sizeof(DIPROPRANGE);
	dipr.diph.dwHeaderSize  = sizeof(dipr.diph);
	dipr.diph.dwHow         = DIPH_BYOFFSET;
	dipr.lMin               = RANGE_MIN;  // negative to the left/top
	dipr.lMax               = RANGE_MAX;  // positive to the right/bottom
    //* x-axis
    dipr.diph.dwObj         = DIJOFS_X;
    //* set the x-axis range property
    hRes = gpdiJoystick->lpVtbl->SetProperty(gpdiJoystick, DIPROP_RANGE, &dipr.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(RANGE, X-Axis) failed.\n");
        return FALSE;
    }
    //* y-axis
    dipr.diph.dwObj         = DIJOFS_Y;
    hRes = gpdiJoystick->lpVtbl->SetProperty(gpdiJoystick, DIPROP_RANGE, &dipr.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(RANGE, Y-Axis) failed.\n");
        return FALSE;
    }
//* z-axis
    dipr.diph.dwObj         = DIJOFS_Z;
    hRes = gpdiJoystick->lpVtbl->SetProperty(gpdiJoystick, DIPROP_RANGE, &dipr.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(RANGE, Z-Axis) failed.\n");
        return FALSE;
    }
    // set the deadzone for the device
    //
    // We will use the same deadzone percentage for the X and Y axes.
    // This call uses a symbolic constant for the deadzone percentage so that
    // it is easy to change if we decide we don't like it.
	//* prepare DIPROPDWORD structure
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwHow        = DIPH_BYOFFSET;
	dipdw.dwData            = DEADZONE;
    //* set the x-axis range property
    dipdw.diph.dwObj         = DIJOFS_X;
    hRes = gpdiJoystick->lpVtbl->SetProperty(gpdiJoystick, DIPROP_DEADZONE, &dipdw.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(DEADZONE, X-Axis) failed.\n");
        return FALSE;
    }
    //* y-axis
    dipdw.diph.dwObj         = DIJOFS_Y;
    hRes = gpdiJoystick->lpVtbl->SetProperty(gpdiJoystick, DIPROP_DEADZONE, &dipdw.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(DEADZONE, Y-Axis) failed.\n");
        return FALSE;
    }
//* z-axis
    dipdw.diph.dwObj         = DIJOFS_Z;
    hRes = gpdiJoystick->lpVtbl->SetProperty(gpdiJoystick, DIPROP_DEADZONE, &dipdw.diph);
    if(FAILED(hRes))
    {
        OutputDebugString("SetProperty(DEADZONE, Z-Axis) failed.\n");
        return FALSE;
    }
    // set the ForceFeedback gain
    //
    // If the device supports feedback, use the user selected gain level
    // to scale the strength of the forces applied to the stick.  We do this
    // so that if a small child is playing the game, the stick does not jerk
    // hard enough to hurt them, yet an adult can have a stronger force
    // experience
    if(fIsFFDevice)
    {
        // BUGBUG get setting from user (done somewhere else)
        dwGain = FF_GAIN;
    }

    // Acquire the device(s)
    //
    // This is being done as a convenience since we unacquired earlier in
    // this function.  This does not guarantee that the device will be
    // acquired at the time we return from the function (in other words, we
    // are not going to spin here until we get a succeessful acquisition).
    inputAcquireDevices();

    // we've actually done somthing here
    return TRUE;

} //** end inputPrepareDevice()


//===========================================================================
// inputInitDirectInput
//
// Creates and initializes DirectInput objects
//
// Parameters:
//
// Returns:
//
//===========================================================================
BOOL inputInitDirectInput(HINSTANCE hInst, HWND hWnd)
{
    HRESULT             hRes;
    LPDIRECTINPUTDEVICE pdiTempDevice       = NULL;
    DIDEVCAPS           didc;
    GUID                guidDevice;
    TCHAR               tszBuf[256];

    // create the base DirectInput object
    hRes = DirectInputCreate(hInst, DIRECTINPUT_VERSION, &gpdi, NULL);
    if(FAILED(hRes))
    {
        wsprintf(tszBuf, TEXT("DirectInputCreate() failed - %08Xh\n\n")
                  TEXT("DirectX 5 or later required."), hRes);
        MessageBox(hWnd, tszBuf, TEXT("Input Device"), MB_OK);
        return FALSE;
    }

    // enumerate for joystick devices
    hRes = gpdi->lpVtbl->EnumDevices(gpdi, DIDEVTYPE_JOYSTICK,
                                    (LPDIENUMDEVICESCALLBACK)inputEnumDeviceProc,
                                    &guidDevice,
                                    DIEDFL_ATTACHEDONLY);
    if(FAILED(hRes))
    {
        wsprintf(tszBuf, TEXT("EnumDevices() failed - %08Xh"), hRes);
        MessageBox(hWnd, tszBuf, TEXT("Input Device"), MB_OK);
        return FALSE;
    }

    // create a temporary "Device 1" object
    hRes = gpdi->lpVtbl->CreateDevice(gpdi, &guidDevice, &pdiTempDevice, NULL);
    if(FAILED(hRes))
    {
//        wsprintf(tszBuf, TEXT("Joystick Not Detected\n\n  Use Mouse Control\n"));
//        wsprintf(tszBuf, TEXT("Joystick CreateDevice() failed - %08Xh\n\n"), hRes);
//        MessageBox(hWnd, tszBuf, TEXT("Input Device"), MB_OK);
        return FALSE;
    }

    // get a "Device 2" object
    //
    // this is needed for access to the ForceFeedback functionality
    hRes = pdiTempDevice->lpVtbl->QueryInterface(pdiTempDevice,
                                                &IID_IDirectInputDevice2,
                                                &gpdiJoystick);
    if(FAILED(hRes))
    {
        wsprintf(tszBuf, TEXT("QueryInterface(IID_IDirectInputDevice2) failed - %08Xh"), hRes);
        MessageBox(hWnd, tszBuf, TEXT("Input Device"), MB_OK);
        return FALSE;
    }

    // we no longer need the temporary device, go ahead and release it.
    if(pdiTempDevice)
    {
        pdiTempDevice->lpVtbl->Release(pdiTempDevice);
        pdiTempDevice = NULL;
    }

    // set the device's data format
    //
    // This tells the device object to act like a specific device --
    // in our case, like a joystick
    hRes = gpdiJoystick->lpVtbl->SetDataFormat(gpdiJoystick, &c_dfDIJoystick);
    if(FAILED(hRes))
    {
        wsprintf(tszBuf, TEXT("SetDataFormat(Joystick) failed - %08Xh"), hRes);
        MessageBox(hWnd, tszBuf, TEXT("Force Feedback"), MB_OK);
        return FALSE;
    }

    // set the device's cooperative level
    //
    // ForceFeedback requires Exclusive access to the device.
    hRes = gpdiJoystick->lpVtbl->SetCooperativeLevel(gpdiJoystick, hWnd,
                                                    DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if(FAILED(hRes))
    {
        wsprintf(tszBuf, TEXT("SetCooperativeLevel(Exclusive | Foreground) failed - %08Xh"), hRes);
        MessageBox(hWnd, tszBuf, TEXT("Force Feedback"), MB_OK);
        return FALSE;
    }

    // set joystick parameters (deadzone, etc)
    if(!inputPrepareDevice())
    {
        MessageBox(hWnd, TEXT("Device preparation failed"),
                  TEXT("Force Feedback"), MB_OK);
        return FALSE;
    }

    // get the device capabilities
    //
    // We're going to check to see if the device we created supports
    // ForceFeedback.  If so, we will create effects, if not, we'll
    // support standard joystick functionality
    fIsFFDevice = FALSE;
    didc.dwSize = sizeof(DIDEVCAPS);
    hRes = gpdiJoystick->lpVtbl->GetCapabilities(gpdiJoystick, &didc);
    if(FAILED(hRes))
    {
        wsprintf(tszBuf, TEXT("GetCapabilities() failed - %08Xh"), hRes);
        MessageBox(hWnd, tszBuf, TEXT("Force Feedback"), MB_OK);
        return FALSE;
    }
    if(didc.dwFlags & DIDC_FORCEFEEDBACK)
    {
        OutputDebugString("ForceFeedback device found.\n");

        // get the gain level from the user
        //DialogBox(hInst, MAKEINTRESOURCE(IDD_FORCE), hWnd, inputForceLevelDlgProc);

        // we're supporting ForceFeedback
        fIsFFDevice = TRUE;
        //if(!inputCreateEffect(EF_BOUNCE | EF_FIRE | EF_EXPLODE))
        //{
        //    OutputDebugString("inputCreateEffects() failed - ForceFeedback disabled\n");
        //}
    } //** end if(ForceFeedback device)

    // if we get here, we succeeded
    return TRUE;

} //*** end inputInitDirectInput()


//===========================================================================
// inputCleanupDirectInput
//
// Cleans up DirectInput objects
//
// Parameters: none
//
// Returns: nothing
//
//===========================================================================
void inputCleanupDirectInput(void)
{
    OutputDebugString("Cleaning up after DirectInput\n");

    // Unacquire() and Release() device objects
    //
    // It is perfectly safe to call Unacquire() on a device that is not
    // currently acquired.  In fact, it is good practice to call
    // Unacquire() just prior to Release().
    if(gpdiJoystick)
    {
        gpdiJoystick->lpVtbl->Unacquire(gpdiJoystick);
        gpdiJoystick->lpVtbl->Release(gpdiJoystick);
        gpdiJoystick = NULL;
    }

    // Release() base object
    if(gpdi)
    {
        gpdi->lpVtbl->Release(gpdi);
        gpdi = NULL;
    }

} //*** end inputCleanupDirectInput()






//===========================================================================
// inputProcessDeviceInput
//
// Processes data from the input device.  Uses GetDeviceState().
//
// Parameters:
//
// Returns:
//
//===========================================================================
DWORD inputProcessDeviceInput(void)
{
    HRESULT     hRes;
    DIJOYSTATE  dijs;
    DWORD       dwInput = 0;
	float input;
    RsKeyStatus ks;
	static int rp_down = 0;
	static int lp_down = 0;

	if(input_control_method != CTRL_JOYSTICK) return dwInput;

    // poll the joystick to read the current state
    hRes = gpdiJoystick->lpVtbl->Poll(gpdiJoystick);


	if(FAILED(hRes))
	{
		switch(hRes) {
			case DIERR_INPUTLOST:
				inputAcquireDevices();
				break;
			case DIERR_NOTACQUIRED:
				inputAcquireDevices();
				break;
			case DIERR_NOTINITIALIZED:
				break;
			default:
				break;
		}
		return(0);
	}

    // read the device state
    hRes = gpdiJoystick->lpVtbl->GetDeviceState(gpdiJoystick, sizeof(DIJOYSTATE),
                                                &dijs);
    if(FAILED(hRes))
    {
        if((hRes == DIERR_INPUTLOST))
        {
            inputAcquireDevices();
        }

        // we did not read anything, return no motion
        return 0;
    }

	input = dijs.lX/-1000.0f;
	//if(input<0) //square input
	//	input *= -input;
	//else
	//	input *= input;
	SetPlayerSteering(input);
	SetPlayerThrottle(dijs.lY/-1000.0f);


    if(dijs.rgbButtons[0] & 0x80)
    {

	if(rp_down == 0)
	{
	    ks.keyCharCode = 'x';
        RsKeyboardEventHandler(rsKEYDOWN, &ks);
		rp_down = 1;
	}
//left paddle
	/*
			wUpshift();
			rp_down = 1;
		}*/
    }
	else
		rp_down = 0;
    if(dijs.rgbButtons[1] & 0x80)
    {
//right paddle
	if(!lp_down)
	{
	 ks.keyCharCode = ' ';
     RsKeyboardEventHandler(rsKEYDOWN, &ks);
	 lp_down = 1;
	}
	/*
			wDownshift();
			lp_down = 1;
		}*/
    }
	else
		lp_down = 0;

    if(dijs.rgbButtons[2] & 0x80)
    {
        ks.keyCharCode = '2';
//        KeyboardHandler(rsKEYDOWN, &ks);
        RsKeyboardEventHandler(rsKEYDOWN, &ks);

    }
    //* button 1 (shield)
    if(dijs.rgbButtons[3] & 0x80)
    {
        ks.keyCharCode = '3';
        RsKeyboardEventHandler(rsKEYDOWN, &ks);
    }
    //* button 2 (stop) - requires a joystick with more than 2 buttons
    if(dijs.rgbButtons[4] & 0x80)
    {
        ks.keyCharCode = '4';
        RsKeyboardEventHandler(rsKEYDOWN, &ks);
    }

    if(dijs.rgbButtons[5] & 0x80)
    {
        ks.keyCharCode = '6';
        RsKeyboardEventHandler(rsKEYDOWN, &ks);
    }

    // return the new device state
    return dwInput;

} //*** end inputProcessDeviceInput()


