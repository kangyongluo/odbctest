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
typedef struct
{
	SQLDOUBLE	serviceId;
	SQLCHAR		imsi[16];
	SQLCHAR		userNumber[65];
	SQLDOUBLE	duration;
	
	SQLLEN	serviceIdLen;
	SQLLEN		imsiLen;
	SQLLEN		userNumberLen;
	SQLLEN	durationLen;
}ParameterLoadData;

int debugRowset(TestInfo *pTestInfo)
{
	char buf[512] = {0};
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
    RETCODE retcode;
    #define CONFIG_ROW_SET_NUM	3
    SQLRETURN returnCode;
    int i, k;
    SQLLEN len;
    TestInfo sTestInfo;
    
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

    addInfoSession(hstmt);
    
    const TCHAR *dropStr = _T("DROP TABLE tb_test");
	const TCHAR *createStr = _T("CREATE TABLE tb_test\
              (\
                SERVICE_ID      NUMERIC(9, 0)\
              , IMSI            VARCHAR(50000)\
              , USER_NUMBER     VARCHAR(64)\
              , DURATION        NUMERIC(8, 0)\
              )\
            ");

	printf("Create table...\n");
#if 1
    printf("%s\n", createStr);
	SQLExecDirect(hstmt, (SQLTCHAR*)dropStr, SQL_NTS);
	returnCode = SQLExecDirect(hstmt, (SQLTCHAR*)createStr, SQL_NTS);
	if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
	}
#else
    returnCode = SQLExecDirect(hstmt, (SQLTCHAR*)"purgedata tb_test", SQL_NTS);
	if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO)){
		printf("purgedata tb_test fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
	}
#endif
	SQLTCHAR sqlStmt[128] = {0};
	
	_tcscat((TCHAR *)sqlStmt, _T("INSERT INTO tb_test VALUES (?, ?, ?, ?)"));
	printf("%s\n", (char *)sqlStmt);
	
	returnCode = SQLPrepare(hstmt, sqlStmt, SQL_NTS);

    if (!SQL_SUCCEEDED(returnCode)){
		printf("SQLPrepare fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
    }

	ParameterLoadData pParameterLoadData[CONFIG_ROW_SET_NUM];

    pParameterLoadData[0].serviceId = 1.0;
    sprintf((char *)pParameterLoadData[0].imsi, "xcf5d6usi4");
    sprintf((char *)pParameterLoadData[0].userNumber, "x9itlgmzi8");

    pParameterLoadData[0].duration = 1.0;
    pParameterLoadData[0].serviceIdLen = SQL_NTS;
    pParameterLoadData[0].imsiLen = SQL_NTS;
    pParameterLoadData[0].userNumberLen = SQL_NTS;
    pParameterLoadData[0].durationLen = SQL_NTS;

    pParameterLoadData[1].serviceId = 2.0;
    sprintf((char *)pParameterLoadData[1].imsi, "6ohx47g1fv");
    sprintf((char *)pParameterLoadData[1].userNumber, "cx063v6sku");
    pParameterLoadData[1].duration = 1.0;
    pParameterLoadData[1].serviceIdLen = SQL_NTS;
    pParameterLoadData[1].imsiLen = SQL_NTS;
    pParameterLoadData[1].userNumberLen = SQL_NTS;
    pParameterLoadData[1].durationLen = SQL_NTS;

    pParameterLoadData[2].serviceId = 3.0;
    sprintf((char *)pParameterLoadData[2].imsi, "7axpd0nfxx");
    sprintf((char *)pParameterLoadData[2].userNumber, "jrq7bxx27x");
    pParameterLoadData[2].duration = 1.0;
    pParameterLoadData[2].serviceIdLen = SQL_NTS;
    pParameterLoadData[2].imsiLen = SQL_NTS;
    pParameterLoadData[2].userNumberLen = SQL_NTS;
    pParameterLoadData[2].durationLen = SQL_NTS;
	
	returnCode = SQLBindParameter(hstmt,
								1,
								SQL_PARAM_INPUT, 
								SQL_C_DOUBLE, 
								SQL_DOUBLE, 
								9, 
								0, 
								&pParameterLoadData[0].serviceId, 
								sizeof(pParameterLoadData[0].serviceId), 
								&pParameterLoadData[0].serviceIdLen
								);
	if (!SQL_SUCCEEDED(returnCode)){
        printf("SQLBindParameter fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
    }
	returnCode = SQLBindParameter(hstmt,
								2,
								SQL_PARAM_INPUT, 
								SQL_C_CHAR, 
								SQL_CHAR, 
								0, 
								0, 
								&pParameterLoadData[0].imsi, 
								sizeof(pParameterLoadData[0].imsi), 
								&pParameterLoadData[0].imsiLen
								);
	if (!SQL_SUCCEEDED(returnCode)){
        printf("SQLBindParameter fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
    }
	returnCode = SQLBindParameter(hstmt,
								3,
								SQL_PARAM_INPUT, 
								SQL_C_CHAR, 
								SQL_CHAR, 
								0, 
								0, 
								&pParameterLoadData[0].userNumber, 
								sizeof(pParameterLoadData[0].userNumber), 
								&pParameterLoadData[0].userNumberLen
								);
	if (!SQL_SUCCEEDED(returnCode)){
        printf("SQLBindParameter fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
    }
	returnCode = SQLBindParameter(hstmt,
								4,
								SQL_PARAM_INPUT, 
								SQL_C_DOUBLE, 
								SQL_DOUBLE, 
								8, 
								0, 
								&pParameterLoadData[0].duration, 
								sizeof(pParameterLoadData[0].duration), 
								&pParameterLoadData[0].durationLen
								);
	if (!SQL_SUCCEEDED(returnCode)){
        printf("SQLBindParameter fail. line %d\n", __LINE__);
		LogAllErrors(henv, hdbc, hstmt);
		FullDisconnect(&sTestInfo);
		return -1;
    }

    SQLUSMALLINT param_status[CONFIG_ROW_SET_NUM];
    SQLINTEGER params_processed = 0;

	SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER) sizeof(ParameterLoadData), 0);
    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)CONFIG_ROW_SET_NUM, 0);
    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR, param_status, 0);
    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &params_processed, 0);

	printf("SERVICE_ID       	IMSI            USER_NUMBER			DURATION\n");
	for(k =0 ; k  < CONFIG_ROW_SET_NUM; k++)
		printf("%f\t%s\t%s\t\t%f\n", 
							pParameterLoadData[k].serviceId,
							pParameterLoadData[k].imsi,
							pParameterLoadData[k].userNumber,
							pParameterLoadData[k].duration
							);
	
	returnCode = SQLExecute(hstmt);
    if (returnCode != SQL_SUCCESS){
		printf("SQLExecute fail. line %d\n", __LINE__);
        printf("param status :row0 %u, row1 %u, row2 %u\n", param_status[0], param_status[1], param_status[2]);
		LogAllErrors(henv, hdbc, hstmt);
        FullDisconnect(&sTestInfo);
		return -1;
    }
    
	printf("check out data...\n");
	printf("SERVICE_ID       	IMSI            USER_NUMBER			DURATION\n");
	SQLExecDirect(hstmt,(SQLTCHAR*)"select * from tb_test",SQL_NTS);
	SQLSMALLINT num;
	
	returnCode = SQLFetch(hstmt);
	returnCode = SQLNumResultCols(hstmt, &num);
	while (returnCode != SQL_NO_DATA)
	{
		for (i = 1; i <= num; ++i)
		{
			returnCode = SQLGetData(hstmt, i, SQL_C_CHAR, (SQLPOINTER)buf, sizeof(buf), &len);
			printf("%s\t", buf);
            if(len == 0)
                printf("\t");
		}
		printf("\n");
		returnCode = SQLFetch(hstmt);
	}
    
    FullDisconnect(&sTestInfo);
    return 0;
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
//./debug -d traf -u trafodion -p traf123
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
    debugRowset(&oriInfo);
    LogMsg(NONE, _T("\n\n"));
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


