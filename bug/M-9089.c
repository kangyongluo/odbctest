#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int testTenant(TestInfo *pTestInfo)
{
    RETCODE retcode;
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
    int ret = 0;
    //===========================================================================================================
    LogMsg(NONE, _T("use \"TenantName=esgyndb\" on DSN\n"));
    //if(FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)  != TRUE)
    if(FullConnectWithOptionsEx(pTestInfo, CONNECT_ODBC_VERSION_3, "")  != TRUE)
    {
        ret = -1;
        LogMsg(NONE, _T("Unable to connect\n"));
    }
    else{
        LogMsg(NONE, _T("connect sucess.\n"));
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    disconnect(henv, hdbc, hstmt);
    LogMsg(NONE, _T("use \"TenantName=esgyndb\" on URL\n"));
    if(FullConnectWithOptionsEx(pTestInfo, CONNECT_ODBC_VERSION_3, "TenantName=esgyndb")  != TRUE)
    {
        ret = -1;
        LogMsg(NONE, _T("Unable to connect\n"));
    }
    else{
        LogMsg(NONE, _T("connect sucess.\n"));
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    disconnect(henv, hdbc, hstmt);

    LogMsg(NONE, _T("Complete test....%s\n"), (ret == 0) ? _T("pass") : _T("fail"));
    
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
//g++ xxx.c util.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug 
//./debug -d traf -u trafodion -p traf123
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
    /*
    [root@localhost bug]# odbcinst -j
    unixODBC 2.3.7
    DRIVERS............: /etc/odbcinst.ini
    SYSTEM DATA SOURCES: /etc/odbc.ini
    FILE DATA SOURCES..: /etc/ODBCDataSources
    USER DATA SOURCES..: /etc/odbc.ini
    SQLULEN Size.......: 8
    SQLLEN Size........: 8
    SQLSETPOSIROW Size.: 8

    //it is bug.    
    */
    //M-9089
    testTenant(&oriInfo);

    LogMsg(NONE, _T("\n\n"));
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

