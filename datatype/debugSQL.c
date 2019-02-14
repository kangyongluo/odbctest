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



static int debugSQL(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode, returncode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	char    outbuf[512] = {0};
	char dropStr[] = "DROP TABLE TB_DEBUG";

	char createStr[] = "CREATE TABLE TB_DEBUG(C1 NUMERIC(5,2))";
	char insertStr[] = "INSERT INTO TB_DEBUG VALUES (2)";
	char selectStr[] = "select * from TB_DEBUG";
	SWORD numOfCols;

	retcode = SQLExecDirect(hstmt, (SQLCHAR*)dropStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "drop table fail:\n");
		//LogAllErrors(henv,hdbc,hstmt);
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "insert table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)selectStr, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "select table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLNumResultCols(hstmt, &numOfCols);
	if (retcode != SQL_SUCCESS){
		LogMsg(NONE, "SQLNumResultCols fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	LogMsg(NONE, "numOfCols = %d\n", numOfCols);
	retcode = SQLFetch(hstmt);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	SWORD num = 1;
	while (retcode != SQL_NO_DATA_FOUND) {
		memset(outbuf, 0, sizeof(outbuf));
		retcode = SQLGetData(hstmt,(SWORD)num++,SQL_C_CHAR,outbuf,sizeof(outbuf),&len);
        if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA_FOUND && retcode != SQL_SUCCESS_WITH_INFO) {
            LogAllErrors(henv,hdbc,hstmt);
        } else if (retcode == SQL_NO_DATA_FOUND) {
            break;
        }
		LogMsg(NONE, "%s\t", outbuf);
		if(num > numOfCols){
			break;
		}
	}
	LogMsg(NONE, "\n");
	LogMsg(NONE, "test debugSQL success\n");
	return 0;
}
int debug_demo(t_ODBCHDLInfo *pTestInfo)
{
	//SQLRETURN retcode, rc;
	RETCODE retcode, rc;
	SQLSMALLINT ColumnNum, i;
	SQLSMALLINT     NumParams;
	unsigned char *TargetValuePtr;
	SQLLEN BufferLength;
	SQLLEN len;
	char *pStr;
	char flg;
	SQLCHAR SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER NativeError;
	SQLSMALLINT MsgLen;
	SQLPOINTER		pToken;
	char tmpBuf[1024];
	char name[20] = "abc";
	int age = 99;
	int id = 10;
	char outbuf[128] = {0};
	int tmp;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	
	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	
/*********************************************************************************************************************************************/
#if 1
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"drop table datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n\n");
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table datatype(c1 INT, c2 char(300))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n\n");
		return -1;
	}
#endif
/*********************************************************************************************************************************************/
#if 1
	retcode = SQLPrepare(hstmt,(SQLCHAR*)"insert into datatype values(?,?)",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
#endif
/*********************************************************************************************************************************************/
#if 1
	len = SQL_NTS;
	//len = SQL_DATA_AT_EXEC;
	retcode = SQLBindParameter(hstmt,
					(SWORD)1,
					SQL_PARAM_INPUT,
					SQL_C_SSHORT,
					SQL_INTEGER,
					0,
					0,
					&id,
					sizeof(id),
					&len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter 1 fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	len = SQL_NTS;
	//len = SQL_DATA_AT_EXEC;
	memset(tmpBuf, 0, sizeof(tmpBuf));
	strcpy(tmpBuf, "8.548180e+01");
	retcode = SQLBindParameter(hstmt,
					(SWORD)2,
					SQL_PARAM_INPUT,
					SQL_C_DEFAULT,
					SQL_CHAR,
					300,
					0,
					tmpBuf,
					strlen(tmpBuf),
					&len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter 2 fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
#endif
/*********************************************************************************************************************************************/
/*********************************************************************************************************************************************/
#if 0 
	retcode = SQLBindCol(hstmt, 1,SQL_C_CHAR, outbuf, sizeof(outbuf), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindCol  1 fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}

  	retcode = SQLBindCol(hstmt, 2,SQL_C_ULONG, &tmp, sizeof(tmp), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindCol  2 fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	} 
#endif
/*********************************************************************************************************************************************/

#if 0
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"insert into datatype values('123456', 12)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLExecDirect  fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
#endif
/******************************************************************************************************************************************/
#if 0
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"insert into datatype values(?,?)",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
#endif
/*********************************************************************************************************************************************/
#if 0
	retcode = SQLNumParams(hstmt, &NumParams);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLNumParams fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	printf("NumParams = %d\n", NumParams);
#endif
/*********************************************************************************************************************************************/

/********************************************************************************************************************************************/
#if 1
	retcode = SQLExecute(hstmt);
	if (retcode != SQL_NEED_DATA){
		printf("execute table fail: %d, %d\n", retcode, SQL_NEED_DATA);
	}
#endif
/*********************************************************************************************************************************************/
#if 0
	if (retcode != SQL_NEED_DATA){
		printf("execute table fail: %d, %d\n", retcode, SQL_NEED_DATA);
	}
	else{
		i = 0;
		while (retcode == SQL_NEED_DATA)
		{
			retcode = SQLParamData(pTestInfo->hstmt,&pToken);
			if (retcode == SQL_NEED_DATA)
			{
				if(i == 0){
					memset(name, 0, sizeof(name));
					sprintf(name, "%s", "1234567890ab");
					rc = SQLPutData(hstmt,name,strlen(name));
					if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
						DEBUG_ODBC_MSG(hstmt);
					}
				}
				else if(i == 1){
					age = 19999;
					rc = SQLPutData(hstmt,&age,sizeof(age));
					if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
						DEBUG_ODBC_MSG(hstmt);
					}
				}
				
			}
			i++;
		}
	}
#endif
/*********************************************************************************************************************************************/
#if 1
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"select * from datatype",SQL_NTS);
	SQLSMALLINT num;
	char buf[100] = {0};
	
	retcode = SQLFetch(hstmt);
	retcode = SQLNumResultCols(hstmt, &num);
	printf("num = %d\n", num); 
	while (retcode != SQL_NO_DATA)
	{
		for (i = 1; i <= num; ++i)
		{
			retcode = SQLGetData(hstmt, i, SQL_C_CHAR, (SQLPOINTER)buf, 100, &len);
			printf("%20s\t", buf);
		}
		printf("\n");
		retcode = SQLFetch(hstmt);
	}
#endif
/*********************************************************************************************************************************************/
#if 0
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"select * from datatype",SQL_NTS);
	SQLSMALLINT num; 
	
	retcode = SQLFetch(hstmt);
	retcode = SQLNumResultCols(hstmt, &num);
	printf("num = %d\n", num); 
	while (retcode != SQL_NO_DATA)
	{
		printf("%s\t%d\n", outbuf, tmp);
		retcode = SQLFetch(hstmt);
	}
#endif
	/*********************************************************************************************************************************************/

	return 0;
}
static int debugDatatype(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode, returncode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;

	henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	char    outbuf[512] = {0};
	char dropStr[] = "DROP TABLE TB_DEBUG";

	char createStr[] = "CREATE TABLE TB_DEBUG(C1 DECIMAL(18,18))";
	char insertStr[] = "INSERT INTO TB_DEBUG VALUES (1.43977e+09)";
	char selectStr[] = "select C1 from TB_DEBUG";
	SWORD numOfCols;

	SQLSMALLINT info_len = 0;
	
	char    buf[100] = {0};
		
#if 1	
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)dropStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "drop table fail:\n");
		//LogAllErrors(henv,hdbc,hstmt);
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	/*
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLExecDirect insert  fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}*/
	/************************************************************************************************/
#endif
	len= SQL_NTS;
	char value[129] = "1.43977e+09";
	retcode = SQLBindParameter(hstmt,
					(SWORD)1,
					SQL_PARAM_INPUT,
					SQL_C_CHAR,
					SQL_DECIMAL,
					18,
					18,
					value,
					11,
					&len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter  fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
#if 1
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"insert into TB_DEBUG values(?)",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
#endif
	SQLFreeStmt(hstmt, SQL_CLOSE);
	len = 0;
	retcode = SQLBindCol(hstmt, 1,SQL_C_CHAR, outbuf, sizeof(outbuf), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindCol  1 fail:\n\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)selectStr, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "select table fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	retcode = SQLNumResultCols(hstmt, &numOfCols);
	if (retcode != SQL_SUCCESS){
		LogMsg(NONE, "SQLNumResultCols fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	LogMsg(NONE, "numOfCols = %d\n", numOfCols);
	retcode = SQLFetch(hstmt);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
#if 0
	SWORD num = 1;
	while (retcode != SQL_NO_DATA_FOUND) {
		memset(outbuf, 0, sizeof(outbuf));
		retcode = SQLGetData(hstmt,(SWORD)num++,SQL_C_CHAR,outbuf,sizeof(outbuf),&len);
        if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA_FOUND && retcode != SQL_SUCCESS_WITH_INFO) {
            LogAllErrors(henv,hdbc,hstmt);
        } else if (retcode == SQL_NO_DATA_FOUND) {
            break;
        }
		LogMsg(NONE, "%s\t", outbuf);
		if(num > numOfCols){
			break;
		}
	}
#else
	while (retcode != SQL_NO_DATA)
	{
		printf("len = %d, %s\n", len, outbuf);
		retcode = SQLFetch(hstmt);
	}
#endif
	LogMsg(NONE, "\n");
	LogMsg(NONE, "test debugDatatype success\n");
	return 0;
}
int    isCharSet = 0;
int    isUCS2 = 0;
// Session Name connection attribute
    #define SQL_ATTR_SESSIONNAME		5000
    // Max Session Name length
    #define SQL_MAX_SESSIONNAME_LEN		25
char *ReturncodeToChar(RETCODE retcode, char *buf)
{
   switch (retcode) {
	  case SQL_SUCCESS:
		 strcpy (buf, "SQL_SUCCESS");
		 break;
	  case SQL_ERROR:
		 strcpy (buf, "SQL_ERROR");
		 break;
	  case SQL_SUCCESS_WITH_INFO:
		 strcpy (buf, "SQL_SUCCESS_WITH_INFO");
		 break;
	  case SQL_NO_DATA_FOUND:
		 strcpy (buf, "SQL_NO_DATA_FOUND");
		 break;
	  case SQL_NEED_DATA:
		 strcpy (buf, "SQL_NEED_DATA");
		 break;
	  case SQL_INVALID_HANDLE:
		 strcpy (buf, "SQL_INVALID_HANDLE");
		 break;
	  case SQL_STILL_EXECUTING:
		 strcpy (buf, "SQL_STILL_EXECUTING");
		 break;
	  default:
		 strcpy (buf, "SQL_ERROR");
	}
	return buf;
}
int CheckReturnCode(RETCODE expected,RETCODE actual,char *FunctionName,char *SourceFile,short LineNum)
{
	char exp[30];
	char act[30];
    // this is a fix for freestatement since SQL returns SQL_SUCCESS_WITH_INFO for more than one stmt while closing.
	if ((actual == SQL_SUCCESS_WITH_INFO) && (expected == SQL_SUCCESS))
			actual = SQL_SUCCESS;
    // end change.

   if(expected==actual) 
   		return(1);
   LogMsg(NONE,"");
   LogMsg(ERRMSG+SHORTTIMESTAMP,"%s: Expected: %s Actual: %s\n",
		FunctionName,ReturncodeToChar(expected,exp),ReturncodeToChar(actual,act));
   LogMsg(NONE,"   File: %s   Line: %d\n",SourceFile,LineNum);
   return(0);
}
#define CHECKRC(expected,actual,FunctionName) (CheckReturnCode((expected),(actual),(FunctionName),(char*)__FILE__,(short)__LINE__))

static int debugQueryID(t_ODBCHDLInfo *pTestInfo)
{
		char			Heading[MAX_STRING_SIZE];
		RETCODE 		returncode;
		SQLHANDLE		henv, henv2;
		SQLHANDLE		hdbc, hdbc2 = (SQLHANDLE)NULL;
		SQLHANDLE		hstmt, hstmt2;
	
		int 			loop = 0;
		char			infoStatsStmt[ 1024 ];
		SQLCHAR 		cursorName[ 1024 ];
		CHAR			jobid[SQL_MAX_SESSIONNAME_LEN];
		CHAR			queryID[256];
		SQLINTEGER		jobidlen;
		SQLLEN			queryIDPtr;
		CHAR			tempStr[256];
								  
		char *droptab	=  "DROP TABLE TB_DEBUG cascade";
		char *createtab =  "CREATE TABLE TB_DEBUG (C int) NO PARTITION";
		char *inserttab =  "INSERT INTO TB_DEBUG VALUES (10)";
		char *selecttab =  "SELECT * FROM TB_DEBUG";
	
		struct
		{
			RETCODE 	returncode;
			CHAR		*jobID;
			CHAR		*jobIDExpected;
		} jobIDMatrix[] = {
			{ SQL_SUCCESS, "1"													, "1"},
			{ SQL_SUCCESS, "11" 												 , "11"},
			{ SQL_SUCCESS, ""													, "" },
			{ SQL_SUCCESS, "H"													, "H" },
			{ SQL_SUCCESS, "hh" 												 , "hh" },
			{ SQL_SUCCESS, "0"													, "0" },
			{ SQL_SUCCESS, "_"													, "_" },
			{ SQL_SUCCESS, "________________________"							, "________________________" },
			{ SQL_SUCCESS, "odbcqa" 											, "odbcqa" },
			{ SQL_SUCCESS, "odbcqa00"											, "odbcqa00" },
			{ SQL_SUCCESS, "00odbcqa"											, "00odbcqa" },
			{ SQL_SUCCESS, "0123_HELLOKITTY"									, "0123_HELLOKITTY" },
			{ SQL_SUCCESS, "_Hello_Kitty_123"									, "_Hello_Kitty_123" },
			{ SQL_SUCCESS, "Hello_Kitty_Went_To_The_"							, "Hello_Kitty_Went_To_The_" },
			{ SQL_SUCCESS, "Hello_Kitty_Went_To_The_"							, "Hello_Kitty_Went_To_The_" },
			{ SQL_SUCCESS, "1234567890_1234567890"								, "1234567890_1234567890" },
			{ SQL_SUCCESS, "123456789012345678901234"							, "123456789012345678901234" },
			{ SQL_SUCCESS, "123456789012345678901234"							, "123456789012345678901234" },
			{ SQL_SUCCESS, "1234567890123456789012345"							, "123456789012345678901234" },
			{ SQL_SUCCESS, "Hello_Kitty_Went_To_The_Store_To_Buy"				, "Hello_Kitty_Went_To_The_" },
			{ SQL_SUCCESS_WITH_INFO, " "										, "" },
			{ SQL_SUCCESS_WITH_INFO, " HelloKitty"								, "" },
			{ SQL_SUCCESS_WITH_INFO, "Hello Kitty"								, "" },
			{ SQL_SUCCESS_WITH_INFO, "HelloKitty "								, "" },
			{ SQL_SUCCESS_WITH_INFO, "1 2"										, "" },
			{ SQL_SUCCESS_WITH_INFO, "12345.67890.123456789012" 				, "" },
			{ SQL_SUCCESS_WITH_INFO, "Hello$Kitty"								, "" },
			{ SQL_SUCCESS_WITH_INFO, "\"HelloKitty\""							, "" },
			{ SQL_SUCCESS_WITH_INFO, "'HelloKitty'" 							, "" },
			{ SQL_SUCCESS_WITH_INFO, "\" \""									, "" },
			{ SQL_SUCCESS_WITH_INFO, "\"\"" 									, "" },
			{ SQL_SUCCESS_WITH_INFO, "\"#@*()-_=+[]{}|:;'<>,.?\""				, "" }
		};
	//======================================================================================================
#if 0	
		if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3))
		{
			LogMsg(NONE, "Unable to connect\n");
			return -1;
		}
		retcode = SQLAllocStmt((SQLHANDLE)pTestInfo->hdbc, &pTestInfo->hstmt);
		if (!CHECKRC(SQL_SUCCESS, retcode, "SQLAllocStmt"))
		{
			LogAllErrors(pTestInfo->henv, pTestInfo->hdbc, pTestInfo->hstmt);
			return -1;
		}
	
		henv = pTestInfo->henv;
		hdbc = pTestInfo->hdbc;
		hstmt = (SQLHANDLE)pTestInfo->hstmt;


		returncode = SQLExecDirect(hstmt, (SQLCHAR*)droptab,SQL_NTS);
	
		returncode = SQLExecDirect(hstmt, (SQLCHAR*)createtab,SQL_NTS);
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLExecDirect"))
		{
			LogAllErrors(henv,hdbc,hstmt);
			return -1;
		}
	
		returncode = SQLExecDirect(hstmt, (SQLCHAR*)inserttab,SQL_NTS);
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLExecDirect"))
		{
			LogAllErrors(henv,hdbc,hstmt);
			return -1;
		}
		FullDisconnect3(pTestInfo);
#endif	
	
	//======================================================================================================
	for(loop = 0; loop < (sizeof(jobIDMatrix) / sizeof(jobIDMatrix[0])); loop++)
	{
		sprintf(Heading,"Test #%d: Testing for jobID: %s\n", loop, jobIDMatrix[ loop ].jobID);
		LogMsg(NONE, Heading);
		returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pTestInfo->henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
		{
			LogMsg(NONE, "Error AllocHandle\n");
			return SQL_ERROR;
		}

		returncode = SQLSetEnvAttr(pTestInfo->henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
		{
			LogMsg(NONE, "Error SetEnv\n");
			SQLFreeHandle(SQL_HANDLE_ENV, pTestInfo->henv);
			return SQL_ERROR;
		}
		// 2. allocate connection handle, set timeout
		returncode = SQLAllocHandle(SQL_HANDLE_DBC, pTestInfo->henv, &pTestInfo->hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
		{
			LogMsg(NONE, "Error AllocHDB %d\n", returncode);
			SQLFreeHandle(SQL_HANDLE_ENV, pTestInfo->henv);
			return SQL_ERROR;
		}

		returncode = SQLSetConnectAttr(hdbc, (SQLINTEGER)SQL_ATTR_SESSIONNAME,(SQLCHAR*) jobIDMatrix[ loop ].jobID, SQL_NTS);
		if (jobIDMatrix[ loop ].returncode == SQL_SUCCESS) {
			if(!CHECKRC(SQL_SUCCESS,returncode,"SQLSetConnectAttr")) {
				LogAllErrors(henv,hdbc,hstmt);
				continue;
			}

		}
		else {
#ifdef unixcli
			if(returncode != SQL_ERROR) {
				LogAllErrors(henv,hdbc,hstmt);
				continue;
			}

#else
			if(!CHECKRC(SQL_SUCCESS,returncode,"SQLSetConnectAttr")) {
				LogAllErrors(henv,hdbc,hstmt);
				continue;
			}
#endif
		}
		returncode = SQLGetConnectAttr(hdbc, (SQLINTEGER)SQL_ATTR_SESSIONNAME, jobid, SQL_MAX_SESSIONNAME_LEN, &jobidlen);
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLGetConnectAttr")) {
			LogAllErrors(henv,hdbc,hstmt);
			continue;
		}
		if (jobIDMatrix[ loop ].returncode == SQL_SUCCESS) {
			if (strcmp(jobIDMatrix[ loop ].jobIDExpected, jobid) == 0) {
				LogMsg(NONE, "DebugIDs are matched. Expected: \"%s\", Actual: \"%s\" for SQLGetConnectAttr\n", jobIDMatrix[ loop ].jobIDExpected, jobid);
			}
			else {
				LogMsg(ERRMSG, "DebugIDs are not matched. Expected: \"%s\", Actual: \"%s\", at line=%d for SQLGetConnectAttr\n", jobIDMatrix[ loop ].jobIDExpected, jobid, __LINE__);
			}
		}
		else {
#ifdef unixcli
			if (strcmp(jobIDMatrix[ loop ].jobIDExpected, jobid) == 0) {
					LogMsg(NONE, "DebugIDs are matched. Expected: \"%s\", Actual: \"%s\" for SQLGetConnectAttr\n", jobIDMatrix[ loop ].jobID, jobid);
			}
#else
			if (strcmp(jobIDMatrix[ loop ].jobID, jobid) == 0) {
				LogMsg(NONE, "DebugIDs are matched. Expected: \"%s\", Actual: \"%s\" for SQLGetConnectAttr\n", jobIDMatrix[ loop ].jobID, jobid);
			}
#endif
			else {
				LogMsg(ERRMSG, "DebugIDs are not matched. Expected: \"%s\", Actual: \"%s\", at line=%d for SQLGetConnectAttr\n", jobIDMatrix[ loop ].jobID, jobid, __LINE__);
			}
		}

		// 3. Connect to the datasource "web" 
		returncode = SQLConnect(hdbc,
							   (SQLCHAR*)"MySQL",SQL_NTS,
							   (SQLCHAR*)"root",SQL_NTS,
							   (SQLCHAR*)"123456",SQL_NTS
							   );	
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
		{
			LogMsg(NONE, "Error SQLConnect %d\n", returncode);
			SQLFreeHandle(SQL_HANDLE_ENV, pTestInfo->henv);
			return SQL_ERROR;
		}
		LogMsg(NONE, "Connected successfull!\n");

		returncode = SQLAllocStmt(hdbc,&hstmt);
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLAllocStmt")) {
			LogAllErrors(henv,hdbc,hstmt);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		returncode = SQLAllocStmt(hdbc,&hstmt2);
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLAllocStmt")) {
			LogAllErrors(henv,hdbc,hstmt);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		returncode = SQLPrepare( hstmt, (SQLCHAR *)selecttab, SQL_NTS );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLPrepare")) {
			LogAllErrors(henv,hdbc,hstmt);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		returncode = SQLGetCursorName(hstmt, cursorName, sizeof(cursorName), NULL );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLPrepare")) {
			LogAllErrors(henv,hdbc,hstmt);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		//Get queryID
		memset(infoStatsStmt, 0, sizeof(infoStatsStmt));
		sprintf( infoStatsStmt, "INFOSTATS %s", (char*)cursorName );
		returncode = SQLBindCol( hstmt2, 1, SQL_C_CHAR, &queryID, sizeof(queryID), &queryIDPtr );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
			LogAllErrors(henv,hdbc,hstmt2);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		returncode = SQLExecDirect( hstmt2, (SQLCHAR *)infoStatsStmt, SQL_NTS );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
			LogAllErrors(henv,hdbc,hstmt2);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		returncode = SQLFetch( hstmt2 );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
			LogAllErrors(henv,hdbc,hstmt2);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		LogMsg(NONE, "queryID: \"%s\"\n", queryID);
		SQLFreeStmt(hstmt2,SQL_CLOSE);
		SQLFreeStmt(hstmt2,SQL_UNBIND);

		//Get Jobid
		if(isCharSet || isUCS2)
			sprintf( infoStatsStmt, "SELECT queryid_extract( _ISO88591'%s',_ISO88591'SESSIONNAME') FROM JOBID", (char*)queryID );
		else
			sprintf( infoStatsStmt, "SELECT queryid_extract('%s','SESSIONNAME') FROM JOBID", (char*)queryID );
		memset(queryID, 0, sizeof(queryID));
		returncode = SQLBindCol( hstmt2, 1, SQL_C_CHAR, &queryID, sizeof(queryID), &queryIDPtr );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
			LogAllErrors(henv,hdbc,hstmt2);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		returncode = SQLExecDirect( hstmt2, (SQLCHAR *)infoStatsStmt, SQL_NTS );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
			LogAllErrors(henv,hdbc,hstmt2);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}
		returncode = SQLFetch( hstmt2 );
		if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
			LogAllErrors(henv,hdbc,hstmt2);
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}

		LogMsg(NONE, "JobID: \"%s\"\n", queryID);
		if (strcmp(jobIDMatrix[ loop ].jobIDExpected, queryID) == 0) {
			LogMsg(NONE, "JobIDs are matched. Expected: \"%s\", Actual: \"%s\" for after queryid_extract\n", jobIDMatrix[ loop ].jobIDExpected, queryID);
		}
		else {
			LogMsg(ERRMSG, "JobIDs are not matched. Expected: \"%s\", Actual: \"%s\", at line=%d for after queryid_extract\n", jobIDMatrix[ loop ].jobIDExpected, queryID, __LINE__);
		}

		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt2);
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return 0;
}
static int debugDatatypeSQL_FLOAT(t_ODBCHDLInfo *pTestInfo)
{
	RETCODE 		retcode;
	SQLHANDLE		henv;
	SQLHANDLE		hdbc;
	SQLHANDLE		hstmt;
	SQLLEN len;
	char	buf[120] = { 0 };
	SWORD numOfCols, i;
	
	//===========================================================================================================

	henv = pTestInfo->henv;
	hdbc = pTestInfo->hdbc;
	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	//====================================================================================================
	SQLExecDirect(hstmt, (SQLTCHAR*)"DROP TABLE IF EXISTS datatype", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"CREATE TABLE datatype (ID INT, C FLOAT)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "create table fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	len = SQL_NTS;
	int id = 2;
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &id, 0, &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter fail\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	SQLREAL fa = 12.56;
	len = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 54, 54, &fa, sizeof(fa), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLBindParameter fail\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"INSERT INTO datatype VALUES (?,?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for insert fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"SELECT * FROM datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLExecDirect for select fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		SQLFreeStmt(hstmt, SQL_CLOSE);
		return -1;
	}
	retcode = SQLNumResultCols(hstmt, &numOfCols);
	if (retcode != SQL_SUCCESS){
		LogMsg(NONE, "SQLNumResultCols fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	LogMsg(NONE, "numOfCols = %d\n", numOfCols);
	retcode = SQLFetch(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		LogMsg(NONE, "SQLFetch fail:\n");
		LogAllErrors(henv, hdbc, hstmt);
		return -1;
	}
	for(i = 1; i <= numOfCols; i++){
		memset(buf, 0, sizeof(buf));
		retcode = SQLGetData(hstmt,(SWORD)i,SQL_C_CHAR,buf,sizeof(buf),&len);
        if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA_FOUND && retcode != SQL_SUCCESS_WITH_INFO) {
            LogAllErrors(henv, hdbc, hstmt);
			break;
        } else if (retcode == SQL_NO_DATA_FOUND) {
            break;
        }
		LogMsg(NONE, "%s\n", buf);
	}
	
	//====================================================================================================
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
	
	//debugSQL(&mODBCHDLInfo);
	//debug_demo(&mODBCHDLInfo);
	//debugSQLErrorc000001d(&mODBCHDLInfo);
	//debugDatatype(&mODBCHDLInfo);
	debugDatatypeSQL_FLOAT(&mODBCHDLInfo);
	/***************************************/
	/*
	RETCODE 		returncode;
	returncode = SQLExecDirect( mODBCHDLInfo.hstmt, (SQLCHAR *)"create schema ODBC_TEST", SQL_NTS );
	if(!CHECKRC(SQL_SUCCESS,returncode,"SQLBindCol")) {
		printf("create schema fail\n");
	}
	else{
		printf("create schema success\n");
	}
	*/
	SQLFree(&mODBCHDLInfo);
	
	return 0;
}




