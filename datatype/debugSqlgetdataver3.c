#include "odbc.h"

static int debugSqlgetdatav3(t_ODBCHDLInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE 	henv;
 	SQLHANDLE 	hdbc;
 	SQLHANDLE	hstmt;
	char    UCS2Output[500] = {0};
	char teststr[300] = "0000000000123456789a";
	char acttstr[300] = "0000000000123456789a";
	char insertStr[500] = {0};
	char  temp[500] = {0};
	int j;
		
	henv = pTestInfo->hevn;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;

	SQLExecDirect(hstmt, (SQLCHAR*)"DROP TABLE debugcoast", SQL_NTS);
	//retcode = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE TABLE debugcoast( C1 CHAR(200) CHARACTER SET UCS2, C2 CHAR(200)  NOT NULL NOT DROPPABLE PRIMARY KEY)", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE TABLE debugcoast(C1 CHAR(20)  CHARACTER SET UCS2)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	//sprintf(insertStr, "%s (_UCS2'%s', '%s')", "INSERT INTO debugcoast VALUES ", teststr, acttstr);
	sprintf(insertStr, "%s ('%s')", "INSERT INTO debugcoast VALUES ", teststr, acttstr);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)insertStr, SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"select * from debugcoast", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select table fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
    retcode = SQLFetch(hstmt);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLFetch fail:\n");
		DEBUG_ODBC_MSG(hstmt);
		return -1;
	}
	j = 0;
	sprintf(temp,"%-*s",200,acttstr);
	memset(insertStr, 0, sizeof(insertStr));
	while (retcode != SQL_NO_DATA_FOUND) {
		retcode = SQLGetData(hstmt,(SWORD)(1),SQL_C_CHAR,UCS2Output,11,&len);
        if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA_FOUND && retcode != SQL_SUCCESS_WITH_INFO) {
            DEBUG_ODBC_MSG(hstmt);
        } else if (retcode == SQL_NO_DATA_FOUND) {
            break;
        }
		printf("-----len = %d------------j = %d----------%s\n", len, j, UCS2Output);
		j++;
	}
	printf("test debugSqlgetdatav3 success\n");
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
	
	debugSqlgetdatav3(&mODBCHDLInfo);
	
	/***************************************/

	SQLFree(&mODBCHDLInfo);
	
	return 0;
}


