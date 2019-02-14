
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
static int debugSQLColAttributes(t_ODBCHDLInfo *pTestInfo)
{
	SQLSMALLINT 	bufferLenUsed;
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	SQLSMALLINT numColumns = 0;
	#define STR_LEN 128 + 1
	#define REM_LEN 254 + 1
	SQLCHAR buf[STR_LEN];
	char createStr[] = "create table TB_DEBUG(C1 CHAR(10),C2 VARCHAR(10),C3 DECIMAL(10,5),C4 DECIMAL(5,2) UNSIGNED,C5 NUMERIC(10,5),C6 NUMERIC(5,2) UNSIGNED,C7 SMALLINT,C8 SMALLINT UNSIGNED,C9 INTEGER,C10 INTEGER UNSIGNED,C11 REAL,C12 FLOAT,C13 DOUBLE PRECISION,C14 DATE,C15 TIME,C16 TIMESTAMP,C17 BIGINT,C18 LONG VARCHAR,C19 NUMERIC(19,0),C20 NUMERIC(19,6),C21 NUMERIC(128,0),C22 NUMERIC(128,128),C23 NUMERIC(128,64),C24 NUMERIC(10,5) UNSIGNED,C25 NUMERIC(18,5) UNSIGNED,C26 NUMERIC(30,10) UNSIGNED) NO PARTITION";
	char insertStr[] = "insert into TB_DEBUG values('0123456789','0123456789',-1234.567,987.12,-1234.567,987.12,-934,3520,-12000,56343,123.45E2,123.45E3,123.45E4,{d '1993-07-01'},{t '09:45:30'},{ts '1993-08-02 08:44:31.001'},456789,'0123456789',1234567890123456789,1234567890123.456789,1234567890123456789012345678901234567890,0.123456789012345678901234567890123456789,1234567890.123456789012345678901234567890123456789,12345.56789,1234567890123.56789,12345678901234567890.0123456789)";
	SQLSMALLINT i, j;
	UWORD DescType[] = {
					SQL_COLUMN_AUTO_INCREMENT,
					SQL_COLUMN_CASE_SENSITIVE,
					SQL_COLUMN_COUNT,
					SQL_COLUMN_DISPLAY_SIZE,
					SQL_COLUMN_LENGTH,
					SQL_COLUMN_MONEY,
					SQL_COLUMN_NULLABLE,
					SQL_COLUMN_PRECISION,
					SQL_COLUMN_SCALE,
					SQL_COLUMN_SEARCHABLE,
					SQL_COLUMN_TYPE,
					SQL_COLUMN_UNSIGNED,
					SQL_COLUMN_UPDATABLE,
					SQL_COLUMN_NAME,
					SQL_COLUMN_TYPE_NAME,
					SQL_COLUMN_OWNER_NAME,
					SQL_COLUMN_QUALIFIER_NAME,
					SQL_COLUMN_TABLE_NAME,
					SQL_COLUMN_LABEL
					};
	char *DescTypeStr[30] = {
					"SQL_COLUMN_AUTO_INCREMENT",
					"SQL_COLUMN_CASE_SENSITIVE",
					"SQL_COLUMN_COUNT",
					"SQL_COLUMN_DISPLAY_SIZE",
					"SQL_COLUMN_LENGTH",
					"SQL_COLUMN_MONEY",
					"SQL_COLUMN_NULLABLE",
					"SQL_COLUMN_PRECISION",
					"SQL_COLUMN_SCALE",
					"SQL_COLUMN_SEARCHABLE",
					"SQL_COLUMN_TYPE",
					"SQL_COLUMN_UNSIGNED",
					"SQL_COLUMN_UPDATABLE",
					"SQL_COLUMN_NAME",
					"SQL_COLUMN_TYPE_NAME",
					"SQL_COLUMN_OWNER_NAME",
					"SQL_COLUMN_QUALIFIER_NAME",
					"SQL_COLUMN_TABLE_NAME",
					"SQL_COLUMN_LABEL"
					};
		
	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
#if 0
	SQLExecDirect(hstmt, (SQLCHAR*)"DROP TABLE TB_DEBUG", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)createStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"SELECT * FROM TB_DEBUG", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLNumResultCols(hstmt, &numColumns);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLNumResultCols fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	for(i = 0; i < numColumns; i++){
		printf("................................................\n");
		for(j = 0; j < (sizeof(DescType) / sizeof(DescType[0])); j++){
			printf("SQLColAttribute ---> %s\n", DescTypeStr[j]);
			memset(buf, 0, sizeof(buf));
			retcode = SQLColAttribute(hstmt, (SQLUSMALLINT)i + 1, DescType[j],buf, (SQLSMALLINT)sizeof(buf), &bufferLenUsed, &len);
			if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLColAttribute fail\n");
				DEBUG_ODBC_MSG(hstmt);
			}
			else{
				printf("%s\n", buf);
			}
		}
		printf("................................................\n");
	//}
#endif
#if 0
	SQLExecDirect(hstmt, (SQLCHAR*)"DROP TABLE TB_DEBUG", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table TB_DEBUG(C1 SMALLINT UNSIGNED)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"insert into TB_DEBUG values(3520)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"SELECT * FROM TB_DEBUG", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLNumResultCols(hstmt, &numColumns);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLNumResultCols fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	for(j = 0; j < (sizeof(DescType) / sizeof(DescType[0])); j++){
		printf("SQLColAttribute ---> %s\n", DescTypeStr[j]);
		memset(buf, 0, sizeof(buf));
		retcode = SQLColAttribute(hstmt, (SQLUSMALLINT)1, DescType[j],buf, (SQLSMALLINT)sizeof(buf), &bufferLenUsed, &len);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
			printf("SQLColAttribute fail\n");
			DEBUG_ODBC_MSG(hstmt);
		}
		else{
			printf("%s\n", buf);
		}
	}
#endif
#if 1
		SQLExecDirect(hstmt, (SQLCHAR*)"DROP TABLE TB_DEBUG", SQL_NTS);
		retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table TB_DEBUG(C1 INTEGER UNSIGNED)", SQL_NTS);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
			printf("create table fail:\n");
			DEBUG_ODBC_MSG(hstmt);
			return -1;
		}
		retcode = SQLExecDirect(hstmt, (SQLCHAR*)"insert into TB_DEBUG values(56343)", SQL_NTS);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
			printf("insert table fail:\n");
			DEBUG_ODBC_MSG(hstmt);
			return -1;
		}
		retcode = SQLExecDirect(hstmt, (SQLCHAR*)"SELECT * FROM TB_DEBUG", SQL_NTS);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
			printf("select table fail:\n");
			DEBUG_ODBC_MSG(hstmt);
			return -1;
		}
		retcode = SQLNumResultCols(hstmt, &numColumns);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
			printf("SQLNumResultCols fail:\n");
			DEBUG_ODBC_MSG(hstmt);
			return -1;
		}
		for(j = 0; j < (sizeof(DescType) / sizeof(DescType[0])); j++){
			printf("SQLColAttribute ---> %s\n", DescTypeStr[j]);
			memset(buf, 0, sizeof(buf));
			retcode = SQLColAttribute(hstmt, (SQLUSMALLINT)1, DescType[j],buf, (SQLSMALLINT)sizeof(buf), &bufferLenUsed, &len);
			if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLColAttribute fail\n");
				DEBUG_ODBC_MSG(hstmt);
			}
			else{
				printf("%s\n", buf);
			}
		}
#endif
	return 0;
}
int main(void)
{
	SQLRETURN retcode;
	t_ODBCHDLInfo mODBCHDLInfo;
	t_ODBCDSNInfo mDSNInfo;

	memset(&mDSNInfo, 0, sizeof(mDSNInfo));

//#ifdef __TRAFODION__ 
#if 0
	strcpy(mDSNInfo.DSN, "TRAF_ANSI");
	strcpy(mDSNInfo.user, "trafodion");
	strcpy(mDSNInfo.password, "traf123");
#else
	strcpy(mDSNInfo.DSN, (char *)"MySQL");
	strcpy(mDSNInfo.user, "root");
	strcpy(mDSNInfo.password, "123456");
#endif
	printf("SQL source:%s,%s,%s\n", mDSNInfo.DSN, mDSNInfo.user, mDSNInfo.password);
	retcode = SQLInit(&mODBCHDLInfo, mDSNInfo, SQL_OV_ODBC2);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		printf("SQL init fail\n");

		return -1;
	}

	/*****************test********************/
	
	debugSQLColAttributes(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}

