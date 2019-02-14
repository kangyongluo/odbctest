#include "../util/util.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../driver/odbcdriver.h"
#include "../case/testcase.h"

static FILE *fpLog = NULL;

static void LogMsg(unsigned int option, TCHAR *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    
    if(fpLog == NULL){
        fpLog = openLog("SQLBindParameter");
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

#ifdef unixcli
void *SQLBindParameterLoadThread(void *args)
#else
DWORD WINAPI SQLBindParameterLoadThread(LPVOID args)
#endif
{
    loadThreadPara *pLoadPara;
    int ret = -1;

    if(args == NULL){
#ifdef unixcli
        pthread_exit(NULL);
#else
        return 0;
#endif 
    }
    pLoadPara = (loadThreadPara *)args;
    if(pLoadPara->mLoadDataInfo.putDataType == SQL_NTS){
    ret = SQLBindParameterSqlNts(pLoadPara->pTestInfo, 
                            pLoadPara->pTableInfo,
                            &pLoadPara->mLoadDataInfo,
                            pLoadPara->szConnStr,
                            pLoadPara->fpLog
                            );
    }
    else{
        ret = SQLBindParameterSQLLen(pLoadPara->pTestInfo, 
                            pLoadPara->pTableInfo,
                            &pLoadPara->mLoadDataInfo,
                            pLoadPara->szConnStr,
                            pLoadPara->fpLog
                            );
    }
    pLoadPara->retcode = ret;
#ifdef unixcli
    pthread_exit(NULL);
#else
    return 0;
#endif 

}

PassFail testSQLBindParameter(TestInfo *pTestInfo)
{
    TEST_DECLARE;
    TEST_INIT;
    fpLog = INIT_LOGS("SQLBindParameter");
    
    TESTCASE_BEGINW(_T("begin SQLBindParameter test...\n"));
    
    struct timeval tv1, tv2;
    sTestConfig *pTestConfig = &gTestGlobal.mcfg;
    CConfig *pConfig = new CConfig;
    sTestTableInfo *psTestTableInfo = NULL;
    sODBCTestData *psODBCTestData = NULL;
    sLoadDataInfo *psLoadDataInfo = NULL;
    sLoadDataInfo mLoadInfo;
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
        LogMsg(NONE, _T("Failed to create configuration object.\n"));
        RELEASE_LOSG(fpLog);
        TEST_RETURN;
    }

    psODBCTestData = pConfig->getTestData();
    psTestTableInfo = &psODBCTestData->mTableInfo;
    psLoadDataInfo = &psODBCTestData->mBindParaInfo;
    gettimeofday(&tv1,NULL);
    while(pConfig->scanTestTable() == 0){   
        if(psODBCTestData->isAvailableTable == FALSE){
            continue;
        }       
        LogMsg( NONE, _T("%s\n"), psTestTableInfo->szTableName);
        LogMsg( NONE, _T("------------------------------------------------------------------------------------------------\n"));
        LogMsg( NONE, _T("colnum    sql c type      sql type     column size       decimal digits       column name\n"));
        LogMsg( NONE, _T("------------------------------------------------------------------------------------------------\n"));
        for(i = 0; i < psTestTableInfo->columNum; i++){
           LogMsg( NONE, _T("%d          %s            %s            %d             %d           %s\n"),
                    i,
                    SQLCTypeToChar(psTestTableInfo->sqlCType[i], szBuf),
                    SQLTypeToChar(psTestTableInfo->sqlType[i], szTmp),
                    psTestTableInfo->columnSize[i],
                    psTestTableInfo->decimalDigits[i],
                    psTestTableInfo->szColName[i]
                    );
        }
        LogMsg( NONE, _T("------------------------------------------------------------------------------------------------\n"));
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
            LogMsg( TIME_STAMP, _T("Begin load test. type:%s begin value:%d batch:%d num of batch:%d threads:%d\n"),
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
                        ploadThreadPara[threadId].pTestInfo = pTestInfo;
                        ploadThreadPara[threadId].pTableInfo = psTestTableInfo;
                        memcpy(&ploadThreadPara[threadId].mLoadDataInfo, psLoadDataInfo, sizeof(ploadThreadPara[threadId].mLoadDataInfo));
                        ploadThreadPara[threadId].mLoadDataInfo.beginVal = beginVal + psLoadDataInfo->batchSize * psLoadDataInfo->totalBatch * threadId;
                        ploadThreadPara[threadId].retcode = 0;
                        ploadThreadPara[threadId].loadType = DDL_LOAD_TYPE_BINDPARA;
                        sprintf(ploadThreadPara[threadId].mLoadDataInfo.szInsertType, "%s", szInsertType[testCount]);
                        sprintf(ploadThreadPara[threadId].szConnStr, "%s", pTestConfig->szConnStr);
                        ploadThreadPara[threadId].args = NULL;
                        sprintf(szBuf, "SQLBindParameter_%s_thread_%d_%s.log", ploadThreadPara[threadId].mLoadDataInfo.szInsertType, threadId, timeStr);
                        LogMsg( TIME_STAMP, "Create multi-threaded test insert performance, check the log file:%s\n", szBuf);
                        ploadThreadPara[threadId].fpLog = fopen(szBuf, "a+");
#ifdef unixcli
                        if(pthread_create(&mPth[threadId], NULL, SQLBindParameterLoadThread, &ploadThreadPara[threadId]) != 0){
                            mThStat[threadId] = FALSE;
                        }
                        else{
                            mThStat[threadId] = TRUE;
                        }
#else
                        mPth[threadId] = CreateThread(NULL, STACK_SIZE, SQLBindParameterLoadThread, &ploadThreadPara[threadId], STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
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
                        if(ploadThreadPara[threadId].fpLog) fclose(ploadThreadPara[threadId].fpLog);
                    }
                    delete ploadThreadPara;
                }
            }
            else{
                memcpy(&mLoadInfo, psLoadDataInfo, sizeof(sLoadDataInfo));
                sprintf(mLoadInfo.szInsertType, "%s", szInsertType[testCount]);
                if(psLoadDataInfo->putDataType == SQL_NTS){
                    ret = SQLBindParameterSqlNts(pTestInfo, 
                            psTestTableInfo,
                            &mLoadInfo,
                            pTestConfig->szConnStr,
                            fpLog
                            );
                }
                else{
                    ret = SQLBindParameterSQLLen(pTestInfo, 
                            psTestTableInfo,
                            &mLoadInfo,
                            pTestConfig->szConnStr,
                            fpLog
                            );
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
    LogMsg( TIME_STAMP, _T("Sql load test complete. Summary: test %s   time:%d ms\n"), 
                    (totalFail == 0) ? "pass" : "fail",
                    (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));
    delete pConfig;
    
    RELEASE_LOSG(fpLog);
    
    TEST_RETURN;
}
