#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int M_PutDataCoreDown(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    int ret = 0;
    int i;
    PTR pToken;

   if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
        return -1;
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    hstmt = (SQLHANDLE)pTestInfo->hstmt;

           
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle fail¡£\n"));
        LogMsg(NONE,_T("Try SQLAllocStmt\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail.\n"));
            disconnect(henv, hdbc, hstmt);
            return -1;
        }
    }

TEST_FAIL:
    disconnect(henv, hdbc, hstmt);
    LogMsg(TIME_STAMP, _T("complete test:%s\n"), (ret == 0) ? _T("sucess") : _T("fail")); 
    
    return ret;
}

typedef struct putdata_
{
    SQLSMALLINT sqltype;
    SQLULEN colDef;
    SQLSMALLINT scale;
    TCHAR szValues[64];
}putdata;
void test(TestInfo *pTestInfo)
{
    TCHAR szBuf[10240] = {0};

    memset(szBuf, 0, sizeof(szBuf));
}
/*******************************************************************************************************************/
int M_SQLPutData(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    int ret = 0;
    int i;
    PTR pToken;

   if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
        return -1;
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    hstmt = (SQLHANDLE)pTestInfo->hstmt;
    TCHAR szDropDDL[] = _T("drop table tb");
    TCHAR szCreateDDL[] =_T("create table tb(\n")
                        _T("c1 CHAR(10) CHARACTER SET ISO88591\n")
                        _T(",c2 VARCHAR(10) CHARACTER SET ISO88591\n")
                        _T(",c3 DECIMAL(10,5)\n")
                        _T(",c4 NUMERIC(10,5)\n")
                        _T(",c5 SMALLINT\n")
                        _T(",c6 INTEGER\n")
                        _T(",c7 REAL\n")
                        _T(",c8 FLOAT\n")
                        _T(",c9 DOUBLE PRECISION\n")
                        _T(",c10 DATE\n")
                        _T(",c11 TIME\n")
                        _T(",c12 TIMESTAMP")
                        _T(",c13 LONG VARCHAR CHARACTER SET ISO88591\n")
                        _T(",c14 BIGINT\n")
                        _T(",c15 CHAR(10) CHARACTER SET UCS2\n")
                        _T(",c16 VARCHAR(10) CHARACTER SET UCS2\n")
                        _T(",c17 LONG VARCHAR CHARACTER SET UCS2\n")
                        _T(",c18 NUMERIC(19,0)\n")
                        _T(",c19 NUMERIC(19,6)\n")
                        _T(",c20 NUMERIC(128,0)\n")
                        _T(",c21 NUMERIC(128,128)\n")
                        _T(",c22 NUMERIC(128,64)\n")
                        _T(",c23 NUMERIC(10,5) UNSIGNED\n")
                        _T(",c24 NUMERIC(18,5) UNSIGNED\n")
                        _T(",c25 NUMERIC(30,10) UNSIGNED\n")
                        _T(")");
    TCHAR szPrepare[] = _T("insert into tb values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
     putdata mputdata[]={
            {SQL_CHAR, 254, 0, _T("z=j3}l0m!S")},
            {SQL_VARCHAR, 254, 0, _T("z=j3}l0m!S")},
            {SQL_DECIMAL, 10, 5, _T("1234.56789")},
            {SQL_NUMERIC, 10, 5, _T("5678.12345")},
            {SQL_SMALLINT, 5, 0, _T("1234")},
            {SQL_INTEGER, 10, 0, _T("12345")},
            {SQL_REAL, 7, 0, _T("12340.0")},
            {SQL_FLOAT, 15, 0, _T("12300.0")},
            {SQL_DOUBLE, 15, 0, _T("12345670.0")},
            {SQL_DATE, 0, 0, _T("1993-12-30")},
            {SQL_TIME, 0, 0, _T("11:45:23")},
            {SQL_TIMESTAMP, 0, 0, _T("1992-12-31 23:45:23.123456")},
            {SQL_LONGVARCHAR, 2000, 0, _T("ASD")},
            {SQL_BIGINT, 0, 0, _T("123")},
            {SQL_WCHAR, 254, 0, _T("ADBC")},
            {SQL_WVARCHAR, 254, 0, _T("ADD")},
            {SQL_WLONGVARCHAR, 2000, 0, _T("DSDA")},
            {SQL_NUMERIC, 19, 0, _T("123.0")},
            {SQL_NUMERIC, 19, 6, _T("123.45")},
            {SQL_NUMERIC, 128, 0, _T("123.0")},
            {SQL_NUMERIC, 128, 128, _T("0.12345")},
            {SQL_NUMERIC, 128, 64, _T("123.45")},
            {SQL_NUMERIC, 10, 5, _T("123.45")},
            {SQL_NUMERIC, 18, 5, _T("123.45")},
            {SQL_NUMERIC, 30, 10, _T("123.45")},
    };               
    test(pTestInfo);
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle fail¡£\n"));
        LogMsg(NONE,_T("Try SQLAllocStmt\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail.\n"));
            disconnect(henv, hdbc, hstmt);
            return -1;
        }
    }
#if 1
    LogMsg(NONE, _T("%s\n"), szDropDDL);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szDropDDL, SQL_NTS);
    LogMsg(NONE, _T("%s\n"), szCreateDDL);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szCreateDDL, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("create table fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
#endif
    retcode = SQLPrepare(hstmt,(SQLTCHAR*)szPrepare, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("SQLPrepare fail !\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
    for(i = 0; i < (sizeof(mputdata) / sizeof(putdata)); i++){
        len = SQL_DATA_AT_EXEC;
        printf("[%s:%d]%d %d %d %d\n", __FILE__, __LINE__, SQL_C_TCHAR, mputdata[i].sqltype,mputdata[i].colDef,mputdata[i].scale);
    	retcode = SQLBindParameter(hstmt,
                                   i+1,
                                    SQL_PARAM_INPUT,
                                    SQL_C_TCHAR,
            						mputdata[i].sqltype,
            						mputdata[i].colDef,
            						mputdata[i].scale,
            						NULL,
            						300,//_tcslen(mputdata[i].szValues),
            						&len);
        if(retcode != SQL_SUCCESS){
            LogMsg(NONE, _T("SQLBindParameter fail ! columns:%d\n"), i+1);
            ret = -1;
            goto TEST_FAIL;
        }
    }
    retcode = SQLExecute(hstmt);
    if(retcode != SQL_NEED_DATA){
        LogMsg(NONE, _T("SQLExecute fail !\n"));
        ret = -1;
        goto TEST_FAIL;
    }
    
    for(i = 0; i < (sizeof(mputdata) / sizeof(putdata)); i++){
        retcode = SQLParamData(hstmt,&pToken);
        if(retcode != SQL_NEED_DATA){
            LogMsg(NONE, _T("SQLParamData fail !\n"));
            LogAllErrors(henv,hdbc,hstmt);
            ret = -1;
            goto TEST_FAIL;
        }
        printf("columns:%d SQLPutData(hstmt, \"%s\", SQL_NTS)\n", i+1, mputdata[i].szValues);
        retcode = SQLPutData(hstmt, mputdata[i].szValues, SQL_NTS);
        if(retcode != SQL_SUCCESS){
            LogMsg(NONE, _T("SQLPutData fail ! columns:%d\n"), i + 1);
            ret = -1;
            goto TEST_FAIL;
        }
    }
    retcode = SQLParamData(hstmt,&pToken);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("SQLParamData fail !\n"));
        LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
        goto TEST_FAIL;
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
        _tcslen(oriInfo.Password) == 0/* ||
        _tcslen(oriInfo.Schema) == 0*/
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    M_SQLPutData(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


