#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

typedef struct ODBCHDLInfo
{
	SQLHANDLE hevn;
	SQLHDBC hdbc;
	SQLHANDLE hstmt;
}t_ODBCHDLInfo;
typedef struct ODBCDSNInfo
{
	char DSN[30];
	char user[30];
	char password[10];
}t_ODBCDSNInfo;


SQLRETURN SQLInit(t_ODBCHDLInfo *pHDL, t_ODBCDSNInfo dsn, SQLUINTEGER odbcVersion);
void SQLFree(t_ODBCHDLInfo *pHDL);

#define CONFIG_SQL_DATA_TYPE_RANGE_DEBUG
#ifdef CONFIG_SQL_DATA_TYPE_RANGE_DEBUG
#define SQL_DATA_TYPE_RANGE_DEBUG(format,...) do{printf("%s,%d:",__FUNCTION__, __LINE__);printf(format,##__VA_ARGS__);}while(0)
#else
#define SQL_DATA_TYPE_RANGE_DEBUG(format,...) do{}while(0)
#endif

void DebugSQLError(SQLHANDLE henv, SQLHANDLE hdbc, SQLHANDLE hstmt)
{
	#define CONFIG_MAX_ERROR_SIZE	512
	#define CONFIG_STATE_SIZE		6
    char	buf[CONFIG_MAX_ERROR_SIZE];
    char	State[6];
	SDWORD	NativeError;
    RETCODE returncode;
	char MsgDisplayed = 0;

	MsgDisplayed=0;

   /* Log any henv error messages */
   returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, CONFIG_MAX_ERROR_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[CONFIG_STATE_SIZE-1]= 0;
      printf("   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",State,NativeError,buf);
		MsgDisplayed=1;
      returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, CONFIG_MAX_ERROR_SIZE, NULL);
   }

   /* Log any hdbc error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, CONFIG_MAX_ERROR_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[CONFIG_STATE_SIZE-1]=0;
        printf("   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",State,NativeError,buf);
		MsgDisplayed=1;
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, CONFIG_MAX_ERROR_SIZE, NULL);
   }

   /* Log any hstmt error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, CONFIG_MAX_ERROR_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[CONFIG_STATE_SIZE-1]=0;
        printf("   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",State,NativeError,buf);
		MsgDisplayed=1;
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, CONFIG_MAX_ERROR_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		printf("There were no error messages for SQLError() to display\n");
}                     

#define DEBUG_ODBC_MSG(hstmt)		\
	do{int RecNumber = 1;\
		SQLCHAR SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];\
		SQLINTEGER NativeError;\
		SQLSMALLINT MsgLen;\
		while(SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, RecNumber, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen) != SQL_NO_DATA){\
				RecNumber++;\
				SQL_DATA_TYPE_RANGE_DEBUG("\n\t %s\n", Msg);\
		}\
	}while(0)


#define MAX_TESTS_PER_FUNCTION	100
#define MAX_CONNECT_STRING      1024
#define STATE_SIZE				6
#define MAX_STRING_SIZE			500
#define MAX_NOS_SIZE			4050
#define MAX_HEADING_SIZE		4000
#define MAXLEN					100
#define MAX_COLUMNS				100
#define MAX_ROW_LEN				1000
#define MAX_COLUMN_NAME			128
#define MAX_DB_NAME_LEN			60
#define	NUM_ENV_HND				10
#define	NUM_CONN_HND			10
#define	MAX_TABLE_NAME			128
#define MAX_SCHEMA_NAME			128
#define MAX_CATALOG_NAME		128
#define MAX_USER_NAME			128+1
#define ZERO_LEN                0
#define BUFF300                 300
#define BUFF600                 600

#define NULL_STRING  '\0'
void LogAllErrors(SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt);

SQLRETURN SQLInit(t_ODBCHDLInfo *pHDL, t_ODBCDSNInfo dsn, SQLUINTEGER odbcVersion)
{
	SQLRETURN ret;

	/*
	*In ODBC 3.x, the ODBC 2.x function SQLAllocEnv has been replaced by SQLAllocHandle. For more information,
	see https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/odbc-api-reference
	*/

	// 1. allocate Environment handle and register version 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pHDL->hevn);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error AllocHandle\n");
		return SQL_ERROR;
	}

	if(odbcVersion == SQL_OV_ODBC2)
		ret = SQLSetEnvAttr(pHDL->hevn, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC2, 0);
	else if(odbcVersion == SQL_OV_ODBC3)
		ret = SQLSetEnvAttr(pHDL->hevn, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	else
		ret = SQLSetEnvAttr(pHDL->hevn, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC2, 0);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error SetEnv\n");
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->hevn);
		return SQL_ERROR;
	}
	// 2. allocate connection handle, set timeout
	ret = SQLAllocHandle(SQL_HANDLE_DBC, pHDL->hevn, &pHDL->hdbc);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error AllocHDB %d\n", ret);
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->hevn);
		return SQL_ERROR;
	}

	SQLSetConnectAttr(pHDL->hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
	// 3. Connect to the datasource "web" 
	ret = SQLConnect(pHDL->hdbc, (SQLCHAR*)dsn.DSN, SQL_NTS,
		(SQLCHAR*)dsn.user, SQL_NTS,
		(SQLCHAR*)dsn.password, SQL_NTS);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error SQLConnect %d\n", ret);
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->hevn);
		return SQL_ERROR;
	}
	printf("Connected successfull!\n");
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pHDL->hdbc, &pHDL->hstmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Fehler im AllocStatement %d\n", ret);
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->hevn);
		return SQL_ERROR;
	}

	return SQL_SUCCESS;
}
void SQLFree(t_ODBCHDLInfo *pHDL)
{
	SQLFreeHandle(SQL_HANDLE_STMT, pHDL->hstmt);
	SQLDisconnect(pHDL->hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, pHDL->hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, pHDL->hevn);
}
/**************************************************************
** LogAllErrors()
**                          
** This function loops through calls to SQLError() building strings
** for all error messages that might be there and writes them out
** the the log file.
***************************************************************/                         
void LogAllErrors(SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt)
{             
    char	buf[MAX_STRING_SIZE];
    char	State[STATE_SIZE];
	SDWORD	NativeError;
    RETCODE returncode;
	BOOL MsgDisplayed;

	MsgDisplayed=0;

   /* Log any henv error messages */
   returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
      printf("   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",State,NativeError,buf);
		MsgDisplayed=1;
      returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* Log any hdbc error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        printf("   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",State,NativeError,buf);
		MsgDisplayed=1;
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* Log any hstmt error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        printf("   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",State,NativeError,buf);
		MsgDisplayed=1;
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLCHAR*)State, &NativeError, (SQLCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		printf("There were no error messages for SQLError() to display\n");
} 
#define DTSIZE 35

typedef struct {
    char name [32];     // SQL data type name
    int  value;         // SQL data type numeric value
} dataTypes;

// Array of SQL Data Types
dataTypes dtList[DTSIZE] = {
    "SQL_UNKNOWN_TYPE",0,
    "SQL_CHAR",1,
    "SQL_NUMERIC",2,
    "SQL_DECIMAL",3,
    "SQL_INTEGER",4,
    "SQL_SMALLINT",5,
    "SQL_FLOAT",6,
    "SQL_REAL",7,
    "SQL_DOUBLE",8,
    "SQL_DATETIME",9,
    "SQL_DATE",9,
    "SQL_INTERVAL",10,
    "SQL_TIME",10,
    "SQL_TIMESTAMP",11,
    "SQL_VARCHAR",12,
    "SQL_TYPE_DATE",91,
    "SQL_TYPE_TIME",92,
    "SQL_TYPE_TIMESTAMP",93,
    "SQL_LONGVARCHAR",-1,
    "SQL_BINARY",-2,
    "SQL_VARBINARY",-3,
    "SQL_LONGVARBINARY",-4,
    "SQL_BIGINT",-5,
    "SQL_TINYINT",-6,
    "SQL_BIT",-7,
    "SQL_WCHAR",-8,
    "SQL_WVARCHAR",-9,
    "SQL_WLONGVARCHAR",-10,
    "SQL_GUID",-11,
    "SQL_SS_VARIANT",-150,
    "SQL_SS_UDT",-151,
    "SQL_SS_XML",-152,
    "SQL_SS_TABLE",-153,
    "SQL_SS_TIME2",-154,
    "SQL_SS_TIMESTAMPOFFSET",-155
};

// Returns name of data type, unknown if not found
char * SQLType (int dataType) {

    int i;

    for (i=0;i<DTSIZE;i++) {
        if (dataType==dtList[i].value)
        break;
    }

    if (i<DTSIZE) {
        return (dtList[i].name);
    } else {
        return (dtList[0].name);
    }
}


static int debugSQLGetTypeInfo(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
 
	SQLCHAR typeName[128];
    SQLSMALLINT dataType;
    SQLINTEGER columnSize;
 	
	struct
	{
		char		*TYPE_NAME;
		SQLSMALLINT		DATA_TYPE;
	}TypeInfo[] = 
		{	//TYPENAME,		DATATYPE,		PREC,LITPRE,LITSUF,	PARAM,			NULL,			CASE,SRCH,				ATTR,MON,INC,LOC,		MIN,MAX
			{"all type",			SQL_ALL_TYPES},	// this is for get all types
			{"CHAR",		SQL_CHAR},
			{"VARCHAR",		SQL_VARCHAR},
			{"DECIMAL",		SQL_DECIMAL},
			{"NUMERIC",		SQL_NUMERIC},
			{"SMALLINT",	SQL_SMALLINT},
			{"INTEGER",		SQL_INTEGER},
			{"REAL",		SQL_REAL},
			{"FLOAT",		SQL_FLOAT},
			{"DOUBLE PRECISION", SQL_DOUBLE},
			{"DATE",		SQL_TYPE_DATE},
			{"TIME",		SQL_TYPE_TIME},
			{"TIMESTAMP",	SQL_TYPE_TIMESTAMP},
			{"BIGINT",		SQL_BIGINT},
			{"LONG VARCHAR",SQL_LONGVARCHAR},
			{"BINARY",		SQL_BINARY},
            { "VARBINARY", SQL_VARBINARY},
            { "LONG VARBINARY", SQL_LONGVARBINARY},
            { "TINYINT", SQL_TINYINT},
            { "INTERVAL MONTH", SQL_INTERVAL_MONTH},
            { "INTERVAL YEAR", SQL_INTERVAL_YEAR},
            { "BIT", SQL_BIT},
			{"",999,}	
	};
	int i;
	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	for(i = 0; TypeInfo[i].DATA_TYPE != 999; i++){
		printf("\n\n\nStart SQLGetTypeInfo: %s\n", TypeInfo[i].TYPE_NAME);
		retcode = SQLGetTypeInfo(hstmt, TypeInfo[i].DATA_TYPE);
	    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLGetTypeInfo fail:\n");
				DEBUG_ODBC_MSG(hstmt);
				continue;
			}

	    retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR,
	                         (SQLPOINTER) typeName,
	                         (SQLLEN) sizeof(typeName), &len);
	    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLBindCol 1 fail:\n");
				DEBUG_ODBC_MSG(hstmt);
				continue;
			}

	    retcode = SQLBindCol(hstmt, 2, SQL_C_SHORT,
	                         (SQLPOINTER) &dataType,
	                         (SQLLEN) sizeof(dataType), &len);
	    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLBindCol 2 fail:\n");
				DEBUG_ODBC_MSG(hstmt);
				continue;
			}

	    retcode = SQLBindCol(hstmt, 3, SQL_C_ULONG,
	                         (SQLPOINTER) &columnSize,
	                         (SQLLEN) sizeof(columnSize), &len);
	    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLBindCol 3 fail:\n");
				DEBUG_ODBC_MSG(hstmt);
				continue;
			}

	    printf("SQL Data Type             Type Name                 Value                 Max Size\n");
	    printf("------------------------- ------------------------- -------               --------\n");

	    // Fetch each row, and display
	    while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS) {
	        printf("%s\t\t", SQLType (dataType) );
	        printf("%s\t\t", (char *) typeName );
	        printf("%i\t\t", (int) dataType );
	        printf ("%i\n", (int) columnSize);
			if((dataType != TypeInfo[i].DATA_TYPE) && (TypeInfo[i].DATA_TYPE != SQL_ALL_TYPES)){
				printf("fail !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			}
	    }
	}
	
	return 0;
}

int main(void)
{
	SQLRETURN retcode;
	t_ODBCHDLInfo mODBCHDLInfo;
	t_ODBCDSNInfo mDSNInfo;    
		
	memset(&mDSNInfo, 0, sizeof(mDSNInfo));

//#ifdef __TRAFODION__ 
#if 1
	strcpy(mDSNInfo.DSN, "TRAF_ANSI");
	strcpy(mDSNInfo.user, "trafodion");
	strcpy(mDSNInfo.password, "traf123");
#else
	strcpy(mDSNInfo.DSN, (char *)"MySQL");
	strcpy(mDSNInfo.user, "root");
	strcpy(mDSNInfo.password, "123456");
#endif
	printf("SQL source:%s,%s,%s\n", mDSNInfo.DSN, mDSNInfo.user, mDSNInfo.password);
	retcode = SQLInit(&mODBCHDLInfo, mDSNInfo, SQL_OV_ODBC3);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		printf("SQL init fail\n");

		return -1;
	}

	/*****************test********************/
	
	debugSQLGetTypeInfo(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}


