#ifdef unixcli
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#else
#include <windows.h>
#endif
#include "testODBC.h"
#include "../util/util.h"
#include "../driver/odbcdriver.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../case/testCase.h"
#include "testSQL.h"

#ifdef unixcli
void *odbcCaseThread(void *args)
#else
DWORD WINAPI odbcCaseThread(LPVOID args)
#endif
{
    CTestODBC *pCTesODBC = NULL;
    caseThreadPara mCaseThPara;
    char szBuf[128] = {0};
    
    if(args == NULL){
#ifdef unixcli
        pthread_exit(NULL);
#else
        return 0;
#endif 
    }
#if defined(unixcli)
    pthread_detach(pthread_self());
#else

#endif
    memcpy(&mCaseThPara, args, sizeof(caseThreadPara));
    pCTesODBC = new CTestODBC();
    if(pCTesODBC){ 
#ifdef unixcli        
        printf("[%s:%d]thread:%lu  section:%s  case:%s\n", 
                                                            __FILE__,
                                                            __LINE__,
                                                            pthread_self(), 
                                                            mCaseThPara.sectionInfo.szSection, 
                                                            mCaseThPara.sectionInfo.szCaseType);
        sprintf(szBuf, "%lu", pthread_self());
#else
        printf("[%s:%d]thread:%lu  section:%s  case:%s\n", 
                                                            __FILE__,
                                                            __LINE__,
                                                            GetCurrentThreadId(), 
                                                            mCaseThPara.sectionInfo.szSection, 
                                                            mCaseThPara.sectionInfo.szCaseType);
        sprintf(szBuf, "%lu", GetCurrentThreadId());
#endif
        pCTesODBC->doCase(mCaseThPara.sectionInfo);

        delete pCTesODBC;
        pCTesODBC = NULL;
    }
    else{
#ifdef unixcli        
        printf("[%s:%d]Create task object failed!!!\n", 
                                                        __FILE__,
                                                        __LINE__,
                                                        pthread_self());
#else
        printf("[%s:%d]Create task object failed!!!\n", 
                                                    __FILE__,
                                                    __LINE__,
                                                    GetCurrentThreadId());
#endif
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
void CTestODBC::runCasesSerial(void)
{
    char szCase[512] = {0};
    int idCase = 0;
    char *pSectionCase = NULL;
    char *pCaseType[CONFIG_TEST_CASES_MAX] = {NULL};
    int totalCaseType;
    char *pTok;
    int idType;
    BOOL status = TRUE;
    cfgSectionInfo mSection;
    int loadMultCount = -1;

    pSectionCase = gTestGlobal.szSection[0];
    for(idCase = 0; pSectionCase; idCase++){
        pSectionCase = gTestGlobal.szSection[idCase];
        if(pSectionCase == NULL) break;
        if(strcmp(pSectionCase, "config") == 0){
            continue;
        }
        postLogMsg(NONE, "[%s]\n\tfinding cases...\n",  pSectionCase);
        memset(szCase, 0, sizeof(szCase));
        if(getConfigString(pSectionCase,
                        "case_type",
                        "",
                        szCase,
                        sizeof(szCase) - 1,
                        ODBC_TEST_CFG_FILE
                        ) <= 0){
            continue;
        }
        getConfigBool(pSectionCase, 
                    "case_status", 
                    "TRUE", 
                    &status, 
                    ODBC_TEST_CFG_FILE);
        if(status != TRUE){
            continue;
        }
        loadMultCount = -1;
        totalCaseType = 0;
        pTok = strtok(szCase, ";");
        while(pTok){
            if(totalCaseType >= CONFIG_TEST_CASES_MAX) break;
            pCaseType[totalCaseType++] = pTok;
            pTok = strtok(NULL, ";");
        }
        for(idType = 0; idType < totalCaseType; idType++){
            sprintf(mSection.szSection, "%s", pSectionCase);
            sprintf(mSection.szCaseType, "%s", pCaseType[idType]);
            if((strcmp(CONFIG_ODBCTEST_CASE_LOAD_DIRECT, pCaseType[idType]) == 0) ||
                (strcmp(CONFIG_ODBCTEST_CASE_LOAD_AT_EXEC, pCaseType[idType]) == 0) ||
                (strcmp(CONFIG_ODBCTEST_CASE_LOB_UPDATE, pCaseType[idType]) == 0) ||
                (strcmp(CONFIG_ODBCTEST_CASE_ROWSET, pCaseType[idType]) == 0)){
                loadMultCount++;
            }
            mSection.loadBegintMultNum = (loadMultCount > 0) ? (loadMultCount) : 0;
            doCase(mSection);
        }
     }

}
void CTestODBC::runCases(void)
{
    int threadId = 0;
    char szCase[256] = {0};
    int idCase = 0, idSection;
    char szSection[64] = {0};
    char szBuf[256] = {0};
    int mTestThreads = 1, mThreadId = 0, mAliveThNum = 0, mActualThreads = 1;
#ifdef unixcli
    void *join;
    pthread_t mpth[CONFIG_TOTAL_THREAD_NUM];
#else
    HANDLE mpth[CONFIG_TOTAL_THREAD_NUM];
    DWORD thStat = 0;
#endif
    BOOL mThreadFlg[CONFIG_TOTAL_THREAD_NUM] = {0};
    caseThreadPara mCaseThPara[CONFIG_TOTAL_THREAD_NUM];
    int totalCases;
    char *p = NULL;
    int ret = 0;
    BOOL thraedOver = TRUE;
    char *pSectionCase = NULL;
    char *pCaseType[CONFIG_TEST_CASES_MAX] = {NULL};
    int totalCaseType;
    char *pTok;
    cfgSectionInfo mSection;
    int loadMultCount = 0;
    BOOL status = FALSE;
    
    for(mThreadId = 0; mThreadId < CONFIG_TOTAL_THREAD_NUM; mThreadId++){
        mThreadFlg[mThreadId] = FALSE;
    }
    
#ifdef unixcli
    int stacksize = 1048576;//1M 
    pthread_attr_t attr;

    pthread_attr_init(&attr);
#endif

    totalCases = gTestGlobal.mTotalSection;
    getConfigInt("config", 
                "case_threads", 
                "1", 
                &mTestThreads,
                ODBC_TEST_CFG_FILE);
    if(mTestThreads <= 1){
        runCasesSerial();
        return;
    }
    pSectionCase = gTestGlobal.szSection[0];
    for(idSection = 0; pSectionCase; idSection++){
        pSectionCase = gTestGlobal.szSection[idSection];
        if(pSectionCase == NULL) break;
        if(strcmp(pSectionCase, "config") == 0){
            continue;
        }
        memset(szCase, 0, sizeof(szCase));
        if(getConfigString(pSectionCase,
                        "case_type",
                        "",
                        szCase,
                        sizeof(szCase) - 1,
                        ODBC_TEST_CFG_FILE
                        ) <= 0){
            continue;
        }
        getConfigBool(pSectionCase, 
                    "case_status", 
                    "TRUE", 
                    &status, 
                    ODBC_TEST_CFG_FILE);
        if(status != TRUE){
            continue;
        }
        totalCaseType = 0;
        pTok = strtok(szCase, ";");
        while(pTok){
            if(totalCaseType >= CONFIG_TEST_CASES_MAX) break;
            pCaseType[totalCaseType++] = pTok;
            pTok = strtok(NULL, ";");
        }
        if(totalCaseType == 0) continue;
        else if(totalCaseType == 1){
            sprintf(mSection.szSection, "%s", pSectionCase);
            sprintf(mSection.szCaseType, "%s", pCaseType[idCase]);
            mSection.loadBegintMultNum = 1;
            doCase(mSection);
            continue;
        }
        mActualThreads = (mTestThreads > totalCaseType) ? totalCaseType : mTestThreads;
        postLogMsg(NONE, "[%s]\n\texpect threads:%d  actual threads:%d cases:%d\n",  
                            pSectionCase, 
                            mTestThreads, 
                            mActualThreads,
                            totalCaseType);
        for(idCase = 0; idCase < totalCaseType; idCase++){
            postLogMsg(NONE, "%s\n", pCaseType[idCase]);
        }
        mAliveThNum = 0;
        idCase = 0;
        loadMultCount = -1;
        while(idCase < totalCaseType){
            if(mAliveThNum < mActualThreads){
                for(mThreadId = 0; (mThreadId < mActualThreads) && (mThreadId < CONFIG_TOTAL_THREAD_NUM); mThreadId++){
                    if(idCase >= totalCaseType) break;
                    if(mThreadFlg[mThreadId] != TRUE){
                        sprintf(mCaseThPara[mThreadId].sectionInfo.szSection, "%s", pSectionCase);
                        sprintf(mCaseThPara[mThreadId].sectionInfo.szCaseType, "%s", pCaseType[idCase]);
                        if((strcmp(CONFIG_ODBCTEST_CASE_LOAD_DIRECT, pCaseType[idCase]) == 0) ||
                            (strcmp(CONFIG_ODBCTEST_CASE_LOAD_AT_EXEC, pCaseType[idCase]) == 0) ||
                            (strcmp(CONFIG_ODBCTEST_CASE_LOB_UPDATE, pCaseType[idCase]) == 0) ||
                            (strcmp(CONFIG_ODBCTEST_CASE_ROWSET, pCaseType[idCase]) == 0)){
                            loadMultCount++;
                        }
                        mCaseThPara[mThreadId].sectionInfo.loadBegintMultNum = (loadMultCount > 0) ? (loadMultCount) : 0;
#ifdef unixcli
                        if(pthread_create(&mpth[mThreadId], NULL, odbcCaseThread, &mCaseThPara[mThreadId]) == 0){
                            mThreadFlg[mThreadId] = TRUE;
                            mAliveThNum++;
                            sleep(1);
                        }
#else
                        mpth[mThreadId] = CreateThread(NULL, 0, odbcCaseThread, &mCaseThPara[mThreadId], STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
                        if(mpth[mThreadId] != NULL){
                            mThreadFlg[mThreadId] = TRUE;
                            mAliveThNum++;
                            Sleep(1000);
                        }
#endif
                        idCase++;
                    }
                }
            }
#ifdef unixcli
            sleep(5);
#else
            Sleep(5000);
#endif
            thraedOver = FALSE;
            while(thraedOver != TRUE){
                for(mThreadId = 0; (mThreadId < mActualThreads) && (mThreadId < CONFIG_TOTAL_THREAD_NUM); mThreadId++){
#ifdef unixcli
                    if(mThreadFlg[mThreadId] == TRUE){
                        ret = pthread_kill(mpth[mThreadId], 0);
                        if(ret != 0 || ret == ESRCH){
                            mThreadFlg[mThreadId] = FALSE;
                            if(mAliveThNum > 0) mAliveThNum--;
                        }
                        sleep(1);
                    }
#else
                    if(mThreadFlg[mThreadId] == TRUE){
                        GetExitCodeThread(mpth[mThreadId], &thStat);
                        if(thStat != STILL_ACTIVE){
                            mThreadFlg[mThreadId] = FALSE;
                            if(mAliveThNum > 0) mAliveThNum--;
                        }
                        Sleep(1000);
                    }
#endif            
                }
                if(mAliveThNum != mActualThreads){
                    thraedOver = TRUE;
                }
            }
            /*****************************************************************/
        };
        while(mAliveThNum > 0){
            for(mThreadId = 0; (mThreadId < mActualThreads) && (mThreadId < CONFIG_TOTAL_THREAD_NUM); mThreadId++){
#ifdef unixcli
                if(mThreadFlg[mThreadId] == TRUE){
                    ret = pthread_kill(mpth[mThreadId], 0);
                    if(ret != 0 || ret == ESRCH){
                        mThreadFlg[mThreadId] = FALSE;
                        if(mAliveThNum > 0) mAliveThNum--;
                    }
                    sleep(1);
                }
#else
                if(mThreadFlg[mThreadId] == TRUE){
                    GetExitCodeThread(mpth[mThreadId], &thStat);
                    if(thStat != STILL_ACTIVE){
                        mThreadFlg[mThreadId] = FALSE;
                        if(mAliveThNum > 0) mAliveThNum--;
                    }
                    Sleep(1000);
                }
#endif            
            }
        }

    }

    printf("[file %s line %d] end test\n", __FILE__, __LINE__);
}
void CTestODBC::doCase(cfgSectionInfo section)
{
    testCaseInfo mCaseInfo;
    sTestConfig *pConfig = &gTestGlobal.mcfg;
   
    memset(&mCaseInfo, 0, sizeof(mCaseInfo));
    sprintf(mCaseInfo.mTestInfo.Catalog, "%s", pConfig->szCatalog);
    sprintf(mCaseInfo.mTestInfo.DataSource, "%s", pConfig->szDataSource);
    sprintf(mCaseInfo.mTestInfo.UserID, "%s", pConfig->szUser);
    sprintf(mCaseInfo.mTestInfo.Password, "%s", pConfig->szPassword);
    sprintf(mCaseInfo.mTestInfo.Schema, "%s", pConfig->szSchema);
    memcpy(&mCaseInfo.mSection, &section, sizeof(cfgSectionInfo));
    
    if((strcmp(CONFIG_ODBCTEST_CASE_LOAD_DIRECT, section.szCaseType) == 0) ||
                (strcmp(CONFIG_ODBCTEST_CASE_LOAD_AT_EXEC, section.szCaseType) == 0) ||
                (strcmp(CONFIG_ODBCTEST_CASE_LOB_UPDATE, section.szCaseType) == 0) ||
                (strcmp(CONFIG_ODBCTEST_CASE_ROWSET, section.szCaseType) == 0)){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doLoadData(mCaseInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_SQL_BIG_OBJECT, section.szCaseType) == 0){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doSQLBigObject(mCaseInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_CLEANUP, section.szCaseType) == 0){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doCleanUpTable(mCaseInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_SQL, section.szCaseType) == 0){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doSQL(mCaseInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_SQL_FILE, section.szCaseType) == 0){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doSqlFile(mCaseInfo);
    }  
    else if(strcmp(CONFIG_ODBCTEST_CASE_SQLCOLUMNS, section.szCaseType) == 0){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doSQLColumns(mCaseInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_SQLTABLES, section.szCaseType) == 0){
        postLogMsg(SHORTTIMESTAMP, "create case   ---->   [%s]:%s\n",  section.szSection, section.szCaseType);
        doSQLTables(mCaseInfo);
    }
}
void CTestODBC::doLoadData(testCaseInfo caseInfo)
{
    FILE *fpCaseLog = NULL;
    char szBuf[256] = {0};
    char szTmp[64] = {0};
    CConfig mConfig;
    sLoadParamInfo *psLoadParamInfo;
    sLoadDataInfo mLoadDataInfo;
    int count;
    int ret = 0;
    struct timeval tvStart, tvEnd;
    sTestConfig *pConfig = &gTestGlobal.mcfg;
    int offsetBegin = 0;

    psLoadParamInfo = new sLoadParamInfo;
    if(psLoadParamInfo == NULL){
        return;
    }
    memset(psLoadParamInfo, 0, sizeof(sLoadParamInfo));

    if(mConfig.readTable(caseInfo.mSection.szSection, &psLoadParamInfo->mTable) != 0){
        postLogMsg(SHORTTIMESTAMP, "Load data fail!!!! Can not found table info.\n");
        if(psLoadParamInfo) delete psLoadParamInfo;
        return;
    }

    getConfigInt(caseInfo.mSection.szSection, 
                    "load_second_begin_offset", 
                    "0", 
                    (int *)&offsetBegin,
                    ODBC_TEST_CFG_FILE);
    
    memcpy(&psLoadParamInfo->mTestInfo, &caseInfo.mTestInfo, sizeof(TestInfo));
    if(strcmp(CONFIG_ODBCTEST_CASE_LOAD_DIRECT, caseInfo.mSection.szCaseType) == 0){
        fpCaseLog = INIT_LOGS("loadDirect");
        psLoadParamInfo->loadType =  BINDPARAM_EXECUTE_DIRECT;
        psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT;
        mConfig.readLoadDirectInfo(caseInfo.mSection.szSection, &mLoadDataInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_LOAD_AT_EXEC, caseInfo.mSection.szCaseType) == 0){
        fpCaseLog = INIT_LOGS("loadDirect");
        psLoadParamInfo->loadType =  BINDPARAM_SQL_PUT_DATA;
        psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT;
        mConfig.readAtExecInfo(caseInfo.mSection.szSection, &mLoadDataInfo);
        psLoadParamInfo->putType = (mLoadDataInfo.putDataType == SQL_DATA_AT_EXEC) ? PUT_DATA_AT_EXEC : PUT_LEN_DATA_AT_EXEC;
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_LOB_UPDATE, caseInfo.mSection.szCaseType) == 0){
        fpCaseLog = INIT_LOGS("loadDirect");
        psLoadParamInfo->loadType =  BINDPARAM_EXECUTE_DIRECT;
        psLoadParamInfo->putType = PUT_DATA_LOB_UPDATE;
        psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT_OUTPUT;
        mConfig.readLobUpdateInfo(caseInfo.mSection.szSection, &mLoadDataInfo);
    }
    else if(strcmp(CONFIG_ODBCTEST_CASE_ROWSET, caseInfo.mSection.szCaseType) == 0){
        fpCaseLog = INIT_LOGS("loadDirect");
        psLoadParamInfo->loadType =  BINDPARAM_ROWSET;
        psLoadParamInfo->inputOutputType = SQL_PARAM_INPUT;
        mConfig.readRowsetInfo(caseInfo.mSection.szSection, &mLoadDataInfo);
    }
    else{
        psLoadParamInfo->loadType =  BINDPARAM_UNKNOW;
        fpCaseLog = NULL;
    }
    if(psLoadParamInfo->mTable.isLobTable == TRUE){
        sprintf(psLoadParamInfo->mTable.szLobFile, "%s", mLoadDataInfo.szLobFile);
        if(strlen(psLoadParamInfo->mTable.szLobFile) > 0)
            psLoadParamInfo->mTable.isLobFile = TRUE;
        sprintf(psLoadParamInfo->mTable.szCharsetOfLobFile, "%s", mLoadDataInfo.szCharsetOfFile);
    }
    else{
        psLoadParamInfo->mTable.szLobFile[0] = 0x00;
        sprintf(psLoadParamInfo->mTable.szCharsetOfLobFile, "%s", "utf-8");
    }
    for(count = 0; count < CONFIG_NUM_CQD_SIZE; count++){
        if(strlen(mLoadDataInfo.szLoadCqd[count]) > 0){
            sprintf(psLoadParamInfo->mTable.szCqd[count], "%s", mLoadDataInfo.szLoadCqd[count]);
        }
        else{
            psLoadParamInfo->mTable.szCqd[count][0] = 0x00;
            break;
        }
    }
    memcpy(psLoadParamInfo->paraValueLen, psLoadParamInfo->mTable.paraValueLen, sizeof(psLoadParamInfo->paraValueLen));
    psLoadParamInfo->lenPutBatchSize = mLoadDataInfo.putDataBatch;
    psLoadParamInfo->beginVal = mLoadDataInfo.beginVal + caseInfo.mSection.loadBegintMultNum * mLoadDataInfo.batchSize * mLoadDataInfo.totalBatch;
    psLoadParamInfo->batchSize = mLoadDataInfo.batchSize;
    psLoadParamInfo->totalBatch = mLoadDataInfo.totalBatch;
    sprintf(psLoadParamInfo->szTable, "%s", psLoadParamInfo->mTable.szTableName);
    sprintf(psLoadParamInfo->szInsertType, "%s", mLoadDataInfo.szInsertType);
    sprintf(psLoadParamInfo->szInsertWhere, "%s", mLoadDataInfo.szInsertWhere);
    sprintf(psLoadParamInfo->szConnStr, "%s", pConfig->szConnStr);
    psLoadParamInfo->fpLog = fpCaseLog;
    LogMsgEx(fpCaseLog,  NONE, "------------------------------------------------------------------------------------------------\n");
    LogMsgEx(fpCaseLog, NONE, "table:%s\n", psLoadParamInfo->mTable.szTableName);
    LogMsgEx(fpCaseLog, NONE, "colnum    sql c type      sql type     column size       decimal digits   data size    column name\n");
    LogMsgEx(fpCaseLog, NONE, "------------------------------------------------------------------------------------------------\n");
    for(count = 0; count < psLoadParamInfo->mTable.columNum; count++){
       LogMsgEx(fpCaseLog, NONE, "%d          %s            %s            %d             %d       (%d,%d)    %s\n",
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
    LogMsgEx(fpCaseLog, NONE, "------------------------------------------------------------------------------------------------\n");
    gettimeofday(&tvStart,NULL);
    //ret = loadData(psLoadParamInfo);
    gettimeofday(&tvEnd,NULL);
    LogMsgEx(fpCaseLog, TIME_STAMP, "Loading is complete. Summary: test %s   time:%d ms\n", 
                    (ret == 0) ? "pass" : "fail",
                    (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
    RELEASE_LOSG(fpCaseLog);

    if(psLoadParamInfo) delete psLoadParamInfo;
}
void CTestODBC::doSQLBigObject(testCaseInfo caseInfo)
{
    CTesSQL *pCTestSql = NULL;
    FILE *fpCaseLog = NULL;
    sSqlStmt *psSqlStmt = NULL;
    int ret, i;
    struct timeval tvStart, tvEnd;
    sTestTableInfo mTable;
    CConfig mConfig;
    char szBuf[256] = {0};
    int testTime = 0, testCount = 0;
    char *pSQL, *start, *end;
    char szTmp[64] = {0};
    int digit, len;

    mConfig.readTable(caseInfo.mSection.szSection, &mTable);
    pCTestSql = new CTesSQL("testSQLBigObject");
    if(pCTestSql){
        fpCaseLog = pCTestSql->fileLogs();
        psSqlStmt = new sSqlStmt;
        if(psSqlStmt){
            memset(psSqlStmt, 0, sizeof(sSqlStmt));
            if(mConfig.readSQLBigObject(caseInfo.mSection.szSection, psSqlStmt) == 0){
                testTime = (psSqlStmt->testTimes > 1) ? psSqlStmt->testTimes : 1;
                digit = psSqlStmt->range[0];
                pSQL = NULL;
                if(strstr(psSqlStmt->szSql, "_digit")){
                    pSQL = strdup(psSqlStmt->szSql);
                }
                for(testCount = 0; testCount < testTime; testCount++){
                    if(pSQL){
                        memset(psSqlStmt->szSql, 0, sizeof(psSqlStmt->szSql));
                        start = pSQL;
                        end = strstr(start, "_digit");
                        i = 0;
                        sprintf(szTmp, "%d", digit);
                        while(end){ 
                            len = end - start;
                            memcpy(&psSqlStmt->szSql[i], start, len);
                            strcat(psSqlStmt->szSql, szTmp);
                            i += strlen(szTmp);
                            i += (SQLSMALLINT)len;
                            start = end + strlen("_digit");
                            end = strstr(start, "_digit");
                        }
                        strcat(psSqlStmt->szSql, start);
                        if(digit < psSqlStmt->range[1]) digit++;
                        else if(psSqlStmt->range[0] == 0 && psSqlStmt->range[1] == 0) digit++;
                        else digit = psSqlStmt->range[0];
                    }
                    gettimeofday(&tvStart,NULL);
                    
                    ret = pCTestSql->testSqlBigObject(&caseInfo.mTestInfo, 
                                                psSqlStmt,
                                                &mTable);
                    gettimeofday(&tvEnd,NULL);
                    LogMsgEx(fpCaseLog, TIME_STAMP, "Big sql object test is complete. Summary: test %s   time:%d ms\n", 
                            (ret == 0) ? "pass" : "fail",
                            (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
                }
                if(pSQL) free(pSQL);
            }
            else{
                LogMsgEx(fpCaseLog, NONE, "Can not found sql info!!!\n");
            }
            delete psSqlStmt;
        }
        delete pCTestSql;
        pCTestSql = NULL;
    }
}
void CTestODBC::doSQL(testCaseInfo caseInfo)
{
    CTesSQL *pCTestSql = NULL;
    FILE *fpCaseLog = NULL;
    sSqlStmt *psSqlStmt = NULL;
    int ret;
    struct timeval tvStart, tvEnd;
    CConfig mConfig;
    int testTime = 0, testCount = 0, i = 0;
    char *pSQL, *start, *end;
    char szTmp[64] = {0};
    int digit, len;

    pCTestSql = new CTesSQL("testSQL");
    if(pCTestSql){
        fpCaseLog = pCTestSql->fileLogs();
        psSqlStmt = new sSqlStmt;
        if(psSqlStmt){
            memset(psSqlStmt, 0, sizeof(sSqlStmt));
            if(mConfig.readSQL(caseInfo.mSection.szSection, psSqlStmt) == 0){
                testTime = (psSqlStmt->testTimes > 1) ? psSqlStmt->testTimes : 1;
                digit = psSqlStmt->range[0];
                pSQL = NULL;
                if(strstr(psSqlStmt->szSql, "_digit")){
                    pSQL = strdup(psSqlStmt->szSql);
                }
                for(testCount = 0; testCount < testTime; testCount++){
                    if(pSQL){
                        memset(psSqlStmt->szSql, 0, sizeof(psSqlStmt->szSql));
                        start = pSQL;
                        end = strstr(start, "_digit");
                        i = 0;
                        sprintf(szTmp, "%d", digit);
                        while(end){ 
                            len = end - start;
                            memcpy(&psSqlStmt->szSql[i], start, len);
                            strcat(psSqlStmt->szSql, szTmp);
                            i += strlen(szTmp);
                            i += (SQLSMALLINT)len;
                            start = end + strlen("_digit");
                            end = strstr(start, "_digit");
                        }
                        strcat(psSqlStmt->szSql, start);
                        if(digit < psSqlStmt->range[1]) digit++;
                        else if(psSqlStmt->range[0] == 0 && psSqlStmt->range[1] == 0) digit++;
                        else digit = psSqlStmt->range[0];
                    }
                    gettimeofday(&tvStart,NULL);
                    ret = pCTestSql->testSQL(&caseInfo.mTestInfo, psSqlStmt);
                    gettimeofday(&tvEnd,NULL);
                    LogMsgEx(fpCaseLog, TIME_STAMP, "Test sql is complete. Summary: test %s   time:%d ms\n", 
                        (ret == 0) ? "pass" : "fail",
                        (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
                } 
                if(pSQL) free(pSQL);
            }
            else{
                LogMsgEx(fpCaseLog, NONE, "Can not found sql info!!!\n");
            }
            delete psSqlStmt;
        }
        delete pCTestSql;
    }
}
void CTestODBC::doCleanUpTable(testCaseInfo caseInfo)
{
    FILE *fpCaseLog = NULL;
    int ret;
    struct timeval tvStart, tvEnd;
    sTestTableInfo mTable;
    CConfig mConfig;
    
    mConfig.readTable(caseInfo.mSection.szSection, &mTable);
    postLogMsg(SHORTTIMESTAMP, "create test:%s\n",  "cleanupTable");
    fpCaseLog = INIT_LOGS("cleanupTable");
    LogMsgEx(fpCaseLog, SHORTTIMESTAMP, "Begin cleanup table test...\n");
    gettimeofday(&tvStart,NULL);
    ret = cleanupTableData(&caseInfo.mTestInfo, 
                            mTable.szTableName, 
                            fpCaseLog);
    gettimeofday(&tvEnd,NULL);
    LogMsgEx(fpCaseLog, TIME_STAMP, "Cleanup table is complete. Summary: test %s   time:%d ms\n", 
    (ret == 0) ? "pass" : "fail",
    (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
    RELEASE_LOSG(fpCaseLog);
}
void CTestODBC::doSQLColumns(testCaseInfo caseInfo)
{
    testSQLColumns(caseInfo);
}
void CTestODBC::doSQLTables(testCaseInfo caseInfo)
{
    testSQLTables(caseInfo);
}
void CTestODBC::doSqlFile(testCaseInfo caseInfo)
{
    char szSqlFile[128] = {0};
    CTesSQL *pCTestSql = NULL;
    FILE *fpCaseLog = NULL;
    sSqlStmt *psSqlStmt = NULL;
    int ret;
    struct timeval tvStart, tvEnd;
    CConfig mConfig;
    FILE *fpSql;
    
    if(getConfigString(caseInfo.mSection.szSection,
                        "sql_file",
                        "",
                        szSqlFile,
                        sizeof(szSqlFile) - 1,
                        ODBC_TEST_CFG_FILE
                        ) <= 0){
        return;
    }
    
    fpSql = fopen(szSqlFile, "r");
    if(fpSql == NULL){
        LogMsgEx(fpCaseLog, NONE, "Can not open %s\n", szSqlFile);
        return;
    }
    pCTestSql = new CTesSQL("testSQLFile");
    if(pCTestSql){
        fpCaseLog = pCTestSql->fileLogs();
        psSqlStmt = new sSqlStmt;
        if(psSqlStmt){
            memset(psSqlStmt, 0, sizeof(sSqlStmt));
            mConfig.readSQLFileInfo(caseInfo.mSection.szSection, psSqlStmt);
            memset(psSqlStmt->szSql, 0, sizeof(psSqlStmt->szSql));
            while(getSQLFromFile(fpSql, psSqlStmt->szSql, sizeof(psSqlStmt->szSql) - 1) == 0){
                gettimeofday(&tvStart,NULL);
                ret = pCTestSql->testSQL(&caseInfo.mTestInfo, psSqlStmt);
                gettimeofday(&tvEnd,NULL);
                LogMsgEx(fpCaseLog, TIME_STAMP, "Test sql is complete. Summary: test %s   time:%d ms\n", 
                    (ret == 0) ? "pass" : "fail",
                    (tvEnd.tv_sec*1000 + tvEnd.tv_usec/1000) - (tvStart.tv_sec*1000 + tvStart.tv_usec/1000));
                memset(psSqlStmt->szSql, 0, sizeof(psSqlStmt->szSql));
            }
            
            delete psSqlStmt;
        }
        delete pCTestSql;
    }

    fclose(fpSql);
}
