#include "odbc.h"

static int DebugSQLSetStmtAttr30(t_ODBCHDLInfo *pTestInfo)
{
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	SQLUINTEGER vParamInt;
	char TestAsync[1000] = {0};
	
	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	vParamInt = SQL_ASYNC_ENABLE_ON;
	retcode = SQLSetStmtAttr(hstmt,SQL_ASYNC_ENABLE,(void *)vParamInt,0);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("set attr fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table TESTCOLPRIV (col char(10)) no partition", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		//return -1;
	}
	retcode = SQL_STILL_EXECUTING;
	while(retcode != SQL_STILL_EXECUTING){
		retcode = SQLTables(hstmt,(SQLCHAR *)"",SQL_NTS,(SQLCHAR *)"",SQL_NTS,(SQLCHAR *)TestAsync,SQL_NTS,(SQLCHAR *)"TABLE",SQL_NTS);
		if (retcode != SQL_STILL_EXECUTING && retcode != SQL_SUCCESS)
		{
			printf("SQLTablePrivileges fail:\n");
			DEBUG_ODBC_MSG(hstmt);
			return -1;
		}
	}
	SQLFreeStmt(hstmt,SQL_CLOSE);
	retcode = SQL_STILL_EXECUTING;
	while(retcode != SQL_STILL_EXECUTING){
		retcode = SQLTablePrivileges(hstmt,(SQLCHAR *)"",SQL_NTS,(SQLCHAR *)"",SQL_NTS,(SQLCHAR *)TestAsync,SQL_NTS);
		if (retcode != SQL_STILL_EXECUTING && retcode != SQL_SUCCESS)
		{
			printf("SQLTablePrivileges fail:\n");
			DEBUG_ODBC_MSG(hstmt);
			return -1;
		}
	}
	
	printf("%s\n", TestAsync);
	
	return 0;
}
int main(void)
{
	SQLRETURN retcode;
	t_ODBCHDLInfo mODBCHDLInfo;
	t_ODBCDSNInfo mDSNInfo;

	memset(&mDSNInfo, 0, sizeof(mDSNInfo));

#ifdef __TRAFODION__ 
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
	
	DebugSQLSetStmtAttr30(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}

