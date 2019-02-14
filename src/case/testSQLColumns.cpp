#include "../util/util.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../driver/odbcdriver.h"
#include "../case/testcase.h"

#define CONFIG_SQL_COLUMNS_VIEW_TABLE_HEAD                  _T("VIEW")
#define CONFIG_SQL_COLUMNS_MATERIALIZED_VIEW_TABLE_HEAD     _T("MATERIALIZED_VIEW")
#define CONFIG_SQL_COLUMNS_SYONOYM_TABLE_HEAD               _T("SYONOYM")

typedef struct sColumnsTableStat_
{
    unsigned int table:1;
    unsigned int view:1;
    unsigned int materializedView:1;
    unsigned int synonym:1;
}sColumnsTableStat;
static sColumnsTableStat gsColumnsTableStat;

static FILE *fpLog = NULL;

static void LogMsg(unsigned int option, TCHAR *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    
    if(fpLog == NULL){
        fpLog = openLog("SQLColumns");
    }
    if(fpLog != NULL){
        if(option == SHORTTIMESTAMP || option == TIME_STAMP){
            _ftprintf(fpLog, "%s", timeStr);
        }
        va_start(argsList,format);\
        _vftprintf(fpLog, format, argsList);
        va_end(argsList);
        fflush(fpLog);
    } 
#ifdef CONFIG_DEBUG_PRINT    
    if(option == SHORTTIMESTAMP || option == TIME_STAMP){       
        printf("%s", timeStr);
    }
    va_start(argsList,format);
#ifdef unixcli
    vprintf(format, argsList);
#else
    vprintf_s(format, argsList);
#endif
   va_end(argsList);
#endif /*CONFIG_DEBUG_PRINT*/
}


static int initSQLColumns(TestInfo *pTestInfo, TCHAR *pTable)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    TCHAR szBuf[128];
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    LogMsg(NONE, _T("init SQLColumns...\n"));
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
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;

    //init view
    _stprintf(szBuf, _T("drop view %s_%s"), CONFIG_SQL_COLUMNS_VIEW_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    memset(szBuf, 0, sizeof(szBuf));
    _stprintf(szBuf, _T("create view %s_%s as select * from %s"), CONFIG_SQL_COLUMNS_VIEW_TABLE_HEAD, pTable, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        gsColumnsTableStat.view = 0;
    }
    else{
        gsColumnsTableStat.view = 1;
    }
    //init materialized view
    _stprintf(szBuf, _T("drop materialized view %s_%s"), CONFIG_SQL_COLUMNS_MATERIALIZED_VIEW_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    memset(szBuf, 0, sizeof(szBuf));
    _stprintf(szBuf,
                _T("create materialized view %s_%s REFRESH ON REQUEST INITIALIZE ON REFRESH as select * from %s\n"), 
                CONFIG_SQL_COLUMNS_MATERIALIZED_VIEW_TABLE_HEAD, 
                pTable, 
                pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        gsColumnsTableStat.materializedView = 0;
    }
    else{
        gsColumnsTableStat.materializedView = 1;
    }
    //init
    _stprintf(szBuf, _T("drop synonym %s_%s"), CONFIG_SQL_COLUMNS_SYONOYM_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    memset(szBuf, 0, sizeof(szBuf));
    _stprintf(szBuf,
                _T("create synonym %s_%s for %s"), 
                CONFIG_SQL_COLUMNS_SYONOYM_TABLE_HEAD, 
                pTable, 
                pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        gsColumnsTableStat.synonym = 0;
    }
    else{
        gsColumnsTableStat.synonym = 1;
    }
    FullDisconnectEx(fpLog, &sTestInfo);
    return ret;
}
static int deleteSQLColumns(TestInfo *pTestInfo, TCHAR *pTable)
{
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    TCHAR szBuf[128];
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    
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
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;

    //drop view
    _stprintf(szBuf, _T("drop view %s_%s"), CONFIG_SQL_COLUMNS_VIEW_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    //drop materialized view
    _stprintf(szBuf, _T("drop materialized view %s_%s"), CONFIG_SQL_COLUMNS_MATERIALIZED_VIEW_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    //drop syonoym
    _stprintf(szBuf, _T("drop synonym %s_%s"), CONFIG_SQL_COLUMNS_SYONOYM_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);

    FullDisconnectEx(fpLog, &sTestInfo);
    return 0;
}

PassFail testSQLColumns(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    CConfig *pConfig = new CConfig;
    TEST_INIT;
    sODBCTestData *psODBCTestData = NULL;
    sTestTableInfo *psTestTableInfo = NULL;
    TCHAR *pCatalog, *pSchema, *pTable;
    int iCase, iTable;
    TCHAR szTestTable[4][128];
    int totalPass, totalFail;
    sTestConfig *pTestConfig = &gTestGlobal.mcfg;
    int i;
    TCHAR szBuf[64] = {0};
    TCHAR szTmp[64] = {0};

    fpLog = INIT_LOGS("SQLColumns");
    TESTCASE_BEGINW(_T("begin SQLColumns test...\n"));
    if(pConfig == NULL){
        LogMsg(NONE, _T("Failed to create configuration object.\n"));
        closeLog(fpLog);
        TEST_FAILED;
        TEST_RETURN;
    }
    psODBCTestData = pConfig->getTestData();
    psTestTableInfo = &psODBCTestData->mTableInfo;

    totalPass = 0;
    totalFail = 0;
    while(pConfig->scanTestTable() == 0){   
        if(psODBCTestData->isAvailableTable == FALSE){
            continue;
        } 
        LogMsg(NONE, _T("%s\n"), psTestTableInfo->szTableName);
        LogMsg(NONE, _T("------------------------------------------------------------------------------------------------\n"));
        LogMsg(NONE, _T("colnum    sql c type      sql type     column size       decimal digits       column name\n"));
        LogMsg(NONE, _T("------------------------------------------------------------------------------------------------\n"));
        for(i = 0; i < psTestTableInfo->columNum; i++){
           LogMsg(NONE, _T("%d          %s            %s            %d             %d           %s\n"),
                    i,
                    SQLCTypeToChar(psTestTableInfo->sqlCType[i], szBuf),
                    SQLTypeToChar(psTestTableInfo->sqlType[i], szTmp),
                    psTestTableInfo->columnSize[i],
                    psTestTableInfo->decimalDigits[i],
                    psTestTableInfo->szColName[i]
                    );
        }
        LogMsg(NONE, _T("------------------------------------------------------------------------------------------------\n"));
        memset(&gsColumnsTableStat, 0, sizeof(gsColumnsTableStat));
        gsColumnsTableStat.table = 1;
        initSQLColumns(pTestInfo, psTestTableInfo->szTableName);
        //test basic
        _stprintf(szTestTable[0], _T("%s"), psTestTableInfo->szTableName);
        //test view
        _stprintf(szTestTable[1], _T("%s_%s"), CONFIG_SQL_COLUMNS_VIEW_TABLE_HEAD, psTestTableInfo->szTableName);
        //test materialized view
        _stprintf(szTestTable[2], _T("%s_%s"), CONFIG_SQL_COLUMNS_MATERIALIZED_VIEW_TABLE_HEAD, psTestTableInfo->szTableName);
        //test synonym
        _stprintf(szTestTable[3], _T("%s_%s"), CONFIG_SQL_COLUMNS_SYONOYM_TABLE_HEAD, psTestTableInfo->szTableName);
        for(iTable = 0; iTable < 4; iTable++){
            if(iTable == 0 && gsColumnsTableStat.table == 0){
                TEST_FAILED;
                LogMsg(NONE, _T("There are no test tables available.\n"));
                totalFail++;
                continue;
            }
            else if(iTable == 1 && gsColumnsTableStat.view == 0){
                TEST_FAILED;
                LogMsg(NONE, _T("There are no test view available.\n"));
                totalFail++;
                continue;
            }
            else if(iTable == 2 && gsColumnsTableStat.materializedView == 0){
                TEST_FAILED;
                LogMsg(NONE, _T("There are no test materializedView view available.\n"));
                totalFail++;
                continue;
            }
            else if(iTable == 3 && gsColumnsTableStat.synonym == 0){
                TEST_FAILED;
                LogMsg(NONE, _T("There are no test synonym available.\n"));
                totalFail++;
                continue;
            }
            for(iCase = 0; iCase < 4; iCase++){
                if(iCase != 0 && pTestConfig->interval > 0){
#ifdef unixcli
                sleep(pTestConfig->interval);
#else
                Sleep(pTestConfig->interval * 1000);
#endif
                }
                pTable = szTestTable[iTable];
                if(iCase == 0){
                    pCatalog = pTestInfo->Catalog;
                    pSchema = pTestInfo->Schema;
                }
                else if(iCase == 1){
                    pCatalog = NULL;
                    pSchema = pTestInfo->Schema;
                }
                else if(iCase == 2){
                    pCatalog = pTestInfo->Catalog;
                    pSchema = NULL;
                }
                else if(iCase == 3){
                    pCatalog = NULL;
                    pSchema = NULL;
                }
                TESTCASE_BEGINW(_T("\ndo SQLColumns test...\n"));
                if(SQLColumnsEx(pTestInfo, 
                                psTestTableInfo, 
                                pCatalog, 
                                pSchema, 
                                pTable, 
                                _T("%"),
                                fpLog) != 0){
                    totalFail++;
                    TEST_FAILED;
                }
                else{
                    totalPass++;
                }
                TESTCASE_END;
            }
        }
        deleteSQLColumns(pTestInfo, psTestTableInfo->szTableName);
    };
    LogMsg(NONE, _T("\nSummary: total pass cases:%d     total fail cases:%d\n"), totalPass, totalFail);
    TESTCASE_END;
    if(pConfig) delete pConfig;
    RELEASE_LOSG(fpLog);
    
    TEST_RETURN;
}

