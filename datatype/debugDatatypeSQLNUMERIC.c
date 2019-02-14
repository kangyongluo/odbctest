#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

typedef struct ODBCHDLInfo
{
	SQLHANDLE henv;
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

#define LogMsg(Options,format,...) do{printf(format,##__VA_ARGS__);}while(0)

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
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pHDL->henv);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error AllocHandle\n");
		return SQL_ERROR;
	}

	if(odbcVersion == SQL_OV_ODBC2)
		ret = SQLSetEnvAttr(pHDL->henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC2, 0);
	else if(odbcVersion == SQL_OV_ODBC3)
		ret = SQLSetEnvAttr(pHDL->henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	else
		ret = SQLSetEnvAttr(pHDL->henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC2, 0);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error SetEnv\n");
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->henv);
		return SQL_ERROR;
	}
	// 2. allocate connection handle, set timeout
	ret = SQLAllocHandle(SQL_HANDLE_DBC, pHDL->henv, &pHDL->hdbc);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error AllocHDB %d\n", ret);
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->henv);
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
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->henv);
		return SQL_ERROR;
	}
	printf("Connected successfull!\n");
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pHDL->hdbc, &pHDL->hstmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		printf("Fehler im AllocStatement %d\n", ret);
		SQLFreeHandle(SQL_HANDLE_ENV, pHDL->henv);
		return SQL_ERROR;
	}

	return SQL_SUCCESS;
}
void SQLFree(t_ODBCHDLInfo *pHDL)
{
	SQLFreeHandle(SQL_HANDLE_STMT, pHDL->hstmt);
	SQLDisconnect(pHDL->hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, pHDL->hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, pHDL->henv);
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
int SQLBindParameter_SQL_C_NUMERIC_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	SQL_NUMERIC_STRUCT numeric, numericOut;
	int i;
	char outBuf[200] = {0};
	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);

	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	memset(&numeric, 0, sizeof(numeric));
	numeric.precision = 3;
	numeric.scale = 0;
	numeric.sign = 1;
	numeric.val[0] = 123;
	LogMsg(NONE, "numeric data is:\n");
	LogMsg(NONE, "precision = %d\nscale = %d\nsign = %d\n", numeric.precision, numeric.scale, numeric.sign);
	for(i = 0; i < 16; i++){
		LogMsg(NONE, "0x%x ", numeric.val[i]);
	}
	LogMsg(NONE, "\n");
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_NUMERIC, SQL_NUMERIC, 0, 0, &numeric, sizeof(numeric), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(&numericOut, 0, sizeof(numericOut));
	retcode = SQLBindCol(hstmt, 1, SQL_C_NUMERIC, &numericOut, sizeof(numericOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "numericOut data is:\n");
	LogMsg(NONE, "precision = %d\nscale = %d\nsign = %d\n", numericOut.precision, numericOut.scale, numericOut.sign);
	for(i = 0; i < 16; i++){
		LogMsg(NONE, "0x%x ", numericOut.val[i]);
	}
	LogMsg(NONE, "\n");
	return 0;
}

int SQLBindParameter_SQL_C_BIT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLCHAR	tmpSTinyint = 1;  
	SQLCHAR	outSTinyint = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_BIT, SQL_NUMERIC, 30, 20, &tmpSTinyint, sizeof(tmpSTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_BIT, &outSTinyint, sizeof(outSTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpSTinyint, outSTinyint);
	return 0;
}

int SQLBindParameter_SQL_C_STINYINT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLSCHAR	tmpSTinyint = 127;  
	SQLSCHAR	outSTinyint = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_STINYINT, SQL_NUMERIC, 30, 20, &tmpSTinyint, sizeof(tmpSTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_STINYINT, &outSTinyint, sizeof(outSTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpSTinyint, outSTinyint);
	return 0;
}
int SQLBindParameter_SQL_C_UTINYINT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLCHAR	tmpUTinyint = 127;  
	SQLCHAR	outUTinyint = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_NUMERIC, 30, 20, &tmpUTinyint, sizeof(tmpUTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_UTINYINT, &outUTinyint, sizeof(outUTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpUTinyint, outUTinyint);
	return 0;
}
int SQLBindParameter_SQL_C_TINYINT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLSCHAR	tmpTinyint = 127;  
	SQLSCHAR	outTinyint = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_NUMERIC, 30, 20, &tmpTinyint, sizeof(tmpTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_TINYINT, &outTinyint, sizeof(outTinyint), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpTinyint, outTinyint);

	return 0;
}
int SQLBindParameter_SQL_C_SBIGINT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLBIGINT	tmpSBigInt = 127;  
	SQLBIGINT	outSBigInt = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_NUMERIC, 30, 20, &tmpSBigInt, sizeof(tmpSBigInt), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_SBIGINT, &outSBigInt, sizeof(outSBigInt), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%lld actual:%lld\n", tmpSBigInt, outSBigInt);
	return 0;
}
int SQLBindParameter_SQL_C_UBIGINT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLUBIGINT	tmpUBigInt = 127;  
	SQLUBIGINT	outUBigInt = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_NUMERIC, 30, 20, &tmpUBigInt, sizeof(tmpUBigInt), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_UBIGINT, &outUBigInt, sizeof(outUBigInt), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%llu actual:%llu\n", tmpUBigInt, outUBigInt);
	return 0;
}
int SQLBindParameter_SQL_C_SSHORT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLSMALLINT	tmpIn = 127;  
	SQLSMALLINT	tmpOut = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_NUMERIC, 30, 20, &tmpIn, sizeof(tmpIn), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_SSHORT, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpIn, tmpOut);
	return 0;
}
int SQLBindParameter_SQL_C_USHORT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLUSMALLINT	tmpIn = 127;  
	SQLUSMALLINT	tmpOut = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_USHORT, SQL_NUMERIC, 30, 20, &tmpIn, sizeof(tmpIn), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_USHORT, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpIn, tmpOut);
	return 0;
}
int SQLBindParameter_SQL_C_SHORT_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLSMALLINT	tmpIn = 127;  
	SQLSMALLINT	tmpOut = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_NUMERIC, 30, 20, &tmpIn, sizeof(tmpIn), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_SHORT, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpIn, tmpOut);
	return 0;
}
int SQLBindParameter_SQL_C_SLONG_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLINTEGER	tmpIn = 127;  
	SQLINTEGER	tmpOut = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_NUMERIC, 30, 20, &tmpIn, sizeof(tmpIn), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_SLONG, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpIn, tmpOut);
	return 0;
}
int SQLBindParameter_SQL_C_ULONG_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLUINTEGER	tmpIn = 127;  
	SQLUINTEGER	tmpOut = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_NUMERIC, 30, 20, &tmpIn, sizeof(tmpIn), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpIn, tmpOut);
	return 0;
}
int SQLBindParameter_SQL_C_LONG_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo, int id)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char outBuf[200] = {0};

	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &id, sizeof(id), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 1 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	SQLINTEGER	tmpIn = 127;  
	SQLINTEGER	tmpOut = 0;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_NUMERIC, 30, 20, &tmpIn, sizeof(tmpIn), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter 2 fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO TB_NUMERIC VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	sprintf(outBuf, "SELECT C FROM TB_NUMERIC WHERE ID=%d", id);
	LogMsg(NONE, "%s\n", outBuf);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)outBuf, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(outBuf, 0, sizeof(outBuf));
	retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "expect:%d actual:%d\n", tmpIn, tmpOut);
	return 0;
}

int SQLBindParameter_SQL_C_X_to_SQL_NUMERIC(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char dropStr[] = "DROP TABLE IF EXISTS TB_NUMERIC";
	char createStr[] = "CREATE TABLE TB_NUMERIC (ID INT, C NUMERIC(30,20))";
	char insertStr[] = "INSERT INTO TB_NUMERIC VALUES (?,?)";
	char selectStr[] = "SELECT C FROM TB_NUMERIC";

	int id = 0;
	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);

	SQLExecDirect(hstmt, (SQLTCHAR*)dropStr, SQL_NTS);
	LogMsg(NONE, "%s\n", createStr);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	SQLBindParameter_SQL_C_NUMERIC_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_BIT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_STINYINT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_UTINYINT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_TINYINT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_SBIGINT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_UBIGINT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_SSHORT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_USHORT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_SHORT_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_SLONG_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_ULONG_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_LONG_to_SQL_NUMERIC(pTestInfo, id++);
	SQLFreeStmt(pTestInfo->hstmt, SQL_CLOSE);
	return 0;
}
int debugSQLNUMERIC(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	char dropStr[] = "DROP TABLE IF EXISTS TB_NUMERIC";
	char createStr[] = "CREATE TABLE TB_NUMERIC (C NUMERIC(30,20))";
	char insertStr[] = "INSERT INTO TB_NUMERIC VALUES (123.456)";
	char selectStr[] = "SELECT C FROM TB_NUMERIC";
	
	SQL_NUMERIC_STRUCT numeric, numericOut;
	int i;
	char outBuf[200] = {0};
	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);

	SQLExecDirect(hstmt, (SQLTCHAR*)dropStr, SQL_NTS);
	LogMsg(NONE, "%s\n", createStr);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	/************************************/
	/*
	*This method allows you to retrieve data correctly
	*/
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)selectStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR, outBuf, sizeof(outBuf), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "numericOut data is:%s\n", outBuf);
	SQLFreeStmt(hstmt, SQL_CLOSE);
	/************************************/
	/*
	*SQLFetch returns failed in the following manner
	*/
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)selectStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	memset(&numericOut, 0, sizeof(numericOut));
	retcode = SQLBindCol(hstmt, 1, SQL_C_NUMERIC, &numericOut, sizeof(numericOut), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	LogMsg(NONE, "numericOut data is:\n");
	LogMsg(NONE, "precision = %d\nscale = %d\nsign = %d\n", numericOut.precision, numericOut.scale, numericOut.sign);
	for(i = 0; i < 16; i++){
		LogMsg(NONE, "0x%x ", numericOut.val[i]);
	}
	LogMsg(NONE, "\n");
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
	//strcpy(mDSNInfo.DSN, "TRAFODION");
	strcpy(mDSNInfo.user, "trafodion");
	strcpy(mDSNInfo.password, "traf123");
#else
	strcpy(mDSNInfo.DSN, (char *)"MySQL");
	strcpy(mDSNInfo.user, "root");
	strcpy(mDSNInfo.password, "123456");
#endif
	printf("SQL source:%s,%s,%s\n", mDSNInfo.DSN, mDSNInfo.user, mDSNInfo.password);
#if 0
	debugQueryID(&mODBCHDLInfo);
	return 0;
#endif
	retcode = SQLInit(&mODBCHDLInfo, mDSNInfo, SQL_OV_ODBC2);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		printf("SQL init fail\n");

		return -1;
	}

	/*****************test********************/
	SQLBindParameter_SQL_C_X_to_SQL_NUMERIC(&mODBCHDLInfo);
	SQLFreeStmt(mODBCHDLInfo.hstmt, SQL_CLOSE);
	debugSQLNUMERIC(&mODBCHDLInfo);
	/***************************************/
	
	SQLFree(&mODBCHDLInfo);
	
	return 0;
}



