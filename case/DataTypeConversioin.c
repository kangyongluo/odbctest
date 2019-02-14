#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef unixcli

#else
#include <windows.h>
#include <tchar.h>
#endif
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <assert.h>
#ifdef unixcli
#include <signal.h>
#endif

/*******************************************************common start*************************************************************************/

#define CONFIG_TEST_NO_COAST
static FILE *gLogFp = NULL;

#ifndef UNICODE
#define LogMsg(Options,format,...) do{\
        if(gLogFp != NULL){\
            fprintf(gLogFp, format, ##__VA_ARGS__);\
            fflush(gLogFp);\
        } \
        printf(format,##__VA_ARGS__);\
    }while(0)
#else
#define LogMsg(Options,format,...) do{;}while(0)
#endif


#ifdef unixcli
#ifndef TCHAR
#define TCHAR	char//SQLTCHAR
#endif
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
    TCHAR szData[512];
}TestInfo;
#ifdef unixcli
#ifndef UNICODE
#ifndef UNIXODBC
typedef char BOOL;
#endif
typedef char CHAR;

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
#define _tcsftime   strftime    
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

typedef enum PassFail {PASSED, FAILED} PassFail;

#define TEST_DECLARE  do{;}while(0)
#define TEST_INIT  do{;}while(0)
#define TESTCASE_BEGIN(x) LogMsg(SHORTTIMESTAMP+LINEBEFORE, x)
#define TESTCASE_BEGINW(x) LogMsg(SHORTTIMESTAMP+LINEBEFORE, x)
#define TEST_FAILED  LogMsg(SHORTTIMESTAMP, _T("<Test Fail>\n"))
#define TESTCASE_END  do{;}while(0)
#define TEST_RETURN  return 0
#define TESTCASE_LOG(x) LogMsg(SHORTTIMESTAMP+LINEBEFORE,x)


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

typedef short TrueFalse;                         
#define SQL_BOOLEAN 13

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
      LogMsg(NONE, _T("   State: %s\n")
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
        LogMsg(NONE, _T("   State: %s\n")
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
        LogMsg(NONE, _T("   State: %s\n")
							  _T("   Native Error: %ld\n")
                _T("   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		LogMsg(NONE, _T("There were no error messages for SQLError() to display\n"));
}  

TrueFalse FullConnectWithOptions(TestInfo *pTestInfo, int Options)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR OutString[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR connectStr[1024] = {0x0};
    
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
#if 1
	returncode = SQLConnect(hdbc,
                           (SQLTCHAR*)pTestInfo->DataSource,(SWORD)_tcslen(pTestInfo->DataSource),
                           (SQLTCHAR*)pTestInfo->UserID,(SWORD)_tcslen(pTestInfo->UserID),
                           (SQLTCHAR*)pTestInfo->Password,(SWORD)_tcslen(pTestInfo->Password)
                           );
#endif
#if 0   
    _stprintf(connectStr, _T("DSN=%s;UID=%s;PWD=%s;CERTIFICATEFILE=test.cer"), pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password);
    LogMsg(NONE, _T(".............................%s\n"), connectStr);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)connectStr, SQL_NTS, (SQLTCHAR*)OutString, sizeof(OutString), &olen, SQL_DRIVER_NOPROMPT);
#endif
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
TrueFalse FullConnectEx(TestInfo *pTestInfo, int Options, TCHAR *connStrEx)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR OutString[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR connectStr[1024] = {0x0};
    
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
    _stprintf(connectStr, _T("DSN=%s;UID=%s;PWD=%s;%s"), pTestInfo->DataSource, 
                                                        pTestInfo->UserID, 
                                                        pTestInfo->Password,
                                                        (connStrEx != NULL) ? connStrEx : ""
                                                        );
    LogMsg(NONE, "%s\n", connectStr);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)connectStr, SQL_NTS, (SQLTCHAR*)OutString, sizeof(OutString), &olen, SQL_DRIVER_NOPROMPT);
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
}  /* FullDisconnect() */
void openLog(TCHAR *headKey)
{
    time_t tm;
    TCHAR szBuf[128] = {0};
    
    if(gLogFp) return;

    _stprintf(szBuf, "%s_", headKey);
    tm = time(NULL);
    strftime(&szBuf[_tcslen(szBuf)], sizeof(szBuf) - _tcslen(szBuf), "%Y-%m-%d_%H.%M.%S.log", localtime(&tm));
    gLogFp = fopen((char *)szBuf, "a+");
    if(gLogFp == NULL) printf("open log %s fail\n", szBuf);

}
void closeLog(void)
{
    if(gLogFp){
        fclose(gLogFp);
        gLogFp = NULL;
    }
}

/*************************************************************end commmon*******************************************************************/
#define UNSIGNED(x) ((x & 0x0000ffff) | 0x80000000)
#define SIGNED(x) (x & 0x0000ffff)


SQLINTEGER sqlCtypeArr[] = {
    SQL_C_CHAR, SQL_C_WCHAR, SQL_C_BIT, SQL_C_NUMERIC, SQL_C_STINYINT, SQL_C_UTINYINT, SQL_C_TINYINT, SQL_C_SBIGINT, SQL_C_UBIGINT, SQL_C_SSHORT, SQL_C_USHORT,
    SQL_C_SHORT, SQL_C_SLONG, SQL_C_ULONG, SQL_C_LONG, SQL_C_FLOAT, SQL_C_DOUBLE, SQL_C_BINARY, SQL_C_TYPE_DATE, SQL_C_TYPE_TIME, SQL_C_TYPE_TIMESTAMP,
    SQL_C_INTERVAL_YEAR,SQL_C_INTERVAL_MONTH,SQL_C_INTERVAL_DAY,SQL_C_INTERVAL_HOUR,SQL_C_INTERVAL_MINUTE,SQL_C_INTERVAL_SECOND,SQL_C_INTERVAL_YEAR_TO_MONTH,
    SQL_C_INTERVAL_DAY_TO_HOUR,SQL_C_INTERVAL_DAY_TO_MINUTE,SQL_C_INTERVAL_DAY_TO_SECOND,SQL_C_INTERVAL_HOUR_TO_MINUTE,SQL_C_INTERVAL_HOUR_TO_SECOND,SQL_C_INTERVAL_MINUTE_TO_SECOND
};
static unsigned int gTotalTest = 0;
static unsigned int gTotalPass = 0;
static unsigned int gTotalFail = 0;
TCHAR *ConvertSqlCTypeString(SQLINTEGER SQLCType, TCHAR *TypeCStr)
{
   switch (SQLCType){
      case SQL_C_CHAR:			
        _stprintf (TypeCStr,_T("SQL_C_CHAR"));
        break;
      case SQL_C_WCHAR:			
        _stprintf (TypeCStr,_T("SQL_C_WCHAR"));
        break;
	  case SQL_C_LONG:			
        _stprintf (TypeCStr,_T("SQL_C_LONG"));
        break;
      case SQL_C_SHORT:			
        _stprintf (TypeCStr,_T("SQL_C_SHORT"));
        break;
      case SQL_C_FLOAT:			
        _stprintf (TypeCStr,_T("SQL_C_FLOAT"));
        break;
      case SQL_C_DOUBLE:		
        _stprintf (TypeCStr,_T("SQL_C_DOUBLE"));
        break;
      case SQL_C_NUMERIC:		
        _stprintf (TypeCStr,_T("SQL_C_NUMERIC"));
        break;
      case SQL_C_DEFAULT:		
        _stprintf (TypeCStr,_T("SQL_C_DEFAULT"));
        break;
      case SQL_C_DATE:			
        _stprintf (TypeCStr,_T("SQL_C_DATE"));
        break;
      case SQL_C_TIME:			
        _stprintf (TypeCStr,_T("SQL_C_TIME"));
        break;
      case SQL_C_TIMESTAMP:			
        _stprintf (TypeCStr,_T("SQL_C_TIMESTAMP"));
        break;
      case SQL_C_TYPE_DATE:			
        _stprintf (TypeCStr,_T("SQL_C_TYPE_DATE"));
        break;
      case SQL_C_TYPE_TIME:			
        _stprintf (TypeCStr,_T("SQL_C_TYPE_TIME"));
        break;
      case SQL_C_TYPE_TIMESTAMP:		
        _stprintf (TypeCStr,_T("SQL_C_TYPE_TIMESTAMP"));
        break;
      case SQL_C_INTERVAL_YEAR:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_YEAR"));
        break;
      case SQL_C_INTERVAL_MONTH:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_MONTH"));
        break;
      case SQL_C_INTERVAL_DAY:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_DAY"));
        break;
      case SQL_C_INTERVAL_HOUR:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_HOUR"));
        break;
      case SQL_C_INTERVAL_MINUTE:	
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_MINUTE"));
        break;
      case SQL_C_INTERVAL_SECOND:	
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_SECOND"));
        break;
      case SQL_C_INTERVAL_YEAR_TO_MONTH:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_YEAR_TO_MONTH"));
        break;
      case SQL_C_INTERVAL_DAY_TO_HOUR:			
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_DAY_TO_HOUR"));
        break;
      case SQL_C_INTERVAL_DAY_TO_MINUTE:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_DAY_TO_MINUTE"));
        break;
      case SQL_C_INTERVAL_DAY_TO_SECOND:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_DAY_TO_SECOND"));
        break;
      case SQL_C_INTERVAL_HOUR_TO_MINUTE:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_HOUR_TO_MINUTE"));
        break;
      case SQL_C_INTERVAL_HOUR_TO_SECOND:		
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_HOUR_TO_SECOND"));
        break;
      case SQL_C_INTERVAL_MINUTE_TO_SECOND:	
        _stprintf (TypeCStr,_T("SQL_C_INTERVAL_MINUTE_TO_SECOND"));
        break;
      case SQL_C_BINARY:		
        _stprintf (TypeCStr,_T("SQL_C_BINARY"));
        break;
      case SQL_C_BIT:			
        _stprintf (TypeCStr,_T("SQL_C_BIT"));
        break;
      case SQL_C_SBIGINT:		
        _stprintf (TypeCStr,_T("SQL_C_SBIGINT"));
        break;
      case SQL_C_UBIGINT:		
        _stprintf (TypeCStr,_T("SQL_C_UBIGINT"));
        break;
      case SQL_C_TINYINT:		
        _stprintf (TypeCStr,_T("SQL_C_TINYINT"));
        break;
      case SQL_C_SLONG:			
        _stprintf (TypeCStr,_T("SQL_C_SLONG"));
        break;
      case SQL_C_SSHORT:		
        _stprintf (TypeCStr,_T("SQL_C_SSHORT"));
        break;
      case SQL_C_STINYINT:		
        _stprintf (TypeCStr,_T("SQL_C_STINYINT"));
        break;
      case SQL_C_ULONG:			
        _stprintf (TypeCStr,_T("SQL_C_ULONG"));
        break;
      case SQL_C_USHORT:		
        _stprintf (TypeCStr,_T("SQL_C_USHORT"));
        break;
      case SQL_C_UTINYINT:		
        _stprintf (TypeCStr,_T("SQL_C_UTINYINT"));
        break;
      default:
         _stprintf (TypeCStr,_T("UNKNOW"));
	}
   
    return TypeCStr;
}

TCHAR *ConvertSqlTypeString(SQLINTEGER SQLType, TCHAR *TypeStr)
{
    switch(SQLType){
    case SQL_CHAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_CHAR"));
        break;
    case SQL_VARCHAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_VARCHAR"));
        break;
    case SQL_LONGVARCHAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_LONGVARCHAR"));
        break;
    case SQL_WCHAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_WCHAR"));
        break;
    case SQL_WVARCHAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_WVARCHAR"));
        break;
    case SQL_WLONGVARCHAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_WLONGVARCHAR"));
        break;
    case SQL_DECIMAL:
        _stprintf(TypeStr, _T("%s"), _T("SQL_DECIMAL"));
        break;
    case SQL_NUMERIC:
        _stprintf(TypeStr, _T("%s"), _T("SQL_NUMERIC"));
        break;
    case SQL_BIT:
        _stprintf(TypeStr, _T("%s"), _T("SQL_BIT"));
        break;
    case SIGNED(SQL_TINYINT):
        _stprintf(TypeStr, _T("%s"), _T("SQL_TINYINT"));
        break;
    case UNSIGNED(SQL_TINYINT):
        _stprintf(TypeStr, _T("%s"), _T("SQL_TINYINT"));
        break;
    case SIGNED(SQL_SMALLINT):
        _stprintf(TypeStr, _T("%s"), _T("SQL_SMALLINT"));
        break;
    case UNSIGNED(SQL_SMALLINT):
        _stprintf(TypeStr, _T("%s"), _T("SQL_SMALLINT"));
        break;
    case SIGNED(SQL_INTEGER):
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTEGER"));
        break;
    case UNSIGNED(SQL_INTEGER):
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTEGER"));
        break;
    case SIGNED(SQL_BIGINT):
        _stprintf(TypeStr, _T("%s"), _T("SQL_BIGINT"));
        break;
    case UNSIGNED(SQL_BIGINT):
        _stprintf(TypeStr, _T("%s"), _T("SQL_BIGINT"));
        break;
    case SQL_REAL:
        _stprintf(TypeStr, _T("%s"), _T("SQL_REAL"));
        break;
    case SQL_FLOAT:
        _stprintf(TypeStr, _T("%s"), _T("SQL_FLOAT"));
        break;
    case SQL_DOUBLE:
        _stprintf(TypeStr, _T("%s"), _T("SQL_DOUBLE"));
        break;
    case SQL_BINARY:
        _stprintf(TypeStr, _T("%s"), _T("SQL_BINARY"));
        break;
    case SQL_VARBINARY:
        _stprintf(TypeStr, _T("%s"), _T("SQL_VARBINARY"));
        break;
    case SQL_LONGVARBINARY:
        _stprintf(TypeStr, _T("%s"), _T("SQL_LONGVARBINARY"));
        break;
    case SQL_TYPE_DATE:
        _stprintf(TypeStr, _T("%s"), _T("SQL_TYPE_DATE"));
        break;
    case SQL_TYPE_TIME:
        _stprintf(TypeStr, _T("%s"), _T("SQL_TYPE_TIME"));
        break;
    case SQL_TYPE_TIMESTAMP:
        _stprintf(TypeStr, _T("%s"), _T("SQL_TYPE_TIMESTAMP"));
        break;
    case SQL_BOOLEAN:
        _stprintf(TypeStr, _T("%s"), _T("SQL_BOOLEAN"));
        break;
    case SQL_INTERVAL_YEAR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_YEAR"));
        break;
    case SQL_INTERVAL_MONTH:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_MONTH"));
        break;
    case SQL_INTERVAL_DAY:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_DAY"));
        break;
    case SQL_INTERVAL_HOUR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_HOUR"));
        break;
    case SQL_INTERVAL_MINUTE:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_MINUTE"));
        break;
    case SQL_INTERVAL_SECOND:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_SECOND"));
        break;
    case SQL_INTERVAL_YEAR_TO_MONTH:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_YEAR_TO_MONTH"));
        break;
    case SQL_INTERVAL_DAY_TO_HOUR:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_DAY_TO_HOUR"));
        break;
    case SQL_INTERVAL_DAY_TO_MINUTE:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_DAY_TO_MINUTE"));
        break;
    case SQL_INTERVAL_DAY_TO_SECOND:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_DAY_TO_SECOND"));
        break;
    case SQL_INTERVAL_HOUR_TO_MINUTE:
       _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_HOUR_TO_MINUTE"));
        break;
    case SQL_INTERVAL_HOUR_TO_SECOND:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_HOUR_TO_SECOND"));
        break;
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        _stprintf(TypeStr, _T("%s"), _T("SQL_INTERVAL_MINUTE_TO_SECOND"));
        break;
    default:
        _stprintf(TypeStr, _T("%s"), _T("SQL_CHAR"));
        break;
    }
    
    return TypeStr;
}
void createTestDDLString(SQLINTEGER SQLType, TCHAR *tableName, SQLCHAR precision, SQLSCHAR scale,TCHAR *ddlStr)
{
    TCHAR szBuf[32] = {0};
    
    switch(SQLType){
    case SQL_CHAR:
        _stprintf(szBuf, _T("%s"), _T("char(32)"));
        break;
    case SQL_VARCHAR:
        _stprintf(szBuf, _T("%s"), _T("varchar(32)"));
        break;
    case SQL_LONGVARCHAR:
        _stprintf(szBuf, _T("%s"), _T("LONGVARCHAR(32)"));
        break;
    case SQL_WCHAR:
        _stprintf(szBuf, _T("%s"), _T("WCHAR(32)"));
        break;
    case SQL_WVARCHAR:
        _stprintf(szBuf, _T("%s"), _T("WVARCHAR"));
        break;
    case SQL_WLONGVARCHAR:
        _stprintf(szBuf, _T("%s"), _T("WLONGVARCHAR"));
        break;
    case SQL_DECIMAL:
        _stprintf(szBuf, _T("decimal(%d,%d)"), precision, scale);
        break;
    case SQL_NUMERIC:
        _stprintf(szBuf, _T("numeric(%d,%d)"), precision, scale);
        break;
    case SQL_BIT:
        _stprintf(szBuf, _T("%s"), _T("BIT"));
        break;
    case SIGNED(SQL_TINYINT):
        _stprintf(szBuf, _T("%s"), _T("TINYINT"));
        break;
    case UNSIGNED(SQL_TINYINT):
        _stprintf(szBuf, _T("%s"), _T("TINYINT UNSIGNED"));
        break;
    case SIGNED(SQL_SMALLINT):
        _stprintf(szBuf, _T("%s"), _T("SMALLINT"));
        break;
    case UNSIGNED(SQL_SMALLINT):
        _stprintf(szBuf, _T("%s"), _T("SMALLINT UNSIGNED"));
        break;
    case SIGNED(SQL_INTEGER):
        _stprintf(szBuf, _T("%s"), _T("int"));
        break;
    case UNSIGNED(SQL_INTEGER):
        _stprintf(szBuf, _T("%s"), _T("int unsigned"));
        break;
    case SIGNED(SQL_BIGINT):
        _stprintf(szBuf, _T("%s"), _T("LARGEINT"));
        break;
    case UNSIGNED(SQL_BIGINT):
        _stprintf(szBuf, _T("%s"), _T("LARGEINT UNSIGNED"));
        break;
    case SQL_REAL:
        _stprintf(szBuf, _T("%s"), _T("REAL"));
        break;
    case SQL_FLOAT:
        _stprintf(szBuf, _T("%s"), _T("FLOAT"));
        break;
    case SQL_DOUBLE:
        _stprintf(szBuf, _T("%s"), _T("DOUBLE PRECISION"));
        break;
    case SQL_BINARY:
        _stprintf(szBuf, _T("%s"), _T("binary"));
        break;
    case SQL_VARBINARY:
        _stprintf(szBuf, _T("%s"), _T("varbinary"));
        break;
    case SQL_LONGVARBINARY:
        _stprintf(szBuf, _T("%s"), _T("longvarbinary"));
        break;
    case SQL_TYPE_DATE:
        _stprintf(szBuf, _T("%s"), _T("DATE"));
        break;
    case SQL_TYPE_TIME:
        _stprintf(szBuf, _T("%s"), _T("TIME"));
        break;
    case SQL_TYPE_TIMESTAMP:
        _stprintf(szBuf, _T("%s"), _T("TIMESTAMP"));
        break;
    case SQL_BOOLEAN:
        _stprintf(szBuf, _T("%s"), _T("BOOLEAN"));
        break;
    case SQL_INTERVAL_YEAR:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL YEAR(4)"));
        break;
    case SQL_INTERVAL_MONTH:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL MONTH"));
        break;
    case SQL_INTERVAL_DAY:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL DAY(4)"));
        break;
    case SQL_INTERVAL_HOUR:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL HOUR"));
        break;
    case SQL_INTERVAL_MINUTE:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL MINUTE"));
        break;
    case SQL_INTERVAL_SECOND:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL SECOND"));
        break;
    case SQL_INTERVAL_YEAR_TO_MONTH:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL YEAR TO MONTH"));
        break;
    case SQL_INTERVAL_DAY_TO_HOUR:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL DAY TO HOUR"));
        break;
    case SQL_INTERVAL_DAY_TO_MINUTE:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL DAY TO MINUTE"));
        break;
    case SQL_INTERVAL_DAY_TO_SECOND:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL DAY TO SECOND"));
        break;
    case SQL_INTERVAL_HOUR_TO_MINUTE:
       _stprintf(szBuf, _T("%s"), _T("INTERVAL HOUR TO MINUTE"));
        break;
    case SQL_INTERVAL_HOUR_TO_SECOND:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL HOUR TO SECOND"));
        break;
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        _stprintf(szBuf, _T("%s"), _T("INTERVAL MINUTE TO SECOND"));
        break;
    default:
        _stprintf(szBuf, _T("%s"), _T("CHAR(32)"));
        break;
    }

    _stprintf(ddlStr, _T("create table %s(c1 int, c2 %s)"), tableName, szBuf);
}
void createSQLDataChar(SQLINTEGER SQLType, SQLCHAR precision, SQLSCHAR scale,TCHAR *valBuf)
{    
    switch(SQLType){
    case SQL_CHAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_VARCHAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_LONGVARCHAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_WCHAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_WVARCHAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_WLONGVARCHAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_DECIMAL:
        _stprintf(valBuf, _T("%s"), _T("1.0"));
        break;
    case SQL_NUMERIC:
        _stprintf(valBuf, _T("%s"), _T("1.0"));
        break;
    case SQL_BIT:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SIGNED(SQL_TINYINT):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case UNSIGNED(SQL_TINYINT):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SIGNED(SQL_SMALLINT):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case UNSIGNED(SQL_SMALLINT):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SIGNED(SQL_INTEGER):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case UNSIGNED(SQL_INTEGER):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SIGNED(SQL_BIGINT):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case UNSIGNED(SQL_BIGINT):
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_REAL:
        _stprintf(valBuf, _T("%s"), _T("1.0"));
        break;
    case SQL_FLOAT:
        _stprintf(valBuf, _T("%s"), _T("1.0"));
        break;
    case SQL_DOUBLE:
        _stprintf(valBuf, _T("%s"), _T("1.0"));
        break;
    case SQL_BINARY:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_VARBINARY:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_LONGVARBINARY:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_DATE:
    case SQL_TYPE_DATE:
        _stprintf(valBuf, _T("%s"), _T("2018-07-06"));
        break;
    case SQL_TIME:
    case SQL_TYPE_TIME:
        _stprintf(valBuf, _T("%s"), _T("08:23:40"));
        break;
    case SQL_TIMESTAMP:
    case SQL_TYPE_TIMESTAMP:
        _stprintf(valBuf, _T("%s"), _T("2006-09-18 23:59:59"));
        break;
    case SQL_BOOLEAN:
        _stprintf(valBuf, _T("%s"), _T("FALSE"));
        break;
    case SQL_INTERVAL_YEAR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_INTERVAL_MONTH:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_INTERVAL_DAY:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_INTERVAL_HOUR:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_INTERVAL_MINUTE:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_INTERVAL_SECOND:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    case SQL_INTERVAL_YEAR_TO_MONTH:
        _stprintf(valBuf, _T("%s"), _T("1-1"));
        break;
    case SQL_INTERVAL_DAY_TO_HOUR:
        _stprintf(valBuf, _T("%s"), _T("1 10"));
        break;
    case SQL_INTERVAL_DAY_TO_MINUTE:
        _stprintf(valBuf, _T("%s"), _T("1 12:12"));
        break;
    case SQL_INTERVAL_DAY_TO_SECOND:
        _stprintf(valBuf, _T("%s"), _T("1 01:01:02.000000"));
        break;
    case SQL_INTERVAL_HOUR_TO_MINUTE:
       _stprintf(valBuf, _T("%s"), _T("1:10"));
        break;
    case SQL_INTERVAL_HOUR_TO_SECOND:
        _stprintf(valBuf, _T("%s"), _T("1:01:12.000000"));
        break;
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        _stprintf(valBuf, _T("%s"), _T("1:20"));
        break;
    default:
        _stprintf(valBuf, _T("%s"), _T("1"));
        break;
    }

}
void convertCharToWchar(TCHAR *valChar, TCHAR *valWchar)
{
    int i, j;

    for(i = 0, j = 0; i < _tcslen(valChar); i++){
        valWchar[j++] = valChar[i];
        valWchar[j++] = 0x00;
    }
}
int doConvertSQL(TestInfo *pTestInfo, SQLINTEGER SQLType, SQLCHAR precision, SQLSCHAR scale)
{
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
    SQLLEN len;
    SQLLEN len1;
    RETCODE retcode;
    unsigned int id = 0;
    unsigned int iSqlCType;
    SQLINTEGER iSQLType = SQLType;
    TCHAR szChar[128] = {0};
    TCHAR szBuf[128] = {0};
    TCHAR szTemp[128];
    unsigned char szWchar[128] = {0};
    unsigned char iu8 = 1;
    unsigned short iu16 = 1;
    unsigned int iu32 = 1;
    unsigned long long iu64 = 1;
    DATE_STRUCT sDate;
    TIME_STRUCT sTime;
    TIMESTAMP_STRUCT sTimestamp;
    SQL_NUMERIC_STRUCT sNumeric;
    SQL_INTERVAL_STRUCT sInterval;
    SQLDOUBLE dVal = 1.0;
    SQLREAL fVal = 1.0;
    unsigned int totalPass, totalFail;
    SWORD numOfCols, index, len2;

    memset(&sDate, 0, sizeof(sDate));
    memset(&sTime, 0, sizeof(sTime));
    memset(&sTimestamp, 0, sizeof(sTimestamp));
    memset(&sNumeric, 0, sizeof(sNumeric));
    memset(&sInterval, 0, sizeof(sInterval));
    totalPass = 0;
    totalFail = 0;
    TCHAR szTableName[] = _T("tb_conversion_test");
    unsigned char szBindStat[64] = {0};
    unsigned char szCheckStat[64] = {0};
  
    if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrors(henv,hdbc,hstmt);
        return -1;
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;

    //Allocate Statement handles
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle fail:line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            disconnect(henv, hdbc, hstmt);
            return -1;
        }
    }

#if 0
    _stprintf(szBuf, _T("DROP TABLE IF EXISTS %s"), szTableName);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    createTestDDLString(SQLType, szTableName, precision, scale, szBuf);
    LogMsg(NONE, _T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
#else
    _stprintf(szBuf, _T("purgedata %s"), szTableName);
    LogMsg(NONE, _T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    retcode = SQL_SUCCESS;
#endif
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_CONVERT_FAIL;
    }
    for(iSqlCType = 0, id = 0; iSqlCType < (sizeof(sqlCtypeArr) / sizeof(sqlCtypeArr[0])); iSqlCType++,  id++){
        len = SQL_NTS;
        len1 = SQL_NTS;

        if(iSqlCType != 0){
            SQLFreeStmt(hstmt, SQL_CLOSE);
            SQLFreeStmt(hstmt, SQL_UNBIND);
        }
        
        LogMsg(NONE, _T("\n%s to %s:\n"), ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                      ConvertSqlTypeString(SQLType, szChar));
        _stprintf(szBuf, _T("insert into %s values(?,?)"), szTableName);
        LogMsg(NONE, _T("SQLPrepare(hstmt,(SQLTCHAR*)%s, SQL_NTS);\n"), szBuf);
        retcode = SQLPrepare(hstmt,(SQLTCHAR*)szBuf, SQL_NTS);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLPrepare  fail\n"));
            LogAllErrors(henv, hdbc, hstmt);
            totalFail++;
            continue;
        }
        LogMsg(NONE, _T("SQLBindParameter(hstmt,1,SQL_PARAM_INPUT,SQL_C_ULONG, SQL_INTEGER, &id(id=%d), %d, SQL_NTS);\n"), id, sizeof(id));
        retcode = SQLBindParameter(hstmt,1,SQL_PARAM_INPUT,SQL_C_ULONG, SQL_INTEGER,0,0,&id,sizeof(id),&len);
    	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLBindParameter 1 fail\n"));
            LogAllErrors(henv, hdbc, hstmt);
            totalFail++;
            continue;
        }
        iSQLType = SQLType;
        if(SQLType == SQL_BOOLEAN) iSQLType = SQL_CHAR;
        switch(sqlCtypeArr[iSqlCType]){
        case SQL_C_CHAR:
            memset(szChar, 0, sizeof(szChar));
            createSQLDataChar(SQLType, precision, scale, szChar);
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, \"%s\", %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                szChar,
                                                                                sizeof(szChar));
    	    retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,szChar,sizeof(szChar),&len1);
            break;
        case SQL_C_WCHAR:
            memset(szChar, 0, sizeof(szChar));
            memset(szWchar, 0, sizeof(szWchar));
            createSQLDataChar(SQLType, precision, scale, szChar);
            convertCharToWchar(szChar, szWchar);
            
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,%s, %s, %d, %d, szWchar( "), 
                                                                                 ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                precision,
                                                                                scale);
            len2 = _tcslen(szChar) * 2;
            for(index = 0; index < len2; index++){
                LogMsg(NONE, _T("0x%x "), szWchar[index]);
            }
            LogMsg(NONE, _T("), "));
            LogMsg(NONE, _T("%d, SQL_NTS);\n"), len2 / 2);
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,precision,scale,szWchar,len2 / 2,&len1);
            break;
        case SQL_C_BIT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu8(iu8=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu8,
                                                                                sizeof(iu8));
    	    retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu8,sizeof(iu8),&len1);
            break;
        case SQL_C_NUMERIC:
            sNumeric.scale = 0;
            sNumeric.precision = 1;
            sNumeric.val[0] = 1;
            sNumeric.sign = 1;
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &sNumeric, %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                precision,
                                                                                scale,
                                                                                sizeof(sNumeric));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,precision,scale,&sNumeric,sizeof(sNumeric),&len1);
            break;
        case SQL_C_STINYINT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu8(iu8=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu8,
                                                                                sizeof(iu8));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu8,sizeof(iu8),&len1);
            break;
        case SQL_C_UTINYINT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu8(iu8=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu8,
                                                                                sizeof(iu8));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu8,sizeof(iu8),&len1);
            break;
        case SQL_C_TINYINT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu8(iu8=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu8,
                                                                                sizeof(iu8));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu8,sizeof(iu8),&len1);
            break;
        case SQL_C_SBIGINT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu64(iu64=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu64,
                                                                                sizeof(iu64));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu64,sizeof(iu64),&len1);
            break;
        case SQL_C_UBIGINT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu64(iu64=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu64,
                                                                                sizeof(iu64));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu64,sizeof(iu64),&len1);
            break;
        case SQL_C_SSHORT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu16(iu16=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu16,
                                                                                sizeof(iu16));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu16,sizeof(iu16),&len1);
            break;
        case SQL_C_USHORT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu16(iu16=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu16,
                                                                                sizeof(iu16));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu16,sizeof(iu16),&len1);
            break;
        case SQL_C_SHORT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu16(iu16=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu16,
                                                                                sizeof(iu16));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu16,sizeof(iu16),&len1);
            break;
        case SQL_C_SLONG:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu32(iu32=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu32,
                                                                                sizeof(iu32));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu32,sizeof(iu32),&len1);
            break;
        case SQL_C_ULONG:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu32(iu32=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu32,
                                                                                sizeof(iu32));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu32,sizeof(iu32),&len1);
            break;
        case SQL_C_LONG:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu32(iu32=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu32,
                                                                                sizeof(iu32));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu32,sizeof(iu32),&len1);
            break;
        case SQL_C_FLOAT:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &fVal(fVal=%f), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                fVal,
                                                                                sizeof(fVal));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&fVal,sizeof(fVal),&len1);
            break;
        case SQL_C_DOUBLE:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &dVal(dVal=%f), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                dVal,
                                                                                sizeof(dVal));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&dVal,sizeof(dVal),&len1);
            break;
        case SQL_C_BINARY:
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &iu8(iu8=%d), %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                iu8,
                                                                                sizeof(iu8));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&iu8,sizeof(iu8),&len1);
            break;
        case SQL_C_TYPE_DATE:
            sDate.year = 1;
            sDate.month = 1;
            sDate.day = 1;
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &sDate, %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                sizeof(sDate));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&sDate,sizeof(sDate),&len1);
            break;
        case SQL_C_TYPE_TIME:
            sTime.hour = 0;
            sTime.minute = 0;
            sTime.second = 1;
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &sTime, %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                sizeof(sTime));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&sTime,sizeof(sTime),&len1);
            break;
        case SQL_C_TYPE_TIMESTAMP:
            sTimestamp.year = 1;
            sTimestamp.month = 1;
            sTimestamp.day = 1;
            sTimestamp.hour = 0;
            sTimestamp.minute = 0;
            sTimestamp.second = 1;
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &sTimestamp, %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                sizeof(sTimestamp));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&sTimestamp,sizeof(sTimestamp),&len1);
            break;
        case SQL_C_INTERVAL_YEAR:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_YEAR){
                sInterval.intval.year_month.year = 1;
            }
        case SQL_C_INTERVAL_MONTH:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_MONTH){
                sInterval.intval.year_month.month = 1;
            }
        case SQL_C_INTERVAL_DAY:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_DAY){
                sInterval.intval.day_second.day = 1;
            }
        case SQL_C_INTERVAL_HOUR:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_HOUR){
                sInterval.intval.day_second.hour = 1;
            }
        case SQL_C_INTERVAL_MINUTE:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_MINUTE){
                sInterval.intval.day_second.minute = 1;
            }
        case SQL_C_INTERVAL_SECOND:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_SECOND){
                sInterval.intval.day_second.second = 1;
            }
        case SQL_C_INTERVAL_YEAR_TO_MONTH:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_YEAR_TO_MONTH){
                sInterval.intval.year_month.year = 0;
                sInterval.intval.year_month.month = 1;
            }
        case SQL_C_INTERVAL_DAY_TO_HOUR:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_DAY_TO_HOUR){
                sInterval.intval.day_second.day = 0;
                sInterval.intval.day_second.hour = 1;
            }
        case SQL_C_INTERVAL_DAY_TO_MINUTE:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_DAY_TO_MINUTE){
                sInterval.intval.day_second.day = 0;
                sInterval.intval.day_second.hour = 0;
                sInterval.intval.day_second.minute = 1;
            }
        case SQL_C_INTERVAL_DAY_TO_SECOND:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_DAY_TO_SECOND){
                sInterval.intval.day_second.day = 0;
                sInterval.intval.day_second.second = 1;
            }
        case SQL_C_INTERVAL_HOUR_TO_MINUTE:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_HOUR_TO_MINUTE){
                sInterval.intval.day_second.hour = 0;
                sInterval.intval.day_second.minute = 1;
            }
        case SQL_C_INTERVAL_HOUR_TO_SECOND:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_HOUR_TO_SECOND){
                sInterval.intval.day_second.hour = 0;
                sInterval.intval.day_second.second = 1;
            }
        case SQL_C_INTERVAL_MINUTE_TO_SECOND:
            if(sqlCtypeArr[iSqlCType] == SQL_C_INTERVAL_MINUTE_TO_SECOND){
                sInterval.intval.day_second.minute = 0;
                sInterval.intval.day_second.second = 1;
            }
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, &sInterval, %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                sizeof(sInterval));
            retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,sqlCtypeArr[iSqlCType], iSQLType,0,0,&sInterval,sizeof(sInterval),&len1);
            break;
        default:
            memset(szChar, 0, sizeof(szChar));
            createSQLDataChar(SQLType, precision, scale, szChar);
            LogMsg(NONE, _T("SQLBindParameter(hstmt,2,SQL_PARAM_INPUT, %s, %s, %d, %d, \"%s\", %d, SQL_NTS);\n"), 
                                                                                ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf),
                                                                                ConvertSqlTypeString(iSQLType, szTemp),
                                                                                0,
                                                                                0,
                                                                                szChar,
                                                                                _tcslen(szChar));
    	    retcode = SQLBindParameter(hstmt,2,SQL_PARAM_INPUT,SQL_C_TCHAR, iSQLType,0,0,szChar,sizeof(szChar),&len1);
            break;
        }
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLBindParameter 2 fail\n"));
            LogAllErrors(henv, hdbc, hstmt);
            totalFail++;
            szBindStat[iSqlCType] = 0;
        }
        else{
            LogMsg(NONE, _T("SQLExecute(hstmt);\n"));
            retcode = SQLExecute(hstmt);
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("execute fail: line %d\n"), __LINE__);
                LogAllErrors(henv, hdbc, hstmt);
                totalFail++;
                szBindStat[iSqlCType] = 0;
            }
            else{
                szBindStat[iSqlCType] = 1;
                totalPass++;
            }
        }
    }

    LogMsg(NONE, _T("Total insert rows:%d, pass=%d fail=%d\n"), iSqlCType, totalPass, totalFail);
    if(totalFail >= iSqlCType){
        goto TEST_CONVERT_FAIL;
    }
    SQLFreeStmt(hstmt, SQL_CLOSE);
    SQLFreeStmt(hstmt, SQL_UNBIND);

    totalPass = 0;
    totalFail = 0;
    LogMsg(NONE, _T("...........................................................................................................\n"));
    LogMsg(NONE, _T("Checkout insert data:\n"));
    for(id = 0; id < (sizeof(sqlCtypeArr) / sizeof(sqlCtypeArr[0])); id++){
        LogMsg(NONE, _T("Checkout %s:"), ConvertSqlCTypeString(sqlCtypeArr[id], szBuf));
        _stprintf(szBuf, _T("select c2 from %s where c1=%d"), szTableName, id);
        LogMsg(NONE, _T("%s\n"), szBuf);
        retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
        if ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO)){
            retcode = SQLFetch(hstmt);
            numOfCols = 0;
            retcode = SQLNumResultCols(hstmt, &numOfCols);
            if((retcode != SQL_NO_DATA) && (numOfCols != 0)){
                for (index = 1; index <= numOfCols; index++){
                    retcode = SQLGetData(hstmt, index, SQL_C_TCHAR, (SQLPOINTER)szBuf, sizeof(szBuf), &len);
                    if (SQL_SUCCEEDED(retcode)){
                        LogMsg(NONE, _T("%s "), szBuf);
                        szCheckStat[id] = 1;
                        totalPass++;
                    }
                    else{
                        szCheckStat[id] = 0;
                        totalFail++;
                        LogMsg(NONE, _T("Can not get data! Please check insert...\n"), __LINE__);
                        break;
                    }
                }
                LogMsg(NONE, _T("\n"));
                retcode = SQLFetch(hstmt);
            }
            else{
                szCheckStat[id] = 0;
                totalFail++;
                LogMsg(NONE, _T("Can not found data! Please check insert...\n"), __LINE__);
            }
        }
        else{
            LogMsg(NONE, _T("select fail,line %d\n"), __LINE__);
            LogAllErrors(henv, hdbc, hstmt);
        }
    }

    LogMsg(NONE, _T("Total check rows:%d, pass=%d fail=%d\n"), id, totalPass, totalFail);
    LogMsg(NONE, _T("Sumary:\n"));
    for(iSqlCType = 0; iSqlCType < (sizeof(sqlCtypeArr) / sizeof(sqlCtypeArr[0])); iSqlCType++){
        _stprintf(szChar, _T("%s"), ConvertSqlCTypeString(sqlCtypeArr[iSqlCType], szBuf));
        if(_tcslen(szChar) < 35){
            for(id = _tcslen(szChar); id < 35; id++){
                szChar[id] = _T(" ")[0];
            }
            szChar[id] = 0;
        }
        LogMsg(NONE, _T("%s   <--->     "), szChar);
        _stprintf(szChar, _T("%s"), ConvertSqlTypeString(SQLType, szBuf));
        if(_tcslen(szChar) < 30){
            for(id = _tcslen(szChar); id < 30; id++){
                szChar[id] = _T(" ")[0];
            }
            szChar[id] = 0;
        }
        LogMsg(NONE, _T("%s  "), szChar);
        LogMsg(NONE, _T("%s\n"), ((szBindStat[iSqlCType] == 1) && (szCheckStat[iSqlCType] == 1)) ? _T("pass") : _T("fail"));
        if((szBindStat[iSqlCType] == 1) && (szCheckStat[iSqlCType] == 1)) gTotalPass++;
        else gTotalFail++;
        gTotalTest++;
    }
    LogMsg(NONE, _T("...........................................................................................................\n"));
TEST_CONVERT_FAIL:    
    disconnect(henv, hdbc, hstmt);
    
    return 0;
}
int testConversion(TestInfo *pTestInfo)
{
    struct sqlTypeInfo_
    {
        SQLINTEGER sqlType;
        SQLCHAR precision;
        SQLSCHAR scale;
    } sqlTypeInfo[] = {
            {SQL_CHAR, 0, 0},
            {SQL_VARCHAR, 0, 0}, 
            {SQL_LONGVARCHAR, 0, 0},
            {SQL_WCHAR, 0, 0},
            {SQL_WVARCHAR, 0, 0}, 
            {SQL_WLONGVARCHAR, 0, 0},
            {SQL_DECIMAL, 10, 2}, 
            {SQL_NUMERIC, 10, 2}, 
            {SQL_BIT, 0, 0}, 
            {SIGNED(SQL_TINYINT), 0, 0}, 
            {UNSIGNED(SQL_TINYINT), 0, 0},
            {SIGNED(SQL_SMALLINT), 0, 0},
            {UNSIGNED(SQL_SMALLINT), 0, 0},
            {SIGNED(SQL_INTEGER), 0, 0},
            {UNSIGNED(SQL_INTEGER), 0, 0},
            {SIGNED(SQL_BIGINT), 0, 0},
            {UNSIGNED(SQL_BIGINT), 0, 0},
            {SQL_REAL, 0, 0},
            {SQL_FLOAT, 0, 0},
            {SQL_DOUBLE, 0, 0},
            {SQL_BINARY, 0, 0},
            {SQL_VARBINARY, 0, 0},
            {SQL_LONGVARBINARY, 0, 0},
            {SQL_TYPE_DATE, 0, 0},
            {SQL_TYPE_TIME, 0, 0},
            {SQL_TYPE_TIMESTAMP,0, 0},
            {SQL_BOOLEAN, 0, 0},
            {SQL_INTERVAL_YEAR, 0, 0},
            {SQL_INTERVAL_MONTH, 0, 0},
            {SQL_INTERVAL_DAY, 0, 0},
            {SQL_INTERVAL_HOUR, 0, 0},
            {SQL_INTERVAL_MINUTE, 0, 0},
            {SQL_INTERVAL_SECOND, 0, 0},
            {SQL_INTERVAL_YEAR_TO_MONTH, 0, 0},
            {SQL_INTERVAL_DAY_TO_HOUR, 0, 0},
            {SQL_INTERVAL_DAY_TO_MINUTE, 0, 0},
            {SQL_INTERVAL_DAY_TO_SECOND, 0, 0},
            {SQL_INTERVAL_HOUR_TO_MINUTE, 0, 0},
            {SQL_INTERVAL_HOUR_TO_SECOND, 0, 0},
            {SQL_INTERVAL_MINUTE_TO_SECOND, 0, 0}
    };
    int iTotalTest = 0; 

    gTotalTest = 0;
    gTotalPass = 0;
    gTotalFail = 0;
    for(iTotalTest = 0; iTotalTest < (sizeof(sqlTypeInfo) / sizeof(sqlTypeInfo[0])); iTotalTest++){
        doConvertSQL(pTestInfo, sqlTypeInfo[iTotalTest].sqlType, sqlTypeInfo[iTotalTest].precision, sqlTypeInfo[iTotalTest].scale);
    }
	LogMsg(NONE, _T("summary:TotalTest=%d TotalPass=%d  TotalFail=%d\n"), gTotalTest, gTotalPass, gTotalFail);
    return 0;
}

void help(void)
{
    printf("\t-h:print this help\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema name\t");
}
/*
*sample:
*        gcc DataTypeConversioin.c -o debug -L /usr/lib64/ -I /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli -g -w
*        ./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST
*/
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
        if(strcmp(argv[num], "-h") == 0){
            help();
            return 0;
        }
        else if(strcmp(argv[num], "-h") == 0){
            help();
            return 0;
        }
        else if((strcmp(argv[num], "-d") == 0) && (argc > (num + 1))){
            num++;
            strcpy ((char *)oriInfo.DataSource, argv[num]);
        }
        else if((strcmp(argv[num], "-u") == 0) && (argc > (num + 1))){
            num++;
            strcpy ((char *)oriInfo.UserID, argv[num]);
        }
        else if((strcmp(argv[num], "-p") == 0) && (argc > (num + 1))){
            num++;
            strcpy ((char *)oriInfo.Password, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-s")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Schema, argv[num]);
        }
    }
    if(strlen((char *)oriInfo.DataSource) == 0 ||
        strlen((char *)oriInfo.UserID) == 0 ||
        strlen((char *)oriInfo.Password) == 0
        ){
        help();
        return -1;
    }  
    openLog("datatype_test");
    
    testConversion(&oriInfo);

#ifndef unixcli
    //LogMsg(NONE, _T("\nPlease enter any key and exit\n"));
    //getchar();
#endif
    closeLog();

    return 0;
}


