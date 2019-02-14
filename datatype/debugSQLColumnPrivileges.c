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


static int debugSQLColumnPrivileges(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	#define STR_LEN 128 + 1
	#define REM_LEN 254 + 1
	SWORD numOfCols = 0;

	SQLCHAR ColumnName[STR_LEN];
	SQLCHAR strCatalog[STR_LEN];
	SQLCHAR strSchema[STR_LEN];
	SQLCHAR strTableName[STR_LEN];
	SQLCHAR strGrantor[STR_LEN];
	SQLCHAR strGrantee[REM_LEN];
	SQLCHAR strPrivilege[STR_LEN];
	SQLCHAR strIsGrantable[STR_LEN];

	SQLSMALLINT    ColumnNameLen;
    SQLSMALLINT    ColumnDataType;
    SQLULEN        ColumnDataSize;
    SQLSMALLINT    ColumnDataDigits;
    SQLSMALLINT    ColumnDataNullable;
	
	SQLLEN lenCatalog;
	SQLLEN lenSchema;
	SQLLEN lenTableName;
	SQLLEN lenGrantor;
	SQLLEN lenGrantee;
	SQLLEN lenPrivilege;
	SQLLEN lenIsGrantable;
	CHAR cn[128] = {0};
	SWORD cl;
	SWORD st;
	SQLULEN cp;
	SWORD cs, cnull;
	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	SQLExecDirect(hstmt, (SQLCHAR*)"create table TB_DEBUG(C1 INTEGER UNSIGNED)", SQL_NTS);
	retcode = SQLColumnPrivileges(hstmt, NULL, 0, NULL, 0,
                                      (SQLCHAR*)"TB_DEBUG",
                                      SQL_NTS, NULL, 0);
	if (retcode != SQL_SUCCESS){
		printf("SQLColumnPrivileges fail:\n");
		DebugSQLError(henv, hdbc, hstmt);
		return -1;
	}
	retcode = SQLNumResultCols(hstmt, &numOfCols);
	if (retcode != SQL_SUCCESS){
		printf("SQLNumResultCols fail:\n");
		DebugSQLError(henv, hdbc, hstmt);
		return -1;
	}
	printf("numOfCols = %d\n", numOfCols);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        // Bind columns in result set to buffers
        SQLBindCol(hstmt, 1, SQL_C_CHAR,
                             strCatalog, STR_LEN, &lenCatalog);
        SQLBindCol(hstmt, 2, SQL_C_CHAR,
                             strSchema, STR_LEN, &lenSchema);
        SQLBindCol(hstmt, 3, SQL_C_CHAR,
                             strTableName, STR_LEN,&lenTableName);
        SQLBindCol(hstmt, 4, SQL_C_CHAR,
                             strGrantor, STR_LEN, &lenGrantor);
        SQLBindCol(hstmt, 5, SQL_C_CHAR,
                             strGrantee, STR_LEN, &lenGrantee);
        SQLBindCol(hstmt, 6, SQL_C_CHAR,
                             strPrivilege, STR_LEN, &lenPrivilege);
        SQLBindCol(hstmt, 7, SQL_C_CHAR,
                             strIsGrantable, STR_LEN, &lenIsGrantable);

        printf ("Catalog     Schema      Table       Grantor    ");
        printf ("Grantee     Is Grantable\n");
        printf ("----------- ----------- ----------- -----------");
        printf ("----------- ------------\n");
        while (SQL_SUCCESS == retcode) {
            retcode = SQLFetch(hstmt);

            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLFetch fail:\n");
				DebugSQLError(henv, hdbc, hstmt);
				return -1;
			}
            if (retcode == SQL_SUCCESS ||
                retcode == SQL_SUCCESS_WITH_INFO) {
                printf ("%s", strCatalog);
                printf ("%s", strSchema);
                printf ("%s", strTableName);
                printf ("%s", strGrantor);
                printf ("%s", strGrantee);
                printf ("%s", strPrivilege);
                printf ("%s\n", strIsGrantable);
            }
        }
    }
	else{
		DebugSQLError(henv, hdbc, hstmt);
		return -1;
	}
	retcode = SQLDescribeCol(hstmt,(SWORD)1,(SQLCHAR*)ColumnName,sizeof(ColumnName),&ColumnNameLen,&ColumnDataType,&ColumnDataSize,&ColumnDataDigits,&ColumnDataNullable);
	if (retcode != SQL_SUCCESS){
		printf("SQLDescribeCol fail:\n");
		DebugSQLError(henv, hdbc, hstmt);
		return -1;
	}
	printf("Column Name : %s\n  Column Name Len : %d\n  SQL Data Type : %d\n  Data Size : %d\n  DecimalDigits : %d\n  Nullable %d\n",
                 ColumnName, 
                 (int)ColumnNameLen, 
                 (int)ColumnDataType,
                 (int)ColumnDataSize, 
                 (int)ColumnDataDigits,
                 (int)ColumnDataNullable);
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
	
	debugSQLColumnPrivileges(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}

