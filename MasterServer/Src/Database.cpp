/*=============================================================================
	Database.cpp: SQL database interface
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

#include "MasterServer.h"

/*-----------------------------------------------------------------------------
	FDatabase implementation
-----------------------------------------------------------------------------*/
FDatabase::~FDatabase()
{
	guard(FDatabase::~FDatabase);
	while( Queries.Num() )
		delete Queries(0);
	unguard;
}

//
// FQueryResult implementation
//
FQueryResult::FQueryResult( FDatabase* InDatabase )
:	Database(InDatabase)
{
    Database->Queries.AddItem(this);
}
FQueryResult::~FQueryResult()
{
    Database->Queries.RemoveItem(this);
}

FQueryField* FQueryResult::FieldByName(TCHAR* FieldName)
{
	guard(FQueryResult::FieldByName);
	for( INT i=0;i<Fields.Num();i++ )
		if( !appStrcmp(*Fields(i)->FieldName, FieldName) )
			return Fields(i);
	return NULL;
	unguard;
}

/*-----------------------------------------------------------------------------
	FMySQL implementation
-----------------------------------------------------------------------------*/

FMySQL::FMySQL()
{
	guard(FMySQL::FMySQL);
	mysql_init( &mysql );
	Connected = 0;
	CurrentTempString = 0;
	unguard;
}

FMySQL::~FMySQL()
{
	guard(FMySQL::~FMySQL);
	if( Connected )
		mysql_close(&mysql);
	unguard;
}

#if MYSQL_STATS
struct FOutputStatItem
{
	FString SQL;
	FMySQLQueryStatItem Stat;
};

INT Compare( FOutputStatItem& A, FOutputStatItem& B )
{
	if( A.Stat.Time < B.Stat.Time )
		return -1;
	else
		return 1;
}
#endif

void FMySQL::MySQLDumpQueryStats()
{
#if MYSQL_STATS
	guard(FMySQL::MySQLDumpQueryStats);
	TArray<FOutputStatItem> Items;
	for( TMap<FString,FMySQLQueryStatItem>::TIterator It(QueryStats); It; ++It )
	{
		INT i = Items.AddZeroed();
		Items(i).SQL  = It.Key().Left(60);
		Items(i).Stat = It.Value();
	}

	if( Items.Num() )
	{
		Sort( &Items(0), Items.Num() );

		DOUBLE TotalTime = 0.f;
		GWarn->Logf(TEXT(" Num #Rows   Tm SQL "));
		for( INT i=0;i<Items.Num();i++ )
		{
			GWarn->Logf(TEXT("%4d %5d %3.2f %-60s"), Items(i).Stat.Count, Items(i).Stat.RowsAffected, Items(i).Stat.Time, *Items(i).SQL );
			TotalTime += Items(i).Stat.Time;
		}
		GWarn->Logf(TEXT("         %5.2f"), TotalTime );
		QueryStats.Empty();
	}
	unguard;
#endif
}

void FMySQL::MySQLWarning( TCHAR* Message )
{
	GWarn->Logf( TEXT("%s: %s"), Message, appFromAnsi(mysql_error(&mysql), GetTempString(1024)) );
}

void FMySQL::MySQLError( TCHAR* Message )
{
	guard(FMySQL::MySQLError);
	appErrorf( TEXT("%s: %s"), Message, appFromAnsi(mysql_error(&mysql), GetTempString(1024)) );
	unguard;
}

UBOOL FMySQL::Connect( const TCHAR* InHost, const TCHAR* InUserName, const TCHAR* InPassword, const TCHAR* InDatabase, UBOOL Delayed )
{
	guard(FMySQL::Connect);
	check(!Connected);

	if( Delayed )
	{
		DelayedHost		= InHost;
		DelayedUserName	= InUserName;
		DelayedPassword	= InPassword;
		DelayedDatabase	= InDatabase;
	}
	else
	{
		if (GExtendedLogging)
			GWarn->Logf(TEXT("Connecting to database %s:%s"), InHost, InDatabase );

		if( !mysql_real_connect( &mysql, appToAnsi(InHost, GetTempAnsiString(1024)), appToAnsi(InUserName, GetTempAnsiString(1024)), appToAnsi(InPassword, GetTempAnsiString(1024)), appToAnsi(InDatabase, GetTempAnsiString(1024)), 0, NULL, 0 ) )
		{
			MySQLWarning( TEXT("mysql_real_connect failed") );
			return 0;
		}
		Connected = 1;
	}

	return 1;
	unguard;
}

FQueryResult* FMySQL::Query( const ANSICHAR* SQL, ... )
{
	guard(FMySQL::Query);
	ANSICHAR* TempStr = GetTempAnsiString(4096);
	GET_VARARGS_ANSI( TempStr, 4096, SQL, SQL );

	if( !Connected )
		Connect( *DelayedHost, *DelayedUserName, *DelayedPassword, *DelayedDatabase, 0 );

#if MYSQL_STATS
	FString TempSQL = appFromAnsi(SQL, GetTempString(4096));
	if( TempSQL.InStr(TEXT("%")) == -1 )
	{
		INT i = TempSQL.InStr(TEXT("="));
		if( i != -1 )
			TempSQL = TempSQL.Left(i);
	}
	FMySQLQueryStatItem* StatItem = QueryStats.Find(*TempSQL);
	if( !StatItem )
		StatItem = &QueryStats.Set( *TempSQL, FMySQLQueryStatItem() );
	DOUBLE QueryStartTime = appSeconds();
#endif

	if( mysql_real_query( &mysql, TempStr, ::strlen(TempStr) ) )
	{
		MySQLWarning(TEXT("MySQL::Query query failed"));
		// display failed query in 512 char bits
		FString Query = appFromAnsi(TempStr, GetTempString(4096));
		while( Query.Len() > 75 )
		{
			GWarn->Logf(TEXT(">>%s<<"), *Query.Left(75) );
			Query = Query.Mid(75);
		}
		GWarn->Logf(TEXT(">>%s<<"), *Query);
		return NULL;
	}
	
	MYSQL_RES* res = mysql_store_result( &mysql );
	if( !res )
	{
		if( mysql_errno(&mysql) != 0 )
			MySQLError( TEXT("mysql_store_result failed") );
		return NULL;
	}

	FMySQLQueryResult* Result = new FMySQLQueryResult( this, res );

#if MYSQL_STATS
	StatItem->Count++;
	StatItem->Time += appSeconds() - QueryStartTime;
	StatItem->RowsAffected += Result->NumRows();
#endif

	return Result;
	unguard;
}

void FMySQL::DoSQL( const ANSICHAR* SQL, ... )
{
	guard(FMySQL::DoSQL);
	ANSICHAR* TempStr = GetTempAnsiString(4096);
	GET_VARARGS_ANSI( TempStr, 4096, SQL, SQL );

	if( !Connected )
		Connect( *DelayedHost, *DelayedUserName, *DelayedPassword, *DelayedDatabase, 0 );

#if MYSQL_STATS
	FString TempSQL = appFromAnsi(SQL, GetTempString(4096));
	if( TempSQL.InStr(TEXT("%")) == -1 )
	{
		INT i = TempSQL.InStr(TEXT("="));
		if( i != -1 )
			TempSQL = TempSQL.Left(i);
		i = TempSQL.InStr(TEXT("values"));
		if( i != -1 )
			TempSQL = TempSQL.Left(i);
	}
	FMySQLQueryStatItem* StatItem = QueryStats.Find(*TempSQL);
	if( !StatItem )
		StatItem = &QueryStats.Set( *TempSQL, FMySQLQueryStatItem() );
	DOUBLE QueryStartTime = appSeconds();
#endif

	if( mysql_real_query( &mysql, TempStr, ::strlen(TempStr) ) )
	{
		MySQLWarning(TEXT("MySQL::DoSQL failed"));
		// display failed query in 75 char bits
		FString Query = appFromAnsi(TempStr, GetTempString(4096));
		while( Query.Len() > 75 )
		{
			GWarn->Logf(TEXT(">>%s<<"), *Query.Left(75) );
			Query = Query.Mid(75);
		}
		GWarn->Logf(TEXT(">>%s<<"), *Query);
		return;
	}

	MYSQL_RES* res = mysql_store_result( &mysql );
	if( res )
	{
		MySQLError( TEXT("FMySQL::DoSQL: result set unexpected") );
		mysql_free_result(res);
		return;
	}
	if( mysql_errno(&mysql) != 0 )
		MySQLError( TEXT("FMySQL::DoSQL: mysql_store_result failed") );
   
#if MYSQL_STATS
	StatItem->Count++;
	StatItem->Time += appSeconds() - QueryStartTime;
	StatItem->RowsAffected += mysql_affected_rows(&mysql);
#endif

	unguard;
}

INT FMySQL::GetInsertID()
{
	return mysql_insert_id(&mysql);
}

TCHAR* FMySQL::GetTempString(INT Len)
{
	CurrentTempString = (CurrentTempString+1)%20;
	Len = (Len + 1)* sizeof(TCHAR);
	if( TempData[CurrentTempString].Num() < Len )
		TempData[CurrentTempString].Add( Len - TempData[CurrentTempString].Num() );
	return (TCHAR*)(&TempData[CurrentTempString](0)); 
}

ANSICHAR* FMySQL::GetTempAnsiString(INT Len)
{
	CurrentTempString = (CurrentTempString+1)%20;
	Len = (Len + 1)* sizeof(ANSICHAR);
	if( TempData[CurrentTempString].Num() < Len )
		TempData[CurrentTempString].Add( Len - TempData[CurrentTempString].Num() );
	return (ANSICHAR*)(&TempData[CurrentTempString](0)); 
}

const TCHAR* FMySQL::TCHARFormatSQL(const TCHAR* String)
{
	return appFromAnsi(FormatSQL(String), GetTempString(4096));
}

const ANSICHAR* FMySQL::FormatSQL(const TCHAR* String)
{
	ANSICHAR* Result = GetTempAnsiString(1024);
	if( appIsPureAnsi(String) )
		mysql_escape_string(Result, appToAnsi(String, GetTempAnsiString(1024)), appStrlen(String) );
	else
	{
		((TCHAR*)Result)[0] = UNICODE_BOM;
		mysql_escape_string(&Result[2], (ANSICHAR*)String, 2*appStrlen(String) );
	}
	return Result;
}

const ANSICHAR* FMySQL::FormatSQL(const ANSICHAR* String)
{
	ANSICHAR* Result = GetTempAnsiString(1024);
	mysql_escape_string(Result, String, strlen(String) );
	return Result;
}

//
// FMySQLQueryResult implementation
//
FMySQLQueryResult::FMySQLQueryResult( FDatabase* InDatabase, MYSQL_RES* InRes )
:	FQueryResult(InDatabase)
,	res(InRes)
,	NumRowsCache(-1)
{
	guard(FMySQLQueryResult::FMySQLQueryResult);
	INT num = mysql_num_fields(res);
	if( num )
	{
		MYSQL_FIELD* mysqlfields = mysql_fetch_fields(res);
		for( INT i=0;i<num;i++ )
		{
			EDatabaseFieldType FieldType;
			switch( mysqlfields[i].type )
			{
			case FIELD_TYPE_SHORT:
			case FIELD_TYPE_LONG:
			case FIELD_TYPE_INT24:
				FieldType = DBFT_Int;
				break;
			case FIELD_TYPE_LONGLONG:
				FieldType = DBFT_BigInt;
				break;
			case FIELD_TYPE_FLOAT:
			case FIELD_TYPE_DOUBLE:
				FieldType = DBFT_Float;
				break;
			case FIELD_TYPE_TIMESTAMP:
			case FIELD_TYPE_DATE:
			case FIELD_TYPE_TIME:
			case FIELD_TYPE_DATETIME:
				FieldType = DBFT_DateTime;
				break;
			case FIELD_TYPE_TINY_BLOB:
			case FIELD_TYPE_MEDIUM_BLOB:
			case FIELD_TYPE_LONG_BLOB:
			case FIELD_TYPE_BLOB:
				FieldType = DBFT_BLOB;
				break;
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_STRING:
				FieldType = DBFT_String;
				break;
			default:
				GWarn->Logf(TEXT("FMySQLQueryResult::FMySQLQueryResult: Skipping field %s of uknown type %d"), appFromAnsi(mysqlfields[i].name), mysqlfields[i].type );
				goto SkipField;
				break;
			}

			FMySQLQueryField* NewField = new FMySQLQueryField((FMySQL*)InDatabase, this, i, appFromAnsi(mysqlfields[i].name), FieldType);
			Fields.AddItem(NewField);
SkipField:;
		}
	}
	unguard;
}

FMySQLQueryResult::~FMySQLQueryResult()
{
	guard(FMySQLQueryResult::~FMySQLQueryResult);
	mysql_free_result(res);
	for( INT i=0;i<Fields.Num();i++ )
		delete Fields(i);
	Fields.Empty();
	unguard;
}

INT FMySQLQueryResult::NumRows()
{
	guard(FMySQLQueryResult::NumRows);
	if( NumRowsCache < 0 )
		NumRowsCache = mysql_num_rows(res);
	return NumRowsCache;
	unguard;
}

FQueryField** FMySQLQueryResult::FetchNextRow()
{
	guard(FMySQLQueryResult::FetchNextRow);
	if(!Fields.Num())
		return NULL;

	CurrentRow = mysql_fetch_row(res);
	if( !CurrentRow )
		return NULL;

	return &Fields(0);
	unguard;
}

//
// FMySQLQueryField implementation
//
FString FMySQLQueryField::AsString()
{
	guard(FMySQLQueryField::AsString);
	if( Result->CurrentRow[FieldIndex]!=NULL )
	{
		if( ((TCHAR*)Result->CurrentRow[FieldIndex])[0] == UNICODE_BOM )
			return FString(appFromUnicode(&((TCHAR*)Result->CurrentRow[FieldIndex])[1]));
		else
			return FString(appFromAnsi(Result->CurrentRow[FieldIndex], MySQL->GetTempString(4096)) );
	}
	return FString(TEXT(""));
	unguard;
}

INT FMySQLQueryField::AsInt()
{
	guard(FMySQLQueryField::AsInt);
	if( Result->CurrentRow[FieldIndex]==NULL )
		return 0;
	if( FieldType!=DBFT_Int && FieldType!=DBFT_Float && FieldType!=DBFT_BigInt )
		return 0;
	return ::atoi(Result->CurrentRow[FieldIndex]);
	unguard;
}

FLOAT FMySQLQueryField::AsFloat()
{
	guard(FMySQLQueryField::AsFloat);
	if( Result->CurrentRow[FieldIndex]==NULL )
		return 0.f;
	if( FieldType!=DBFT_Int && FieldType!=DBFT_Float && FieldType!=DBFT_BigInt )
		return 0.f;
	return ::strtod(Result->CurrentRow[FieldIndex],NULL);
	unguard;
}

SQWORD FMySQLQueryField::AsBigInt()
{
	guard(FMySQLQueryField::AsBigInt);
	if( Result->CurrentRow[FieldIndex]==NULL )
		return 0.f;
	if( FieldType!=DBFT_Int && FieldType!=DBFT_Float && FieldType!=DBFT_BigInt )
		return 0.f;
	return ::_strtoi64(Result->CurrentRow[FieldIndex],NULL,10);
	unguard;
}

UBOOL FMySQLQueryField::AsBool()
{
	guard(FMySQLQueryField::AsBool);
	if( Result->CurrentRow[FieldIndex]==NULL )
		return 0.f;
	if( FieldType == DBFT_String )
	{
		if( !*Result->CurrentRow[FieldIndex] )
			return 0;
		else
			return *Result->CurrentRow[FieldIndex] == 'T';
	}
	else
	if( FieldType == DBFT_Int )
		return ::atoi(Result->CurrentRow[FieldIndex]) != 0;
	else
		return 0;
	unguard;
}

time_t FMySQLQueryField::AsDateTime()
{
	guard(FMySQLQueryField::AsDateTime);
	// later
	check(0);
	return 0;
	unguard;
}

UBOOL FMySQLQueryField::IsNull()
{
	return Result->CurrentRow[FieldIndex] == NULL;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

