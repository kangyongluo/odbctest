#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int M_8380(TestInfo *pTestInfo)
{
	RETCODE     retcode, rc;
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
	SQLLEN len;
    int ret = 0;
    SWORD numOfCols, index;
    SQLPOINTER pToken;
	TCHAR szOut[128] = {0};
    
    LogMsg(NONE, _T("\nstart  test %s......\n"), __FUNCTION__);

    if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrors(henv,hdbc,hstmt);
        return -1;
    }
    LogMsg(NONE, _T("connect sucess...\n"));
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;

    //Allocate Statement handles
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle fail:line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            disconnect(henv, hdbc, hstmt);
            return -1;
        }
    }
    TCHAR *szDdl = _T("create table tb(")
                    _T("c1 CHAR(10),")
                    _T("c2 VARCHAR(10),")
                    _T("c3 DECIMAL(10,5),")
                    _T("c4 DECIMAL(10,5),")
                    _T("c5 SMALLINT,")
                    _T("c6 INTEGER,")
                    _T("c7 REAL,")
                    _T("c8 FLOAT,")
                    _T("c9 DOUBLE PRECISION,")
                    _T("c10 DATE,")
                    _T("c11 TIME,")
                    _T("c12 TIMESTAMP,")
                    _T("c13 NUMERIC(19,0),")
                    _T("c14 NUMERIC(19,6),")
                    _T("c15 NUMERIC(64,0),")
                    _T("C16 NUMERIC(64,28),")
                    _T("C17 NUMERIC(64,28),")
                    _T("C18 NUMERIC(10,5) UNSIGNED,")
                    _T("C19 NUMERIC(18,5) UNSIGNED,")
                    _T("C20 NUMERIC(30,10) UNSIGNED")
                    _T(")");
 #define CONFIG_COLUMNS_VALUES      20                   
    struct
	{
	    SQLSMALLINT SQLType[CONFIG_COLUMNS_VALUES];
		SQLUINTEGER			ColPrec[CONFIG_COLUMNS_VALUES];
		SQLSMALLINT			ColScale[CONFIG_COLUMNS_VALUES];
		TCHAR				*szIn[CONFIG_COLUMNS_VALUES];
		TCHAR				*szOut[CONFIG_COLUMNS_VALUES];
	} sTestDataInfo = 
	{	
		{
    		SQL_CHAR, SQL_VARCHAR,SQL_DECIMAL,SQL_NUMERIC,SQL_SMALLINT, SQL_INTEGER, SQL_REAL, SQL_FLOAT,SQL_DOUBLE, SQL_DATE,
            SQL_TIME, SQL_TIMESTAMP,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC,SQL_NUMERIC
        },
		{10,10,10,10,0,0,0,0,0,0,  0,0,19,19,64, 64,64,10,18,30},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 6, 0, 28, 28, 5, 5, 10},
		{_T("--"),_T("--"),_T("1234.56789"),_T("5678.12345"),_T("1234"),_T("12345"),_T("12340.0"),_T("12300.0"),_T("12345670.0"),_T("1993-12-30"),_T("11:45:23"),_T("1992-12-31 23:45:23.123456"),_T("1234567890123456789"),_T("1234567890123.456789"),_T("1234567890123456789012345678901234567890"),_T("0.123456789012345678901234567890123456789"),_T("1234567890.123456789012345678901234567890123456789"),_T("12345.56789"),_T("1234567890123.56789"),_T("12345678901234567890.0123456789")},
		{_T("--"),_T("--"),_T("1234.56789"),_T("5678.12345"),_T("1234"),_T("12345"),_T("12340.0"),_T("12300.0"),_T("12345670.0"),_T("1993-12-30"),_T("11:45:23"),_T("1992-12-31 23:45:23.123456"),_T("1234567890123456789"),_T("1234567890123.456789"),_T("1234567890123456789012345678901234567890"),_T("0.123456789012345678901234567890123456789"),_T("1234567890.123456789012345678901234567890123456789"),_T("12345.56789"),_T("1234567890123.56789"),_T("12345678901234567890.0123456789")}
	
	}; 
#if 0
    LogMsg(NONE, _T("Initialize table...\n"));
    SQLExecDirect(hstmt, (SQLTCHAR*)"drop table if exists tb", SQL_NTS);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szDdl, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail.\n"));
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }
#else
    LogMsg(NONE, _T("purgedata...\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)_T("purgedata tb"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail.\n"));
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }
#endif
    retcode = SQLPrepare(hstmt, (SQLTCHAR*)_T("insert into tb values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLPrepare  fail.\n"));
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }
    for(index = 1; index <= CONFIG_COLUMNS_VALUES; index++){
        len = SQL_DATA_AT_EXEC;
        retcode = SQLBindParameter(hstmt, 
                                    index, 
                                    SQL_PARAM_INPUT, 
                                    SQL_C_CHAR, 
                                    sTestDataInfo.SQLType[index - 1], 
                                    sTestDataInfo.ColPrec[index - 1], 
                                    sTestDataInfo.ColScale[index - 1], 
                                    NULL, 
                                    128, 
                                    &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLBindParameter  fail: %d\n"), index);
            LogAllErrors(henv, hdbc, hstmt);
            ret = -1;
            goto DEBUG_SQL_PARAM_DATA_FAIL;
        }
    }
    retcode = SQLExecute(hstmt);
    if(retcode != SQL_NEED_DATA)
    {
        LogMsg(NONE, _T("SQLExecute  fail.\n"));
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
        goto DEBUG_SQL_PARAM_DATA_FAIL;
    }
  
    for(index = 1; index <= CONFIG_COLUMNS_VALUES; index++){
        LogMsg(NONE, _T("SQLPutData(hstmt, \"%s\", %s);\n"), sTestDataInfo.szIn[index - 1], _T("SQL_NTS"));
        retcode = SQLParamData(hstmt,&pToken);
        if(retcode == SQL_NEED_DATA){
            retcode = SQLPutData(hstmt, sTestDataInfo.szIn[index - 1], SQL_NTS);
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("SQLPutData  fail: row:%d, columns:%d\n"), 1, index);
                LogAllErrors(henv, hdbc, hstmt);
                ret = -1;
                goto DEBUG_SQL_PARAM_DATA_FAIL;
            }
        }
        else{
            LogMsg(NONE, _T("SQLParamData  fail: row:%d, columns:%d\n"), 1, index);
            LogAllErrors(henv, hdbc, hstmt);
            ret = -1;
            goto DEBUG_SQL_PARAM_DATA_FAIL;
        }
    }
    do {
        retcode = SQLParamData(hstmt, &pToken);
    }while(retcode == SQL_NEED_DATA);
    

    SQLFreeStmt(hstmt,SQL_UNBIND);
    SQLFreeStmt(hstmt,SQL_CLOSE);
    LogMsg(NONE,_T("verify data...\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)_T("select * from tb"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE,_T("do select fail.\n"));
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        retcode = SQLFetch(hstmt);
        numOfCols = 0;
        retcode = SQLNumResultCols(hstmt, &numOfCols);
        while ((retcode != SQL_NO_DATA) && (numOfCols != 0)){
            for (index = 1; index <= numOfCols; index++){
                retcode = SQLGetData(hstmt, index, SQL_C_CHAR, (SQLPOINTER)szOut, sizeof(szOut), &len);
                if (SQL_SUCCEEDED(retcode)){
                    LogMsg(NONE, _T("%s\n"), szOut);
                }
                else{
                    break;
                }
            }
            retcode = SQLFetch(hstmt);
        }
    }

DEBUG_SQL_PARAM_DATA_FAIL:
    disconnect(henv, hdbc, hstmt);
    LogMsg(NONE, _T("Complete test....%s\n"), (ret == 0) ? _T("pass") : _T("fail"));
    
    return 0;
}

void help(void)
{
    printf("\t-h:print this help\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema name\t");
}

/*
*sample:
*       trafodion:
*        gcc xxx.c util.c -o debug -L /usr/lib64/ -I /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli -g -w
*         ./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST
*       unixODBC:
*        gcc xxx.c util.c -o debug  -lodbc -lpthread -lz  -Dunixcli -DUNIXODBC -g -w
*        ./debug -d traf -u trafodion -p traf123 -s ODBCTEST
*/
int main(int argc, char *argv[])
{
    TestInfo oriInfo;
    char buf[256] = {0};
    int num;

    memset(&oriInfo, 0, sizeof(oriInfo));

    if(argc <= 1){
        help();
        return -1;
    }
    for(num = 1; num < argc; num++){
        if(strcmp(argv[num], "-h") == 0){
            help();
            return 0;
        }
        else if(strcmp(argv[num], "-h") == 0){
            help();
            return 0;
        }
        else if((strcmp(argv[num], "-d") == 0) && (argc > (num + 1))){
            num++;
            strcpy ((char *)oriInfo.DataSource, argv[num]);
        }
        else if((strcmp(argv[num], "-u") == 0) && (argc > (num + 1))){
            num++;
            strcpy ((char *)oriInfo.UserID, argv[num]);
        }
        else if((strcmp(argv[num], "-p") == 0) && (argc > (num + 1))){
            num++;
            strcpy ((char *)oriInfo.Password, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-s")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Schema, argv[num]);
        }
    }
    if(strlen((char *)oriInfo.DataSource) == 0 ||
        strlen((char *)oriInfo.UserID) == 0 ||
        strlen((char *)oriInfo.Password) == 0
        ){
        help();
        return -1;
    }   
    M_8380(&oriInfo);
#ifndef unixcli
    LogMsg(NONE, _T("\nPlease enter any key and exit\n"));
    getchar();
#endif
    return 0;
}

