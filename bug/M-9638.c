#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int M_Tables(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    int cases, ret;
    SQLSMALLINT i, num;
    char szBuf[512] = {0};

    ret = 0;

    if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
        LogAllErrors(henv,hdbc,hstmt);
        return -1;
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    hstmt = (SQLHANDLE)pTestInfo->hstmt;
    
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
    
    TCHAR *szCreate = (TCHAR *)"create table tb(\n"
                                "c1 char(10) CHARACTER SET ISO88591\n"
                                ",c2 varchar(10) CHARACTER SET ISO88591\n"
                                ",c3 decimal(10,5)\n"
                                ",c4 real\n"
                                ",c5 float\n"
                                ",c6 double precision\n"
                                ") no partition\n";
    
    LogMsg(NONE,_T("ddl:\n"));
    LogMsg(NONE,_T("%s\n"), szCreate);
#if 1
    SQLExecDirect(hstmt, (SQLTCHAR*)_T("drop table tb"), SQL_NTS);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szCreate, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("create table  fail:\n"));
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_FAIL;
    }
#endif
    addInfoSession(hstmt);
    for(cases = 0; cases < 2; cases++){
        LogMsg(NONE, _T("Call:\n"), __LINE__);
        _stprintf(szBuf, "SQLTables(hstmt, \"TRAFODION\",SQL_NTS,\"%s\",SQL_NTS,\"%s\",SQL_NTS,\"\",SQL_NTS)", pTestInfo->Schema, (cases == 0) ? _T("tb") : _T("TB"));
        LogMsg(NONE, _T("%s\n"), szBuf);
        if(cases == 0){
            _stprintf(szBuf, _T("%s"), _T("tb"));
        }
        else{
            _stprintf(szBuf, _T("%s"), _T("TB"));
        }
        retcode = SQLTables(hstmt, (SQLTCHAR*)"TRAFODION", SQL_NTS, (SQLTCHAR*)pTestInfo->Schema, SQL_NTS, (SQLTCHAR*)szBuf, SQL_NTS, (SQLTCHAR*)"", SQL_NTS);
        if (retcode != SQL_SUCCESS){
            ret = -1;
    		LogMsg(NONE, _T("SQLTables fail, line %d\n"), __LINE__);
            LogAllErrors(henv,hdbc,hstmt);
            continue;
    	}
        num = 0;
        retcode = SQLNumResultCols(hstmt, &num);
        if(num == 0){
            ret = -1;
            LogMsg(NONE, _T("SQLNumResultCols fail, line %d\n"), __LINE__);
            LogAllErrors(henv,hdbc,hstmt);
            continue;
        }
        retcode = SQLFetch(hstmt);
        if(retcode == SQL_NO_DATA){
            LogMsg(NONE, _T("SQLFetch and return SQL_NO_DATA\n"));
            ret = -1;
        }
        else{
            printf("................................................\n");
            printf("Catalog         schema          table       type\n");
            printf("................................................\n");
        }
        while (retcode != SQL_NO_DATA){
            for (i = 1; i <= num; ++i){
                memset(szBuf, 0, sizeof(szBuf));
                retcode = SQLGetData(hstmt, i, SQL_C_CHAR, (SQLPOINTER)szBuf, sizeof(szBuf), &len);
                if (SQL_SUCCEEDED(retcode)){
                    printf("%s\t", szBuf);
                }
                else{
                    break;
                }
            }
            printf("\n");
            retcode = SQLFetch(hstmt);
        } 
    }
    
TEST_FAIL:
    disconnect(henv, hdbc, hstmt);
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
//g++  xxx.c util.c -o debug -I /usr/lib64 -L /usr/lib64 -ltrafodbc64  -Dunixcli
//g++ xxx.c  util.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -DUNIXODBC -o debug
//./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST
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
        _tcslen(oriInfo.Password) == 0 ||
        _tcslen(oriInfo.Schema) == 0
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    M_Tables(&oriInfo);
    LogMsg(NONE, _T("\n\n"));
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


