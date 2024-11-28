/*=============================================================================
	CDKeyGen.cpp: CD key generator commandlet
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/


#include "MasterServer.h"
#include "MasterServerLink.h"

/*-----------------------------------------------------------------------------
	GenerateCDKey
-----------------------------------------------------------------------------*/

FString GenerateCDKey( QWORD Seed, const TCHAR* AppendString = CDKEYAPPENDSTRING_UT2004 )
{
	FString Key = HexToCDKeyMap(appQtoa(Seed, CDKEYBASE));
	while( Key.Len() < 14 )
		Key = FString::Printf( TEXT("%c%s"), CDKEYBASEMAP[0], *Key );
	check( Key.Len() == 14 );

	// generate check characters
	FString Check = FString::Printf(TEXT("%I64d%s"), Seed, AppendString);
	const ANSICHAR* AnsiCheck = appToAnsi(*Check);
	FCdKeyMD5Qword md5;

	FMD5Context Context;
	appMD5Init( &Context );
	appMD5Update( &Context, (unsigned char*)AnsiCheck, Check.Len() );
	appMD5Final( md5.Digest, &Context );

	FString CheckOutput = HexToCDKeyMap(appQtoa(md5.Q,CDKEYBASE));
	CheckOutput = CheckOutput.Left(6);
	while( CheckOutput.Len() < 6 )
		CheckOutput = FString::Printf( TEXT("%c%s"), CDKEYBASEMAP[0], *CheckOutput );

	FString FullKey = Key + CheckOutput;

	check( FullKey.Len() == 20 );

	FString FinalKey = FString::Printf(TEXT("%s-%s-%s-%s"), *FullKey.Mid(10,5), *FullKey.Mid(5,5), *FullKey.Mid(0,5), *FullKey.Mid(15,5) );

	check( ValidateCDKey( *FinalKey, AppendString ) );
	return FinalKey;
}


/*-----------------------------------------------------------------------------
	UCDKeyGenCommandlet
-----------------------------------------------------------------------------*/

class UCDKeyGenCommandlet : public UCommandlet
{
	DECLARE_CLASS(UCDKeyGenCommandlet,UCommandlet,CLASS_Transient,MasterServer);
	void StaticConstructor()
	{
		guard(UCDKeyGenCommandlet::StaticConstructor);

		LogToStdout     = 0;
		IsClient        = 1;
		IsEditor        = 1;
		IsServer        = 1;
		LazyLoad        = 1;
		ShowErrorCount  = 1;
		unguard;
	}

	INT Main( const TCHAR* Parms )
	{
		guard(UCDKeyGenCommandlet::Main);

		FString CountStr;
		ParseToken(Parms,CountStr,0);

		INT Count = appAtoi( *CountStr );
		if( Count <= 0 )
			appErrorf(TEXT("You must specify the number of CD keys to generate"));

		FString Product;
		ParseToken(Parms,Product,0);
		Product = Product.Caps();
		if( Product != TEXT("UT2003") && Product != TEXT("UT2004") )
			appErrorf(TEXT("You must specify either UT2003 or UT2004 for the key type to generate"));

		FString Comment = ++Parms;
		if( !Comment.Len() )
			appErrorf(TEXT("You must specify a comment for these CD keys"));

		INT BatchID = 0;
		FMySQL MySQL;

		if( Comment.Caps() != TEXT("NODB") )
		{
			FString DBServer;
			if( !GConfig->GetString( TEXT("MasterServer"), TEXT("DBServer"), DBServer, TEXT("masterserver.ini") ) )
				appErrorf(TEXT("Missing DBServer in INI"));
			appStrcpy( GDatabaseServer, *DBServer );

			if( !MySQL.Connect( GDatabaseServer, GDatabaseUser, GDatabasePass, MASTER_SERVER_DATABASE ) )
				appErrorf(TEXT("Database connect failed"));

			MySQL.DoSQL( "insert into cdkeybatch (created, description, count) values (NOW(), '%s', '%d')", MySQL.FormatSQL(*Comment), Count );
			BatchID = MySQL.GetInsertID();
		}
		else
			GWarn->Logf(TEXT("NOT WRITING TO DATABASE"));

        GWarn->Logf( TEXT("Generating %d %s CD keys for '%s'"), Count, *Product, *Comment );
		GWarn->Logf( TEXT("-----") );
		for( INT i=1;i<=Count;i++ )
		{
			FString CDKey = GenerateCDKey( ((QWORD)(appCycles())<<56)^((QWORD)(appCycles())<<32)^(QWORD)(appCycles()) ^ (QWORD)(appRand()) ^ ((QWORD)(appRand())<<32) ^ ((QWORD)(appRand())<<56) | 0x8000000000000000, Product==TEXT("UT2004") ? CDKEYAPPENDSTRING_UT2004 : CDKEYAPPENDSTRING_UT2003 );
			if( Comment.Caps() != TEXT("NODB") )
			{
				MySQL.DoSQL( "insert into cdkey(md5hash, cdkey, batchid, product) values (MD5('%s'), '%s', '%d', '%d')", MySQL.FormatSQL(*CDKey), MySQL.FormatSQL(*CDKey), BatchID, Product==TEXT("UT2004") ? 1 : 0 );
				INT KeyID = MySQL.GetInsertID();
				//!! OLDVER include cdkey with spaces instead for UT2003.
				if( Product==TEXT("UT2003") )
					MySQL.DoSQL( "insert into cdkeyspaces(cdkeyid, md5hashsp) values (%d, MD5(REPLACE('%s', '-', ' ')))", KeyID, MySQL.FormatSQL(*CDKey) );
			}
			GWarn->Logf( TEXT("%s"), *CDKey );
		}
		GWarn->Logf( TEXT("-----") );

		return 0;
		unguard;
	}
};

IMPLEMENT_CLASS(UCDKeyGenCommandlet);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

