#include "../util/util.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../driver/odbcdriver.h"
#include "../case/testcase.h"

#define CONFIG_SQL_TABLES_VIEW_TABLE_HEAD                  _T("VIEW")

static FILE *fpLog = NULL;

static void LogMsg(unsigned int option, TCHAR *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    
    if(fpLog == NULL){
        fpLog = openLog("SQLTables");
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
#ifdef unixcli
        printf("%s", timeStr);
#endif
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

static int initSQLTables(TestInfo *pTestInfo, TCHAR *pTable)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    TCHAR szBuf[128];
  
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    LogMsg(NONE, _T("init SQLTables...\n"));
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
    _stprintf(szBuf, _T("drop view %s_%s"), CONFIG_SQL_TABLES_VIEW_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    memset(szBuf, 0, sizeof(szBuf));
    _stprintf(szBuf, _T("create view %s_%s as select * from %s"), CONFIG_SQL_TABLES_VIEW_TABLE_HEAD, pTable, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, _T("SQLExecDirect  fail: line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
    }

    FullDisconnectEx(fpLog, &sTestInfo);
    return ret;
}
static int deleteSQLTables(TestInfo *pTestInfo, TCHAR *pTable)
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
    _stprintf(szBuf, _T("drop view %s_%s"), CONFIG_SQL_TABLES_VIEW_TABLE_HEAD, pTable);
    LogMsg(NONE, _T("%s\n"), szBuf);
    SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);

    FullDisconnectEx(fpLog, &sTestInfo);
    return 0;
}

PassFail testSQLTables(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    CConfig *pConfig = new CConfig;
    TEST_INIT;
    sODBCTestData *psODBCTestData = NULL;
    sTestTableInfo *psTestTableInfo = NULL;
    TCHAR *pCatalog, *pSchema, *pTable;
    int iCase, iTable;
    int totalPass, totalFail;
    TCHAR szTestTable[2][128];
    BOOL isTestView = FALSE;
    TCHAR *szTableType[] = {_T(""), 
                            _T("TABLE"), 
                            _T("TABLE,VIEW,SYSTEM TABLE"), 
                            _T("TABLE,SYSTEM TABLE,VIEW"), 
                            _T("TABLE,VIEW,SYSTEM TABLE,SYNONYM,MV,MVG"),
                            _T("END")
                            };
    TCHAR **p, *pTableType;
    sTestConfig *pTestConfig = &gTestGlobal.mcfg;
    int i;
    TCHAR szBuf[64] = {0};
    TCHAR szTmp[64] = {0};
    fpLog = INIT_LOGS("SQLTables");
    
    TESTCASE_BEGINW(_T("begin SQLTables test...\n"));
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
        //test basic
        _stprintf(szTestTable[0], _T("%s"), psTestTableInfo->szTableName);
        //test view
        _stprintf(szTestTable[1], _T("%s_%s"), CONFIG_SQL_TABLES_VIEW_TABLE_HEAD, psTestTableInfo->szTableName);
        /*if(initSQLTables(pTestInfo, psTestTableInfo->szTableName) != 0){
            isTestView = FALSE;
        }
        else{
            isTestView = TRUE;
        }*/
        for(iTable = 0; iTable < 2; iTable++){
            if(iTable == 1 && isTestView == FALSE){
                TEST_FAILED;
                LogMsg(NONE, _T("There are no test view available.\n"));
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
                else{
                    pCatalog = NULL;
                    pSchema = NULL;
                }
                TESTCASE_BEGINW(_T("\ndo SQLTables test...\n"));
                p = szTableType;
                while(p){
                    if(_tcscmp(*p, _T("END")) == 0) break;
                    pTableType = *p;
                    p++;
                    if(SQLTablesEx(pTestInfo, 
                                    psTestTableInfo, 
                                    pCatalog, 
                                    pSchema, 
                                    pTable, 
                                    pTableType,
                                    (iTable == 1) ? TRUE : FALSE,
                                    fpLog
                                    ) != 0){
                        totalFail++;
                        TEST_FAILED;
                    }
                    else{
                        totalPass++;
                    }
                    TESTCASE_END;
                }
            }
        }
        //deleteSQLTables(pTestInfo, psTestTableInfo->szTableName);
    };
    LogMsg(NONE, _T("\nSummary: total pass cases:%d     total fail cases:%d\n"), totalPass, totalFail);
    if(pConfig) delete pConfig;
    RELEASE_LOSG(fpLog);
    
    TEST_RETURN;
}
