/*=============================================================================
	GCNInput.cpp: UGCNInputManager implementation.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Andrew Scheidecker

  GameCube Mapping
	IK_Joy1			L Shoulder
	IK_Joy2			R Shoulder
	IK_Joy3			Z Trigger
	IK_Joy4			Y Button
	IK_Joy5			X Button
	IK_Joy6			A Button
	IK_Joy7			B Button
	IK_Joy8			D-Pad Left
	IK_Joy9			D-Pad Right
	IK_Joy10		D-Pad Up
	IK_Joy11		D-Pad Down
	IK_Joy12		Start/Pause
	IK_JoyX			Left Analog X
	IK_JoyY			Left Analog Y
	IK_JoyU			Right Analog X
	IK_JoyV			Right Analog Y
=============================================================================*/

#include <dolphin.h>
#include <math.h>
#include "GCNDrv.h"

//
// Dual shock macros/variables.
//

#define DO_ANALOG_INPUT( Port, Offset, KeyCode, Max, DeadZone ) \
	MoveRatio = ((char*)(&pad[Port]))[Offset] / (FLOAT)Max; \
	if (fabs(MoveRatio) > DeadZone) \
		Viewport->CauseInputEvent( KeyCode, IST_Axis, MoveRatio*DeltaSeconds*1000 );

#define DO_PAD_INPUT(Port, GCNCode, KeyCode) \
	if (PADButtonDown(PadStates[Port].OldPadData, PadStates[Port].PadData) & GCNCode) \
		Viewport->CauseInputEvent( KeyCode, IST_Press ); \
	else if (PADButtonUp(PadStates[Port].OldPadData, PadStates[Port].PadData) & GCNCode) \
		Viewport->CauseInputEvent( KeyCode, IST_Release ); \

/*-----------------------------------------------------------------------------
	UGCNInputManager implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UGCNInputManager);

UGCNInputManager::UGCNInputManager() :
	USubsystem()
{
}

void UGCNInputManager::StaticConstructor()
{
	guard(UGCNInputManager::StaticConstructor);

	UProperty*	Prop;

	USubsystem::StaticConstructor();

	Prop = new(GetClass(),TEXT("VibrationEnabled"),RF_Public) UBoolProperty(CPP_PROPERTY(VibrationEnabled),TEXT("Vibration"),CPF_Config);
	Prop->ArrayDim = 4;

	GetClass()->ClassConfigName = NAME_User;

	for(int Index = 0;Index < 4;Index++)
		VibrationEnabled[Index] = 1;

	unguard;
}

void UGCNInputManager::Init()
{
	guard(UGCNInputManager::Init);

	LastUpdateTime = appSeconds();

	INT i;
	for(i = 0;i < 4;i++)
	{
		PadStates[i].Opened = 0;
		PadStates[i].IsVibrating = 0;
	}

	unguard;
}


UBOOL UGCNInputManager::ClosePort( INT i )
{
	if(PadStates[i].Opened)
	{
		PadStates[i].Opened = 0;
#ifndef EMU
		if (PadStates[i].IsVibrating)
			PADStopMotorHard(i);
#endif
		PadStates[i].IsVibrating = 0;
	}
	return 1;
}

void UGCNInputManager::Tick()
{
	guard(UGCNInputManager::Tick);


	UGCNViewport*	Viewport;
	DOUBLE			UpdateTime = appSeconds();
	FLOAT			DeltaSeconds = UpdateTime - LastUpdateTime;
	INT*			ResultPtr;
	BYTE			ActuatorData[6];

	LastUpdateTime = UpdateTime;

	PADStatus   pad[PAD_MAX_CONTROLLERS]; // game pad state
	PADRead(pad);

	// Update the status of the pads.
	INT i;
	for(i = 0;i < 4;i++)
	{
		// Find the viewport for this device.
		Viewport = NULL;

		for(INT j = 0;j < GetOuterUGCNClient()->Viewports.Num();j++)
			if(GetOuterUGCNClient()->Viewports(j) && Cast<UGCNViewport>(GetOuterUGCNClient()->Viewports(j))->ViewportIndex == i)
			{
				Viewport = Cast<UGCNViewport>(GetOuterUGCNClient()->Viewports(j));
				break;
			}

		if(!Viewport)
		{
			if(!ClosePort(i))
				continue;
			if(!Viewport)
				continue;
		}

		FLOAT MoveRatio;
		PadStates[i].OldPadData = PadStates[i].PadData;
		PadStates[i].PadData = pad[i].button;

		// Process right analog stick.
		DO_ANALOG_INPUT(i,2,IK_JoyX, 128.0, 0.2);
		DO_ANALOG_INPUT(i,3,IK_JoyY, 128.0, 0.2);
		DO_ANALOG_INPUT(i,4,IK_JoyU, 96.0, 0.2);
		DO_ANALOG_INPUT(i,5,IK_JoyV, 96.0, 0.2);

		DO_PAD_INPUT(i,PAD_TRIGGER_L,	IK_Joy1);
		DO_PAD_INPUT(i,PAD_TRIGGER_R,	IK_Joy2);
		DO_PAD_INPUT(i,PAD_TRIGGER_Z,	IK_Joy3);

		DO_PAD_INPUT(i,PAD_BUTTON_Y,	IK_Joy4);
		DO_PAD_INPUT(i,PAD_BUTTON_X,	IK_Joy5);
		DO_PAD_INPUT(i,PAD_BUTTON_A,	IK_Joy6);
		DO_PAD_INPUT(i,PAD_BUTTON_B,	IK_Joy7);

		DO_PAD_INPUT(i,PAD_BUTTON_LEFT, IK_Joy8);
		DO_PAD_INPUT(i,PAD_BUTTON_RIGHT,IK_Joy9);
		DO_PAD_INPUT(i,PAD_BUTTON_UP,   IK_Joy1);
		DO_PAD_INPUT(i,PAD_BUTTON_DOWN, IK_Joy11);

		DO_PAD_INPUT(i,PAD_BUTTON_MENU, IK_Joy12);

#ifndef EMU
		// Handle vibration
		if (PadStates[i].VibrateDecay > 0.0)
		{
			if (PadStates[i].IsVibrating == false)
			{
				PADStartMotor(i);
				PadStates[i].IsVibrating = true;
			}
			else
			{
				PadStates[i].VibrateDecay -= DeltaSeconds;
				if (PadStates[i].VibrateDecay <= 0.0f)
				{
					PADStopMotor(i);
					PadStates[i].IsVibrating = false;
				}
			}
		}
#endif
	}


	unguard;
}

UBOOL UGCNInputManager::Exec(const TCHAR* Cmd,FOutputDevice& Ar)
{
	guard(UGCNInputManager::Exec);

	// is there a controller here?
	if(ParseCommand(&Cmd,TEXT("ControllerAvailable")))
	{
		INT	Port;

		Parse(Cmd,TEXT("PORT="),Port);

		// SL TODO: Decide which one of these
		Ar.Logf(TEXT("True"));
		Ar.Logf(TEXT("False"));

		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("EnableVibration")))
	{
		VibrationEnabled[appAtoi(Cmd)] = 1;
		SaveConfig();

		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("DisableVibration")))
	{
		VibrationEnabled[appAtoi(Cmd)] = 0;
		SaveConfig();

		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("VibrationEnabled")))
	{
		Ar.Logf(TEXT("%s"),VibrationEnabled[appAtoi(Cmd)] != 0 ? TEXT("True") : TEXT("False"));

		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("GetBind")))
	{
		FString		KeyName;
		UViewport*	Viewport = NULL;
		INT			Index,
					Port;
		EInputKey	Key;

		Parse(Cmd,TEXT("Port="),Port);
		Parse(Cmd,TEXT("KeyName="),KeyName);

		for(Index = 0;Index < GetOuterUGCNClient()->Viewports.Num();Index++)
		{
			if(GetOuterUGCNClient()->Viewports(Index) && Cast<UGCNViewport>(GetOuterUGCNClient()->Viewports(Index))->ViewportIndex == Port)
			{
				Viewport = GetOuterUGCNClient()->Viewports(Index);
				break;
			}
		}

		if(Viewport != NULL && Viewport->Input->FindKeyName(*KeyName,Key))
			Ar.Logf(TEXT("%s"),*Viewport->Input->Bindings[Key]);
		else
			Ar.Logf(TEXT(""));


		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("SetBind")))
	{
		FString		KeyName,
					Command;
		UViewport*	Viewport = NULL;
		INT			Index,
					Port;
		EInputKey	Key;

		Parse(Cmd,TEXT("Port="),Port);
		Parse(Cmd,TEXT("KeyName="),KeyName);
		Parse(Cmd,TEXT("Command="),Command);

		for(Index = 0;Index < GetOuterUGCNClient()->Viewports.Num();Index++)
		{
			if(GetOuterUGCNClient()->Viewports(Index) && Cast<UGCNViewport>(GetOuterUGCNClient()->Viewports(Index))->ViewportIndex == Port)
			{
				Viewport = GetOuterUGCNClient()->Viewports(Index);
				break;
			}
		}

		if(Viewport != NULL && Viewport->Input->FindKeyName(*KeyName,Key))
			Viewport->Input->Bindings[Key] = Command;

		return 1;
	}
	else if(ParseCommand(&Cmd,TEXT("SaveBind")))
	{
		UViewport*	Viewport = NULL;
		INT			Index,
					Port;

		Parse(Cmd,TEXT("Port="),Port);

		for(Index = 0;Index < GetOuterUGCNClient()->Viewports.Num();Index++)
		{
			if(GetOuterUGCNClient()->Viewports(Index) && Cast<UGCNViewport>(GetOuterUGCNClient()->Viewports(Index))->ViewportIndex == Port)
			{
				Viewport = GetOuterUGCNClient()->Viewports(Index);
				break;
			}
		}

		if(Viewport != NULL)
			Viewport->Input->SaveConfig();

		return 1;
	}
	return 0;
	unguard;
}

