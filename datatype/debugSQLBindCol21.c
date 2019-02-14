#include "odbc.h"

static int debugSQLBindCol21(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	char    buf[2][20] = {0};
		
	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	SQLExecDirect(hstmt, (SQLCHAR*)"DROP TABLE TB_DEBUG", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE TABLE TB_DEBUG(C1 NUMERIC(10,5), C2 NUMERIC(1,0) UNSIGNED)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"INSERT INTO TB_DEBUG VALUES (123.0, 9.0)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"select * from TB_DEBUG", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	SQLBindCol(hstmt,(SWORD)1,SQL_C_DEFAULT,buf[0],sizeof(buf[0]),&len);
	SQLBindCol(hstmt,(SWORD)2,SQL_C_DEFAULT,buf[1],sizeof(buf[1]),&len);
    retcode = SQLFetch(hstmt);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLFetch fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	printf("%s, %s\n", buf[0], buf[1]);
	
	printf("test debugSQLBindCol21 success\n");
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
	
	debugSQLBindCol21(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}

