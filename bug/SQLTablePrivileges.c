#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define STR_LEN 128
// Declare buffers for result set data
SQLCHAR strCatalog[STR_LEN];
SQLCHAR strSchema[STR_LEN];
SQLCHAR strTableName[STR_LEN];
SQLCHAR strGrantor[STR_LEN];
SQLCHAR strGrantee[STR_LEN];
SQLCHAR strPrivilege[STR_LEN];
SQLCHAR strIsGrantable[STR_LEN];

SQLINTEGER BufferLength;

// Declare buffers for bytes available to return
SQLLEN lenCatalog;
SQLLEN lenSchema;
SQLLEN lenTableName;
SQLLEN lenGrantor;
SQLLEN lenGrantee;
SQLLEN lenPrivilege;
SQLLEN lenIsGrantable;

static int exampleSQLTablePrivileges(TestInfo *pTestInfo)
{
    RETCODE		retcode;
 	SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
    int ret = 0;
    TCHAR szBuf[256] = {0};
    TCHAR		szConnStrIn[256], szConnStrOut[256];
	TCHAR		connstr[256], connstr1[256];
	SWORD		cbConnStrOut;
    int i;

    LogMsg(NONE, _T("\n................................SQLTablePrivileges...............................\n"));
#if 1
    if(!FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrors(henv,hdbc,hstmt);
        return -1;
    }
    henv = pTestInfo->henv;
 	hdbc = pTestInfo->hdbc;
 	hstmt = (SQLHANDLE)pTestInfo->hstmt;
#else   
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE,_T("SQLAllocHandle fail, line %d\n"), __LINE__);
        return -1;
    }     
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        LogMsg(NONE,_T("SQLSetEnvAttr fail, line %d\n"), __LINE__);
        return -1;
    }

    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        LogMsg(NONE,_T("SQLAllocHandle fail, line %d\n"), __LINE__);
        return -1;
    }
    _stprintf(szConnStrIn, _T("DSN=%s;SERVER=TCP:10.10.14.40:23400/23400;UID=%s;PWD=%s;"), (SQLTCHAR*)pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password);
    LogMsg(NONE, _T("Connect string:%s\n"), szConnStrIn);
    retcode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)szConnStrIn, SQL_NTS, (SQLTCHAR*)szConnStrOut, sizeof(szConnStrOut), &cbConnStrOut, SQL_DRIVER_NOPROMPT);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("connect fail: line %d\n"), __LINE__);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }
#endif  
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsg(NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) and return fail,line = %d\n"), __LINE__);
        LogMsg(NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            disconnect(henv, hdbc, hstmt);
            return -1;
        }
    }
    /*
    *MySQL:
    *       create table tb(c int);
    *       grant select,delete,update,create,drop on tb to root@"%" identified by "123456";
    *       flush privileges;
    */
    retcode = SQLTablePrivileges(hstmt, 
                                _T("TRAFODION"), SQL_NTS, 
                                _T("ODBCTEST"), SQL_NTS,
                                (SQLCHAR*)_T("tb"), SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        // Bind columns in result set to buffers
        SQLBindCol(hstmt, 1, SQL_C_CHAR,
                             strCatalog, STR_LEN, &lenCatalog);
        SQLBindCol(hstmt, 2, SQL_C_CHAR,
                             strSchema, STR_LEN, &lenSchema);
        SQLBindCol(hstmt, 3, SQL_C_CHAR,
                             strTableName, STR_LEN,&lenTableName);
        SQLBindCol(hstmt, 4, SQL_C_CHAR,
                             strGrantor, STR_LEN, &lenGrantor);
        SQLBindCol(hstmt, 5, SQL_C_CHAR,
                             strGrantee, STR_LEN, &lenGrantee);
        SQLBindCol(hstmt, 6, SQL_C_CHAR,
                             strPrivilege, STR_LEN, &lenPrivilege);
        SQLBindCol(hstmt, 7, SQL_C_CHAR,
                             strIsGrantable, STR_LEN, &lenIsGrantable);
        i = 0;
        retcode = SQLFetch(hstmt);
        if(retcode == SQL_NO_DATA){
            LogMsg(NONE,_T("SQLFetch fail.\n"));
            LogAllErrors(henv,hdbc,hstmt);
            ret = -1;
        }
        else{
            printf ("Catalog     Schema      Table       Grantor    ");
            printf ("Grantee     Is Grantable\n");
            printf ("----------- ----------- ----------- -----------");
            printf ("----------- ------------\n");
        }
        while (SQL_NO_DATA != retcode) {
            printf("%s  %s  %s  %s  %s  %s  %s\n", strCatalog, strSchema, strTableName, strGrantor, strGrantee, strPrivilege, strIsGrantable);
            retcode = SQLFetch(hstmt);
        }
    } 
    else {
        LogMsg(NONE,_T("SQLTablePrivileges fail, line %d\n"), __LINE__);
        LogAllErrors(henv,hdbc,hstmt);
    }
    
    disconnect(henv, hdbc, hstmt);
    LogMsg(NONE, _T("Complete test....%s\n"), (ret == 0) ? _T("pass") : _T("fail"));
    
    return ret;
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
*        gcc xxx.c util.c -o debug -L /usr/lib64/ -I /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli -g -w
*        ./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST
*       gcc xxx.c util.c -o debug -lodbc -lpthread -lz -Dunixcli -g -w
*       ./debug -d MySQL -u root -p 123456 -s test
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
    exampleSQLTablePrivileges(&oriInfo);
#ifndef unixcli
    LogMsg(NONE, _T("\nPlease enter any key and exit\n"));
    getchar();
#endif
    return 0;
}


