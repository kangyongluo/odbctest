#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int M_7691(TestInfo *pTestInfo)
{
    RETCODE     retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE       hdbc = SQL_NULL_HANDLE;
    SQLHANDLE       hstmt = SQL_NULL_HANDLE;
    TCHAR   szInfo[256] = {0};
    TCHAR   szConnect[256] = {0};
    SQLLEN len;

    // Allocate Environment Handle
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode != SQL_SUCCESS){
        printf("SQLAllocHandle fail. line %d\n", __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        return -1;
    }

    // Set ODBC version to 3.0
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (retcode != SQL_SUCCESS){
        printf("SQLSetEnvAttr fail. line %d\n", __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }
    // Allocate Connection handles
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (retcode != SQL_SUCCESS){
        printf("SQLAllocHandle fail. line %d\n", __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }

    //Connect to the database
    _stprintf(szConnect, _T("DSN=%s;UID=%s;PWD=%s;%c"), pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password, '\0');
    printf("Using Connect String: %s\n", szConnect);
    retcode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR *)szConnect, sizeof(szConnect), (SQLTCHAR *)szInfo, sizeof(szInfo), (SQLSMALLINT *)&len, SQL_DRIVER_NOPROMPT);
    if (!SQL_SUCCEEDED(retcode)){
        printf("SQLDriverConnect fail. line %d\n", __LINE__);
        LogAllErrors(henv, hdbc, hstmt);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }
    else{
        printf("Connect server sucess:\n\t%s\n", szInfo);
    }

    disconnect(henv, hdbc, hstmt);
    
    return 0;
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
//g++  xxx.c util.c -o debug -I /usr/lib64 -L /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli
//g++ xxx.c util.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug
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
    M_7691(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


