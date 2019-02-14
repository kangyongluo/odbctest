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
int SQLBindParameter_SQL_C_UBIGINT_to_SQL_VARCHAR(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	char dropStr[] = "DROP TABLE IF EXISTS TB_VARCHAR";
	char createStr[] = "CREATE TABLE TB_VARCHAR (C VARCHAR(200))";
	char insertStr[] = "INSERT INTO TB_VARCHAR VALUES (?)";
	char selectStr[] = "SELECT C FROM TB_CHAR";
	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);
	unsigned long long tmp64 = 123;
	unsigned long long tmpOut = 0;
	
	SQLExecDirect(hstmt, (SQLTCHAR*)dropStr, SQL_NTS);
	LogMsg(NONE, "%s\n", createStr);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_CHAR, 200, 0, &tmp64, sizeof(tmp64), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)selectStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLBindCol(hstmt, 1, SQL_C_UBIGINT, &tmpOut, sizeof(tmpOut), &StrLen_or_IndPtr);
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
	LogMsg(NONE, "expect:%lld\tactual:%lld\n", tmp64, tmpOut);
	return 0;
}
int SQLBindParameter_SQL_C_NUMERIC_to_SQL_VARCHAR(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char dropStr[] = "DROP TABLE IF EXISTS TB_VARCHAR";
	char createStr[] = "CREATE TABLE TB_VARCHAR (C VARCHAR(200))";
	char insertStr[] = "INSERT INTO TB_VARCHAR VALUES (?)";
	char selectStr[] = "SELECT C FROM TB_CHAR";

	SQL_NUMERIC_STRUCT numeric, numericOut;
	int i;
	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);

	SQLExecDirect(hstmt, (SQLTCHAR*)dropStr, SQL_NTS);
	LogMsg(NONE, "%s\n", createStr);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
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
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_NUMERIC, SQL_VARCHAR, 200, 0, &numeric, sizeof(numeric), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
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
int SQLBindParameter_SQL_C_BINARY_to_SQL_VARCHAR(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN StrLen_or_IndPtr;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	char dropStr[] = "DROP TABLE IF EXISTS TB_VARCHAR";
	char createStr[] = "CREATE TABLE TB_VARCHAR (C VARCHAR(200))";
	char insertStr[] = "INSERT INTO TB_VARCHAR VALUES (?)";
	char selectStr[] = "SELECT C FROM TB_CHAR";

	char tmp1, tmp2;
	int i;
	LogMsg(NONE, "\nstart  test %s..............\n", __FUNCTION__);

	SQLExecDirect(hstmt, (SQLTCHAR*)dropStr, SQL_NTS);
	LogMsg(NONE, "%s\n", createStr);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	StrLen_or_IndPtr = SQL_NTS;
	tmp1 = 15;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, 200, 0, &tmp1, sizeof(tmp1), &StrLen_or_IndPtr);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter fail\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)selectStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	tmp2 = 0;
	retcode = SQLBindCol(hstmt, 1, SQL_C_BINARY, &tmp2, sizeof(tmp2), &StrLen_or_IndPtr);
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
	LogMsg(NONE, "expect:0x%x\tactual:0x%x\n", tmp2, tmp2);
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
	SQLBindParameter_SQL_C_UBIGINT_to_SQL_VARCHAR(&mODBCHDLInfo);
	SQLFreeStmt(mODBCHDLInfo.hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_NUMERIC_to_SQL_VARCHAR(&mODBCHDLInfo);
	SQLFreeStmt(mODBCHDLInfo.hstmt, SQL_CLOSE);
	SQLBindParameter_SQL_C_BINARY_to_SQL_VARCHAR(&mODBCHDLInfo);
	/***************************************/
	
	SQLFree(&mODBCHDLInfo);
	
	return 0;
}

