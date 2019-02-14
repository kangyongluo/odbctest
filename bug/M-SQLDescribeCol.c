#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int debugSQLDescribeCol(TestInfo *pTestInfo)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT num, i;
    char buf[512] = {0};
    SQLLEN len;
    SQLCHAR         colname[32];        // column name
    SQLSMALLINT     coltype;            // column type
    SQLSMALLINT     colnamelen;         // length of column name
    SQLSMALLINT     nullable;           // whether column can have NULL value
    SQLLEN          collen[32];    // array of column lengths
    SQLSMALLINT     decimaldigits;      // no of digits if column is numeric
    SQLLEN          outlen[32];    // lengths of column values returned
#if 0
    TCHAR szDDL[] = _T("create table tb(\n")
                    _T("C01 CHAR(254) CHARACTER SET ISO88591 NOT NULL\n")
                    _T(",C02 VARCHAR(254) CHARACTER SET ISO88591 NOT NULL\n")
                    _T(",C03 DECIMAL(18,6) NOT NULL\n")
                    _T(",C04 NUMERIC(18,6) NOT NULL\n")
                    _T(",C05 SMALLINT NOT NULL\n")
                    _T(",C06 INTEGER NOT NULL\n")
                    _T(",C07 REAL NOT NULL\n")
                    _T(",C08 FLOAT NOT NULL\n")
                    _T(",C09 DOUBLE PRECISION NOT NULL\n")
                    _T(",C10 DATE NOT NULL")
                    _T(",C11 TIME NOT NULL\n")
                    _T(",C12 TIMESTAMP NOT NULL\n")
                    _T(",C13 LONG VARCHAR(2000) CHARACTER SET ISO88591 NOT NULL\n")
                    _T(",C14 LARGEINT NOT NULL\n")
                    _T(",C15 CHAR(254) CHARACTER SET UCS2 NOT NULL\n")
                    _T(",C16 VARCHAR(127) CHARACTER SET UCS2 NOT NULL\n")
                    _T(",C17 LONG VARCHAR(2000) CHARACTER SET UCS2 NOT NULL\n")
                    _T(",C18 NUMERIC(19,0) NOT NULL\n")
                    _T(",C19 NUMERIC(19,6) NOT NULL\n")
                    _T(",C20 NUMERIC(128,0) NOT NULL\n")
                    _T(",C21 NUMERIC(128,128) NOT NULL\n")
                    _T(",C22 NUMERIC(10,5) UNSIGNED NOT NULL\n")
                    _T(",C23 NUMERIC(18,5) UNSIGNED NOT NULL\n")
                    _T(",C24 NUMERIC(30,10) UNSIGNED NOT NULL\n")
                    _T(") NO PARTITION");
#else
                  TCHAR szDDL[] = "create table tb("
                                "GK5QSX1GWP CHAR(254) CHARACTER SET ISO88591 NOT NULL"
                                ",IOBAI9_S8N VARCHAR(254) CHARACTER SET ISO88591 NOT NULL"
                                ",LPIOBAI9_S DECIMAL(18,6) NOT NULL"
                                ",NQ3KXGK5QS NUMERIC(18,6) NOT NULL"
                                ",X1GWP4VDZA SMALLINT NOT NULL"
                                ",WNVT2DEURL INTEGER NOT NULL"
                                ",UJZ6R0EHYY REAL NOT NULL"
                                ",FCTCBMMOHJ FLOAT NOT NULL"
                                ",FLPIOBAI9_ DOUBLE PRECISION NOT NULL"
                                ",VDZAWNVT2D DATE NOT NULL"
                                ",EURLUJZ6R0 TIME NOT NULL"
                                ",EHYYFCTCBM TIMESTAMP NOT NULL"
                                ",MOHJ7FLPIO LONG VARCHAR(2000) CHARACTER SET ISO88591 NOT NULL"
                                ",BAI9_S8NQ3 LARGEINT NOT NULL"
                                ",KXGK5QSX1G CHAR(254) CHARACTER SET UCS2 NOT NULL"
                                ",WP4VDZAWNV VARCHAR(127) CHARACTER SET UCS2 NOT NULL"
                                ",T2DEURLUJZ LONG VARCHAR(2000) CHARACTER SET UCS2 NOT NULL"
                                ",R0EHYYFCTC NUMERIC(19,0) NOT NULL"
                                ",BMMOHJ7FLP NUMERIC(19,6) NOT NULL"
                                ",Q3KXGK5QSX NUMERIC(128,0) NOT NULL"
                                ",GWP4VDZAWN NUMERIC(128,128) NOT NULL"
                                ",VT2DEURLUJ NUMERIC(10,5) UNSIGNED NOT NULL"
                                ",Z6R0EHYYFC NUMERIC(18,5) UNSIGNED NOT NULL"
                                ",TCBMMOHJ7F NUMERIC(30,10) UNSIGNED NOT NULL) NO PARTITION"; 
#endif
    TCHAR szInsert[] = _T("insert into tb values(")
                       _T("'xgK~5qSX1G'")
                       _T(",'w!`<}[P4V{'")
                       _T(",1234.56789")
                       _T(",5678.12345")
                       _T(",1234,12345")
                       _T(",12340")
                       _T(",12300")
                       _T(",12345670")
                       _T(",{d '1993-12-30'}")
                        _T(",{t '11:45:23'}")
                        _T(",{ts '1992-12-31 23:45:23.123456'}")
                        _T(",'D\"ZaW.nvT&'")
                        _T(",123456")
                        _T(",':|jz 6R0Eh'")
                        _T(",'>YyFCtcb.m'")
                        _T(",'M=701HJ$7(+'")
                        _T(",1234567890")
                        _T(",1234567890123")
                        _T(".456789")
                        _T(",12345678901234567890")
                        _T(",0.123456789012345678901234567890123456789")
                        _T(",12345.56789")
                        _T(",1234567890123.56789")
                        _T(",12345678901234567890.0123456789")
                        _T(")");
    
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
#if 1
    LogMsg(NONE, _T("\n%s\n"), szDDL);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)_T("drop table tb"), SQL_NTS);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szDDL, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("create table fail !\n"));
        ret = -1;
        goto TEST_FAIL;
    }
    retcode = SQLExecDirect(hstmt, (SQLTCHAR *)szInsert, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("inert fail !\n"));
        ret = -1;
        goto TEST_FAIL;
    }
#endif
    //retcode = SQLExecDirect(hstmt, (SQLTCHAR *)_T("select * from tb"), SQL_NTS);
    retcode = SQLPrepare(hstmt,(SQLTCHAR*)"select * from tb",SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogAllErrors(henv,hdbc,hstmt);
        LogMsg(NONE, _T("select fail !\n"));
        ret = -1;
        goto TEST_FAIL;
    }

    num = 0;
	retcode = SQLNumResultCols(hstmt, &num);
    if(num == 0){
        LogMsg(NONE, _T("SQLNumResultCols fail, line %d\n"), __LINE__);
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
    retcode = SQLFetch(hstmt);
    if(retcode == SQL_NO_DATA){
        LogMsg(NONE, _T("SQLFetch and return SQL_NO_DATA\n"));
        ret = -1;
        LogAllErrors(henv,hdbc,hstmt);
        goto TEST_FAIL;
    }
    while (retcode != SQL_NO_DATA){
        for (i = 1; i <= num; ++i){
            retcode = SQLDescribeCol (hstmt,
                                  (SQLUSMALLINT)i,
                                  colname,
                                  sizeof (colname),
                                  &colnamelen,
                                  &coltype,
                                  (SQLULEN *)&collen[i - 1],
                                  &decimaldigits,
                                  &nullable);
            LogMsg(NONE, _T("columns:%d sqltype:%d colName:%s colLen:%d precision:%d nullable:%d\n"), 
                            i, 
                            coltype, 
                            colname, 
                            colnamelen,
                            decimaldigits,
                            nullable);
        }
        retcode = SQLFetch(hstmt);
        break;
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
    debugSQLDescribeCol(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}

