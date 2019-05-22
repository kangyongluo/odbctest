#include "testODBC.h"
#include "../util/util.h"
#include "../driver/odbcdriver.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../case/testCase.h"
#include "testSQL.h"

static testCase gtestCase[] = {
        {_T("SQLColumns"), testSQLColumns},
        {_T("SQLTables"), testSQLTables},
        {_T("testRowset"), testRowset},
        {_T("testSelect"), NULL},
        {_T("testStmt"), NULL},
        {_T("testLoadDirect"), testLoadDirect},
        {_T("testLoadAtExec"), testLoadAtExec},
        {_T("testLobUpdate"), testLobUpdate},
        {_T("testSqlFile"), NULL},
        {_T("cleanupTable"), NULL}
        };

static BOOL isDdlTest(char *pCase)
{
    if(_tcscmp(_T("testStmt"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("testSqlFile"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("testSelect"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("cleanupTable"), pCase) == 0){
        return TRUE;
    }
    else{
        return FALSE;
    }
}
#ifdef unixcli
void *odbcTestThread(void *args)
#else
DWORD WINAPI odbcTestThread(LPVOID args)
#endif
{
    CTesSQL *pCTesSQL = NULL;
    testThreadPara mtestThreadPara;
    int iTest;
    int testCount = 0;

    if(args == NULL){
        if(pCTesSQL) delete pCTesSQL;
#ifdef unixcli
        pthread_exit(NULL);
#else
        return 0;
#endif 
    }
    memcpy(&mtestThreadPara, args, sizeof(testThreadPara));
    for(iTest = 0; iTest < (sizeof(gtestCase) / sizeof(gtestCase[0])); iTest++){
        if(_tcscmp(gtestCase[iTest].szCase, mtestThreadPara.szTestCase) == 0){
            if(isDdlTest(mtestThreadPara.szTestCase)){
                pCTesSQL = new CTesSQL(mtestThreadPara.szTestCase);
                if(pCTesSQL){ 
                    for(testCount = 0; testCount < mtestThreadPara.testTimes; testCount++){
#ifdef unixcli
                        if((testCount != 0) && (mtestThreadPara.interval > 0)) 
                            sleep(mtestThreadPara.interval);
#else
                        if((testCount != 0) && (mtestThreadPara.interval > 0))
                            Sleep(mtestThreadPara.interval * 1000);
#endif          
                        pCTesSQL->doSqlTest(&mtestThreadPara.mTestInfo, mtestThreadPara.szTestCase);
                    }
                    delete pCTesSQL;
                    pCTesSQL = NULL;
                }
            }
            else if(gtestCase[iTest].run){
                for(testCount = 0; testCount < mtestThreadPara.testTimes; testCount++){
#ifdef unixcli
                    if((testCount != 0) && (mtestThreadPara.interval > 0)) 
                        sleep(mtestThreadPara.interval);
#else
                    if((testCount != 0) && (mtestThreadPara.interval > 0))
                        Sleep(mtestThreadPara.interval * 1000);
#endif
                    gtestCase[iTest].run(&mtestThreadPara.mTestInfo);
                }
                break;
            }
        } 
    }
#ifdef unixcli
    pthread_exit(NULL);
#else
    return 0;
#endif    
}


CTestODBC::CTestODBC(void)
{

}
CTestODBC::~CTestODBC()
{

}
void CTestODBC::runTest(void)
{
    TestInfo mTestInfo;
    sTestConfig *pConfig = &gTestGlobal.mcfg;
    int threadId = 0;
    TCHAR szCaseBuf[1204] = {0};
    char *pCasePtr[CONFIG_MAX_TEST_CASE];
    int iTest;
    int caseOffset = 0;

    for(iTest = 0; iTest < CONFIG_MAX_TEST_CASE; iTest++){
        pCasePtr[iTest] = NULL;
    }
    memset(&mTestInfo, 0, sizeof(TestInfo));
    postLogMsg(NONE, _T("---------------------------------------------------\n"));

    _stprintf(mTestInfo.Catalog, _T("%s"), pConfig->szCatalog);
    _stprintf(mTestInfo.DataSource, _T("%s"), pConfig->szDataSource);
    _stprintf(mTestInfo.UserID, _T("%s"), pConfig->szUser);
    _stprintf(mTestInfo.Password, _T("%s"), pConfig->szPassword);
    _stprintf(mTestInfo.Schema, _T("%s"), pConfig->szSchema);
    memcpy(&mtestThreadPara.mTestInfo, &mTestInfo, sizeof(TestInfo));
    mtestThreadPara.interval = pConfig->interval;
    mtestThreadPara.testTimes = (pConfig->testTimes == 0) ? 1 : pConfig->testTimes;

    postLogMsg(NONE, _T("test info:\n"));
    postLogMsg(NONE, _T("Catalog:%s\n"), mTestInfo.Catalog);
    postLogMsg(NONE, _T("DSN:%s\n"), mTestInfo.DataSource);
    postLogMsg(NONE, _T("user:%s\n"), mTestInfo.UserID);
    postLogMsg(NONE, _T("password:%s\n"), mTestInfo.Password);
    postLogMsg(NONE, _T("schema:%s\n"), mTestInfo.Schema);
    postLogMsg(NONE, _T("init table:%s\n"), (pConfig->isInitTable == TRUE) ? _T("TRUE") : _T("FALSE"));
    postLogMsg(NONE, _T("interval:%d\n"), pConfig->interval);
    postLogMsg(NONE, _T("is multi task:%s\n"), (pConfig->isMultiTask == TRUE) ? _T("TRUE") : _T("FALSE"));
#ifdef unixcli
    void *join;
    int stacksize = 1048576;//1M 
    pthread_attr_t attr;

    pthread_attr_init(&attr);
#endif
    int ret = getConfigString("config", 
                    "test_case", 
                    "", 
                    szCaseBuf, 
                    sizeof(szCaseBuf), 
                    ODBC_TEST_CFG_FILE);
    postLogMsg(NONE, _T("---------------------------------------------------\n"));
    for(iTest = 0, threadId = 0; iTest < CONFIG_MAX_TEST_CASE; iTest++){
        pCasePtr[iTest] = &szCaseBuf[threadId];
        if(pCasePtr[iTest][0] == 0x00){
            pCasePtr[iTest] = NULL;
            break;
        }
        else if(pCasePtr[iTest][0] == ','){
            pCasePtr[iTest] = &szCaseBuf[threadId + 1];
            szCaseBuf[threadId] = 0x00;
            threadId++;
        }
        while(szCaseBuf[threadId]){
            if(szCaseBuf[threadId] == ','){
                break;
            }
            threadId++;
        }
    }

    threadId = 0;
    caseOffset = 0;
    while(pCasePtr[caseOffset]) {
        ret = -1;
        for(iTest = 0; iTest < (sizeof(gtestCase) / sizeof(gtestCase[0])); iTest++){
            if(_tcscmp(gtestCase[iTest].szCase, pCasePtr[caseOffset]) == 0){
                ret = 0;
                break;
            }
        }
        _stprintf(mtestThreadPara.szTestCase, _T("%s"), pCasePtr[caseOffset]);
        postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  mtestThreadPara.szTestCase);
#ifndef unixcli
        printf("create test:%s\n", mtestThreadPara.szTestCase);
#endif
        if(threadId < CONFIG_TOTAL_THREAD_NUM){
#ifdef unixcli
            if(pthread_create(&mPth[threadId], &attr, odbcTestThread, &mtestThreadPara) != 0){
                mThStat[threadId] = FALSE;
            }
            else{
                mThStat[threadId] = TRUE;
                sleep(1);
            }
#else
            mPth[threadId] = CreateThread(NULL, STACK_SIZE, odbcTestThread, &mtestThreadPara, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
            if(mPth[threadId] == NULL){
                mThStat[threadId] = FALSE;
            }
            else{
                mThStat[threadId] = TRUE;
                Sleep(1000);
            }
#endif
        }
        if(pConfig->isMultiTask == FALSE){
#ifdef unixcli
            if(mThStat[threadId] == TRUE){
                pthread_join(mPth[threadId], &join);
            }    
#else
            if(mThStat[threadId] == TRUE){
                WaitForSingleObject(mPth[threadId], INFINITE);
            }
#endif
            mThStat[threadId] = FALSE;
        }
        else{
            threadId++;
        }
        caseOffset++;
    }
    if(pConfig->isMultiTask){
        for(threadId = 0; threadId < CONFIG_TOTAL_THREAD_NUM; threadId++){
#ifdef unixcli
            if(mThStat[threadId] == TRUE){
                pthread_join(mPth[threadId], &join);
            }    
#else
            if(mThStat[threadId] == TRUE){
                WaitForSingleObject(mPth[threadId], INFINITE);
            }
#endif    
        }
    }
#ifdef unixcli
    pthread_attr_destroy(&attr); 
#endif 

    printf("[file %s line %d] end test\n", __FILE__, __LINE__);
}
void CTestODBC::runCases(void)
{
    char szCase[32] = {0};
    int idCase = 0;
    char szSection[64] = {0};
    TCHAR szBuf[256] = {0};
    TCHAR szTmp[64] = {0};
    CConfig mConfig;
    sLoadParamInfo *psLoadParamInfo;
    sTestConfig *pConfig = &gTestGlobal.mcfg;
    FILE *fpCaseLog = NULL;
    sLoadDataInfo mLoadDataInfo;
    int count;
    int ret;
    CTesSQL *pCTestSql = NULL;
    sSqlStmt *psSqlStmt;
    struct timeval tvStart, tvEnd;
#define CONFIG_CASE_WHERE_NUM_MAX  100000000
    int caseMin = 0;
    int caseMax = CONFIG_CASE_WHERE_NUM_MAX;
    char *p;

    idCase = 0;
    psLoadParamInfo = new sLoadParamInfo;
    if(psLoadParamInfo == NULL){
        return;
    }
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString("config",
                        "case_where",
                        "",
                        szBuf,
                        sizeof(szBuf) - 1,
                        ODBC_TEST_CFG_FILE
                        );
    p = strstr(szBuf, ">=");
    if(p){
        p += 2;
        caseMin = atoi(p);
    }
    else{
       p = strstr(szBuf, ">");
        if(p){
            p += 1;
            caseMin = atoi(p) + 1;
        } 
    }
    p = strstr(szBuf, "<=");
    if(p){
        p += 2;
        caseMax = atoi(p) + 1;
    }
    else{
       p = strstr(szBuf, "<");
        if(p){
            p += 1;
            caseMax = atoi(p);
        } 
    }
    do{
        if(idCase == 0){
            sprintf(szSection, "%s", "case");
        }
        else{
            sprintf(szSection, "case_%d", idCase);
        }
        if(caseMin >= 1){
            if(caseMin > idCase){
               idCase++;
               printf("Ignored case:%s\n", szSection);
               continue;
            }
        }
        if(caseMax < CONFIG_CASE_WHERE_NUM_MAX){
            if(caseMax <= idCase){
               idCase = -100;
               printf("Ignored case:%s\n", szSection);
               break;
            }
        }
        szCase[0] = 0x00;
        if(getConfigString(szSection,
                        "case_type",
                        "",
                        szCase,
                        sizeof(szCase) - 1,
                        ODBC_TEST_CFG_FILE
                        ) <= 0){
            idCase = -100;
        }
        
        memset(psLoadParamInfo, 0, sizeof(sLoadParamInfo));
        //init odbc source
        _stprintf(psLoadParamInfo->mTestInfo.Catalog, _T("%s"), pConfig->szCatalog);
        _stprintf(psLoadParamInfo->mTestInfo.DataSource, _T("%s"), pConfig->szDataSource);
        _stprintf(psLoadParamInfo->mTestInfo.UserID, _T("%s"), pConfig->szUser);
        _stprintf(psLoadParamInfo->mTestInfo.Password, _T("%s"), pConfig->szPassword);
        _stprintf(psLoadParamInfo->mTestInfo.Schema, _T("%s"), pConfig->szSchema);
        //
        mConfig.readTable(szSection, &psLoadParamInfo->mTable);
        if(strlen(szCase) > 0) printf("create case:%s\n", szCase);
        if((strcmp("testLoadDirect", szCase) == 0) ||
            (strcmp("testLoadAtExec", szCase) == 0) ||
            (strcmp("testLobUpdate", szCase) == 0) ||
            (strcmp("testRowset", szCase) == 0)){
            memset(&mLoadDataInfo, 0, sizeof(sLoadDataInfo));
            if(strcmp("testLoadDirect", szCase) == 0){
                postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("loadDirect"));
                fpCaseLog = INIT_LOGS("loadDirect");
                psLoadParamInfo->loadType =  BINDPARAM_EXECUTE_DIRECT;
                psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT;
                mConfig.readLoadDirectInfo(szSection, &mLoadDataInfo);
            }
            else if(strcmp("testLoadAtExec", szCase) == 0){
                postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("testLoadAtExec"));
                fpCaseLog = INIT_LOGS("loadDirect");
                psLoadParamInfo->loadType =  BINDPARAM_SQL_PUT_DATA;
                psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT;
                mConfig.readAtExecInfo(szSection, &mLoadDataInfo);
                psLoadParamInfo->putType = (mLoadDataInfo.putDataType == SQL_DATA_AT_EXEC) ? PUT_DATA_AT_EXEC : PUT_LEN_DATA_AT_EXEC;
            }
            else if(strcmp("testLobUpdate", szCase) == 0){
                postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("testLobUpdate"));
                fpCaseLog = INIT_LOGS("loadDirect");
                psLoadParamInfo->loadType =  BINDPARAM_EXECUTE_DIRECT;
                psLoadParamInfo->putType = PUT_DATA_LOB_UPDATE;
                psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT_OUTPUT;
                mConfig.readLobUpdateInfo(szSection, &mLoadDataInfo);
            }
            else if(strcmp("testRowset", szCase) == 0){
                postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("testRowset"));
                fpCaseLog = INIT_LOGS("loadDirect");
                psLoadParamInfo->loadType =  BINDPARAM_ROWSET;
                psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT;
                mConfig.readRowsetInfo(szSection, &mLoadDataInfo);
            }
            else{
                psLoadParamInfo->loadType =  BINDPARAM_UNKNOW;
                fpCaseLog = NULL;
            }
            if(psLoadParamInfo->mTable.isLobTable == TRUE){
                _stprintf(psLoadParamInfo->mTable.szLobFile, _T("%s"), mLoadDataInfo.szLobFile);
                if(_tcslen(psLoadParamInfo->mTable.szLobFile) > 0)
                    psLoadParamInfo->mTable.isLobFile = TRUE;
            }
            else{
                psLoadParamInfo->mTable.szLobFile[0] = 0x00;
            }
            for(count = 0; count < CONFIG_NUM_CQD_SIZE; count++){
                if(_tcslen(mLoadDataInfo.szLoadCqd[count]) > 0){
                    _stprintf(psLoadParamInfo->mTable.szCqd[count], _T("%s"), mLoadDataInfo.szLoadCqd[count]);
                }
                else{
                    psLoadParamInfo->mTable.szCqd[count][0] = 0x00;
                    break;
                }
            }
            psLoadParamInfo->lenPutBatchSize = mLoadDataInfo.putDataBatch;
            psLoadParamInfo->beginVal = mLoadDataInfo.beginVal;
            psLoadParamInfo->batchSize = mLoadDataInfo.batchSize;
            psLoadParamInfo->totalBatch = mLoadDataInfo.totalBatch;
            _stprintf(psLoadParamInfo->szInsertType, _T("%s"), mLoadDataInfo.szInsertType);
            _stprintf(psLoadParamInfo->szConnStr, _T("%s"), pConfig->szConnStr);
            psLoadParamInfo->fpLog = fpCaseLog;
            LogMsgEx(fpCaseLog, NONE, _T("%s\n"), psLoadParamInfo->mTable.szTableName);
            LogMsgEx(fpCaseLog,  NONE, _T("------------------------------------------------------------------------------------------------\n"));
            LogMsgEx(fpCaseLog, NONE, _T("colnum    sql c type      sql type     column size       decimal digits   data size    column name\n"));
            LogMsgEx(fpCaseLog, NONE, _T("------------------------------------------------------------------------------------------------\n"));
            for(count = 0; count < psLoadParamInfo->mTable.columNum; count++){
               LogMsgEx(fpCaseLog, NONE, _T("%d          %s            %s            %d             %d       (%d,%d)    %s\n"),
                        count,
                        SQLCTypeToChar(psLoadParamInfo->mTable.sqlCType[count], szBuf),
                        SQLTypeToChar(psLoadParamInfo->mTable.sqlType[count], szTmp),
                        psLoadParamInfo->mTable.columnSize[count],
                        psLoadParamInfo->mTable.decimalDigits[count],
                        psLoadParamInfo->mTable.actualDataSize[count][0],
                        psLoadParamInfo->mTable.actualDataSize[count][1],
                        psLoadParamInfo->mTable.szColName[count]
                        );
            }
            LogMsgEx(fpCaseLog, NONE, _T("------------------------------------------------------------------------------------------------\n"));
            gettimeofday(&tvStart,NULL);
            ret = loadData(psLoadParamInfo);
            gettimeofday(&tvEnd,NULL);
            LogMsgEx(fpCaseLog, TIME_STAMP, _T("Sql load test is complete. Summary: test %s   time:%d ms\n"), 
                            (ret == 0) ? "pass" : "fail",
                            (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
            RELEASE_LOSG(fpCaseLog);
        }
        else if(strcmp("testSelect", szCase) == 0){
            postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("testSelect"));
            pCTestSql = new CTesSQL(_T("testSelect"));
            if(pCTestSql){
                fpCaseLog = pCTestSql->fileLogs();
                psSqlStmt = new sSqlStmt;
                if(psSqlStmt){
                    memset(psSqlStmt, 0, sizeof(sSqlStmt));
                    mConfig.readSelect(szSection, psSqlStmt);
                    gettimeofday(&tvStart,NULL);
                    ret = pCTestSql->testSelect(&psLoadParamInfo->mTestInfo, 
                                                psSqlStmt,
                                                &psLoadParamInfo->mTable);
                    gettimeofday(&tvEnd,NULL);
                    LogMsgEx(fpCaseLog, TIME_STAMP, _T("Select test is complete. Summary: test %s   time:%d ms\n"), 
                            (ret == 0) ? "pass" : "fail",
                            (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
                    delete psSqlStmt;
                }
                delete pCTestSql;
                pCTestSql = NULL;
            }
        }
        else if(strcmp("cleanupTable", szCase) == 0){
            postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("cleanupTable"));
            fpCaseLog = INIT_LOGS("cleanupTable");
            LogMsgEx(fpCaseLog, SHORTTIMESTAMP, _T("Begin cleanup table test...\n"));
            gettimeofday(&tvStart,NULL);
            ret = cleanupTableData(&psLoadParamInfo->mTestInfo, 
                                    psLoadParamInfo->mTable.szTableName, 
                                    fpCaseLog);
            gettimeofday(&tvEnd,NULL);
            LogMsgEx(fpCaseLog, TIME_STAMP, _T("Cleanup table is complete. Summary: test %s   time:%d ms\n"), 
                (ret == 0) ? "pass" : "fail",
                (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
            RELEASE_LOSG(fpCaseLog);
        }
        else if(strcmp("testStmt", szCase) == 0){
            postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("testStmt"));
            pCTestSql = new CTesSQL(_T("testStmt"));
            if(pCTestSql){
                fpCaseLog = pCTestSql->fileLogs();
                psSqlStmt = new sSqlStmt;
                if(psSqlStmt){
                    memset(psSqlStmt, 0, sizeof(sSqlStmt));
                    mConfig.readStmt(szSection, psSqlStmt);
                    gettimeofday(&tvStart,NULL);
                    ret = pCTestSql->testSQL(&psLoadParamInfo->mTestInfo, 
                                                psSqlStmt,
                                                &psLoadParamInfo->mTable);
                    gettimeofday(&tvEnd,NULL);
                    LogMsgEx(fpCaseLog, TIME_STAMP, _T("Test stmt is complete. Summary: test %s   time:%d ms\n"), 
                        (ret == 0) ? "pass" : "fail",
                        (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
                    delete psSqlStmt;
                }
                delete pCTestSql;
            }
        }
        else if(strcmp("testSqlFile", szCase) == 0){
            postLogMsg(SHORTTIMESTAMP, _T("create test:%s\n"),  _T("testSqlFile"));
            pCTestSql = new CTesSQL(_T("testSqlFile"));
            if(pCTestSql){
                fpCaseLog = pCTestSql->fileLogs();
                psSqlStmt = new sSqlStmt;
                if(psSqlStmt){
                    memset(psSqlStmt, 0, sizeof(sSqlStmt));
                    while(mConfig.readSqlFile(szSection, psSqlStmt) == 0){
                        gettimeofday(&tvStart,NULL);
                        ret = pCTestSql->testSQL(&psLoadParamInfo->mTestInfo, 
                                                psSqlStmt,
                                                &psLoadParamInfo->mTable);
                        gettimeofday(&tvEnd,NULL);
                        LogMsgEx(fpCaseLog, TIME_STAMP, _T("Sql file test is complete. Summary: test %s   time:%d ms\n"), 
                            (ret == 0) ? "pass" : "fail",
                            (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
                    }
                    delete psSqlStmt;
                }
                delete pCTestSql;
            }
        }
        idCase++;
#ifdef unixcli
        sleep(1);
#else
        Sleep(1000);
#endif

    }while(idCase >= 0);
    if(psLoadParamInfo) delete psLoadParamInfo;
    
}
