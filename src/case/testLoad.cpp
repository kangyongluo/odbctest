#include "../util/util.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../driver/odbcdriver.h"
#include "../case/testcase.h"

typedef enum{
    LOAD_TYPE_DIRECT = 0,
    LOAD_TYPE_AT_EXEC,
    LOAD_TYPE_ROWSET,
    LOAD_TYPE_LOB_UPDATE
}LOAD_TEST_TYPE;

#ifdef unixcli
void *loadThread(void *args)
#else
DWORD WINAPI loadThread(LPVOID args)
#endif
{
    loadThreadPara *pLoadThreadPara;
    sLoadParamInfo mLoadParamInfo;
    int ret = -1;

    if(args == NULL){
#ifdef unixcli
        pthread_exit(NULL);
#else
        return 0;
#endif 
    }
    pLoadThreadPara = (loadThreadPara *)args;

    memcpy(&mLoadParamInfo, &pLoadThreadPara->mLoadParam, sizeof(sLoadParamInfo));
    pLoadThreadPara->retcode = loadData(&mLoadParamInfo);
#ifdef unixcli
    pthread_exit(NULL);
#else
    return 0;
#endif 
}

int testLoad(TestInfo *pTestInfo, LOAD_TEST_TYPE loadType)
{
    FILE *fpTestLog = NULL;
    
    if(loadType == LOAD_TYPE_DIRECT){
        fpTestLog = INIT_LOGS("loadDirect");
    }
    else if(loadType == LOAD_TYPE_AT_EXEC){
       fpTestLog = INIT_LOGS("loadAtExec");
    }
    else if(loadType == LOAD_TYPE_ROWSET){
        fpTestLog = INIT_LOGS("rowset");
    }
    else if(loadType == LOAD_TYPE_LOB_UPDATE){
        fpTestLog = INIT_LOGS("lobupdate");
    }
    else{
        printf("Unknow load type. file:%s line %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    TESTCASE_BEGINW(_T("begin load test...\n"));
    
    struct timeval tv1, tv2;
    sTestConfig *pTestConfig = &gTestGlobal.mcfg;
    CConfig *pConfig = new CConfig;
    sTestTableInfo *psTestTableInfo = NULL;
    sODBCTestData *psODBCTestData = NULL;
    sLoadDataInfo *psLoadDataInfo = NULL;
    int totalPass = 0;
    int totalFail = 0;
    TCHAR szBuf[256] = {0};
    TCHAR szTmp[64] = {0};
    char *result;
    int testCount = 0;
    int beginVal = 0;
    int i;
    int ret = 0;
    int threadId;
    char szInsertType[CONFIG_LOAD_TYPE_MAX][50];
#ifdef unixcli
    pthread_t mPth[CONFIG_LOAD_THREAD_MAX];
    void *join;
#else
        HANDLE mPth[CONFIG_LOAD_THREAD_MAX];
#endif
    BOOL mThStat[CONFIG_LOAD_THREAD_MAX];
    int szRet[CONFIG_LOAD_THREAD_MAX] = {0};
    loadThreadPara *ploadThreadPara;

    if(pConfig == NULL){
        LogMsgEx(fpTestLog, NONE, _T("Failed to create configuration object.\n"));
        RELEASE_LOSG(fpTestLog);
        TEST_RETURN;
    }

    psODBCTestData = pConfig->getTestData();
    psTestTableInfo = &psODBCTestData->mTableInfo;
    if(loadType == LOAD_TYPE_DIRECT){
        psLoadDataInfo = &psODBCTestData->mLoadDirect;
    }
    else if(loadType == LOAD_TYPE_AT_EXEC){
       psLoadDataInfo = &psODBCTestData->mLoadAtExec;
    }
    else if(loadType == LOAD_TYPE_ROWSET){
        psLoadDataInfo = &psODBCTestData->mRowsetInfo;
    }
    else if(loadType == LOAD_TYPE_LOB_UPDATE){
        psLoadDataInfo = &psODBCTestData->mLobUpdate;
    }
    gettimeofday(&tv1,NULL);
    while(pConfig->scanTestTable() == 0){   
        if(psODBCTestData->isAvailableTable == FALSE){
            continue;
        }       
        LogMsgEx(fpTestLog, NONE, _T("%s\n"), psTestTableInfo->szTableName);
        LogMsgEx(fpTestLog,  NONE, _T("------------------------------------------------------------------------------------------------\n"));
        LogMsgEx(fpTestLog, NONE, _T("colnum    sql c type      sql type     column size       decimal digits   data size    column name\n"));
        LogMsgEx(fpTestLog, NONE, _T("------------------------------------------------------------------------------------------------\n"));
        for(i = 0; i < psTestTableInfo->columNum; i++){
           LogMsgEx(fpTestLog, NONE, _T("%d          %s            %s            %d             %d       (%d,%d)    %s\n"),
                    i,
                    SQLCTypeToChar(psTestTableInfo->sqlCType[i], szBuf),
                    SQLTypeToChar(psTestTableInfo->sqlType[i], szTmp),
                    psTestTableInfo->columnSize[i],
                    psTestTableInfo->decimalDigits[i],
                    psTestTableInfo->actualDataSize[i][0],
                    psTestTableInfo->actualDataSize[i][1],
                    psTestTableInfo->szColName[i]
                    );
        }
        LogMsgEx(fpTestLog, NONE, _T("------------------------------------------------------------------------------------------------\n"));
        beginVal = 0;
        testCount = 0;
        memset(&szInsertType, 0, sizeof(szInsertType));
        sprintf(szBuf, "%s", psLoadDataInfo->szInsertType);
        result = strtok(szBuf, ",");
        while( result != NULL ) {
            sprintf(szInsertType[testCount++], "%s", result);
            result = strtok(NULL, ",");
            if(testCount >= CONFIG_LOAD_TYPE_MAX) break;
        }
        testCount = 0;
        while(strlen(szInsertType[testCount]) > 0) {
            beginVal = psLoadDataInfo->beginVal + (psLoadDataInfo->batchSize * psLoadDataInfo->totalBatch * testCount * psLoadDataInfo->threads);
            LogMsgEx(fpTestLog, TIME_STAMP, _T("Begin load test. type:%s begin value:%d batch:%d num of batch:%d threads:%d\n"),
                                szInsertType[testCount],
                                beginVal,
                                psLoadDataInfo->batchSize,
                                psLoadDataInfo->totalBatch,
                                psLoadDataInfo->threads
                                );
            if(psLoadDataInfo->threads > 1){
                ploadThreadPara = new loadThreadPara[psLoadDataInfo->threads];
                if(ploadThreadPara != NULL){
                    char timeStr[50] = {0};
                    time_t tm = time( NULL);
                    strftime(timeStr, sizeof(timeStr) - 1, "%Y-%m-%d_%H.%M.%S", localtime(&tm));
                    
                    for(threadId = 0; threadId < psLoadDataInfo->threads; threadId++){
                        if(threadId >= CONFIG_LOAD_THREAD_MAX) break;
                        memcpy(&ploadThreadPara[threadId].mLoadParam.mTestInfo, pTestInfo, sizeof(TestInfo));
                        if(loadType == LOAD_TYPE_DIRECT){
                            ploadThreadPara[threadId].mLoadParam.loadType =  BINDPARAM_EXECUTE_DIRECT;
                        }
                        else if(loadType == LOAD_TYPE_AT_EXEC){
                            ploadThreadPara[threadId].mLoadParam.loadType =  BINDPARAM_SQL_PUT_DATA;
                        }
                        else if(loadType == LOAD_TYPE_ROWSET){
                            ploadThreadPara[threadId].mLoadParam.loadType =  BINDPARAM_ROWSET;
                        }
                        else if(loadType == LOAD_TYPE_LOB_UPDATE){
                            ploadThreadPara[threadId].mLoadParam.loadType =  BINDPARAM_SQL_PUT_DATA;
                        }
                        else{
                            ploadThreadPara[threadId].mLoadParam.loadType =  BINDPARAM_EXECUTE_DIRECT;
                        }
                        ploadThreadPara[threadId].mLoadParam.inputOutputType = SQL_PARAM_INPUT;
                        memcpy(&ploadThreadPara[threadId].mLoadParam.mTable, psTestTableInfo, sizeof(sTestTableInfo));
                        if(ploadThreadPara[threadId].mLoadParam.mTable.isLobTable == TRUE){
                            _stprintf(ploadThreadPara[threadId].mLoadParam.mTable.szLobFile, _T("%s"), psLoadDataInfo->szLobFile);
                            if(_tcslen(ploadThreadPara[threadId].mLoadParam.mTable.szLobFile) > 0)
                                ploadThreadPara[threadId].mLoadParam.mTable.isLobFile = TRUE;
                        }
                        else{
                            ploadThreadPara[threadId].mLoadParam.mTable.szLobFile[0] = 0x00;
                        }
                        for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
                            if(_tcslen(psLoadDataInfo->szLoadCqd[i]) > 0){
                                _stprintf(ploadThreadPara[threadId].mLoadParam.mTable.szCqd[i], _T("%s"), psLoadDataInfo->szLoadCqd[i]);
                            }
                            else{
                                ploadThreadPara[threadId].mLoadParam.mTable.szCqd[i][0] = 0x00;
                                break;
                            }
                        }
                        ploadThreadPara[threadId].mLoadParam.lenPutBatchSize = psLoadDataInfo->putDataBatch;
                        if(loadType == LOAD_TYPE_LOB_UPDATE){
                            ploadThreadPara[threadId].mLoadParam.putType = PUT_DATA_LOB_UPDATE;
                        }
                        else{
                            ploadThreadPara[threadId].mLoadParam.putType = (psLoadDataInfo->putDataType == SQL_DATA_AT_EXEC) ? PUT_DATA_AT_EXEC : PUT_LEN_DATA_AT_EXEC;
                        }
                        ploadThreadPara[threadId].mLoadParam.beginVal = psLoadDataInfo->beginVal;
                        ploadThreadPara[threadId].mLoadParam.batchSize = psLoadDataInfo->batchSize;
                        ploadThreadPara[threadId].mLoadParam.totalBatch = psLoadDataInfo->totalBatch;
                        _stprintf(ploadThreadPara[threadId].mLoadParam.szInsertType, _T("%s"), psLoadDataInfo->szInsertType);
                        _stprintf(ploadThreadPara[threadId].mLoadParam.szConnStr, _T("%s"), pTestConfig->szConnStr);
                        sprintf(szBuf, "loadDirect_%s_thread_%d_%s.log", ploadThreadPara[threadId].mLoadParam.szInsertType, threadId, timeStr);
                        LogMsgEx(fpTestLog, TIME_STAMP, "Create multi-threaded test load performance, check the log file:%s\n", szBuf);
                        ploadThreadPara[threadId].mLoadParam.fpLog = fopen(szBuf, "a+");
#ifdef unixcli
                        if(pthread_create(&mPth[threadId], NULL, loadThread, &ploadThreadPara[threadId]) != 0){
                            mThStat[threadId] = FALSE;
                        }
                        else{
                            mThStat[threadId] = TRUE;
                        }
#else
                        mPth[threadId] = CreateThread(NULL, STACK_SIZE, loadThread, &ploadThreadPara[threadId], STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
                        if(mPth[threadId] == NULL){
                            mThStat[threadId] = FALSE;
                        }
                        else{
                            mThStat[threadId] = TRUE;
                        }
#endif    
                    }
                    for(threadId = 0; threadId < psLoadDataInfo->threads; threadId++){
                        if(threadId >= CONFIG_LOAD_THREAD_MAX) break;
#ifdef unixcli
                        if(mThStat[threadId] == TRUE){
                            pthread_join(mPth[threadId], &join);
                            if(ploadThreadPara[threadId].retcode == 0) totalPass++;
                            else totalFail++;
                        }    
#else
                        if(mThStat[threadId] == TRUE){
                            WaitForSingleObject(mPth[threadId], INFINITE);
                            if(ploadThreadPara[threadId].retcode == 0) totalPass++;
                            else totalFail++;
                        }
#endif    
                        if(ploadThreadPara[threadId].mLoadParam.fpLog) fclose(ploadThreadPara[threadId].mLoadParam.fpLog);
                    }
                    delete ploadThreadPara;
                }
            }
            else{
                ret = -1;
                ploadThreadPara = new loadThreadPara;
                if(ploadThreadPara){
                    memcpy(&ploadThreadPara->mLoadParam.mTestInfo, pTestInfo, sizeof(ploadThreadPara->mLoadParam.mTestInfo));
                    if(loadType == LOAD_TYPE_DIRECT){
                        ploadThreadPara->mLoadParam.inputOutputType = SQL_PARAM_INPUT;
                        ploadThreadPara->mLoadParam.loadType =  BINDPARAM_EXECUTE_DIRECT;
                    }
                    else if(loadType == LOAD_TYPE_AT_EXEC){
                        ploadThreadPara->mLoadParam.inputOutputType = SQL_PARAM_INPUT;
                        ploadThreadPara->mLoadParam.loadType =  BINDPARAM_SQL_PUT_DATA;
                    }
                    else if(loadType == LOAD_TYPE_ROWSET){
                        ploadThreadPara->mLoadParam.inputOutputType = SQL_PARAM_INPUT;
                        ploadThreadPara->mLoadParam.loadType =  BINDPARAM_ROWSET;
                    }
                    else if(loadType == LOAD_TYPE_LOB_UPDATE){
                        ploadThreadPara->mLoadParam.inputOutputType = SQL_PARAM_INPUT_OUTPUT;
                        ploadThreadPara->mLoadParam.loadType =  BINDPARAM_SQL_PUT_DATA;
                    }
                    else{
                        ploadThreadPara->mLoadParam.inputOutputType = SQL_PARAM_INPUT;
                        ploadThreadPara->mLoadParam.loadType =  BINDPARAM_EXECUTE_DIRECT;
                    }
                    memcpy(&ploadThreadPara->mLoadParam.mTable, psTestTableInfo, sizeof(ploadThreadPara->mLoadParam.mTable));
                    if(ploadThreadPara->mLoadParam.mTable.isLobTable == TRUE){
                        _stprintf(ploadThreadPara->mLoadParam.mTable.szLobFile, _T("%s"), psLoadDataInfo->szLobFile);
                        if(_tcslen(ploadThreadPara->mLoadParam.mTable.szLobFile) > 0)
                            ploadThreadPara->mLoadParam.mTable.isLobFile = TRUE;
                    }
                    else{
                        ploadThreadPara->mLoadParam.mTable.szLobFile[0] = 0x00;
                    }
                    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
                        if(_tcslen(psLoadDataInfo->szLoadCqd[i]) > 0){
                            _stprintf(ploadThreadPara->mLoadParam.mTable.szCqd[i], _T("%s"), psLoadDataInfo->szLoadCqd[i]);
                        }
                        else{
                            ploadThreadPara->mLoadParam.mTable.szCqd[i][0] = 0x00;
                            break;
                        }
                    }
                    ploadThreadPara->mLoadParam.lenPutBatchSize = psLoadDataInfo->putDataBatch;
                    if(loadType == LOAD_TYPE_LOB_UPDATE){
                        ploadThreadPara->mLoadParam.putType = PUT_DATA_LOB_UPDATE;
                    }
                    else{
                        ploadThreadPara->mLoadParam.putType = (psLoadDataInfo->putDataType == SQL_DATA_AT_EXEC) ? PUT_DATA_AT_EXEC : PUT_LEN_DATA_AT_EXEC;
                    }
                    ploadThreadPara->mLoadParam.beginVal = psLoadDataInfo->beginVal;
                    ploadThreadPara->mLoadParam.batchSize = psLoadDataInfo->batchSize;
                    ploadThreadPara->mLoadParam.totalBatch = psLoadDataInfo->totalBatch;
                    _stprintf(ploadThreadPara->mLoadParam.szInsertType, _T("%s"), psLoadDataInfo->szInsertType);
                    _stprintf(ploadThreadPara->mLoadParam.szConnStr, _T("%s"), pTestConfig->szConnStr);
                    ploadThreadPara->mLoadParam.fpLog = fpTestLog;
                    ret = loadData(&ploadThreadPara->mLoadParam);
                    delete ploadThreadPara;
                }
                if(ret == 0){
                    totalPass++;
                }
                else{
                    totalFail++;
                }
            }
            testCount++;
            if(testCount >= CONFIG_LOAD_TYPE_MAX) break;
        }
    };
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpTestLog, TIME_STAMP, _T("Sql load test complete. Summary: test %s   time:%d ms\n"), 
                    (totalFail == 0) ? "pass" : "fail",
                    (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));
    delete pConfig;
    
    RELEASE_LOSG(fpTestLog);
    
    TEST_RETURN;
}

PassFail testLoadDirect(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    TEST_INIT;
    
    testLoad(pTestInfo, LOAD_TYPE_DIRECT);

    TEST_RETURN;
}
PassFail testLoadAtExec(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    TEST_INIT;
    
    testLoad(pTestInfo, LOAD_TYPE_AT_EXEC);

    TEST_RETURN;
}

PassFail testRowset(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    TEST_INIT;
    
    testLoad(pTestInfo, LOAD_TYPE_ROWSET);

    TEST_RETURN;
}
PassFail testLobUpdate(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    TEST_INIT;
    
    testLoad(pTestInfo, LOAD_TYPE_LOB_UPDATE);

    TEST_RETURN;
}

