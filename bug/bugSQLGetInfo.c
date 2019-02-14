#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef unixcli
#include <unistd.h>
#else
#include <windows.h>
#include <tchar.h>
#endif
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <assert.h>


#define LogMsg(Options,format,...) do{printf(format,##__VA_ARGS__);}while(0)
#ifndef TCHAR
typedef char TCHAR;
#endif
#ifndef BOOL 
typedef char BOOL;
#endif

typedef  struct tabTestInfo
{
    SQLHANDLE henv;
    SQLHDBC hdbc;
    SQLHANDLE hstmt;
    TCHAR DataSource[128];
    TCHAR Server[128];
    TCHAR Port[10 * 2];
    TCHAR UserID[128];
    TCHAR Password[50];
    TCHAR Database[50];
    TCHAR Catalog[128];
    TCHAR Schema[128];
}TestInfo;
#ifdef unixcli
#ifndef UNICODE
//typedef char BOOL;
typedef char CHAR;
/*******************************************************************************************************************/

#define _tprintf    printf
#define _ftprintf   fprintf
#define _vftprintf  vfprintf
#define _sprintf    printf
#define _stprintf   sprintf

/*** String operation functions ***/

#define _tcslen			strlen
#define _tcscspn        strcspn
#define _tcstok         strtok
#define _tcsstr         strstr
#define _tcsncpy        strncpy
#define _stprintf		sprintf
#define _tcscpy			strcpy
#define _tcscat			strcat
#define _tcsicmp		strcmp
#define _strcasecmp		strcasecmp
#define _tcscmp			strcmp
#define _tcsncmp		strncmp
#define _tfopen         fopen
#define _tfclose	fclose
#define TFILE		FILE
#define __T(x)		x

#else
#define __T(x)		(SQLTCHAR *)(L##x)
#endif

#else
//#define _tfopen         fopen
#define _tfclose          fclose
#define TFILE             FILE
#ifdef UNICODE
#define _tcsicmp        wcscmp
#define _strcasecmp     wcscmp
#define _tcscmp         wcscmp
#define __T(x)		L##x
#else
#define _tcsicmp        _stricmp
#define _strcasecmp     _stricmp
#define _tcscmp         strcmp
#endif
#endif
#define _T(x)		__T(x)

/*
** Valid Values for LogMsg() <Options> parameter
** These can be easily used to make the LogMsg function display standard things
** like a time stamp and/or a standard error prefix like "***ERROR:" instead of
** you having to build those things.  More than one option may be used by
** ORing or adding options.  Example1: ERRMSG | TIME_STAMP	 Example 2: ERRMSG + LINES
*/ 
#define NONE            0x00     /* Don't use any options.  Equivalent to using LogPrintf() */
#define ERRMSG          0x01     /* Prefix the <ErrorString> to the front of the message */
#define TIME_STAMP      0x02     /* Display a timestamp on the line before the message */
#define LINEBEFORE      0x04     /* Display separating lines before and after the message */
#define LINEAFTER       0x08     /* Display a separating line after the message */
#define ENDLINE         LINEAFTER/* Some older programs used to use ENDLINE instead of LINEAFTER */
#define INTERNALERRMSG  0x10     /* Prefix ***INTERNAL ERROR: to the fromt of the message */
#define SHORTTIMESTAMP  0x20     /* Prefix a shorter timestamp to the front of the message */
#define INFO			0x30	 /* Will print only if debugMode = on */


#define CONNECT_AUTOCOMMIT_OFF        0x01    // Default is on.
#define CONNECT_ODBC_VERSION_2        0x02
#define CONNECT_ODBC_VERSION_3        0x04

#define MAX_SQLSTRING_LEN 1000
#define STATE_SIZE 6
#define MAX_CONNECT_STRING 256
#define TRUE 1
#define FALSE 0
#define MAX_STRING_SIZE            500

#define NULL_STRING  '\0'

#define CONFIG_BINDPARA_SQL_TYPE_DEFAULT

#define CONFIG_DATA_TYPE_MAX    256
#define CONFIG_DATA_CHAR_SIZE   512
#define CONFIG_STMT_BUF_MAX    20480

typedef enum PassFail {PASSED, FAILED} PassFail;

typedef short TrueFalse;  

/*******************************************************************************************************************/
void LogAllErrors(SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt)
{             
    TCHAR	buf[MAX_STRING_SIZE];
    TCHAR	State[STATE_SIZE];
	SDWORD	NativeError;
    RETCODE returncode;
	BOOL MsgDisplayed;
    int errorCount = 0;

	MsgDisplayed = 0;

   /* Log any henv error messages */
   returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
      _tprintf(_T("   State: %s\n")
							  _T("   Native Error: %ld\n")
                _T("   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* Log any hdbc error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        _tprintf(_T("   State: %s\n")
							  _T("   Native Error: %ld\n")
                _T("   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* Log any hstmt error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        _tprintf(_T("   State: %s\n")
							  _T("   Native Error: %ld\n")
                _T("   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		_tprintf(_T("There were no error messages for SQLError() to display\n"));
}  
TrueFalse FullConnectWithOptions(TestInfo *pTestInfo, int Options)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;

	assert( (Options&CONNECT_ODBC_VERSION_2) || (Options&CONNECT_ODBC_VERSION_3) );

	/* Initialize the basic handles needed by ODBC */
	if (Options&CONNECT_ODBC_VERSION_2)
	{
		returncode = SQLAllocEnv(&henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return (FALSE);
        }
		
		returncode = SQLAllocConnect(henv,&hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeEnv(henv);
			return(FALSE);
		}
	}
	if (Options&CONNECT_ODBC_VERSION_3)
	{
		returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return(FALSE);
        }

		returncode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return (FALSE);
		}

		returncode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return (FALSE);
		}
	}

	// Handle Autocommit_Off Option
	if (Options&CONNECT_AUTOCOMMIT_OFF)
    {/*
		returncode = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			// Cleanup.  No need to check return codes since we are already failing
			SQLFreeEnv(henv);
			return (FALSE);
		}*/
	}

	returncode = SQLConnect(hdbc,
                           (SQLTCHAR*)pTestInfo->DataSource,(SWORD)_tcslen(pTestInfo->DataSource),
                           (SQLTCHAR*)pTestInfo->UserID,(SWORD)_tcslen(pTestInfo->UserID),
                           (SQLTCHAR*)pTestInfo->Password,(SWORD)_tcslen(pTestInfo->Password)
                           );

	if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
	{
		// Free up handles since we hit a problem.
		if (Options&CONNECT_ODBC_VERSION_2){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return (FALSE);
		}
		if (Options&CONNECT_ODBC_VERSION_3){
			/* Cleanup.  No need to check return codes since we are already failing */		   
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
            return (FALSE);
		}
        return(FALSE);
	}
	
	pTestInfo->henv = (SQLHANDLE)henv;
	pTestInfo->hdbc = (SQLHANDLE)hdbc;
	
	/* Connection established */
	return (TRUE);
}  /* FullConnectWithOptions() */

void disconnect(SQLHANDLE henv, SQLHANDLE hdbc, SQLHANDLE hstmt)
{
    if(hstmt != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
    if(hdbc != SQL_NULL_HANDLE)
    {
        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
    }
    if(henv != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_ENV,henv);
}
int FullDisconnect(TestInfo *pTestInfo)
{
  RETCODE returncode;                        
  TrueFalse Result;
  
  Result=TRUE;
   
  /* Disconnect from the data source.  If errors, still go on and */
  /* try to free the handles */
  returncode = SQLDisconnect((SQLHANDLE)pTestInfo->hdbc);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
        LogAllErrors(pTestInfo->henv, pTestInfo->hdbc, pTestInfo->hstmt);
	}
   
  /* Free up all handles used by this connection */
  returncode = SQLFreeConnect((SQLHANDLE)pTestInfo->hdbc);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
		LogAllErrors(pTestInfo->henv,pTestInfo->hdbc,pTestInfo->hstmt);
	}

   
  returncode = SQLFreeEnv((SQLHANDLE)pTestInfo->henv);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
		LogAllErrors(pTestInfo->henv,pTestInfo->hdbc,pTestInfo->hstmt);
	}
   
  return(Result);
}  /* FullDisconnectEx() */
int addInfoSession(SQLHSTMT hstmt)
{
    SQLRETURN returnCode;
    char buf[256];
    SQLLEN len;
    int i;
    
    returnCode = SQLExecDirect(hstmt, (SQLTCHAR*)"info session", SQL_NTS);
    if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, "Execute \"info session\" fail\n");
        return -1;
    }
    else{
        SQLSMALLINT num;
        const char *InfoSession[] = {
            "SESSION_ID",
            "SERVER_PROCESS_NAME",
            "SERVER_PROCESS_ID",
            "SERVER_HOST",
            "SERVER_PORT",
            "MAPPED_SLA",
            "MAPPED_CONNECT_PROFILE",
            "MAPPED_DISCONNECT_PROFILE",
            "CONNECTED_INTERVAL_SEC",
            "CONNECT_TIME",
            "DATABASE_USER_NAME",
            "USER_NAME",
            "ROLE_NAME",
            "APP_NAME",
            "TENANT_NAME",
            "END"
        };
        int sessionId;
        
        returnCode = SQLFetch(hstmt);
        returnCode = SQLNumResultCols(hstmt, &num);
        if(num > 15){
            num = 15;
        }
        sessionId = 0;
        while (returnCode != SQL_NO_DATA)
        {
            for (i = 1; i <= num; ++i)
            {
                returnCode = SQLGetData(hstmt, i, SQL_C_CHAR, (SQLPOINTER)buf, sizeof(buf), &len);
                if (SQL_SUCCEEDED(returnCode)){
                    if(strcmp(InfoSession[sessionId], "END") != 0){
                        if(strcmp(InfoSession[sessionId], "SESSION_ID") == 0 ||
                            strcmp(InfoSession[sessionId], "SERVER_HOST") == 0 ||
                            strcmp(InfoSession[sessionId], "SERVER_PORT") == 0){
                            LogMsg(NONE, "%s:%s\n", InfoSession[sessionId], buf);
                        }
                        sessionId++;
                    }
                    else{
                        break;
                    }
                }
                else{
                    break;
                }
            }
            returnCode = SQLFetch(hstmt);
        }
    }
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);
        
    return 0;
}

/*******************************************************************************************************************/
int debugSQLGetInfoNull(TestInfo *pTestInfo)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT num, i;
    char buf[512] = {0};
    SQLLEN len;
    union{
	    SQLLEN      longInfoValue; 
		SQLUINTEGER	intInfoValue;
		SQLUSMALLINT smiInfoValue;
		TCHAR charInfoValue[512];
		} u1;
    SWORD	pcbInfoValue;
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));

    LogMsg(NONE, _T("..............................\n"));
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
        LogMsg(NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnect(&sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    LogMsg(NONE, _T("server info:\n"));
    addInfoSession(hstmt);
    LogMsg(NONE, _T("..............................\n"));
    LogMsg(NONE, _T("SQLGetInfo((SQLHANDLE)hdbc,SQL_DRIVER_HSTMT,NULL,(SWORD)sizeof(u1.charInfoValue),&pcbInfoValue);\n"));
    retcode = SQLGetInfo((SQLHANDLE)hdbc,
                        SQL_DRIVER_HSTMT,
                        NULL,
                        (SWORD)sizeof(u1.charInfoValue),
                        &pcbInfoValue);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("Call SQLGetInfo fail !\n"));
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        LogMsg(NONE, _T("%d\n%d\n%d\n%s\n"), u1.longInfoValue, u1.intInfoValue, u1.smiInfoValue, u1.charInfoValue);
    }

TEST_FAIL:

    FullDisconnect(&sTestInfo);
    LogMsg(TIME_STAMP, _T("complete test:%s\n"), (ret == 0) ? _T("sucess") : _T("fail"));
    
    return ret;
}
int debugSQLGetInfoAll(TestInfo *pTestInfo)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT num, i;
    char buf[512] = {0};
    SQLLEN len;
    union{
	    SQLLEN      longInfoValue; 
		SQLUINTEGER	intInfoValue;
		SQLUSMALLINT smiInfoValue;
		TCHAR charInfoValue[512];
		} u1;
    SWORD	pcbInfoValue;
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));

    LogMsg(NONE, _T("..............................\n"));
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
        LogMsg(NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnect(&sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    LogMsg(NONE, _T("server info:\n"));
    addInfoSession(hstmt);
    LogMsg(NONE, _T("..............................\n"));
    

    struct InfoTypeContext{
        SQLUSMALLINT type;
        TCHAR name[50];
    }InfoType[] = {
		{SQL_ACCESSIBLE_PROCEDURES,_T("SQL_ACCESSIBLE_PROCEDURES")},
		{SQL_ACCESSIBLE_TABLES,_T("SQL_ACCESSIBLE_TABLES")},
		{SQL_ACTIVE_ENVIRONMENTS,_T("SQL_ACTIVE_ENVIRONMENTS")},
		{SQL_AGGREGATE_FUNCTIONS,_T("SQL_AGGREGATE_FUNCTIONS")},
		{SQL_ALTER_DOMAIN,_T("SQL_ALTER_DOMAIN")},
		{SQL_ALTER_TABLE,_T("SQL_ALTER_TABLE")},
		{SQL_ASYNC_MODE,_T("SQL_ASYNC_MODE")},
		{SQL_BATCH_ROW_COUNT,_T("SQL_BATCH_ROW_COUNT")},
		{SQL_BATCH_SUPPORT,_T("SQL_BATCH_SUPPORT")},
		{SQL_BOOKMARK_PERSISTENCE,_T("SQL_BOOKMARK_PERSISTENCE")},
		{SQL_CATALOG_LOCATION,_T("SQL_CATALOG_LOCATION")},
		{SQL_CATALOG_NAME,_T("SQL_CATALOG_NAME")},
		{SQL_CATALOG_NAME_SEPARATOR,_T("SQL_CATALOG_NAME_SEPARATOR")},
		{SQL_CATALOG_TERM,_T("SQL_CATALOG_TERM")},
		{SQL_CATALOG_USAGE,_T("SQL_CATALOG_USAGE")},
		{SQL_COLLATION_SEQ,_T("SQL_COLLATION_SEQ")},
		{SQL_COLUMN_ALIAS,_T("SQL_COLUMN_ALIAS")},
		{SQL_CONCAT_NULL_BEHAVIOR,_T("SQL_CONCAT_NULL_BEHAVIOR")},
		{SQL_CONVERT_BIGINT,_T("SQL_CONVERT_BIGINT")},
		{SQL_CONVERT_BINARY,_T("SQL_CONVERT_BINARY")},
		{SQL_CONVERT_BIT,_T("SQL_CONVERT_BIT")},
		{SQL_CONVERT_CHAR,_T("SQL_CONVERT_CHAR")},
		{SQL_CONVERT_DATE,_T("SQL_CONVERT_DATE")},
		{SQL_CONVERT_DECIMAL,_T("SQL_CONVERT_DECIMAL")},
		{SQL_CONVERT_DOUBLE,_T("SQL_CONVERT_DOUBLE")},
		{SQL_CONVERT_FLOAT,_T("SQL_CONVERT_FLOAT")},
		{SQL_CONVERT_FUNCTIONS,_T("SQL_CONVERT_FUNCTIONS")},
		{SQL_CONVERT_INTERVAL_DAY_TIME, _T("SQL_CONVERT_INTERVAL_DAY_TIME")},
		{SQL_CONVERT_INTERVAL_YEAR_MONTH, _T("SQL_CONVERT_INTERVAL_YEAR_MONTH")},
		{SQL_CONVERT_INTEGER,_T("SQL_CONVERT_INTEGER")},
		{SQL_CONVERT_LONGVARBINARY,_T("SQL_CONVERT_LONGVARBINARY")},
		{SQL_CONVERT_LONGVARCHAR,_T("SQL_CONVERT_LONGVARCHAR")},
		{SQL_CONVERT_NUMERIC,_T("SQL_CONVERT_NUMERIC")},
		{SQL_CONVERT_REAL,_T("SQL_CONVERT_REAL")},
		{SQL_CONVERT_SMALLINT,_T("SQL_CONVERT_SMALLINT")},
		{SQL_CONVERT_TIME,_T("SQL_CONVERT_TIME")},
		{SQL_CONVERT_TIMESTAMP,_T("SQL_CONVERT_TIMESTAMP")},
		{SQL_CONVERT_TINYINT,_T("SQL_CONVERT_TINYINT")},
		{SQL_CONVERT_VARBINARY,_T("SQL_CONVERT_VARBINARY")},
		{SQL_CONVERT_VARCHAR,_T("SQL_CONVERT_VARCHAR")},
		{SQL_CONVERT_WCHAR, _T("SQL_CONVERT_WCHAR")},
		//{SQL_CONVERT_WLONGVARCHAR, _T("SQL_CONVERT_WLONGVARCHAR")},
		{SQL_CONVERT_WVARCHAR, _T("SQL_CONVERT_WVARCHAR")},
		{SQL_CORRELATION_NAME,_T("SQL_CORRELATION_NAME")},
		{SQL_CREATE_ASSERTION, _T("SQL_CREATE_ASSERTION")},
		{SQL_CREATE_CHARACTER_SET, _T("SQL_CREATE_CHARACTER_SET")},
		{SQL_CREATE_COLLATION, _T("SQL_CREATE_COLLATION")},
		{SQL_CREATE_DOMAIN, _T("SQL_CREATE_DOMAIN")},
		{SQL_CREATE_SCHEMA, _T("SQL_CREATE_SCHEMA")},
		{SQL_CREATE_TABLE, _T("SQL_CREATE_TABLE")},
		{SQL_CREATE_TRANSLATION, _T("SQL_CREATE_TRANSLATION")},
		{SQL_CREATE_VIEW,_T("SQL_CREATE_VIEW")},
		{SQL_CURSOR_COMMIT_BEHAVIOR,_T("SQL_CURSOR_COMMIT_BEHAVIOR")},
		{SQL_CURSOR_ROLLBACK_BEHAVIOR,_T("SQL_CURSOR_ROLLBACK_BEHAVIOR")},
		{SQL_CURSOR_SENSITIVITY,_T("SQL_CURSOR_SENSITIVITY")},
		{SQL_DATA_SOURCE_NAME,_T("SQL_DATA_SOURCE_NAME")},
		{SQL_DATA_SOURCE_READ_ONLY,_T("SQL_DATA_SOURCE_READ_ONLY")},
		{SQL_DATABASE_NAME,_T("SQL_DATABASE_NAME")},
		{SQL_DATETIME_LITERALS, _T("SQL_DATETIME_LITERALS")},
		{SQL_DBMS_NAME,_T("SQL_DBMS_NAME")},
		{SQL_DBMS_VER,_T("SQL_DBMS_VER")},
		{SQL_DDL_INDEX, _T("SQL_DDL_INDEX")},
		{SQL_DEFAULT_TXN_ISOLATION,_T("SQL_DEFAULT_TXN_ISOLATION")},
		{SQL_DESCRIBE_PARAMETER,_T("SQL_DESCRIBE_PARAMETER")},
		{SQL_DM_VER, _T("SQL_DM_VER")},
		{SQL_DRIVER_HDBC,_T("SQL_DRIVER_HDBC")},
		{SQL_DRIVER_HENV,_T("SQL_DRIVER_HENV")},
//		{SQL_DRIVER_HLIB,_T("SQL_DRIVER_HLIB")},
		{SQL_DRIVER_HSTMT,_T("SQL_DRIVER_HSTMT")},
		{SQL_DRIVER_NAME,_T("SQL_DRIVER_NAME")},
		{SQL_DRIVER_ODBC_VER,_T("SQL_DRIVER_ODBC_VER")},
		{SQL_DRIVER_VER,_T("SQL_DRIVER_VER")},
		{SQL_DROP_ASSERTION, _T("SQL_DROP_ASSERTION")},
		{SQL_DROP_CHARACTER_SET, _T("SQL_DROP_CHARACTER_SET")},
		{SQL_DROP_COLLATION,_T("SQL_DROP_COLLATION")},
		{SQL_DROP_DOMAIN, _T("SQL_DROP_DOMAIN")},
		{SQL_DROP_SCHEMA, _T("SQL_DROP_SCHEMA")},
		{SQL_DROP_TABLE,_T("SQL_DROP_TABLE")},
		{SQL_DROP_TRANSLATION, _T("SQL_DROP_TRANSLATION")},
		{SQL_DROP_VIEW, _T("SQL_DROP_VIEW")},
		{SQL_DYNAMIC_CURSOR_ATTRIBUTES1,_T("SQL_DYNAMIC_CURSOR_ATTRIBUTES1")},
		{SQL_DYNAMIC_CURSOR_ATTRIBUTES2,_T("SQL_DYNAMIC_CURSOR_ATTRIBUTES2")},
		{SQL_EXPRESSIONS_IN_ORDERBY,_T("SQL_EXPRESSIONS_IN_ORDERBY")},
		{SQL_FETCH_DIRECTION,_T("SQL_FETCH_DIRECTION")},
		{SQL_FILE_USAGE,_T("SQL_FILE_USAGE")},
		{SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1,_T("SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1")},
		{SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2, _T("SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2")},
		{SQL_GETDATA_EXTENSIONS,_T("SQL_GETDATA_EXTENSIONS")},
		{SQL_GROUP_BY,_T("SQL_GROUP_BY")},
		{SQL_IDENTIFIER_CASE,_T("SQL_IDENTIFIER_CASE")},
		{SQL_IDENTIFIER_QUOTE_CHAR,_T("SQL_IDENTIFIER_QUOTE_CHAR")},
		{SQL_INDEX_KEYWORDS, _T("SQL_INDEX_KEYWORDS")},
		{SQL_INFO_SCHEMA_VIEWS, _T("SQL_INFO_SCHEMA_VIEWS")},
		{SQL_INSERT_STATEMENT, _T("SQL_INSERT_STATEMENT")},
		{SQL_INTEGRITY,_T("SQL_INTEGRITY")},
		{SQL_KEYWORDS,_T("SQL_KEYWORDS")},
		{SQL_KEYSET_CURSOR_ATTRIBUTES1,_T("SQL_KEYSET_CURSOR_ATTRIBUTES1")},
		{SQL_KEYSET_CURSOR_ATTRIBUTES2, _T("SQL_KEYSET_CURSOR_ATTRIBUTES2")},
		{SQL_LIKE_ESCAPE_CLAUSE,_T("SQL_LIKE_ESCAPE_CLAUSE")},
		{SQL_LOCK_TYPES, _T("SQL_LOCK_TYPES")},
		{SQL_MAX_ASYNC_CONCURRENT_STATEMENTS,_T("SQL_MAX_ASYNC_CONCURRENT_STATEMENTS")},
		{SQL_MAX_BINARY_LITERAL_LEN,_T("SQL_MAX_BINARY_LITERAL_LEN")},
		{SQL_MAX_CATALOG_NAME_LEN,_T("SQL_MAX_CATALOG_NAME_LEN")},
		{SQL_MAX_CHAR_LITERAL_LEN,_T("SQL_MAX_CHAR_LITERAL_LEN")},
		{SQL_MAX_COLUMN_NAME_LEN,_T("SQL_MAX_COLUMN_NAME_LEN")},
		{SQL_MAX_COLUMNS_IN_GROUP_BY,_T("SQL_MAX_COLUMNS_IN_GROUP_BY")},
		{SQL_MAX_COLUMNS_IN_INDEX,_T("SQL_MAX_COLUMNS_IN_INDEX")},
		{SQL_MAX_COLUMNS_IN_ORDER_BY,_T("SQL_MAX_COLUMNS_IN_ORDER_BY")},
		{SQL_MAX_COLUMNS_IN_ORDER_BY,_T("SQL_MAX_COLUMNS_IN_ORDER_BY")},
		{SQL_MAX_COLUMNS_IN_TABLE,_T("SQL_MAX_COLUMNS_IN_TABLE")},
		{SQL_MAX_CONCURRENT_ACTIVITIES,_T("SQL_MAX_CONCURRENT_ACTIVITIES")},
		{SQL_MAX_CURSOR_NAME_LEN,_T("SQL_MAX_CURSOR_NAME_LEN")},
		{SQL_MAX_DRIVER_CONNECTIONS,_T("SQL_MAX_DRIVER_CONNECTIONS")},
		{SQL_MAX_IDENTIFIER_LEN,_T("SQL_MAX_IDENTIFIER_LEN")},
		{SQL_MAX_INDEX_SIZE,_T("SQL_MAX_INDEX_SIZE")},
		{SQL_MAX_PROCEDURE_NAME_LEN,_T("SQL_MAX_PROCEDURE_NAME_LEN")},
		{SQL_MAX_ROW_SIZE,_T("SQL_MAX_ROW_SIZE")},
		{SQL_MAX_ROW_SIZE_INCLUDES_LONG,_T("SQL_MAX_ROW_SIZE_INCLUDES_LONG")},
		{SQL_MAX_SCHEMA_NAME_LEN,_T("SQL_MAX_SCHEMA_NAME_LEN")},
		{SQL_MAX_STATEMENT_LEN,_T("SQL_MAX_STATEMENT_LEN")},
		{SQL_MAX_TABLE_NAME_LEN,_T("SQL_MAX_TABLE_NAME_LEN")},
		{SQL_MAX_TABLES_IN_SELECT,_T("SQL_MAX_TABLES_IN_SELECT")},
		{SQL_MAX_USER_NAME_LEN,_T("SQL_MAX_USER_NAME_LEN")},
		{SQL_MULT_RESULT_SETS,_T("SQL_MULT_RESULT_SETS")},
		{SQL_MULTIPLE_ACTIVE_TXN,_T("SQL_MULTIPLE_ACTIVE_TXN")},
		{SQL_NEED_LONG_DATA_LEN,_T("SQL_NEED_LONG_DATA_LEN")},
		{SQL_NON_NULLABLE_COLUMNS,_T("SQL_NON_NULLABLE_COLUMNS")},
		{SQL_NULL_COLLATION,_T("SQL_NULL_COLLATION")},
		{SQL_NUMERIC_FUNCTIONS,_T("SQL_NUMERIC_FUNCTIONS")},
		{SQL_ODBC_API_CONFORMANCE,_T("SQL_ODBC_API_CONFORMANCE")},
		{SQL_ODBC_INTERFACE_CONFORMANCE,_T("SQL_ODBC_INTERFACE_CONFORMANCE")},
		{SQL_ODBC_SAG_CLI_CONFORMANCE, _T("SQL_ODBC_SAG_CLI_CONFORMANCE")},
		{SQL_ODBC_SQL_CONFORMANCE,_T("SQL_ODBC_SQL_CONFORMANCE")},
//		{SQL_ODBC_VER,_T("SQL_ODBC_VER")},
		{SQL_ODBC_VER,_T("SQL_ODBC_VER")},
		{SQL_OJ_CAPABILITIES,_T("SQL_OJ_CAPABILITIES")},
		{SQL_ORDER_BY_COLUMNS_IN_SELECT,_T("SQL_ORDER_BY_COLUMNS_IN_SELECT")},
		{SQL_OUTER_JOINS,_T("SQL_OUTER_JOINS")},
		{SQL_PARAM_ARRAY_ROW_COUNTS, _T("SQL_PARAM_ARRAY_ROW_COUNTS")},
		{SQL_PARAM_ARRAY_SELECTS, _T("SQL_PARAM_ARRAY_SELECTS")},
		{SQL_POS_OPERATIONS, _T("SQL_POS_OPERATIONS")},
		{SQL_POSITIONED_STATEMENTS, _T("SQL_POSITIONED_STATEMENTS")},
		{SQL_PROCEDURE_TERM,_T("SQL_PROCEDURE_TERM")},
		{SQL_PROCEDURES,_T("SQL_PROCEDURES")},
		{SQL_QUOTED_IDENTIFIER_CASE,_T("SQL_QUOTED_IDENTIFIER_CASE")},
		{SQL_ROW_UPDATES,_T("SQL_ROW_UPDATES")},
		{SQL_SCHEMA_TERM,_T("SQL_SCHEMA_TERM")},
		{SQL_SCHEMA_USAGE,_T("SQL_SCHEMA_USAGE")},
		{SQL_SCROLL_CONCURRENCY, _T("SQL_SCROLL_CONCURRENCY")},
		{SQL_SCROLL_OPTIONS,_T("SQL_SCROLL_OPTIONS")},
		{SQL_SEARCH_PATTERN_ESCAPE,_T("SQL_SEARCH_PATTERN_ESCAPE")},
		{SQL_SERVER_NAME,_T("SQL_SERVER_NAME")},
		{SQL_SPECIAL_CHARACTERS,_T("SQL_SPECIAL_CHARACTERS")},
		{SQL_SQL_CONFORMANCE, _T("SQL_SQL_CONFORMANCE")},
		{SQL_SQL92_DATETIME_FUNCTIONS, _T("SQL_SQL92_DATETIME_FUNCTIONS")},
		{SQL_SQL92_FOREIGN_KEY_DELETE_RULE,_T("SQL_SQL92_FOREIGN_KEY_DELETE_RULE")},
		{SQL_SQL92_FOREIGN_KEY_UPDATE_RULE, _T("SQL_SQL92_FOREIGN_KEY_UPDATE_RULE")},
		{SQL_SQL92_GRANT, _T("SQL_SQL92_GRANT")},
		{SQL_SQL92_NUMERIC_VALUE_FUNCTIONS, _T("SQL_SQL92_NUMERIC_VALUE_FUNCTIONS")},
		{SQL_SQL92_PREDICATES,_T("SQL_SQL92_PREDICATES")},
		{SQL_SQL92_RELATIONAL_JOIN_OPERATORS, _T("SQL_SQL92_RELATIONAL_JOIN_OPERATORS")},
		{SQL_SQL92_REVOKE, _T("SQL_SQL92_REVOKE")},
		{SQL_SQL92_ROW_VALUE_CONSTRUCTOR, _T("SQL_SQL92_ROW_VALUE_CONSTRUCTOR")},
		{SQL_SQL92_STRING_FUNCTIONS, _T("SQL_SQL92_STRING_FUNCTIONS")},
		{SQL_SQL92_VALUE_EXPRESSIONS, _T("SQL_SQL92_VALUE_EXPRESSIONS")},
		{SQL_STANDARD_CLI_CONFORMANCE, _T("SQL_STANDARD_CLI_CONFORMANCE")},
		{SQL_STATIC_CURSOR_ATTRIBUTES1,_T("SQL_STATIC_CURSOR_ATTRIBUTES1")},
		{SQL_STATIC_CURSOR_ATTRIBUTES2, _T("SQL_STATIC_CURSOR_ATTRIBUTES2")},
		{SQL_STATIC_SENSITIVITY, _T("SQL_STATIC_SENSITIVITY")},
		{SQL_STRING_FUNCTIONS,_T("SQL_STRING_FUNCTIONS")},
		{SQL_SUBQUERIES,_T("SQL_SUBQUERIES")},
		{SQL_SYSTEM_FUNCTIONS,_T("SQL_SYSTEM_FUNCTIONS")},
		{SQL_TABLE_TERM,_T("SQL_TABLE_TERM")},
		{SQL_TIMEDATE_ADD_INTERVALS,_T("SQL_TIMEDATE_ADD_INTERVALS")},
		{SQL_TIMEDATE_DIFF_INTERVALS,_T("SQL_TIMEDATE_DIFF_INTERVALS")},
		{SQL_TIMEDATE_FUNCTIONS,_T("SQL_TIMEDATE_FUNCTIONS")},
		{SQL_TXN_CAPABLE,_T("SQL_TXN_CAPABLE")},
		{SQL_TXN_ISOLATION_OPTION,_T("SQL_TXN_ISOLATION_OPTION")},
		{SQL_UNION,_T("SQL_UNION")},
		{SQL_USER_NAME,_T("SQL_USER_NAME")},
		{SQL_XOPEN_CLI_YEAR,_T("SQL_XOPEN_CLI_YEAR")}
	  };
    
    for(i = 0; i < (sizeof(InfoType) / sizeof(InfoType[0])); i++){
        LogMsg(NONE, _T("SQLGetInfo ------- > %s\n"), InfoType[i].name);
        retcode = SQLGetInfo((SQLHANDLE)hdbc,InfoType[i].type,u1.charInfoValue,
								(SWORD)sizeof(u1.charInfoValue),&pcbInfoValue);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLGetInfo fail !\n"));
        }
        else{
            LogMsg(NONE, _T("%d\n%d\n%d\n%s\n"), u1.longInfoValue, u1.intInfoValue, u1.smiInfoValue, u1.charInfoValue);
        }
    }

TEST_FAIL:

    FullDisconnect(&sTestInfo);
    LogMsg(TIME_STAMP, _T("complete test:%s\n"), (ret == 0) ? _T("sucess") : _T("fail"));
    
    return ret;
}

/*******************************************************************************************************************/
void help(void)
{
    printf("\t-h:print this help\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema name\t");
    printf("sample:\t./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST\n");
}
//g++  xxx.c -o debug -I /usr/lib64 -L /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli
//g++ xxx.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug
//./debug -d TRAF_ANSI -u trafodion -p traf123
int main(int argc, char *argv[])
{
    TestInfo oriInfo;
    char buf[256] = {0};
    int num;
 
    memset(&oriInfo, 0, sizeof(oriInfo));
   
    if(argc <= 1){
        help();
        return -1;
    }
    for(num = 1; num < argc; num++){
        if(_tcscmp(argv[num], _T("-h")) == 0){
            help();
            return 0;
        }
        else if((_tcscmp(argv[num], _T("-d")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.DataSource, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-u")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.UserID, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-p")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Password, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-s")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Schema, argv[num]);
        }
    }
    if(_tcslen(oriInfo.DataSource) == 0 ||
        _tcslen(oriInfo.UserID) == 0 ||
        _tcslen(oriInfo.Password) == 0
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    /*
    *linux odbc:core dump
    *unixodbc 2.2.14 bug
    *10.10.14.5
    */
    debugSQLGetInfoNull(&oriInfo);
    //debugSQLGetInfoAll(&oriInfo);
    LogMsg(NONE, _T("\n\n"));
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

