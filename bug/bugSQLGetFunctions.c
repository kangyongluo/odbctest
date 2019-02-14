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
int debugSQLGetFunctions(TestInfo *pTestInfo, 
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));

    LogMsg(TIME_STAMP, _T("SQLGetFunctions(hdbc, %d, %s)\n"), 
                                                            FunctionId, 
                                                            (Supported == NULL) ? _T("NULL") : _T("Supported"));
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
    
    retcode = SQLGetFunctions((SQLHANDLE)hdbc, FunctionId, Supported);
    if(retcode != SQL_SUCCESS){
        LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
    }

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
    *linux odbc:
    *ARM :core dump ,unixODBC bug
    *172.16.20.18 
    *daily20181013_0133_rh6
    */
    LogMsg(NONE, _T("Verifying the version of ARM..........\n"));
    debugSQLGetFunctions(&oriInfo, 44, NULL);
    debugSQLGetFunctions(&oriInfo, 57, NULL);
    debugSQLGetFunctions(&oriInfo, 71, NULL);
    LogMsg(NONE, _T("\n\n"));
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

