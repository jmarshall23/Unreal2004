#include "UTVCommandlet.h"
#include "BunchDelayer.h"
#include "UTVPackageMap.h"

void UUTVCommandlet::StaticConstructor()
{
	guard(UUTVCommandlet::StaticConstructor);

	LogToStdout = 1;
	IsClient    = 0;
	IsEditor    = 0;
	IsServer    = 1;
	LazyLoad    = 1;

	unguard;
}

static void GetValue(FString& s,const TCHAR* key,FString& value)
{
	if(s.InStr(key)!=-1){
		FString k=key;
		FString s2=s.Mid(s.InStr(key)+k.Len());
		if(s2.InStr(TEXT(" "))!=-1)
			s2=s2.Left(s2.InStr(TEXT(" ")));
		if(s2.InStr(TEXT("?"))!=-1)
			s2=s2.Left(s2.InStr(TEXT("?")));
		value=s2;
		//debugf(TEXT("%s %s"),key,value);
	}
}

INT UUTVCommandlet::Main( const TCHAR* Parms )
{
	guard(UUTVCommandlet::Main);

	//Say hello
	debugf (TEXT ("UTV2004 ") UTVVERSION TEXT(" starting up..."));

	// Language.
	TCHAR Temp[256];
	if( GConfig->GetString( TEXT("Engine.Engine"), TEXT("Language"), Temp, ARRAY_COUNT(Temp) ) )
	UObject::SetLanguage( Temp );

    appResetTimer(); // sjs

	// Create the replicator engine class.
	UClass* EngineClass = UObject::StaticLoadClass( UEngine::StaticClass(), NULL, TEXT("utv2004.ReplicatorEngine"), NULL, LOAD_NoFail, NULL );
	UReplicatorEngine* Engine = (UReplicatorEngine*)ConstructObject<UEngine>( EngineClass );

	FString inifile=TEXT("utv.ini");
	FString s=Parms;
	GetValue(s,TEXT("inifile="),inifile);

	Engine->Init(inifile);
	UtvEngine->ParseCmdLine(Parms);


	//UClass* UplinkClass = UObject::StaticLoadClass( UEngine::StaticClass(), NULL, TEXT("utv2004.ReplicatorEngine"), NULL, LOAD_NoFail, NULL );

	// Main loop.
	GIsRunning = 1;

	DOUBLE OldTime = appSeconds();
	DOUBLE SecondStartTime = OldTime;
	INT TickCount = 0;
	UTVStats = new UUTVStats ();
	UTVStats->Clear ();
	DOUBLE lastStats = appSeconds ();
	while( GIsRunning && !GIsRequestingExit )
	{
		INT TotalCycles = 0;
		clock (TotalCycles);
		INT RunCycles = 0;
		clock (RunCycles);

		// Clear stats (will also update old stats).
		GStats.Clear();

		// Update the world.
		guard(UpdateWorld);
		DOUBLE NewTime = appSeconds();
		Engine->Tick( NewTime - OldTime );
		if( appSeconds() < NewTime )
            SecondStartTime = NewTime = appSeconds();

		OldTime = NewTime;
		TickCount++;
		if( OldTime > SecondStartTime + 1 )
		{
			Engine->CurrentTickRate = (FLOAT)TickCount / (OldTime - SecondStartTime);
			SecondStartTime = OldTime;
			TickCount = 0;
		}
		if(Engine->DoRestart){
			Engine->Restart();
		}
		unguard;

		//mu
		unclock (RunCycles);

		INT SleepCycles = 0;
		clock (SleepCycles);

		// Enforce optional maximum tick rate.
		guard(EnforceTickRate);
		FLOAT MaxTickRate = Engine->GetMaxTickRate();
		if( MaxTickRate>0.f )
		{
			FLOAT Delta = (1.f/MaxTickRate) - (appSeconds()-OldTime);
			appSleep( Max(0.f,Delta) );
		}
		unguard;

		unclock (SleepCycles);
		unclock (TotalCycles);

		UTVStats->Add (0, TotalCycles);
		UTVStats->Add (1, SleepCycles);
		UTVStats->Add (2, RunCycles);

		UTVStats->Add (3, Engine->TickCycles);
		UTVStats->Add (4, Engine->GameCycles);

		if (Engine->clockInterval > 0) {
			if (appSeconds() - lastStats > Engine->clockInterval) {
				UTVStats->Show ();
				lastStats = appSeconds ();
			}
		} 
		else {
			UTVStats->Clear ();
		}
	}	

	debugf (TEXT ("UTV2004 Exiting..."));
	
	GIsRunning = 0; 
	return 0;
	unguard;
}

IMPLEMENT_CLASS(UUTVCommandlet);

IMPLEMENT_PACKAGE(UTV2004);