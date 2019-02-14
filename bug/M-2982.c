#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int M2982(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
#define STR_LEN 128 + 1
#define REM_LEN 254 + 1
    SQLCHAR szSchema[STR_LEN];
    SQLCHAR szCatalog[STR_LEN];
    SQLCHAR szColumnName[STR_LEN];
    SQLCHAR szTableName[STR_LEN];
    SQLCHAR szTypeName[STR_LEN];
    SQLSMALLINT DataType;
    SQLINTEGER cbCatalog;
    SQLINTEGER cbSchema;
    SQLINTEGER cbTableName;
    SQLINTEGER cbColumnName;
    SQLINTEGER cbDataType;
    SQLINTEGER cbTypeName;
    int i, ret;
    char createStr[200] = {0};
#define STR_LEN 128 + 1
#define REM_LEN 254 + 1

    SQLCHAR strSchema[STR_LEN];
    SQLCHAR strCatalog[STR_LEN];
    //SQLCHAR strColumnName[STR_LEN];
    SQLCHAR strTableName[STR_LEN];
    SQLCHAR strTypeName[STR_LEN];
    SQLCHAR strRemarks[REM_LEN];
    SQLCHAR strColumnDefault[STR_LEN];
    SQLCHAR strIsNullable[STR_LEN];
    SQLINTEGER ColumnSize;
    SQLINTEGER BufferLength;
    SQLINTEGER CharOctetLength;
    SQLINTEGER OrdinalPosition;

    SQLSMALLINT DecimalDigits;
    SQLSMALLINT NumPrecRadix;
    SQLSMALLINT Nullable;
    SQLSMALLINT SQLDataType;
    SQLSMALLINT DatetimeSubtypeCode;

    SQLLEN lenCatalog;
    SQLLEN lenSchema;
    //SQLLEN lenTableName;
    SQLLEN lenColumnName;
    SQLLEN lenDataType;
    SQLLEN lenTypeName;
    SQLLEN lenColumnSize;
    SQLLEN lenBufferLength;
    SQLLEN lenDecimalDigits;
    SQLLEN lenNumPrecRadix;
    SQLLEN lenNullable;
    SQLLEN lenRemarks;
    SQLLEN lenColumnDefault;
    SQLLEN lenSQLDataType;
    SQLLEN lenDatetimeSubtypeCode;
    SQLLEN lenCharOctetLength;
    SQLLEN lenOrdinalPosition;
    SQLLEN lenIsNullable;

    ret = 0;
    LogMsg(NONE, _T("\nstart  test %s.......M-2982.......\n"), __FUNCTION__);
    LogMsg(NONE, _T("Description:\n\t SQLColumns didn't return expected result when column defination is date and time\n"));

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

    TCHAR *szCreate = (TCHAR *)_T("create table tb (\n")
                        _T("C1 char(10) CHARACTER SET ISO88591,\n")
                        _T("C2 varchar(10) CHARACTER SET ISO88591,\n")
                        _T("C3 decimal(10,5),\n")
                        _T("C4 decimal(5,2) unsigned,\n")
                        _T("C5 numeric(10,5),\n")
                        _T("C6 numeric(5,2) unsigned,\n")
                        _T("C7 smallint,\n")
                        _T("C8 smallint unsigned,\n")
                        _T("C9 integer,\n")
                        _T("C10 integer unsigned,\n")
                        _T("C11 bigint,\n")
                        _T("C12 real,\n")
                        _T("C13 float,\n")
                        _T("C14 double precision,\n")
                        _T("C15 date,\n")
                        _T("C16 time,\n")
                        _T("C17 timestamp) no partition\n");

#if 0
    
    SQLExecDirect(hstmt, (SQLTCHAR*)_T("drop table tb"), SQL_NTS);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szCreate, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("create table  fail:\n"));
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_FAIL;
    }
#endif
    LogMsg(NONE, _T("%s\n"), szCreate);
    retcode = SQLColumns(hstmt, 
                         (SQLTCHAR*)_T("TRAFODION"),
                         (SWORD)_tcslen(_T("TRAFODION")),
                         (SQLTCHAR*)pTestInfo->Schema,
                         (SWORD)_tcslen(pTestInfo->Schema),
                         (SQLTCHAR*)_T("tb"),
                         (SWORD)_tcslen("tb"),
                         (SQLTCHAR*)"%",
                         (SWORD)_tcslen("%"));
    //retcode = SQLColumns(hstmt, (SQLTCHAR*)"TRAFODION", SQL_NTS, (SQLTCHAR*)"ODBCTEST", SQL_NTS, (SQLTCHAR*)"tb", SQL_NTS, (SQLTCHAR*)"%", SQL_NTS);
    if (retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("SQLColumns  fail:\n"));
        LogAllErrors(henv, hdbc, hstmt);
        goto TEST_FAIL;
    }     
    SQLBindCol(hstmt, 1, SQL_C_CHAR, szCatalog, STR_LEN,&len);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, szSchema, STR_LEN, &len);
    SQLBindCol(hstmt, 3, SQL_C_CHAR, szTableName, STR_LEN,&len);
    SQLBindCol(hstmt, 4, SQL_C_CHAR, szColumnName, STR_LEN, &len);
    SQLBindCol(hstmt, 5, SQL_C_SSHORT, &DataType, 0, &len);
    SQLBindCol(hstmt, 6, SQL_C_CHAR, szTypeName, STR_LEN, &len);
    SQLBindCol(hstmt, 7,  SQL_C_SLONG, &ColumnSize, 0, &lenColumnSize);
    SQLBindCol(hstmt, 9,  SQL_C_SSHORT, &DecimalDigits, 0, &lenDecimalDigits);

    i = 1;
    ret = 0;
    LogMsg(NONE, _T("catalog        schema          table name          column name     type        type string         ColumnSize          DecimalDigits\n"));
    while (SQL_SUCCESS == retcode) {
        retcode = SQLFetch(hstmt);
        if(retcode != SQL_SUCCESS){
            if(i == 1){
                ret = 1;
                LogMsg(NONE, _T("SQLFetch fail\n"));
            }
            break;
        }
        LogMsg(NONE, _T("%s      %s      %s      %s      %d      %s         %d          %d\n"), 
                        szCatalog, szSchema, szTableName, szColumnName, DataType, szTypeName, ColumnSize, DecimalDigits);
        if((i == 15) && (ColumnSize != 10)){
            ret = 1;
            LogMsg(NONE, _T("\texcept:%d  actual:%d\n"), 10, ColumnSize);
        }
        else if((i == 16) && (ColumnSize != 8)){
            ret = 1;
            LogMsg(NONE, _T("\texcept:%d  actual:%d\n"), 8, ColumnSize);
        }
        i++;
    }
    if(ret){
        LogMsg(NONE, _T("test fail\n"), 10, ColumnSize);
    }
TEST_FAIL:
    disconnect(henv, hdbc, hstmt);
    
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
    M2982(&oriInfo);
    LogMsg(NONE, _T("\n\n"));
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


