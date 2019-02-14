#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


/*******************************************************************************************************************/
int M_9933(TestInfo *pTestInfo)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT num, i;
    char szBuf[256] = {0};
    SQLLEN len;
    PTR pToken;
    SWORD numOfCols, index;
    SFLOAT fValue;

    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));

    LogMsg(NONE, _T("..............................\n"));
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
        LogMsg(NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnect(&sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    LogMsg(NONE, _T("server info:\n"));
    addInfoSession(hstmt);
    LogMsg(NONE, _T("..............................\n"));
    TCHAR *szDdl = _T("create table tb(")
                    _T("c1 NUMERIC(18,0),")
                    _T("c2 NUMERIC(19,0),")
                    _T("c3 NUMERIC(20,0),")
                    _T("C4 NUMERIC(18,5),")
                    _T("C5 NUMERIC(19,5)")
                    _T(")");
 #define CONFIG_COLUMNS_VALUES      5                   
    struct
	{
		SQLUINTEGER			ColPrec[CONFIG_COLUMNS_VALUES];
		SQLSMALLINT			ColScale[CONFIG_COLUMNS_VALUES];
		SFLOAT				fInValue[CONFIG_COLUMNS_VALUES];
		SFLOAT				fOutValue[CONFIG_COLUMNS_VALUES];
        TCHAR               szOutValue[CONFIG_COLUMNS_VALUES][32];
	} sTestDataInfo = 
	{	
		{18,19,20,18,19},
		{0, 0, 0, 5, 5},
		{12345678.0, 12345678.0, 12345678.0, 1.23456123e+5, 1.23456123e+5},
		{12345678.0, 12345678.0, 12345678.0, 1.23456123e+5, 1.23456123e+5},
        {_T("12345678"), _T("12345678"), _T("12345678"), _T("123456.123"), _T("123456.123")}      
	
	}; 
    LogMsg(NONE, _T("ddl info:\n\t\t\t\t%s\n"), szDdl);

#if 1
#if 0
    LogMsg(NONE, _T("initialize table...\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"drop table if exists tb", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
    }
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szDdl, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_FAIL;
    }
#else
    LogMsg(NONE, _T("purgedata...\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"purgedata tb", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
    }
#endif
    retcode = SQLPrepare(hstmt, (SQLTCHAR*)_T("insert into tb values(?,?,?,?,?)"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLPrepare  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_FAIL;
    }
    for(index = 1; index <= CONFIG_COLUMNS_VALUES; index++){
        len = SQL_NTS;
        LogMsg(NONE, _T("insert data:column:%d value:%f\n"), index, sTestDataInfo.fInValue[index - 1]);
        retcode = SQLBindParameter(hstmt, 
                                    index, 
                                    SQL_PARAM_INPUT, 
                                    SQL_C_FLOAT, 
                                    SQL_NUMERIC, 
                                    sTestDataInfo.ColPrec[index - 1], 
                                    sTestDataInfo.ColScale[index - 1], 
                                    &sTestDataInfo.fInValue[index - 1], 
                                    sizeof(SFLOAT), 
                                    &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLBindParameter  fail: %d\n"), index);
            LogAllErrors(henv, hdbc, hstmt);
            goto TEST_FAIL;
        }
    }
    retcode = SQLExecute(hstmt);
    if(retcode != SQL_SUCCESS)
    {
        LogMsg(NONE, _T("SQLExecute  fail: %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_FAIL;
    }
    SQLFreeStmt(hstmt,SQL_UNBIND);
    SQLFreeStmt(hstmt,SQL_CLOSE);

#endif
    LogMsg(NONE,_T("verify data...,SQLGetData by SQL_C_TCHAR\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)_T("select * from tb"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE,_T("do select fail, line %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
    }
    else{
        retcode = SQLFetch(hstmt);
        numOfCols = 0;
        retcode = SQLNumResultCols(hstmt, &numOfCols);
        while ((retcode != SQL_NO_DATA) && (numOfCols != 0)){
            for (index = 1; index <= numOfCols; index++){
                retcode = SQLGetData(hstmt, index, SQL_C_TCHAR, (SQLPOINTER)szBuf, sizeof(szBuf), &len);
                if (SQL_SUCCEEDED(retcode)){
                    LogMsg(NONE, _T("column:%d expect:%s actual:%s\n"), 
                                index,
                                sTestDataInfo.szOutValue[index - 1], 
                                szBuf);
                    if(_tcsncmp(sTestDataInfo.szOutValue[index - 1], szBuf, _tcslen(sTestDataInfo.szOutValue[index - 1])) != 0){
                        LogMsg(NONE, _T("\tnot match\n"));
                        ret = -1;
                    }
                }
                else{
                    break;
                }
            }
            retcode = SQLFetch(hstmt);
        }
    }
    SQLFreeStmt(hstmt,SQL_UNBIND);
    SQLFreeStmt(hstmt,SQL_CLOSE);
    LogMsg(NONE,_T("verify data...,SQLGetData by SQL_C_FLOAT\n"));
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)_T("select * from tb"), SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE,_T("do select fail, line %d\n"), __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
    }
    else{
        retcode = SQLFetch(hstmt);
        numOfCols = 0;
        retcode = SQLNumResultCols(hstmt, &numOfCols);
        while ((retcode != SQL_NO_DATA) && (numOfCols != 0)){
            for (index = 1; index <= numOfCols; index++){
                retcode = SQLGetData(hstmt, index, SQL_C_FLOAT, (SQLPOINTER)&fValue, sizeof(fValue), &len);
                if (SQL_SUCCEEDED(retcode)){
                    LogMsg(NONE, _T("column:%d expect:%f actual:%f\n"), 
                                index,
                                sTestDataInfo.fOutValue[index - 1], 
                                fValue);
                    if(fabs(sTestDataInfo.fOutValue[index - 1] - fValue) >= 0.1){
                        LogMsg(NONE, _T("\tnot match\n"));
                        ret = -1;
                    }
                }
                else{
                    break;
                }
            }
            retcode = SQLFetch(hstmt);
        }
    }
TEST_FAIL:

    FullDisconnect(&sTestInfo);
    LogMsg(TIME_STAMP, _T("complete test:%s\n"), (ret == 0) ? _T("sucess") : _T("fail"));
    
    return ret;
}

/*******************************************************************************************************************/
void help(void)
{
    printf("\t-h:print this help\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema name\t");
    printf("sample:\t./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST\n");
}
//g++  xxx.c -o debug -I /usr/lib64 -L /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli
//g++ xxx.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug
//./debug -d TRAF_ANSI -u trafodion -p traf123
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
        if(_tcscmp(argv[num], _T("-h")) == 0){
            help();
            return 0;
        }
        else if((_tcscmp(argv[num], _T("-d")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.DataSource, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-u")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.UserID, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-p")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Password, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-s")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Schema, argv[num]);
        }
    }
    if(_tcslen(oriInfo.DataSource) == 0 ||
        _tcslen(oriInfo.UserID) == 0 ||
        _tcslen(oriInfo.Password) == 0
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    M_9933(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

