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
        {_T("testRowset"), NULL},
        {_T("testSelect"), NULL},
        {_T("testStmt"), NULL},
        {_T("testInsert"), NULL},
        {_T("testLob"), NULL},
        {_T("testSqlFile"), NULL},
        {_T("cleanupTable"), NULL}
        };

static BOOL isDdlTest(char *pCase)
{
    if(_tcscmp(_T("testStmt"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("testInsert"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("testSqlFile"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("testRowset"), pCase) == 0){
        return TRUE;
    }
    else if(_tcscmp(_T("testLob"), pCase) == 0){
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
        postLogMsg(NONE, _T("create test:%s\n"),  mtestThreadPara.szTestCase);
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

