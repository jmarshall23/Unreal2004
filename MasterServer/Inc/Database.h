/*=============================================================================
	Database.h: SQL database interface
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

/*-----------------------------------------------------------------------------
	FDatabase and associated classes
-----------------------------------------------------------------------------*/

extern UBOOL GExtendedLogging;

//
//	FDatabase
//
class FDatabase
{
	friend class FQueryResult;
	TArray<FQueryResult*> Queries;
public:

	// tors
	virtual ~FDatabase();

	// FDatabase interface
	virtual UBOOL Connect( const TCHAR* InHost, const TCHAR* InUserName, const TCHAR* InPassword, const TCHAR* InDatabase, UBOOL Delayed ) = 0;
	virtual FQueryResult* VARARGS Query( const ANSICHAR* SQL, ... )=0;
	virtual void VARARGS DoSQL( const ANSICHAR* SQL, ... )=0;
	virtual const ANSICHAR* FormatSQL(const TCHAR* String)=0;
	virtual const ANSICHAR* FormatSQL(const ANSICHAR* String)=0;
	virtual const TCHAR* TCHARFormatSQL(const TCHAR* String)=0;
};

//
//	FQueryResult
//
class FQueryResult
{
	FDatabase* Database;
protected:
	TArray<class FQueryField*> Fields;
public:

	// tors
	FQueryResult( FDatabase* InDatabase );
	virtual ~FQueryResult();

	// FQueryResult interface
	virtual INT NumRows()=0;
	virtual class FQueryField** FetchNextRow()=0;
	virtual FQueryField* FieldByName(TCHAR* FieldName);
};

//
//	FQueryField
//

enum EDatabaseFieldType
{
	DBFT_String		= 0,
	DBFT_Int		= 1,
	DBFT_Float		= 2,
	DBFT_DateTime	= 3,
	DBFT_BLOB		= 4,
	DBFT_BigInt		= 5,
};

class FQueryField
{
public:
	FString FieldName;
	EDatabaseFieldType FieldType;

	// tors
	FQueryField(const TCHAR* InFieldName, EDatabaseFieldType InFieldType)
	:	FieldName(InFieldName)
	,	FieldType(InFieldType)
	{}
	
	// FQueryField interface
	virtual FString AsString()=0;
	virtual INT AsInt()=0;
	virtual FLOAT AsFloat()=0;
	virtual UBOOL AsBool()=0;
	virtual time_t AsDateTime()=0;
	virtual SQWORD AsBigInt()=0;
	virtual UBOOL IsNull()=0;
};

/*-----------------------------------------------------------------------------
	FMySQL
-----------------------------------------------------------------------------*/

//#define MYSQL_STATS 1

//
//	FMySQLQueryStats
//
struct FMySQLQueryStatItem
{
    INT Count;
	INT RowsAffected;
	FLOAT Time;

	FMySQLQueryStatItem()
	:	Count(0)
	,	RowsAffected(0)
	,	Time(0.f)
	{}
};

//
//	FMySQL
//
class FMySQL : public FDatabase
{
	MYSQL mysql;

	// for auto-reconnect
	UBOOL Connected;
	FString DelayedHost, DelayedUserName, DelayedPassword, DelayedDatabase;
	TArray<BYTE> TempData[20];
	INT CurrentTempString;

	// stats
	TMap<FString, FMySQLQueryStatItem> QueryStats;
public:
	// tors.
	FMySQL();
	virtual ~FMySQL();

	// FDatabase interface
	UBOOL Connect( const TCHAR* InHost, const TCHAR* InUserName, const TCHAR* InPassword, const TCHAR* InDatabase, UBOOL Delayed=0 );
	virtual FQueryResult* VARARGS Query( const ANSICHAR* SQL, ... );
	virtual void VARARGS DoSQL( const ANSICHAR* SQL, ... );
	virtual const ANSICHAR* FormatSQL(const TCHAR* String);
	virtual const ANSICHAR* FormatSQL(const ANSICHAR* String);
	virtual const TCHAR* TCHARFormatSQL(const TCHAR* String);

	TCHAR* GetTempString(INT Len);
	ANSICHAR* GetTempAnsiString(INT Len);

	// FMySQL interface
	void MySQLDumpQueryStats();
	void MySQLError( TCHAR* Message );
	void MySQLWarning( TCHAR* Message );
	INT GetInsertID();
};

//
//	FMySQLQueryResult
//
class FMySQLQueryResult : public FQueryResult
{
	INT NumRowsCache;
	MYSQL_RES* res;
	MYSQL_ROW CurrentRow;
	friend class FMySQLQueryField;
public:
	// tors.
	FMySQLQueryResult( FDatabase* InDatabase, MYSQL_RES* InRes );
	virtual ~FMySQLQueryResult();

	// FQueryResult Interface
	INT NumRows();
	FQueryField** FetchNextRow();
};

//
//	FMySQLQueryField;
//
class FMySQLQueryField : public FQueryField
{
	friend class FMySQLQueryResult;
	FMySQLQueryResult* Result;
	INT FieldIndex;
	FMySQL* MySQL;
public:
	FMySQLQueryField( FMySQL* InMySQL, FMySQLQueryResult* InResult, INT InFieldIndex, const TCHAR* InFieldName, EDatabaseFieldType InFieldType)
	:	FQueryField(InFieldName,InFieldType)
	,	Result(InResult)
	,	FieldIndex(InFieldIndex)
	,	MySQL(InMySQL)
	{}

	// FQueryField interface
	FString AsString();
	INT AsInt();
	FLOAT AsFloat();
	UBOOL AsBool();
	time_t AsDateTime();
	virtual SQWORD AsBigInt();
	UBOOL IsNull();
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

