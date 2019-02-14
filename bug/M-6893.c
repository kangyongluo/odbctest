#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
#define DTSIZE 37

typedef struct {
    TCHAR name [32];     // SQL data type name
    int  value;         // SQL data type numeric value
} dataTypes;

// Array of SQL Data Types
dataTypes dtList[] = {
    {_T("SQL_UNKNOWN_TYPE"),0},
    {_T("SQL_CHAR"),1},
    {_T("SQL_NUMERIC"),2},
    {_T("SQL_DECIMAL"),3},
    {_T("SQL_INTEGER"),4},
    {_T("SQL_SMALLINT"),5},
    {_T("SQL_FLOAT"),6},
    {_T("SQL_REAL"),7},
    {_T("SQL_DOUBLE"),8},
    {_T("SQL_DATETIME"),9},
    {_T("SQL_DATE"),9},
    {_T("SQL_INTERVAL"),10},
    {_T("SQL_TIME"),10},
    {_T("SQL_TIMESTAMP"),11},
    {_T("SQL_VARCHAR"),12},
    {_T("SQL_TYPE_DATE"),91},
    {_T("SQL_TYPE_TIME"),92},
    {_T("SQL_TYPE_TIMESTAMP"),93},
    {_T("SQL_LONGVARCHAR"),-1},
    {_T("SQL_BINARY"),-2},
    {_T("SQL_VARBINARY"),-3},
    {_T("SQL_LONGVARBINARY"),-4},
    {_T("SQL_BIGINT"),-5},
    {_T("SQL_TINYINT"),-6},
    {_T("SQL_BIT"),-7},
    {_T("SQL_WCHAR"),-8},
    {_T("SQL_WVARCHAR"),-9},
    {_T("SQL_WLONGVARCHAR"),-10},
    {_T("SQL_GUID"),-11},
    {_T("SQL_INTERVAL_YEAR"), 101},
    {_T("SQL_INTERVAL_MONTH"), 102},
    {_T("SQL_INTERVAL_DAY"), 103},
    {_T("SQL_INTERVAL_HOUR"), 104},
    {_T("SQL_INTERVAL_MINUTE"), 105},
    {_T("SQL_INTERVAL_SECOND"), 106},
    {_T("SQL_INTERVAL_YEAR_TO_MONTH"), 107},
    {_T("SQL_INTERVAL_DAY_TO_HOUR"), 108},
    {_T("SQL_INTERVAL_DAY_TO_MINUTE"), 109},
    {_T("SQL_INTERVAL_DAY_TO_SECOND"), 110},
    {_T("SQL_INTERVAL_HOUR_TO_MINUTE"), 111},
    {_T("SQL_INTERVAL_HOUR_TO_SECOND"), 112},
    {_T("SQL_INTERVAL_MINUTE_TO_SECOND"), 113},
    {_T("SQL_SS_VARIANT"),-150},
    {_T("SQL_SS_UDT"),-151},
    {_T("SQL_SS_XML"),-152},
    {_T("SQL_SS_TABLE"),-153},
    {_T("SQL_SS_TIME2"),-154},
    {_T("SQL_SS_TIMESTAMPOFFSET"),-155}
};
static TCHAR * SQLType (int dataType) {

    int i;

    for (i=0;i<(sizeof(dtList)/sizeof(dtList[0]));i++) {
        if (dataType==dtList[i].value)
        break;
    }

    if (i<(sizeof(dtList)/sizeof(dtList[0]))) {
        return (dtList[i].name);
    } else {
        return (dtList[0].name);
    }
}

int M_6893(TestInfo *pTestInfo)
{
    RETCODE            retcode;
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
    SQLLEN len;
    int ret = 0;
    TCHAR    buf[2][20] = {0};
    //===========================================================================================================
    if(FullConnectWithOptions(pTestInfo, CONNECT_ODBC_VERSION_3) != TRUE)
    {
        LogMsg(NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;

    retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogAllErrors(henv, hdbc, hstmt);
        LogMsg(NONE, _T("call SQLAllocStmt fail\n"));
        disconnect(henv, hdbc, hstmt);
        return -1;
    }
    //====================================================================================================
    SQLTCHAR typeName[128];
    SQLSMALLINT dataType;
    SQLINTEGER columnSize;
     
    struct
    {
        TCHAR        *TYPE_NAME;
        SQLSMALLINT        DATA_TYPE;
    }TypeInfo[] = 
        {    //TYPENAME,        DATATYPE,        PREC,LITPRE,LITSUF,    PARAM,            NULL,            CASE,SRCH,                ATTR,MON,INC,LOC,        MIN,MAX
           {(TCHAR *)_T("all type"),            SQL_ALL_TYPES},    // this is for get all types
            /*{(TCHAR *)_T("CHAR"),        SQL_CHAR},
            {(TCHAR *)_T("VARCHAR"),        SQL_VARCHAR},
            {(TCHAR *)_T("DECIMAL"),        SQL_DECIMAL},
            {(TCHAR *)_T("NUMERIC"),        SQL_NUMERIC},
            {(TCHAR *)_T("SMALLINT"),    SQL_SMALLINT},
            {(TCHAR *)_T("INTEGER"),        SQL_INTEGER},
            {(TCHAR *)_T("REAL"),        SQL_REAL},
            {(TCHAR *)_T("FLOAT"),        SQL_FLOAT},
            {(TCHAR *)_T("DOUBLE PRECISION"), SQL_DOUBLE},
            {(TCHAR *)_T("DATE"),        SQL_TYPE_DATE},
            {(TCHAR *)_T("TIME"),        SQL_TYPE_TIME},
            {(TCHAR *)_T("TIMESTAMP"),    SQL_TYPE_TIMESTAMP},
            {(TCHAR *)_T("BIGINT"),        SQL_BIGINT},
            {(TCHAR *)_T("LONG VARCHAR"),SQL_LONGVARCHAR},
            {(TCHAR *)_T("BINARY"),        SQL_BINARY},
            {(TCHAR *)_T("VARBINARY"), SQL_VARBINARY},
            {(TCHAR *)_T("LONG VARBINARY"), SQL_LONGVARBINARY},
            {(TCHAR *)_T("TINYINT"), SQL_TINYINT},
            {(TCHAR *)_T("INTERVAL MONTH"), SQL_INTERVAL_MONTH},
            {(TCHAR *)_T("INTERVAL YEAR"), SQL_INTERVAL_YEAR},
            {(TCHAR *)_T("BIT"), SQL_BIT},*/
            {(TCHAR *)_T(""),999,}    
    };
    SQLSMALLINT i, numOfCols, num;

    for(i = 0; TypeInfo[i].DATA_TYPE != 999; i++){
        if(i != 0){
            SQLFreeStmt(hstmt, SQL_CLOSE);
        }
        LogMsg(NONE, _T("\n\n\nStart SQLGetTypeInfo: %s\n"), TypeInfo[i].TYPE_NAME);
        retcode = SQLGetTypeInfo(hstmt, TypeInfo[i].DATA_TYPE);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE, _T("SQLGetTypeInfo fail:\n"));
            LogAllErrors(henv, hdbc, hstmt);
            continue;
        }
#if 1
        retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR,
                             (SQLPOINTER) typeName,
                             (SQLLEN) sizeof(typeName), &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("SQLBindCol 1 fail:\n"));
                LogAllErrors(henv, hdbc, hstmt);
                continue;
            }

        retcode = SQLBindCol(hstmt, 2, SQL_C_SHORT,
                             (SQLPOINTER) &dataType,
                             (SQLLEN) sizeof(dataType), &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("SQLBindCol 2 fail:\n"));
                LogAllErrors(henv, hdbc, hstmt);
                continue;
            }

        retcode = SQLBindCol(hstmt, 3, SQL_C_ULONG,
                             (SQLPOINTER) &columnSize,
                             (SQLLEN) sizeof(columnSize), &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("SQLBindCol 3 fail:\n"));
                LogAllErrors(henv, hdbc, hstmt);
                continue;
            }

        LogMsg(NONE, _T("SQL Data Type             Type Name                 Value                 Max Size\n"));
        LogMsg(NONE, _T("------------------------- ------------------------- -------               --------\n"));

        // Fetch each row, and display
        while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS) {
            LogMsg(NONE, _T("%s\t\t%s\t\t%i\t\t%i\n"), SQLType (dataType), (TCHAR *) typeName, (int) dataType, (int) columnSize);
            if((dataType != TypeInfo[i].DATA_TYPE) && (TypeInfo[i].DATA_TYPE != SQL_ALL_TYPES)){
                LogMsg(NONE, _T("fail !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
                ret = -1;
            }
            if(columnSize != 10){
                ret = -1;
                LogMsg(NONE, "column size: expect:%d actual:%d\n", 10, columnSize);
            }
        }
 #else
        numOfCols = 0;
        retcode = SQLNumResultCols(hstmt, &numOfCols);
        if (retcode != SQL_SUCCESS){
            LogMsg(NONE, _T("SQLNumResultCols fail:\n"));
            LogAllErrors(henv, hdbc, hstmt);
            continue;
        }
        LogMsg(NONE, _T("numOfCols = %d\n"), numOfCols);
        retcode = SQLFetch(hstmt);
        while (retcode != SQL_NO_DATA){
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                LogMsg(NONE, _T("SQLFetch fail:\n"));
                LogAllErrors(henv, hdbc, hstmt);
                continue;
            }
            for(num = 1; num <= numOfCols; num++){
                retcode = SQLGetData(hstmt,(SWORD)num,SQL_C_TCHAR,buf,sizeof(buf),&len);
                if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA_FOUND && retcode != SQL_SUCCESS_WITH_INFO) {
                    LogAllErrors(henv, hdbc, hstmt);
                    break;
                } else if (retcode == SQL_NO_DATA_FOUND) {
                    break;
                }
                if(num < 5){
                    _tprintf(_T("%s\t"), buf);
                }
            }
            _tprintf(_T("\n"));
            retcode = SQLFetch(hstmt);
        }
 #endif
    }
    //====================================================================================================
    disconnect(henv, hdbc, hstmt);
    LogMsg(NONE, _T("Complete test....%s\n"), (ret == 0) ? _T("pass") : _T("fail"));
    
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
//g++ bugSQLGetTypeInfo.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug 
//g++ xxx.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug
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

    M_6893(&oriInfo);

#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

