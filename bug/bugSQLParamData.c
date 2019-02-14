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


#define CONFIG_TEST_NO_COAST

#ifndef UNICODE
#define LogMsg(Options,format,...) do{printf(format,##__VA_ARGS__);}while(0)
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
int debugSQLParamData(TestInfo *pTestInfo)
{
	RETCODE     retcode, rc;
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
	SQLLEN len;
    int ret;
    SWORD numOfCols, index;
    SQLPOINTER pToken;
	TCHAR szOut[128] = {0};
    
    LogMsg(NONE, _T("\nstart  test %s......\n"), __FUNCTION__);

    if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrors(henv,hdbc,hstmt);
        return -1;
    }
    LogMsg(NONE, _T("connect sucess...\n"));
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
    TCHAR *szDdl = _T("create table tb(")
                    _T("c1 CHAR(10),")
                    _T("c2 VARCHAR(10),")
                    _T("c3 DECIMAL(10,5),")
                    _T("c4 DECIMAL(10,5),")
                    _T("c5 SMALLINT,")
                    _T("c6 INTEGER,")
                    _T("c7 REAL,")
                    _T("c8 FLOAT,")
                    _T("c9 DOUBLE PRECISION,")
                    _T("c10 DATE,")
                    _T("c11 TIME,")
                    _T("c12 TIMESTAMP,")
                    _T("c13 NUMERIC(19,0),")
                    _T("c14 NUMERIC(19,6),")
                    _T("c15 NUMERIC(64,0),")
                    _T("C16 NUMERIC(64,28),")
                    _T("C17 NUMERIC(64,28),")
                    _T("C18 NUMERIC(10,5) UNSIGNED,")
                    _T("C19 NUMERIC(18,5) UNSIGNED,")
                    _T("C20 NUMERIC(30,10) UNSIGNED")
                    _T(")");
 #define CONFIG_COLUMNS_VALUES      20                   
    struct
	{
	    SQLSMALLINT SQLType[CONFIG_COLUMNS_VALUES];
		SQLUINTEGER			ColPrec[CONFIG_COLUMNS_VALUES];
		SQLSMALLINT			ColScale[CONFIG_COLUMNS_VALUES];
		TCHAR				*szIn[CONFIG_COLUMNS_VALUES];
		TCHAR				*szOut[CONFIG_COLUMNS_VALUES];
	} sTestDataInfo = 
	{	
		{
    		SQL_CHAR, SQL_VARCHAR,SQL_DECIMAL,SQL_NUMERIC,SQL_SMALLINT, SQL_INTEGER, SQL_REAL, SQL_FLOAT,SQL_DOUBLE, SQL_DATE,
            SQL_TIME, SQL_TIMESTAMP,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC
        },
		{10,10,10,10,0,0,0,0,0,0,  0,0,19,19,64, 64,64,10,18,30},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 6, 0, 28, 28, 5, 5, 10},
		{_T("--"),_T("--"),_T("1234.56789"),_T("5678.12345"),_T("1234"),_T("12345"),_T("12340.0"),_T("12300.0"),_T("12345670.0"),_T("1993-12-30"),_T("11:45:23"),_T("1992-12-31 23:45:23.123456"),_T("1234567890123456789"),_T("1234567890123.456789"),_T("1234567890123456789012345678901234567890"),_T("0.123456789012345678901234567890123456789"),_T("1234567890.123456789012345678901234567890123456789"),_T("12345.56789"),_T("1234567890123.56789"),_T("12345678901234567890.0123456789")},
		{_T("--"),_T("--"),_T("1234.56789"),_T("5678.12345"),_T("1234"),_T("12345"),_T("12340.0"),_T("12300.0"),_T("12345670.0"),_T("1993-12-30"),_T("11:45:23"),_T("1992-12-31 23:45:23.123456"),_T("1234567890123456789"),_T("1234567890123.456789"),_T("1234567890123456789012345678901234567890"),_T("0.123456789012345678901234567890123456789"),_T("1234567890.123456789012345678901234567890123456789"),_T("12345.56789"),_T("1234567890123.56789"),_T("12345678901234567890.0123456789")}
	
	}; 

    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"drop table if exists tb", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
    }
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szDdl, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }

    retcode = SQLPrepare(hstmt, (SQLTCHAR*)_T("insert into tb values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLPrepare  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }
    for(index = 1; index <= CONFIG_COLUMNS_VALUES; index++){
        len = SQL_DATA_AT_EXEC;
        retcode = SQLBindParameter(hstmt, 
                                    index, 
                                    SQL_PARAM_INPUT, 
                                    SQL_C_CHAR, 
                                    sTestDataInfo.SQLType[index - 1], 
                                    sTestDataInfo.ColPrec[index - 1], 
                                    sTestDataInfo.ColScale[index - 1], 
                                    NULL, 
                                    128, 
                                    &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLBindParameter  fail: %d\n"), index);
            LogAllErrors(henv, hdbc, hstmt);
            goto DEBUG_SQL_PARAM_DATA_FAIL;
        }
    }
    retcode = SQLExecute(hstmt);
    if(retcode != SQL_NEED_DATA)
    {
        LogMsg(NONE, _T("SQLExecute  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }
  
    for(index = 1; index <= CONFIG_COLUMNS_VALUES; index++){
        LogMsg(NONE, _T("SQLPutData(hstmt, \"%s\", %s);\n"), sTestDataInfo.szIn[index - 1], _T("SQL_NTS"));
        retcode = SQLParamData(hstmt,&pToken);
        if(retcode == SQL_NEED_DATA){
            retcode = SQLPutData(hstmt, sTestDataInfo.szIn[index - 1], SQL_NTS);
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("SQLPutData  fail: row:%d, columns:%d\n"), 1, index);
                LogAllErrors(henv, hdbc, hstmt);
                goto DEBUG_SQL_PARAM_DATA_FAIL;
            }
        }
        else{
            LogMsg(NONE, _T("SQLParamData  fail: row:%d, columns:%d\n"), 1, index);
            LogAllErrors(henv, hdbc, hstmt);
            goto DEBUG_SQL_PARAM_DATA_FAIL;
        }
    }
    do {
        retcode = SQLParamData(hstmt, &pToken);
    }while(retcode == SQL_NEED_DATA);
    

    SQLFreeStmt(hstmt,SQL_UNBIND);
    SQLFreeStmt(hstmt,SQL_CLOSE);
    LogMsg(NONE,_T("verify data...\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)_T("select * from tb"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE,_T("do select fail, line %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
    }
    else{
        retcode = SQLFetch(hstmt);
        numOfCols = 0;
        retcode = SQLNumResultCols(hstmt, &numOfCols);
        while ((retcode != SQL_NO_DATA) && (numOfCols != 0)){
            for (index = 1; index <= numOfCols; index++){
                retcode = SQLGetData(hstmt, index, SQL_C_CHAR, (SQLPOINTER)szOut, sizeof(szOut), &len);
                if (SQL_SUCCEEDED(retcode)){
                    LogMsg(NONE, _T("%s\n"), szOut);
                }
                else{
                    break;
                }
            }
            retcode = SQLFetch(hstmt);
        }
    }

DEBUG_SQL_PARAM_DATA_FAIL:
    disconnect(henv, hdbc, hstmt);
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
*       trafodion:
*        gcc SQLParamData.c -o debug -L /usr/lib64/ -I /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli -g -w
*         ./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST
*       unixODBC:
*        gcc SQLParamData.c -o debug  -lodbc -lpthread -lz  -Dunixcli -DUNIXODBC -g -w
*        ./debug -d traf -u trafodion -p traf123 -s ODBCTEST
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
    debugSQLParamData(&oriInfo);
#ifndef unixcli
    LogMsg(NONE, _T("\nPlease enter any key and exit\n"));
    getchar();
#endif
    return 0;
}

