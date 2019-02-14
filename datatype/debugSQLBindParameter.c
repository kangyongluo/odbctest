#include "odbc.h"

static int debugSQLBindParameter30(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	char fa[10] = "23";

	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	SQLExecDirect(hstmt, (SQLCHAR*)"drop table testbindpara", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table testbindpara(c1 NUMERIC(8,0))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLPrepare(hstmt,(SQLCHAR*)"insert into testbindpara values(?)",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLPrepare fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	len = SQL_NTS;
	retcode = SQLBindParameter(hstmt,
					(SWORD)1,
					SQL_PARAM_INPUT,
					SQL_C_CHAR,
					SQL_NUMERIC,
					8,
					0,
					fa,
					sizeof(fa),
					&len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter  fail:\n\n");
		return -1;
	}
	retcode = SQLExecute(hstmt);
	if (retcode != SQL_SUCCESS){
		printf("SQLExecute  fail:\n\n");
		LogAllErrors(henv,hdbc,hstmt);
		return -1;
	}
	printf("test SQLBindParameter success\n");
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
	
	debugSQLBindParameter30(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}

