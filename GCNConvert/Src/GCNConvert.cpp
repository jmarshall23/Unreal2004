/*=============================================================================
	SampleClass.cpp
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	This is a minimal example of how to mix UnrealScript and C++ code
	within a class (defined by SampleClass.uc and SampleClass.cpp),
	for a package (defined by SamplePackage.u and SamplePackage.dll).
=============================================================================*/

// Includes.
#include "Engine.h"
#include "EditorPrivate.h"

// Package implementation.
//class UGCNConvertPackage : public UPackageBase {};
IMPLEMENT_PACKAGE(GCNConvert);


typedef struct {
	BYTE	IDFieldLen;			//length of additional bytes following header - default(0)
	BYTE	ColorMapType;		//color map included (0/1)
	BYTE	ImageType;			//color palette -1 , no palette - 2
	BYTE	ColorMapOriginLo;	//index of first palette entry
	BYTE	ColorMapOriginHi;
	BYTE	ColorMapLenLo;		//length of color palette
	BYTE	ColorMapLenHi;
	BYTE	ColorMapEntrySize;	//size of each palette entry 16/24/32
	BYTE	XOriginLo;			//origin of image - default(0)
	BYTE	XOriginHi;
	BYTE	YOriginLo;
	BYTE	YOriginHi;
	BYTE	WidthLo;			//Width
	BYTE	WidthHi;
	BYTE	HeightLo;			//Height
	BYTE	HeightHi;
	BYTE	ImagePixelSize;		//# of bits in each pixel, 8,16,24,32
	BYTE	ImageDiscriptor;	//# of bits in Alpha Channel (0,8)
} FTargaFileHeader;

UBOOL ExportBinary( UTexture* Texture, FArchive& Ar )
{
	guard(UTexture::ExportBinary);

	// Figure out format.
	ETextureFormat Format      = (ETextureFormat)Texture->Format;
	TArray<FMipmap>& TheseMips = Texture->Mips;

	// Make sure the Mip data is loaded
	Texture->Mips(0).DataArray.Load();

	INT RealUSize = Texture->USize;
	INT RealVSize = Texture->VSize;

	// File header.
	FTargaFileHeader hdr;
	hdr.IDFieldLen = 0;
	hdr.ColorMapType = 0;
	hdr.ImageType = 2;
	hdr.ColorMapOriginLo = 0;
	hdr.ColorMapOriginHi = 0;
	hdr.ColorMapLenLo = 0;
	hdr.ColorMapLenHi = 0;
	hdr.ColorMapEntrySize = 0;
	hdr.XOriginLo = 0;
	hdr.XOriginHi = 0;
	hdr.YOriginLo = 0;
	hdr.YOriginHi = 0;
	hdr.WidthLo = RealUSize & 0xFF;
	hdr.WidthHi = (RealUSize & 0xFF00) >> 8;
	hdr.HeightLo = RealVSize & 0xFF;
	hdr.HeightHi = (RealVSize & 0xFF00) >> 8;
	hdr.ImagePixelSize = 32;
	hdr.ImageDiscriptor = 0;

	Ar.Serialize(&hdr, sizeof(hdr));

	FMipmap& MipMap = TheseMips(0);
	// Write data.
	// Upside-down scanlines.
	for( INT i=Texture->VSize-1; i>=0; i-- )
	{
		for( INT j=0; j<Texture->USize; j++ )
		{
			DWORD PixelValue;
			if (Format == TEXF_RGBA8)
			{
				PixelValue = ((DWORD*)&MipMap.DataArray(0))[i*Texture->USize + j];

				// Turn off alpha for black portion of masked texture
				if ((Texture->PolyFlags & PF_Masked) && (PixelValue & 0xFFFFFF) == 0)
					PixelValue &= 0xFFFFFF;
			}
			else
			{
				BYTE PaletteIndex = MipMap.DataArray(i*Texture->USize + j);
				FColor& PaletteEntry = Texture->Palette->Colors(PaletteIndex);
				PixelValue = PaletteEntry.A << 24 | PaletteEntry.R << 16 | PaletteEntry.G << 8 | PaletteEntry.B;

				// Turn off alpha for palette element 0 of masked texture
				if (Texture->PolyFlags & PF_Masked)
				{
					if (PaletteIndex == 255 || PaletteIndex == 0) // let's see how we do
						PixelValue &= 0xFFFFFF;
					else
						PixelValue |= 0xFF000000;
				}
			}
			
			Ar.Serialize(&PixelValue, 4);
		}
	}
	return 1;
	unguard;
}


class UGCNConvertCommandlet : public UCommandlet
{
	DECLARE_CLASS(UGCNConvertCommandlet,UCommandlet,CLASS_Transient,GCNConvert);
	void StaticConstructor()
	{
		guard(UGCNConvertCommandlet::StaticConstructor);

		LogToStdout     = 0;
		IsClient        = 1;
		IsEditor        = 1;
		IsServer        = 1;
		LazyLoad        = 0;
		ShowErrorCount  = 1;

		unguard;
	}
	INT Main( const TCHAR* Parms )
	{
		guard(UGCNConvertCommandlet::Main);
		
		UClass* SoundClass = FindObjectChecked<UClass>( ANY_PACKAGE, TEXT("Sound") );
		UClass* TexClass = FindObjectChecked<UClass>( ANY_PACKAGE, TEXT("Texture") );
		UClass* StaticMeshClass = FindObjectChecked<UClass>( ANY_PACKAGE, TEXT("StaticMesh") );
		FString WildCard;


		GEditor = NULL;

		if( !ParseToken(Parms,WildCard,0) )
			appErrorf(TEXT("Package search mask not specified."));
		TArray<FString> FilesFound = GFileManager->FindFiles( *WildCard, 1, 0 );
		FString WCPath = WildCard;

		while( **WCPath && (*WCPath)[WCPath.Len()-1]!='\\' )
			WCPath = WCPath.Left(WCPath.Len()-1);

		GIsRequestingExit = 1; // Causes ctrl-c to immediately exit.

		for( INT i=0; i<FilesFound.Num(); i++ )
		{
			FString Pkg = WCPath+FilesFound(i);
			GWarn->Logf( TEXT("Package %s..."), *Pkg );

			UObject* Package = NULL;
			if (appStrstr(*Pkg, TEXT(".ut2")))
			{
				if (GEditor == NULL)
				{
					UClass* EditorEngineClass = UObject::StaticLoadClass( UEditorEngine::StaticClass(), NULL, TEXT("ini:Engine.Engine.EditorEngine"), NULL, LOAD_NoFail | LOAD_DisallowFiles, NULL );
					GEditor  = ConstructObject<UEditorEngine>( EditorEngineClass );
					GEditor->UseSound = 0;
					GEditor->Init();
				}

				GEditor->Exec( *FString::Printf(TEXT("MAP LOAD FILE=\"%s\""), *Pkg ) );
				Package = CastChecked<UPackage>(GEditor->Level->GetOuter());
			}
			else
			{
				Package = LoadPackage(NULL,*Pkg,LOAD_NoWarn);
			}

			if( Package )
			{
				ResetLoaders(NULL, 0, 1);
				UBOOL Dirty = false;

				for( TObjectIterator<UObject> It; It; ++It )
				{
					if( It->IsA(SoundClass) && It->IsIn(Package))
					{
						USound* Sound = (USound*)*It;
						Sound->Data.Load();
GWarn->Logf(TEXT("Exporting %s, Format = %s"), Sound->GetName(), *Sound->FileType);
						FWaveModInfo WavData;
						WavData.ReadWaveInfo( Sound->Data );

						// Check loop.
						INT SoundLoops = WavData.SampleLoopsNum;

						FArchive* WAVWriter = GFileManager->CreateFileWriter(TEXT("gcnconv.wav"));
						WAVWriter->Serialize(&Sound->Data(0), Sound->Data.Num());
						delete WAVWriter;

						char command[1024];
						
						if (*WavData.pChannels == 2)
						{
							sprintf(command, "sox gcnconv.wav gcnconv2.wav avg");
							system(command);
							unlink("gcnconv.wav");
							system("move gcnconv2.wav gcnconv.wav");
						}

						if (WavData.bitsPerSample == 2)
						{
							sprintf(command, "sox gcnconv.wav gcnconv2.wav avg");
							system(command);
							unlink("gcnconv.wav");
							system("move gcnconv2.wav gcnconv.wav");
						}

						// SL TODO: Should we use this loop flag we just found?
						if(SoundLoops)
							sprintf(command, "dspadpcm -e gcnconv.wav ..\\GCNSounds\\%s.dsp -l%d-%d", appToAnsi(Sound->GetName()), WavData.pSampleLoop->dwStart, WavData.pSampleLoop->dwEnd);
						else
							sprintf(command, "dspadpcm -e gcnconv.wav ..\\GCNSounds\\%s.dsp", appToAnsi(Sound->GetName()));
						system(command);
					}
					else if( It->IsA(StaticMeshClass) && It->IsIn(Package) )
					{
					}
					else if( It->IsA(TexClass) && It->IsIn(Package) )
					{
						UTexture* Texture = (UTexture*) *It;
						INT NumMips = Texture->Mips.Num();
						if (NumMips > 5) NumMips = 5;

						// make sure we haven't gone overboard
						INT NumRealMips = 0;
						INT Size = Min(Texture->Mips(0).USize, Texture->Mips(0).VSize);
						while (NumRealMips < NumMips)
						{
							Size /= 2;
							NumRealMips++;
							if (Size <= 8)
								break;
						}
						NumMips = NumRealMips;

						// SAVE OUT A PCX WITH EVERYTHING
						FArchive* TGAWriter = GFileManager->CreateFileWriter(TEXT("gcnconv.tga"));
						if (ExportBinary(Texture, *TGAWriter) == 0)
						{
							GWarn->Logf(TEXT("Bad tga export!"));
						}
						delete TGAWriter;
							
						// Shell out to TEXCONV (found in System directory, along with a premade script file)
						GWarn->Logf(TEXT("Converting %s.tpl...(%d x %d x %d mips)"), Texture->GetName(), Texture->Mips(0).USize, Texture->Mips(0).VSize, NumMips);
						char command[1024];
						sprintf(command, "texconv gcnconv%d.scr ..\\GCNTextures\\%s.tpl", NumMips, appToAnsi(Texture->GetName()));
						system(command);

//						GWarn->Logf(appFromAnsi(command));
					}
				}
				if (Dirty)
				{
					GWarn->Logf(TEXT("Saving..."));
					if (appStrstr(*Pkg, TEXT(".ut2")))
					{
						// Save the updated map
						GEditor->Exec( *FString::Printf(TEXT("MAP SAVE FILE=\"%s\""), *Pkg ) );
					}
					else
						SavePackage( Package, NULL, RF_Standalone, *Pkg, GError, NULL );
				}
//				ResetLoaders(Package, 0, 1);
				CollectGarbage(RF_Native | RF_Standalone);
			}
		}
		GIsRequestingExit = 1;
		return 0;
		unguard;
	}
};
IMPLEMENT_CLASS(UGCNConvertCommandlet)


/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

