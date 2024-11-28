/*=============================================================================
	FFileManagerLinux.h: Unreal Linux based file manager.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
		* Major changes by Daniel Vogel
=============================================================================*/

#ifndef _INCL_FFILEMANAGERLINUX_H_
#define _INCL_FFILEMANAGERLINUX_H_

#include <dirent.h>
#include <unistd.h>
#include "FFileManagerGeneric.h"
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "UnGnuG.h"

#include <sys/types.h>
#include <sys/stat.h>


// !!! FIXME: This should really be renamed "Unix" from "Linux". --ryan.


/*-----------------------------------------------------------------------------
	File Manager.
-----------------------------------------------------------------------------*/

// File manager.
class FArchiveFileReader : public FArchive
{
public:
	FArchiveFileReader( FILE* InFile, FOutputDevice* InError, INT InSize )
	:	File			( InFile )
	,	Error			( InError )
	,	Size			( InSize )
	,	Pos				( 0 )
	,	BufferBase		( 0 )
	,	BufferCount		( 0 )
	{
		guard(FArchiveFileReader::FArchiveFileReader);
		fseek( File, 0, SEEK_SET );
		ArIsLoading = ArIsPersistent = 1;
		unguard;
	}
	~FArchiveFileReader()
	{
		guard(FArchiveFileReader::~FArchiveFileReader);
		if( File )
			Close();
		unguard;
	}
	void Precache( INT HintCount )
	{
		guardSlow(FArchiveFileReader::Precache);
		checkSlow(Pos==BufferBase+BufferCount);
		BufferBase = Pos;
		BufferCount = Min( Min( HintCount, (INT)(ARRAY_COUNT(Buffer) - (Pos&(ARRAY_COUNT(Buffer)-1))) ), Size-Pos );
		DOUBLE	StartTime	= appSeconds();
		if( fread( Buffer, BufferCount, 1, File )!=1 && BufferCount!=0 )
		{
			ArIsError = 1;
			Error->Logf( TEXT("fread failed: BufferCount=%i Error=%i"), BufferCount, ferror(File) );
			return;
		}
		FLOAT	DeltaTime	= (FLOAT) (appSeconds() - StartTime);
		if( DeltaTime > 100.f ) DeltaTime = 0.0f; //!!
		GFileManagerBytesRead+=BufferCount; GFileManagerLoadingTime+=DeltaTime;
		unguardSlow;
	}
	void Seek( INT InPos )
	{
		guard(FArchiveFileReader::Seek);
		check(InPos>=0);
		check(InPos<=Size);
		DOUBLE StartTime = appSeconds();
		if( fseek(File,InPos,SEEK_SET) )
		{
			ArIsError = 1;
			Error->Logf( TEXT("seek Failed %i/%i: %i %i"), InPos, Size, Pos, ferror(File) );
		}
		DOUBLE DeltaTime = appSeconds()-StartTime;
		if( DeltaTime < 100.f )
			GFileManagerSeekTime   += DeltaTime;
		Pos         = InPos;
		BufferBase  = Pos;
		BufferCount = 0;
		unguard;
	}
	INT Tell()
	{
		return Pos;
	}
	INT TotalSize()
	{
		return Size;
	}
	UBOOL Close()
	{
		guardSlow(FArchiveFileReader::Close);
		if( File )
			fclose( File );
		File = NULL;
		return !ArIsError;
		unguardSlow;
	}
	void Serialize( void* V, INT Length )
	{
		guardSlow(FArchiveFileReader::Serialize);
		while( Length>0 )
		{
			INT Copy = Min( Length, BufferBase+BufferCount-Pos );
			if( Copy==0 )
			{
				if( Length >= ARRAY_COUNT(Buffer) )
				{
					DOUBLE	StartTime	= appSeconds();
					if( fread( V, Length, 1, File )!=1 )
					{
						ArIsError = 1;
						Error->Logf( TEXT("fread failed: Length=%i Error=%i"), Length, ferror(File) );
					}
					FLOAT	DeltaTime	= appSeconds()-StartTime;
					if( DeltaTime > 100.f ) DeltaTime = 0; //!!
					GFileManagerBytesRead+=Length; GFileManagerLoadingTime+=DeltaTime;
					Pos += Length;
					BufferBase += Length;
					return;
				}
				Precache( MAXINT );
				Copy = Min( Length, BufferBase+BufferCount-Pos );
				if( Copy<=0 )
				{
					ArIsError = 1;
					Error->Logf( TEXT("ReadFile beyond EOF %i+%i/%i"), Pos, Length, Size );
				}
				if( ArIsError )
					return;
			}
			appMemcpy( V, Buffer+Pos-BufferBase, Copy );
			Pos       += Copy;
			Length    -= Copy;
			V          = (BYTE*)V + Copy;
		}
		unguardSlow;
	}
protected:
	FILE*			File;
	FOutputDevice*	Error;
	INT				Size;
	INT				Pos;
	INT				BufferBase;
	INT				BufferCount;
	BYTE			Buffer[1024];
};
class FArchiveFileWriter : public FArchive
{
public:
	FArchiveFileWriter( FILE* InFile, FOutputDevice* InError )
	:	File		( InFile )
	,	Error		( InError )
	,	Pos		(0)
	,	BufferCount	(0)
	{
		ArIsSaving = ArIsPersistent = 1;
	}
	~FArchiveFileWriter()
	{
		guard(FArchiveFileWriter::~FArchiveFileWriter);
		if( File )
			Close();
		File = NULL;
		unguard;
	}
	void Seek( INT InPos )
	{
		Flush();
		if( fseek(File,InPos,SEEK_SET) )
		{
			ArIsError = 1;
			Error->Logf( LocalizeError("SeekFailed",TEXT("Core")) );
		}
		Pos = InPos;
	}
	INT Tell()
	{
		return Pos;
	}
	UBOOL Close()
	{
		guardSlow(FArchiveFileWriter::Close);
		Flush();
		if( File && fclose( File ) )
		{
			ArIsError = 1;
			Error->Logf( LocalizeError("WriteFailed",TEXT("Core")) );
		}
		File = NULL;
		return !ArIsError;
		unguardSlow;
	}
	void Serialize( void* V, INT Length )
	{
		Pos += Length;
		INT Copy;
		while( Length > (Copy=ARRAY_COUNT(Buffer)-BufferCount) )
		{
			appMemcpy( Buffer+BufferCount, V, Copy );
			BufferCount += Copy;
			Length      -= Copy;
			V            = (BYTE*)V + Copy;
			Flush();
		}
		if( Length )
		{
			appMemcpy( Buffer+BufferCount, V, Length );
			BufferCount += Length;
		}
	}
	void Flush()
	{
		if( BufferCount && fwrite( Buffer, BufferCount, 1, File )!=1 )
		{
			ArIsError = 1;
			Error->Logf( LocalizeError("WriteFailed",TEXT("Core")) );
		}
		BufferCount=0;
	}
protected:
	FILE*			File;
	FOutputDevice*	Error;
	INT				Pos;
	INT				BufferCount;
	BYTE			Buffer[4096];
};

class FFileManagerLinux : public FFileManagerGeneric
{
public:
    FFileManagerLinux(void)
    {
        BaseDir[0] = 0;
    }

    FILE *fopenReadRespectHomeDir(const TCHAR *FakeFilename)
    {
        char ansipath[PATH_MAX];
        TCHAR tmp[PATH_MAX];
		FILE* File = NULL;

		if ( BaseDir[0] )   // Use home dir?
        {
    		appStrcpy( tmp, BaseDir );
            appStrcat( tmp, FakeFilename );
            unixToANSI( ansipath, tmp );
    		char* Filename = appUnixPath(ansipath);
		    File = fopen(Filename, "rb");
        }

        if ( File == NULL )
        {
            unixToANSI( ansipath, FakeFilename );
            char *Filename = appUnixPath(ansipath);
			File = fopen(Filename, "rb");
        }
        return(File);
    }

	UBOOL GetFileLastModTime( const TCHAR* FakeFilename, DWORD &Hours, DWORD &Minutes, DWORD &Secs, DWORD &Day, DWORD &Month, DWORD &Year )
	{
        FILE* File = fopenReadRespectHomeDir(FakeFilename);
		if ( !File )
			return 0;

		struct stat statbuf;
        int rc = fstat(fileno(File), &statbuf);
		fclose(File);
        if (rc == -1)
            return 0;

        struct tm tm;
        if (localtime_r(&statbuf.st_mtime, &tm) == NULL)
            return 0;

        Hours = (DWORD) (tm.tm_hour);  // 0 - 23
        Minutes = (DWORD) (tm.tm_min);  // 0 - 59
        Secs = (DWORD) (tm.tm_sec);  // 0 - 59, may be 60 in rare occasions.
        Day = (DWORD) (tm.tm_mday);  // 1 - 31
        Month = (DWORD) (tm.tm_mon + 1);   // 1 - 12
        Year = (DWORD) (tm.tm_year + 1900); // actual year, not offset.

        if (Secs > 59) Secs = 59;  // Pesky leap seconds!

        return 1;
	}

	FArchive* CreateFileReader( const TCHAR* FakeFilename, DWORD Flags, FOutputDevice* Error )
	{
		guard(FFileManagerLinux::CreateFileReader);		

        if ( (FakeFilename == NULL) ||
             (appStrlen(FakeFilename) + appStrlen(BaseDir) >= PATH_MAX) )
            return NULL;

        FILE* File = fopenReadRespectHomeDir(FakeFilename);
		if ( !File )
		{
			if( Flags & FILEREAD_NoFail )
				appErrorf(TEXT("Failed to read file: %s"),FakeFilename);
			return NULL;
		}
		fseek( File, 0, SEEK_END );

		return new(TEXT("LinuxFileReader"))FArchiveFileReader(File,Error,ftell(File));
		unguard;
	}
	FArchive* CreateFileWriter( const TCHAR* FakeFilename, DWORD Flags, FOutputDevice* Error )
	{	
		guard(FFileManagerLinux::CreateFileWriter);

        if ( (FakeFilename == NULL) ||
             (appStrlen(FakeFilename) + appStrlen(BaseDir) >= PATH_MAX) )
            return NULL;

        char ansipath[PATH_MAX];
        TCHAR tmp[PATH_MAX];

		appStrcpy( tmp, BaseDir );
        appStrcat( tmp, FakeFilename );
        unixToANSI( ansipath, tmp );
		char* Filename = appUnixPath(ansipath);

        if (BaseDir[0])  // might need to build dirs in $HOME...
        {
            char ansitmp[PATH_MAX];
            strcpy(ansitmp, Filename);
            char *ptr = strrchr(ansitmp, '/');
            if (ptr != NULL)
            {
                for (ptr = ansitmp; *ptr; ptr++)  // build each chunk.
                {
                    if (*ptr == '/')
                    {
                        *ptr = '\0';
                        // don't care if it fails, since fopen() will tell us.
                        mkdir(ansitmp, S_IREAD | S_IWRITE | S_IEXEC);
                        *ptr = '/';
                    }
                }
            }
        }

	    if( Flags & FILEWRITE_EvenIfReadOnly )
			chmod(Filename, S_IREAD | S_IWRITE);
		if( (Flags & FILEWRITE_NoReplaceExisting) && (access(Filename, F_OK) != -1))
			return NULL;
		const char* Mode = (Flags & FILEWRITE_Append) ? "ab" : "wb";

		FILE* File = fopen(Filename, Mode);
		if( !File )
		{
			if( Flags & FILEWRITE_NoFail )
				appErrorf( TEXT("Failed to write: %s"), &Filename );
			return NULL;
		}
		if( Flags & FILEWRITE_Unbuffered )
			setvbuf( File, 0, _IONBF, 0 );
		return new(TEXT("LinuxFileWriter"))FArchiveFileWriter(File,Error);

		unguard;
	}
	UBOOL Delete( const TCHAR* FakeFilename, UBOOL RequireExists=0, UBOOL EvenReadOnly=0 )
	{	       
		guard(FFileManagerLinux::Delete);

        if ( (FakeFilename == NULL) ||
             (appStrlen(FakeFilename) + appStrlen(BaseDir) >= PATH_MAX) )
            return 0;

        char ansipath[PATH_MAX];
        TCHAR tmp[PATH_MAX];

		appStrcpy( tmp, BaseDir );
        appStrcat( tmp, FakeFilename );
        unixToANSI( ansipath, tmp );
		char* Filename = appUnixPath(ansipath);
		if( EvenReadOnly )
			chmod(Filename, S_IREAD | S_IWRITE);
		return(unlink(Filename)==0 || (errno==ENOENT && !RequireExists));
		unguard;
	}
    // gam ---
    UBOOL IsReadOnly( const TCHAR* Filename )
    {
		guard(FFileManagerLinux::IsReadOnly);

        if ( (Filename == NULL) ||
             (appStrlen(Filename) >= PATH_MAX) )
            return 0;

        if( FileSize( Filename ) < 0 )
            return( 0 );

        char ansipath[PATH_MAX];
        unixToANSI(ansipath, Filename);
		return (access(appUnixPath(ansipath), W_OK) != 0);
		unguard;
    }
    // --- gam
	SQWORD GetGlobalTime( const TCHAR* Filename )
	{
		guard(FFileManagerLinux::GetGlobalTime);
		return 0;
		unguard;
	}
	UBOOL SetGlobalTime( const TCHAR* Filename )
	{
		guard(FFileManagerLinux::SetGlobalTime);
		return 0;	
		unguard;
	}
	UBOOL MakeDirectory( const TCHAR* FakePath, UBOOL Tree=0 )
	{
		guard(FFileManagerLinux::MakeDirectory);

        if ( (FakePath == NULL) ||
             (appStrlen(FakePath) + appStrlen(BaseDir) >= PATH_MAX) )
            return 0;

        char ansipath[PATH_MAX];
        TCHAR tmp[PATH_MAX];

		appStrcpy( tmp, BaseDir );
		TCHAR* Path = appStrcat( tmp, FakePath );
		if( Tree )
			return FFileManagerGeneric::MakeDirectory( Path, Tree );

        unixToANSI(ansipath, tmp);
		return mkdir(appUnixPath(ansipath), S_IREAD | S_IWRITE | S_IEXEC)==0 || errno==EEXIST;
		unguard;
	}
	UBOOL DeleteDirectory( const TCHAR* FakePath, UBOOL RequireExists=0, UBOOL Tree=0 )
	{
		guard(FFileManagerLinux::DeleteDirectory);

        if ( (FakePath == NULL) ||
             (appStrlen(FakePath) + appStrlen(BaseDir) >= PATH_MAX) )
            return 0;

        char ansipath[PATH_MAX];
        TCHAR tmp[PATH_MAX];

		appStrcpy( tmp, BaseDir );	
		TCHAR* Path = appStrcat( tmp, FakePath );	
		if( Tree )
			return FFileManagerGeneric::DeleteDirectory( Path, RequireExists, Tree );

        unixToANSI(ansipath, tmp);

		return rmdir(appUnixPath(ansipath))==0 || (errno==ENOENT && !RequireExists);
		unguard;
	}
	TArray<FString> FindFiles( const TCHAR* FakeFilename, UBOOL Files, UBOOL Directories )
	{
		guard(FFileManagerLinux::FindFiles);
		TArray<FString> Result;

		if ( BaseDir[0] )
        {
    		TCHAR* tmp = appStaticString1024();
    		appStrcpy( tmp, BaseDir );
		    TCHAR* Filename = appStrcat( tmp, FakeFilename );
			SearchDirectory( Result, Filename );
        }
		SearchDirectory( Result, FakeFilename );

		return Result;
		unguard;
	}
	UBOOL SetDefaultDirectory( const TCHAR* Filename )
	{
		guard(FFileManagerLinux::SetDefaultDirectory);

        if ( (Filename == NULL) ||
             (appStrlen(Filename) >= PATH_MAX) )
            return 0;

        char ansipath[PATH_MAX];
        unixToANSI(ansipath, Filename);

		return chdir(appUnixPath(ansipath))==0;
		unguard;
	}
	FString GetDefaultDirectory()
	{
		guard(FFileManagerLinux::GetDefaultDirectory);
		{
			ANSICHAR Buffer[PATH_MAX]="";
			getcwd( Buffer, ARRAY_COUNT(Buffer) );
			return appFromAnsi( Buffer );
		}
		unguard;
	}
	// gam ---
	INT CompareFileTimes( const TCHAR* FileA, const TCHAR* FileB )
	{
		guard(FFileManagerLinux::CompareFileTimes);
	
		struct stat StatA, StatB;

		if( stat( appToAnsi( FileA ), &StatA ) != 0 )
		    return 0;

		if( stat( appToAnsi( FileB ), &StatB ) != 0 )
		    return 0;
		    
        return( INT(StatB.st_mtime - StatA.st_mtime) );

        unguard;
	}
	// --- gam
	void Init(UBOOL Startup) 
	{
        if ((ParseParam(appCmdLine(),TEXT("nohomedir"))))
            BaseDir[0] = 0;
        else
            appStrcpy(BaseDir, CalcHomeDir());
	}

    const TCHAR *CalcHomeDir(void)
    {
        if (ParseParam(appCmdLine(),TEXT("nohomedir")))
            return(appBaseDir());

	    char *Home = getenv("HOME");
    	if ( Home == NULL )
    	{
    	    uid_t id = getuid();
    		struct passwd *pwd;
    
    		setpwent();
    		while( (pwd = getpwent()) != NULL )
    		{
    		    if( pwd->pw_uid == id )
    			{
    			    Home = pwd->pw_dir;
    				break;
    			}
    		}
    		endpwent();
    	}

    	TCHAR* tmp = appStaticString1024();
    	appStrcpy( tmp, appFromAnsi(Home) );

	// !!! FIXME: abstract game.
	#if MACOSX
		// Make sure these two parent folders exist...
		appStrcat( tmp, TEXT("/Library") );
		mkdir(appToAnsi(tmp), S_IRWXU);
		appStrcat( tmp, TEXT("/Application Support") );
		mkdir(appToAnsi(tmp), S_IRWXU);

		#if DEMOVERSION
			appStrcat( tmp, TEXT("/Unreal Tournament 2004 Demo") );
		#else
			appStrcat( tmp, TEXT("/Unreal Tournament 2004") );
		#endif
	#else
		#if DEMOVERSION
			appStrcat( tmp, TEXT("/.ut2004demo") );
		#else
			appStrcat( tmp, TEXT("/.ut2004") );
		#endif
	#endif

        mkdir(appToAnsi(tmp), S_IRWXU);
        appStrcat( tmp, TEXT("/System") );
        mkdir(appToAnsi(tmp), S_IRWXU);

        // If STILL not accessible don't use it.
        if ( access( appToAnsi(tmp), F_OK ) == -1 )
        {
            //debugf(TEXT("can't find homedir at [%s]."), homedir);
        }
        else
        {
            appStrcat( tmp, TEXT("/") );
        }

        return(tmp);
    }


protected:
	void SearchDirectory( TArray<FString> &Result, const TCHAR* Filename )
	{
		guard(FFileManagerLinux::SearchDirectory);
		DIR *Dirp;
		struct dirent* Direntp;
		char Path[PATH_MAX];
		char File[PATH_MAX];
		char *Filestart;
		char *Cur;
		UBOOL Match;

		// Initialize Path to Filename.
        unixToANSI( File, Filename );
		strcpy( Path, appUnixPath(File) );

        /* Do this (and case sensitivity check) in appUnixPath, above. --ryan.
		// Convert MS "\" to Unix "/".
		for( Cur = Path; *Cur != '\0'; Cur++ )
			if( *Cur == '\\' )
				*Cur = '/';
        */
	
		// Separate path and filename.
		Filestart = Path;
		for( Cur = Path; *Cur != '\0'; Cur++ )
        {
			if( *Cur == '/' )
				Filestart = Cur + 1;
        }

		// Store filename and remove it from Path.
		strcpy( File, Filestart );
		*Filestart = '\0';

		// Check for empty path.
		if (strlen( Path ) == 0)
			sprintf( Path, "./" );

		// Open directory, get first entry.
		Dirp = opendir( Path );
		if (Dirp == NULL)
			return;
	
		Direntp = readdir( Dirp );

		// Check each entry.
		while( Direntp != NULL )
		{
			Match = false;

			if( strcmp( File, "*" ) == 0 )
			{
				// Any filename.
				Match = true;
			}
			else if( strcmp( File, "*.*" ) == 0 )
			{
				// Any filename with a '.'.
				if( strchr( Direntp->d_name, '.' ) != NULL )
					Match = true;
			}
			else if( File[0] == '*' )
			{
				// "*.ext" filename.
				if( strstr( Direntp->d_name, (File + 1) ) != NULL )
					Match = true;
			}
			else if( File[strlen( File ) - 1] == '*' )
			{
				// "name.*" filename.
				if( strncmp( Direntp->d_name, File, strlen( File ) - 1 ) == 0 )
					Match = true;
			}
			else if( strstr( File, "*" ) != NULL )
			{
				// single str.*.str match.
				char* star = strstr( File, "*" );
				INT filelen = strlen( File );
				INT starlen = strlen( star );
				INT starpos = filelen - (starlen - 1);
				char prefix[256];
				strncpy( prefix, File, starpos );
				star++;
				if( strncmp( Direntp->d_name, prefix, starpos - 1 ) == 0 )
				{
					// part before * matches
					char* postfix = Direntp->d_name + (strlen(Direntp->d_name) - starlen) + 1;
					if ( strcmp( postfix, star ) == 0 )
						Match = true;
				}
			}
			else
			{
				// Literal filename.
				if( strcmp( Direntp->d_name, File ) == 0 )
					Match = true;
			}

			// Does this entry match the Filename?
			if( Match )
			{
				// Yes, add the file name to Result.
				new(Result)FString(Direntp->d_name);
			}
		
			// Get next entry.
			Direntp = readdir( Dirp );
		}
	
		// Close directory.
		closedir( Dirp );
		unguard;
	}

	TCHAR BaseDir[PATH_MAX];
};

#endif // include-once blocker.

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

