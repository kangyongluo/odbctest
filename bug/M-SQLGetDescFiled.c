#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

struct sDescType
{
    SQLINTEGER descType;
    TCHAR szDesc[32];
};
/*******************************************************************************************************************/
int debugSQLGetDescField(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    int ret = 0;
    SQLSMALLINT		AllocTypeValue;
    int i;
    SQLHDESC hDesc;
    SQLINTEGER  cbIRD;
    
    struct sDescType sDescs[] = {	
					{SQL_ATTR_APP_PARAM_DESC, "SQL_ATTR_APP_PARAM_DESC"}, 
					{SQL_ATTR_IMP_PARAM_DESC, "SQL_ATTR_IMP_PARAM_DESC"},
					{SQL_ATTR_APP_ROW_DESC, "SQL_ATTR_APP_ROW_DESC"},
					{SQL_ATTR_IMP_ROW_DESC, "SQL_ATTR_IMP_ROW_DESC"}
                    };
    
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
    
    //addInfoSession(hstmt);
    /*
    *unixODBC:
    *In the unixODBC version, I need to call an API such as SQLExecDirect or SQLPrepare, 
    *and "SQLGetDescField(hDesc, 1, SQL_DESC_ALLOC_TYPE, &AllocTypeValue, 0, NULL)" can return SQL_SUCCESS.
    */
    //SQLExecDirect(hstmt, (SQLTCHAR*)"show  tables", SQL_NTS);
    /*******************************************************************************/
    LogMsg(NONE, _T("SQLGetStmtAttr(hstmt, SQL_ATTR_IMP_ROW_DESC, hDesc,0, NULL)\n"));
    retcode = SQLGetStmtAttr(hstmt, SQL_ATTR_IMP_ROW_DESC, &hDesc, 0, NULL);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("call SQLGetStmtAttr fail.\n"));
        LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
    }
    else{
        LogMsg(NONE, _T("SQLGetDescField(hDesc, 0, SQL_DESC_ALLOC_TYPE, &AllocTypeValue, 0, NULL)\n"));
        retcode = SQLGetDescField(hDesc, 1, SQL_DESC_ALLOC_TYPE, &AllocTypeValue, 0, NULL);
        if(retcode != SQL_SUCCESS){
            LogMsg(NONE, _T("call SQLGetDescField fail. retcode=%d\n"), retcode);
            LogAllErrors(henv,hdbc,hstmt);
            ret = -1;
        }
        else{
            LogMsg(NONE, _T("type value: expect:%d actual:%d\n"), SQL_DESC_ALLOC_AUTO, AllocTypeValue);
        }
    }
    /*******************************************************************************/
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
    debugSQLGetDescField(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


