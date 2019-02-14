#include <stdio.h>
#include <string.h>
#include "util.h"

static int M_4710(TestInfo *pTestInfo)
{
	SQLLEN len;
	RETCODE retcode;
	SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
	#define STR_LEN 128 + 1
	#define REM_LEN 254 + 1
	SWORD i, numOfCols = 0;

	SQLCHAR ColumnName[STR_LEN];
	SQLCHAR strCatalog[STR_LEN];
	SQLCHAR strSchema[STR_LEN];
	SQLCHAR strTableName[STR_LEN];
	SQLCHAR strGrantor[STR_LEN];
	SQLCHAR strGrantee[REM_LEN];
	SQLCHAR strPrivilege[STR_LEN];
	SQLCHAR strIsGrantable[STR_LEN];

	SQLSMALLINT    ColumnNameLen;
    SQLSMALLINT    ColumnDataType;
    SQLULEN        ColumnDataSize;
    SQLSMALLINT    ColumnDataDigits;
    SQLSMALLINT    ColumnDataNullable;
	
	SQLLEN lenCatalog;
	SQLLEN lenSchema;
	SQLLEN lenTableName;
	SQLLEN lenGrantor;
	SQLLEN lenGrantee;
	SQLLEN lenPrivilege;
	SQLLEN lenIsGrantable;
	CHAR cn[128] = {0};
	SWORD cl;
	SWORD st;
	SQLULEN cp;
	SWORD cs, cnull;
    int ret = 0;
    TCHAR szBuf[128] = {0};

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
#if 0
    SQLExecDirect(hstmt, (SQLCHAR*)"drop table TB_DEBUG(C1 INTEGER UNSIGNED)", SQL_NTS);
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table TB_DEBUG(C1 INTEGER UNSIGNED)", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE,_T("create table fail, line %d\n"), __LINE__);
        LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
        goto TEST_FAIL;
    }
#endif
#if 0
	retcode = SQLColumnPrivileges(hstmt, NULL, 0, NULL, 0,
                                      (SQLCHAR*)"TB_DEBUG",
                                      SQL_NTS, NULL, 0);
#else
    retcode = SQLColumnPrivileges(hstmt, "%", SQL_NTS, "%", SQL_NTS,
                                      (SQLCHAR*)"%",
                                      SQL_NTS, NULL, 0);
#endif
	if (retcode != SQL_SUCCESS){
		LogMsg(NONE, _T("Call SQLColumnPrivileges fail.\n"));
		LogAllErrors(henv,hdbc,hstmt);
		ret = -1;
        goto TEST_FAIL;
	}
#if 0
    numOfCols = 0;
	retcode = SQLNumResultCols(hstmt, &numOfCols);
	if (retcode != SQL_SUCCESS){
		printf("SQLNumResultCols fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
        goto TEST_FAIL;
	}
	printf("numOfCols = %d\n", numOfCols);
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

        printf ("Catalog     Schema      Table       Grantor    ");
        printf ("Grantee     Is Grantable\n");
        printf ("----------- ----------- ----------- -----------");
        printf ("----------- ------------\n");
        while (SQL_SUCCESS == retcode) {
            retcode = SQLFetch(hstmt);

            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
				printf("SQLFetch fail:\n");
				LogAllErrors(henv,hdbc,hstmt);
                disconnect(henv, hdbc, hstmt);
				return -1;
			}
            if (retcode == SQL_SUCCESS ||
                retcode == SQL_SUCCESS_WITH_INFO) {
                printf ("%s", strCatalog);
                printf ("%s", strSchema);
                printf ("%s", strTableName);
                printf ("%s", strGrantor);
                printf ("%s", strGrantee);
                printf ("%s", strPrivilege);
                printf ("%s\n", strIsGrantable);
            }
        }
    }
	else{
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
        goto TEST_FAIL;
	}
 #else
    numOfCols = 0;
    retcode = SQLNumResultCols(hstmt, &numOfCols);
    if(numOfCols == 0){
        LogMsg(NONE, _T("SQLNumResultCols fail.\n"));
        LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
        goto TEST_FAIL;
    }
    LogMsg(NONE, _T("total cols:%d\n"), numOfCols);
    retcode = SQLFetch(hstmt);
    if(retcode == SQL_NO_DATA){
        LogMsg(NONE, _T("call SQLFetch and return SQL_NO_DATA\n"));
        ret = -1;
    }
    else{
        printf ("Catalog     Schema      Table       Grantor    ");
        printf ("Grantee     Is Grantable\n");
        printf ("----------- ----------- ----------- -----------");
        printf ("----------- ------------\n");
    }
    while (retcode != SQL_NO_DATA){
        for (i = 1; i <= numOfCols; ++i){
            memset(szBuf, 0, sizeof(szBuf));
            retcode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)szBuf, sizeof(szBuf), &len);
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
 #endif
 
	/*retcode = SQLDescribeCol(hstmt,(SWORD)1,(SQLCHAR*)ColumnName,sizeof(ColumnName),&ColumnNameLen,&ColumnDataType,&ColumnDataSize,&ColumnDataDigits,&ColumnDataNullable);
	if (retcode != SQL_SUCCESS){
		printf("SQLDescribeCol fail:\n");
		LogAllErrors(henv,hdbc,hstmt);
        disconnect(henv, hdbc, hstmt);
		return -1;
	}
	printf("Column Name : %s\n  Column Name Len : %d\n  SQL Data Type : %d\n  Data Size : %d\n  DecimalDigits : %d\n  Nullable %d\n",
                 ColumnName, 
                 (int)ColumnNameLen, 
                 (int)ColumnDataType,
                 (int)ColumnDataSize, 
                 (int)ColumnDataDigits,
                 (int)ColumnDataNullable);*/
TEST_FAIL:    
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
    M_4710(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


