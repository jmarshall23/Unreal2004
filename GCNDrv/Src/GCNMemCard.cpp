/*=============================================================================
	GCNInput.cpp: UGCNInputManager implementation.
	Copyright 2000-2002 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Andrew Scheidecker

  =============================================================================*/

#include "GCNDrv.h"
#include "../../Core/Inc/FConfigCacheIni.h"

#define MCDIRNAME TEXT("BASLUS-00000")
#define MCGAMENAME TEXT("Unreal")
#define MCICON TEXT("unrealu.ico")

/*-----------------------------------------------------------------------------
	UGCNMcManager implementation.
-----------------------------------------------------------------------------*/

#define PORT(i) Client->InputManager->PadStates[i].PortNum
#define SLOT(i) Client->InputManager->PadStates[i].SlotNum

IMPLEMENT_CLASS(UGCNMcManager);

UGCNMcManager::UGCNMcManager() :
	USubsystem()
{
}

void UGCNMcManager::StaticConstructor()
{
	guard(UGCNMcManager::StaticConstructor);
	Super::StaticConstructor();
	unguard;
}

void UGCNMcManager::Init(UGCNClient* InClient)
{
	guard(UGCNMcManager::Init);
	Client = InClient;

	for( INT i=0;i<4;i++ )
	{
		McStates[i].CheckedStatus = 0;
		McStates[i].Inserted = 0;
		McStates[i].CheckCountdown = 0;
	}
	CurSlot = 0;
	Checking = 0;

	debugf(TEXT("Initializing memory card library"));
//	if(sceMcInit() < 0)
//		debugf(TEXT("Memory card library init failed."));

	unguard;
}
void UGCNMcManager::Destroy()
{
	Super::Destroy();
	debugf(TEXT("Shutting down memory card"));
	WaitStatus(1);
}
void UGCNMcManager::Tick()
{
	guard(UGCNMcManager::Tick);
	WaitStatus(0);
	CheckStatus();
	unguard;
}
UBOOL UGCNMcManager::Exec(const TCHAR* Cmd,FOutputDevice& Ar)
{
	guard(UGCNMcManager::Exec);

	if(ParseCommand(&Cmd,TEXT("MemoryCardStatus")))
	{
		INT i = appAtoi(Cmd);
		if(i<0||i>3)
		{
			Ar.Logf(TEXT("Bad index"));
			return 1;
		}

		if(!McStates[i].CheckedStatus)
			Ar.Logf(TEXT("UNKNOWN"));
		else if(McStates[i].Inserted)
		{
			if( McStates[i].Invalid )
			{
				Ar.Logf(TEXT("INVALID"));
			}
			else
			{
				if( McStates[i].Formatted )
				{
					if( McStates[i].HasData )
						Ar.Logf(*FString::Printf( TEXT("HASDATA %d"), McStates[i].FreeSpace ));
					else
						Ar.Logf(*FString::Printf( TEXT("VALID %d"), McStates[i].FreeSpace ));

				}
				else
					Ar.Logf(TEXT("UNFORMATTED"));
			}
		}
		else
			Ar.Logf(TEXT("NO CARD"));
		return 1;
	}
	else
	if(ParseCommand(&Cmd,TEXT("FormatMemoryCard")))
	{
		INT result, cmd;
		INT i = appAtoi(Cmd);
		if(i<0||i>3)
			return 1;
		if( !McStates[i].Inserted || McStates[i].Invalid )
			Ar.Logf(TEXT("False"));
		WaitStatus(1);
//		if( sceMcFormat(PORT(i), SLOT(i)) == 0 &&
//			sceMcSync(0,&cmd,&result)!=-1 )
//		{
//			Ar.Logf(TEXT("True"));
//			McStates[i].Inserted = 0;
//			McStates[i].CheckedStatus = 0;
//		}
		return 1;
	}
	else
	if(ParseCommand(&Cmd,TEXT("UnFormatMemoryCard")))
	{
		INT result, cmd;
		INT i = appAtoi(Cmd);
		if(i<0||i>3)
			return 1;
		if( !McStates[i].Inserted || McStates[i].Invalid )
			Ar.Logf(TEXT("False"));
		WaitStatus(1);
//		if( sceMcUnformat(PORT(i), SLOT(i)) == 0 &&
//			sceMcSync(0,&cmd,&result)!=-1 )
//		{
//			Ar.Logf(TEXT("True"));
//			McStates[i].Inserted = 0;
//		}
		return 1;
	}
	else
	if(ParseCommand(&Cmd,TEXT("SaveMemoryCard")))
	{
		INT i = appAtoi(Cmd);
		if(i<0||i>3)
			return 1;
		// retry 3 times
		for( INT j=0;j<3;j++ )
		{
			if(SaveINIToMC(i))
			{
				Ar.Logf(TEXT("True"));
				return 1;
			}
		}
		Ar.Logf(TEXT("False"));
		return 1;
	}
	else
	if(ParseCommand(&Cmd,TEXT("LoadMemoryCard")))
	{
		INT i = appAtoi(Cmd);
		if(i<0||i>3)
			return 1;
		// retry 3 times
		for( INT j=0;j<3;j++ )
		{
			if(LoadINIFromMC(i))
			{
				Ar.Logf(TEXT("True"));
				return 1;
			}
		}
		Ar.Logf(TEXT("False"));
		return 1;
	}
	return 0;
	unguard;
}

void UGCNMcManager::CheckStatus()
{
	if( Checking )
		return;

}

void UGCNMcManager::WaitStatus(UBOOL Wait)
{
	INT cmd, result;

	if(!Checking)
		return;
/*
	INT i = sceMcSync(!Wait,&cmd,&result);

	if( i==0 )	// still executing
		return;
	else
	if( i==-1 )
	{
		debugf(TEXT("sceMcSync reported no function registered"));
		Checking = 0;
	}
	else
	{
		Checking = 0;
		if( cmd != sceMcFuncNoCardInfo )
			debugf(TEXT("sceMcSync was waiting on unexpected function %d"), cmd);
		else
		{
			if( result == 0 )
			{
				//debugf(TEXT("No change on slot %d, type: %d free %d format %d"), CurSlot, CheckType, CheckFree, CheckFormat);
				if( !McStates[CurSlot].Inserted )
					result = CheckFormat?-1:-2;
				if( CheckFormat )
					McStates[i].FreeSpace = CheckFree;
				McStates[CurSlot].CheckCountdown = 20;
			}
			if( result == -1 )
			{
				debugf(TEXT("Inserted formatted mc into slot %d, free space is %d"), CurSlot, CheckFree);
				McStates[CurSlot].Inserted = 1;
				McStates[CurSlot].Formatted = 1;
				McStates[CurSlot].Invalid = CheckType != 2;
				McStates[CurSlot].CheckCountdown = 20;
				McStates[CurSlot].FreeSpace = CheckFree;
				McStates[CurSlot].HasData = HasGameData(CurSlot);
			}
			if( result == -2 )
			{
				debugf(TEXT("Inserted unformatted mc into slot %d, checking...."), CurSlot);
				sceMcGetInfo( PORT(CurSlot), SLOT(CurSlot), &CheckType, &CheckFree, &CheckFormat );
				sceMcSync(0,&cmd,&result);
				if(CheckFormat)
					return;
				debugf(TEXT("Verified unformatted mc in slot %d"), CurSlot);
				McStates[CurSlot].Inserted = 1;
				McStates[CurSlot].Formatted = 0;
				McStates[CurSlot].Invalid = CheckType != 2;
				McStates[CurSlot].CheckCountdown = 20;
				McStates[CurSlot].FreeSpace = 0;
			}		
			if(result <= -10)
			{
				if( McStates[CurSlot].Inserted )
				{
					debugf(TEXT("Removed mc from slot %d"), CurSlot);
					McStates[CurSlot].Inserted = 0;
					McStates[CurSlot].FreeSpace = 0;
				}
				//debugf(TEXT("Slot %d is empty"), CurSlot);
				McStates[CurSlot].CheckCountdown = 50;
			}
			McStates[CurSlot].CheckedStatus = 1;
		}
	}
*/
}

UBOOL UGCNMcManager::CopyToMc( INT Port, const TCHAR *Source, const TCHAR* Dest )
{
	FArchive* S = GFileManager->CreateFileReader( Source, 0, GLog );
	if( !S )
	{
		debugf(TEXT("Failed to open %s"), Source);
		return 0;
	}
//	FArchive* D = CreateFileWriter( Port, Dest, SCE_WRONLY|SCE_CREAT, GLog );
//	if( !D )
//	{
//		delete S;
//		return 0;
//	}
//	INT Size = S->TotalSize();
//	TArray<ANSICHAR> Ch( Size+2 );
//	S->Serialize( &Ch(0), Size );
//	D->Serialize( &Ch(0), Size );
//
//	if( D->IsError() )
//	{
//		delete S;
//		delete D;
//		return 0;
//	}
//	delete S;
//	delete D;
	return 1;
}

UBOOL UGCNMcManager::HasGameData( INT Port )
{
	FArchive* Load = CreateFileReader( Port, *FString::Printf(TEXT("/%s/icon.sys"), MCDIRNAME), 0, GLog );
	debugf(TEXT("HasGameData: %d"), Load?1:0);
	if(!Load)
		return 0;
	delete Load;
	return 1;	
}

UBOOL UGCNMcManager::LoadINIFromMC( INT Port )
{
	debugf(TEXT("LoadINIFromMC 1"));
	FConfigFile* UserIni = ((FConfigCacheIni*)(GConfig))->Find( TEXT("user.ini"), 0 );
	if( !UserIni )
		return 0;
	TCHAR FileName[256];
	appStrcpy( FileName, TEXT("/") );
	appStrcat( FileName, MCDIRNAME );
	appStrcat( FileName, TEXT("/") );
	appStrcat( FileName, MCDIRNAME );

	FArchive* Load = CreateFileReader( Port, FileName, 0, GLog );
	if(!Load)
	{
		debugf(TEXT("couldn't read UT data file"));
		return 0;
	}
#define MaxDataSize (128 * 1024)
	TCHAR* Data = (TCHAR*)appMalloc(MaxDataSize, "");
	INT Pos=0;
	while(!Load->AtEnd() && Pos+1024<MaxDataSize)
	{
		Load->Serialize( &Data[Pos], 1024 );
		Pos += 1024;
	}
	if( Load->IsError() )
	{
		delete Load;
		return 0;
	}
	INT Size=Min<INT>(Load->TotalSize(), MaxDataSize-1);
	Data[Size] = 0;
	debugf( TEXT("Loaded %d bytes from memory card, strlen %d"), Size, appStrlen(Data));

	if( ParseParam(appCmdLine(),TEXT("host")) )
	{
		debugf(TEXT("Saving memory card data as memcard.ini"));
		appSaveStringToFile( FString(&Data[0]), TEXT("memcard.ini"), GFileManager );
	}

//SLUW	UserIni->SetText( &Data[0] );

	delete Load;
	debugf(TEXT("LoadINIFromMC 2"));

	TArray<UClass*> Classes;
	for( TObjectIterator<UClass> ItC; ItC; ++ItC )
		if( ItC->ClassConfigName == NAME_User )
		{
			Classes.AddItem( *ItC );
			debugf(TEXT("Loading config for class %s"), ItC->GetName() );
			AActor* A = Cast<AActor>(ItC->GetDefaultObject());
//SLUW			if( A && A->IsProbing(ENGINE_PreLoadConfig) )
//SLUW				A->eventPreLoadConfig();
			ItC->GetDefaultObject()->LoadConfig( 0 );
//SLUW			if( A && A->IsProbing(ENGINE_PostLoadConfig) )
//SLUW				A->eventPostLoadConfig();
		}
	debugf(TEXT("LoadINIFromMC 3"));

	TArray<UObject*> Objects;
	for(TObjectIterator<UObject> ItO; ItO; ++ItO )
	{
		for( INT i=0;i<Classes.Num();i++ )
		{
			if( ItO->IsA(Classes(i)) )
			{
				if( Objects.FindItemIndex( *ItO ) == INDEX_NONE )
				{
					debugf(TEXT("Loading config for instance %s"), ItO->GetName() );
					AActor* A = Cast<AActor>(*ItO);		
//SLUW					if( A && A->IsProbing(ENGINE_PreLoadConfig) )
//SLUW						A->eventPreLoadConfig();
					ItO->LoadConfig( 0 );
//SLUW					if( A && A->IsProbing(ENGINE_PostLoadConfig) )
//SLUW						A->eventPostLoadConfig();
					Objects.AddItem( *ItO );
				}
			}
		}
	}
	debugf(TEXT("LoadINIFromMC 4"));
	appFree(Data);
	return 1;
}

UBOOL UGCNMcManager::SaveINIToMC( INT Port )
{
	FConfigFile* UserIni = ((FConfigCacheIni*)(GConfig))->Find( TEXT("user.ini"), 0 );
	if( !UserIni )
	{
		debugf(TEXT("Couldn't find ini"));
		return 0;
	}
	FString IniText ;//SLUW= UserIni->GetText();
	TCHAR FileName[256];
	appStrcpy( FileName, TEXT("/") );
	appStrcat( FileName, MCDIRNAME );
	appStrcat( FileName, TEXT("/") );
	appStrcat( FileName, MCDIRNAME );

//	FArchive* Save = CreateFileWriter( Port, FileName, SCE_WRONLY|SCE_CREAT, GLog );
//	if(!Save)
//	{
//		debugf(TEXT("Couldn't create save file"));
//		return 0;
//	}
//	Save->Serialize( (TCHAR*)(*IniText), IniText.Len() );
//
//	BYTE Zeros[1024];
//	INT BytesWritten = IniText.Len();
//	appMemzero( Zeros, sizeof(Zeros) );
//	while( BytesWritten < 65536 )
//	{
//		Save->Serialize( Zeros, Min<INT>(65536-BytesWritten, 1024) );
//		BytesWritten += 1024;
//	}
//
//	if( Save->IsError() )
//	{
//		debugf(TEXT("error saving"));
//		delete Save;
//		return 0;
//	}
//	delete Save;
	debugf(TEXT("Saved %d bytes of data"), IniText.Len() );
	McStates[Port].HasData = 1;
	return 1;
}

FArchive* UGCNMcManager::CreateFileReader( INT Port, const TCHAR* filename, DWORD flags, FOutputDevice* error )
{
	guard(UGCNMcManager::CreateFileReader);
	WaitStatus(1);
	INT cmd, file;
//	if( sceMcOpen(PORT(Port), SLOT(Port) ,(char *)filename,SCE_RDONLY) || sceMcSync(0,&cmd,&file)==-1 )
//		file = -1;
	if( file >= 0 )
		return new(TEXT("GCNFileReader"))FArchiveFileReaderMC(this,file,error);
	else
		return NULL;
	unguard;
}

FArchive* UGCNMcManager::CreateFileWriter( INT Port, const TCHAR* Filename, DWORD Flags, FOutputDevice* Error )
{
	guard(UGCNMcManager::CreateFileReader);
	WaitStatus(1);
	INT cmd, file, result;

//	sceMcDelete(PORT(Port), SLOT(Port), (char *)Filename);
//	sceMcSync(0, &cmd,&result);
//
//	if(result < 0)
//		debugf(TEXT("Tried to delete, returned %d"), result);
//
//	if( result=(sceMcOpen(PORT(Port), SLOT(Port), (char *)Filename,Flags)) )
//	{
//		debugf(TEXT("sceMcOpen returned %d"), result);
//		file = -1;
//	}
//	else
//	if( (result=sceMcSync(0,&cmd,&file))==-1 )
//	{
//		debugf(TEXT("sceMcSync returned %d"), result);
//		file = -1;
//	}
	if( file>=0 )
		return new(TEXT("GCNFileWriter"))FArchiveFileWriterMC(this,file,Error);
	else
	{
		debugf(TEXT("sceMcOpen: file was %d"), file);
		return NULL;
	}
	unguard;
}


/*-----------------------------------------------------------------------------
	Memory card file reader.
-----------------------------------------------------------------------------*/

UBOOL FArchiveFileReaderMC::Close()       
{
	INT result, cmd;
	McManager->WaitStatus(1);
	if(File)
	{
//		sceMcClose(File);
//		sceMcSync(0, &cmd, &result);
	}
	File=NULL; 
	return !ArIsError;
}

void FArchiveFileReaderMC::Serialize( void* V, INT Length )
{
	McManager->WaitStatus(1);
	char *Buffer = (char*)V;
	INT cmd, result;

	while( Length > 0 )
	{
		INT req = Min<INT>(Length, 1024);
//		if( sceMcRead(File,Buffer,req) || sceMcSync(0, &cmd, &result)==-1 || result<0 )
//		{
//			ArIsError = 1;
//			Error->Logf( TEXT("sceMcRead failed") );
//			break;
//		}
		Pos += result;
		Buffer += result;
		Length -= result;
		Size = Pos;
		if( result < req )
		{
			ArAtEnd = 1;
			break;
		}
	}
}

/*-----------------------------------------------------------------------------
	Memory card file writer.
-----------------------------------------------------------------------------*/

void FArchiveFileWriterMC::Seek( INT InPos )
{
	INT cmd,result;
	McManager->WaitStatus(1);
//	if( sceMcSeek(File,InPos,SCE_SEEK_SET) || sceMcSync(0,&cmd,&result)==-1 || result )
//	{
//		ArIsError = 1;
//		Error->Logf( LocalizeError("SeekFailed",TEXT("Core")) );
//	}
	Pos = InPos;
}
UBOOL FArchiveFileWriterMC::Close()
{
	INT result,cmd;
	McManager->WaitStatus(1);
//	if( File && ( sceMcClose(File) || sceMcSync(0,&cmd,&result)==-1 || result) )
//	{
//		ArIsError = 1;
//		Error->Logf( LocalizeError("WriteFailed",TEXT("Core")) );
//	}
	File = NULL;
	return !ArIsError;
}
void FArchiveFileWriterMC::Serialize( void* V, INT Length )
{
	Pos += Length;
	McManager->WaitStatus(1);
	while( Length>0 )
	{
		INT result, cmd;
		INT ToWrite=Min(Length,1024);
		debugf(TEXT("remaining %i %i"),Length,ToWrite);
//		if( sceMcWrite( File, V, ToWrite ) || sceMcSync(0,&cmd,&result)==-1 || result<0 )
//		{
//			ArIsError = 1;
//			Error->Logf( LocalizeError("WriteFailed",TEXT("Core")) );
//		}
		Length = Length   - ToWrite;
		V      = (BYTE*)V + ToWrite;
	}
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

