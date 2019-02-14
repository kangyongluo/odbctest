#include "odbc.h"

static int DebugTestSQLSetConnectOption(t_ODBCHDLInfo *pTestInfo)
{
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	SQLUINTEGER		pvParamInt=0;

	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
	
	retcode = SQLFreeStmt(hstmt,SQL_DROP);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLFreeStmt fail:\n");
		return -1;
	}
	pvParamInt=SQL_ASYNC_ENABLE_ON;
	retcode = SQLSetConnectOption((SQLHANDLE)hdbc,SQL_ASYNC_ENABLE,pvParamInt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLSetConnectOption fail:SQL_ASYNC_ENABLE, SQL_ASYNC_ENABLE_ON\n");
	}
	else{
		pvParamInt= ~pvParamInt;
		retcode = SQLGetConnectOption((SQLHANDLE)hdbc,SQL_ASYNC_ENABLE,&pvParamInt);
		if (retcode != SQL_SUCCESS){
			//DEBUG_ODBC_MSG(hdbc);
			printf("SQLGetConnectOption fail:SQL_MAX_ROWS, SQL_ASYNC_ENABLE_ON\n");
		}
		else{
			if(pvParamInt != SQL_ASYNC_ENABLE_ON){
				printf("SQLSetConnectOption fail! %d\n", pvParamInt);
			}
		}
	}
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
	retcode = SQLInit(&mODBCHDLInfo, mDSNInfo, SQL_OV_ODBC2);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		printf("SQL init fail\n");

		return -1;
	}

	/*****************test********************/
	
	DebugTestSQLSetConnectOption(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}