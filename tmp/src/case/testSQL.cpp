#include "../util/util.h"
#include "../util/config.h"
#include "../case/testSQL.h"
#include "../driver/odbcdriver.h"


CTesSQL::CTesSQL(void)
{   
    fpLog = openLog("ddl");
    isBindOk = FALSE;
    mBeginVal = 0;
    pGlobalCfg = &gTestGlobal.mcfg;
}
CTesSQL::CTesSQL(TCHAR *szKey)
{   
    TCHAR szBuf[128] = {0};

    sprintf(szBuf, "ddl_%s", szKey);
    fpLog = openLog(szBuf);
    isBindOk = FALSE;
    mBeginVal = 0;
    pGlobalCfg = &gTestGlobal.mcfg;
}

CTesSQL::~CTesSQL()
{
    closeLog(fpLog);
}
int CTesSQL::testSelect(TestInfo *pTestInfo, 
                      sSqlStmt *psStmt,
                      sTestTableInfo *psTestTableInfo
                      )
{
    SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
    RETCODE retcode;
    TestInfo sTestInfo;
    #define CONFIG_GET_DATA_SIZE    (16777216 + 2)
    unsigned char *pData = NULL;
    struct timeval tv1, tv2, tv3, tv4;
    SQLSMALLINT cols, i;
    SQLLEN len, minLen, maxLen;
    int loop;
    int totalRows;
    int crcTimes = 0;
    unsigned int crc = 0;
    unsigned int crc1 = 0;
    int totalTimes = 0;
    int ret = 0;
    char szBuf[256] = {0};
    char szSelect[CONFIG_SELECT_STMT_MAX][256] = {0};
    int totalSelect = 0;
    char *result;

    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pGlobalCfg->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pGlobalCfg->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
   
    SQLExecDirect(hstmt, (SQLTCHAR*)_T("set FETCHSIZE 1"), SQL_NTS);

    LogMsgEx(fpLog, NONE, _T(".........................................\n"));
    addInfoSessionEx(hstmt, fpLog);
    LogMsgEx(fpLog, NONE, _T(".........................................\n"));
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(psStmt->szStmtCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), psStmt->szStmtCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)psStmt->szStmtCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    pData = (unsigned char *)malloc(CONFIG_GET_DATA_SIZE * sizeof(TCHAR) + 2);
    if(pData == NULL){
        LogMsgEx(fpLog, NONE, _T("malloc  fail: %d\n"), __LINE__);
        FullDisconnectEx(fpLog, &sTestInfo);
        return -1;
    }
    memset(szSelect, 0, sizeof(szSelect));
    sprintf((char *)pData, "%s", psStmt->szSql);
    result = strtok((char *)pData, ";");
    totalSelect = 0;
    while( result != NULL ) {
        _stprintf(szSelect[totalSelect++], result);
        result = strtok(NULL, ";");
        if(totalSelect >= CONFIG_SELECT_STMT_MAX) break;
    }
    for(loop = 0; loop < totalSelect; loop++){
        LogMsgEx(fpLog, TIME_STAMP, _T("%s\n"), szSelect[loop]);
        gettimeofday(&tv1,NULL);
        retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szSelect[loop], SQL_NTS);
        if(retcode != SQL_SUCCESS){
            gettimeofday(&tv2,NULL);
            totalTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            LogMsgEx(fpLog, TIME_STAMP, _T("SQLExecDirect select fail: exe time:%d (ms)\n"), totalTimes);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            SQLFreeStmt(hstmt, SQL_UNBIND);
            SQLFreeStmt(hstmt, SQL_CLOSE);
            continue;
        }
        crcTimes = 0;
        totalTimes = 0;
        totalRows = 0;
        cols = 0;
        minLen = 0;
        maxLen = 0;
        retcode = SQLFetch(hstmt);
        retcode = SQLNumResultCols(hstmt, &cols);
        while (retcode == SQL_SUCCESS)
        {
            for (i = 1; i <= cols; i++)
            {
                //retcode = SQLGetData(hstmt, i, SQL_C_BINARY, (SQLPOINTER)pData, CONFIG_GET_DATA_SIZE - 1, &len);
                retcode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)pData, CONFIG_GET_DATA_SIZE - 1, &len);
                if (retcode == SQL_SUCCESS){
                    if(maxLen == 0 && minLen == 0) maxLen = minLen = len;
                    if(maxLen <= len) maxLen = len;
                    else if(minLen > len) minLen = len;
                    //LogMsgEx(fpLog, NONE, _T("%s\n"), pData);
                    gettimeofday(&tv3,NULL);
                    if((len > 10) &&
                        (psTestTableInfo->sqlType[i - 1] == SQL_CHAR ||
                         psTestTableInfo->sqlType[i - 1] == SQL_VARCHAR ||
                         psTestTableInfo->sqlType[i - 1] == SQL_LONGVARCHAR
                        )){
                        if((psStmt->isSaveFile) && (psStmt->saveColId == (i - 1))){
                            FILE *fp;
                            char szSaveFile[32] = {0};
                            if((loop == 0) && (i == 1)){
                                _stprintf(szSaveFile, "%s", psStmt->szSaveFile);
                            }
                            else{
                                _stprintf(szSaveFile, "%d_%d_%s", loop, i, psStmt->szSaveFile);
                            }
                            fp = fopen(szSaveFile, "wb+");
                            if(fp){
                                if(psStmt->isCalcCrc == FALSE){
                                    LogMsgEx(fpLog, NONE, _T("%s size is %d\n"), szSaveFile, len);
                                    fwrite(pData, len, 1, fp);
                                }
                                else{
                                    LogMsgEx(fpLog, NONE, _T("%s size is %d\n"), szSaveFile, len - 8);
                                    fwrite(&pData[8], len - 8, 1, fp);
                                }
                                fclose(fp);
                            }
                            else{
                                LogMsgEx(fpLog, NONE, _T("open % fail!\n"), szSaveFile);
                            }
                        }
                        if(psStmt->isCalcCrc){
                            crc = calcCRC32(&pData[8], len - 8);
                            crc1 = 0;
                            asciiToHex(pData, (unsigned char *)&crc1, 8);
                            if(crc != crc1){
                                LogMsgEx(fpLog, NONE, _T("row:%d columns:%d check crc fail, expect:0x%x  actual:0x%x, line = %d\n"), 
                                                totalRows, i, crc, crc1, __LINE__);
                                ret = -1;
                            }
                        }
                        gettimeofday(&tv4,NULL);
                        crcTimes += (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                    }
                }
                else if(i == 1){
                    LogMsgEx(fpLog, TIME_STAMP, _T("Can not get data...\n")); 
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    break;
                }
                else{
                    break;
                }
            }
#ifdef CLEANUP_KNOW_ERROR
            //bug:When the first column is empty, no data is returned, and then leads to core dump
            if((minLen == 0) && (maxLen == 0)){
                break;
            }
#endif
            totalRows++;
            retcode = SQLFetch(hstmt);
        }
        gettimeofday(&tv2,NULL);
        totalTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        LogMsgEx(fpLog, TIME_STAMP, _T("Summary select: execute times:%d (ms) select time:%d (ms) times/rows:%d (ms/row) crc time:%d (ms) total rows:%d cols:%d min:%d btyes max:%d bytes\n"), 
                    totalTimes,
                    totalTimes - crcTimes,
                    (totalRows > 0) ? ((totalTimes - crcTimes) / totalRows) : (totalTimes - crcTimes),
                    crcTimes,
                    totalRows,
                    cols,
                    minLen,
                    maxLen); 
        SQLFreeStmt(hstmt, SQL_UNBIND);
        SQLFreeStmt(hstmt, SQL_CLOSE);
    }
    if(pData) free(pData);
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}
int CTesSQL::testSQL(TestInfo *pTestInfo, sSqlStmt *psSqlStmt, sTestTableInfo *psTestTableInfo)
{
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    RETCODE retcode;
    TestInfo sTestInfo;
    struct timeval tv1, tv2, tvStart;
    SQLSMALLINT cols, i, rows;
    SQLLEN len;
    int ret;
    TCHAR szBuf[CONFIG_TEST_RESULT_SIZE + 2] = {0};
   
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
  
    gettimeofday(&tvStart,NULL);
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pGlobalCfg->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pGlobalCfg->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    SQLExecDirect(hstmt, (SQLTCHAR*)_T("set FETCHSIZE 1"), SQL_NTS);
    LogMsgEx(fpLog, NONE, _T(".........................................\n"));
    addInfoSessionEx(hstmt, fpLog);
    LogMsgEx(fpLog, NONE, _T(".........................................\n"));
    ret = 0;
    gettimeofday(&tv1,NULL);
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(psSqlStmt->szStmtCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), psSqlStmt->szStmtCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)psSqlStmt->szStmtCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    LogMsgEx(fpLog, NONE, _T("sql stmt:%s\n"), psSqlStmt->szSql);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)psSqlStmt->szSql, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogMsgEx(fpLog, NONE,_T("SQLExecDirect fail, expect %d    actual:%d\n"), psSqlStmt->retcode, retcode);
        if(retcode != psSqlStmt->retcode){
           if(psSqlStmt->isIgnoreFail == TRUE){
                LogMsgEx(fpLog, NONE, "Ignore the error.\n");
           }
           else{
                ret = -1;
           }
        }
        else if(retcode == psSqlStmt->retcode){
            LogMsgEx(fpLog, NONE, "Expected error and ignore the error.\n");
        }
        else{
            ret = -1;
        }
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
    }
    else if(psSqlStmt->isCheckResult){
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect sucess.\n"));
        cols = 0;
        retcode = SQLFetch(hstmt);
        retcode = SQLNumResultCols(hstmt, &cols);
        ret = (retcode != SQL_SUCCESS) ? (-1) : 0;
        if((psSqlStmt->isResultNull == TRUE) && ((cols == 0) || (retcode != SQL_SUCCESS))){
            ret = 0;
            LogMsgEx(fpLog, NONE, "The expected result set is null.\n");
        }
        rows = 0;
        if(retcode != SQL_SUCCESS){
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        }
        while (retcode == SQL_SUCCESS)
        {
            LogMsgEx(fpLog, NONE, "Check the return value of row %d\n", rows + 1);
            LogMsgEx(fpLog, NONE, "-------------\n");
            for (i = 1; i <= cols; i++)
            {
                if(i > CONFIG_TEST_NUM_RESULT) break;
                memset(szBuf, 0, sizeof(szBuf));
                retcode = SQLGetData(hstmt, i, SQL_C_CHAR, (SQLPOINTER)szBuf, sizeof(szBuf) - 1, &len);
                if (SQL_SUCCEEDED(retcode)){
                    LogMsgEx(fpLog, NONE, _T("columns:%d expect:%s    actual:%s\n"), i, psSqlStmt->mResult[rows].szResult[i-1], szBuf);
                    if((psSqlStmt->mResult[rows].numOfResult > 0) && (i <= psSqlStmt->mResult[rows].numOfResult)){
                        if(_tcsncmp(psSqlStmt->mResult[rows].szResult[i-1], szBuf, _tcslen(psSqlStmt->mResult[rows].szResult[i-1])) != 0){
                            ret = -1;
                            LogMsgEx(fpLog, NONE,_T("Does not match the expected value\n"));
                        }
                    }
                    else{
                        LogMsgEx(fpLog, NONE, "Ignore the return value...\n");
                    }
                }
                else if(i == 1){
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    if(psSqlStmt->isResultNull == TRUE){
                        LogMsgEx(fpLog, NONE, "The expected result set is null.Ignore the error.\n");
                    }
                    else{
                        ret = -1;
                        LogMsgEx(fpLog, TIME_STAMP, _T("Can not get data...\n"));
                    }
                    break;
                }
                else{
                    break;
                }
            }
            LogMsgEx(fpLog, NONE, "-------------\n");
            rows++;
            retcode = SQLFetch(hstmt);
            if(rows >= CONFIG_TEST_RESULT_ROWS) break;
        }
    }
    else{
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect sucess.\n"));
    }

    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("Test complete. Summary: test %s   time:%d ms\n"), 
                (ret == 0) ? "pass" : "fail",
                (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));
    
    
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return 0;
}
int CTesSQL::doStmt(TestInfo *pTestInfo)
{
    struct timeval tv1, tv2;
    CConfig *pConfig = new CConfig;
    sTestTableInfo *psTestTableInfo = NULL;
    sTestConfig *pTestConfig = &gTestGlobal.mcfg;
    int totalPass = 0;
    int totalFail = 0;
    TCHAR szBuf[256] = {0};
    sODBCTestData *psODBCTestData = NULL;
    sSqlStmt *psStmtInfo = NULL;
    int loop;

    if(pConfig == NULL) return -1;

    psODBCTestData = pConfig->getTestData();
    psStmtInfo = &psODBCTestData->mStmtInfo;
    gettimeofday(&tv1,NULL);

    while(pConfig->scanTestTable() == 0){   
        if(psODBCTestData->isAvailableStmt == FALSE){
            continue;
        }
        LogMsgEx(fpLog, NONE,_T("\n"));
        LogMsgEx(fpLog, NONE,_T("----------------------------------------------------------------\n"));
        LogMsgEx(fpLog, TIME_STAMP, _T("begin stmt test......\n"));
        LogMsgEx(fpLog, NONE, _T("...............\n"));
        LogMsgEx(fpLog, NONE, _T("Test info:\n"));
        LogMsgEx(fpLog, NONE,_T("%s\n"), psStmtInfo->szSql);
        LogMsgEx(fpLog, NONE,_T("expect return:%d\n"), psStmtInfo->retcode);
        LogMsgEx(fpLog, NONE,_T("ignore erro:%s\n"), (psStmtInfo->isIgnoreFail == TRUE) ? _T("TRUE") : _T("FALSE"));
        LogMsgEx(fpLog, NONE,_T("checkout result:%s\n"), (psStmtInfo->isCheckResult == TRUE) ? _T("TRUE") : _T("FALSE"));
        LogMsgEx(fpLog, NONE,_T("result null:%s\n"), (psStmtInfo->isResultNull == TRUE) ? _T("TRUE") : _T("FALSE"));
        LogMsgEx(fpLog, NONE,_T("test times:%d\n"), psStmtInfo->testTimes);
        LogMsgEx(fpLog, NONE, _T("...............\n"));
        for(loop = 0; loop < psStmtInfo->testTimes; loop++){
#ifdef unixcli
            sleep(psStmtInfo->interval);
#else
            Sleep(psStmtInfo->interval * 1000);
#endif
            if(testSQL(pTestInfo, psStmtInfo, &psODBCTestData->mTableInfo) == 0){
                totalPass++;
            }
            else{
                totalFail++;
            }
        }
    };
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("Statement test complete. Summary: test %s   time:%d ms\n"), 
                    (totalFail == 0) ? "pass" : "fail",
                    (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));
    delete pConfig;

    return (totalFail == 0) ? 0 : (-1);
}

int CTesSQL::doSqlFile(TestInfo *pTestInfo)
{
    struct timeval tv1, tv2;
    CConfig *pConfig = new CConfig;
    sODBCTestData *psODBCTestData = NULL;
    sSqlStmt *psSqlFileInfo = NULL;
    int totalPass = 0;
    int totalFail = 0;
    TCHAR szBuf[256] = {0};
    TCHAR szTmp[64] = {0};

    if(pConfig == NULL) return -1;

    psODBCTestData = pConfig->getTestData();
    psSqlFileInfo = &psODBCTestData->mSqlFileInfo;
    gettimeofday(&tv1,NULL);
    while(pConfig->scanTestSqlFile() == 0){ 
        if(psODBCTestData->isAvailableSqlFile == FALSE){
            if((totalPass == 0) && (totalFail == 0)){
                LogMsgEx(fpLog, NONE, _T("No sql file information to test.\n"));
            }
            break;
        }
        LogMsgEx(fpLog, NONE,_T("\n"));
        LogMsgEx(fpLog, NONE,_T("----------------------------------------------------------------\n"));
        LogMsgEx(fpLog, TIME_STAMP, _T("begin sql test......\n"));
        LogMsgEx(fpLog, NONE, _T("...............\n"));
        LogMsgEx(fpLog, NONE, _T("Test info:\n"));
        LogMsgEx(fpLog, NONE,_T("%s\n"), psSqlFileInfo->szSql);
        LogMsgEx(fpLog, NONE,_T("expect return:%d\n"), psSqlFileInfo->retcode);
        LogMsgEx(fpLog, NONE,_T("ignore erro:%s\n"), (psSqlFileInfo->isIgnoreFail == TRUE) ? _T("TRUE") : _T("FALSE"));
        LogMsgEx(fpLog, NONE,_T("checkout result:%s\n"), (psSqlFileInfo->isCheckResult == TRUE) ? _T("TRUE") : _T("FALSE"));
        LogMsgEx(fpLog, NONE,_T("result null:%s\n"), (psSqlFileInfo->isResultNull == TRUE) ? _T("TRUE") : _T("FALSE"));
        LogMsgEx(fpLog, NONE, _T("...............\n"));
        if(testSQL(pTestInfo, psSqlFileInfo, &psODBCTestData->mTableInfo) == 0){
            totalPass++;
        }
        else{
            totalFail++;
        }
    };
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("Sql file test complete. Summary: test %s   time:%d ms\n"), 
                    (totalFail == 0) ? "pass" : "fail",
                    (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));
    delete pConfig;

    return (totalFail == 0) ? 0 : (-1);
}
int CTesSQL::doSelect(TestInfo *pTestInfo)
{
    struct timeval tv1, tv2;
    CConfig *pConfig = new CConfig;
    sTestTableInfo *psTestTableInfo = NULL;
    int totalPass = 0;
    int totalFail = 0;
    TCHAR szBuf[256] = {0};
    sODBCTestData *psODBCTestData = NULL;
    sSqlStmt *psStmtInfo = NULL;
    int loop;

    if(pConfig == NULL) return -1;

    psODBCTestData = pConfig->getTestData();
    psStmtInfo = &psODBCTestData->mSelectInfo;
    gettimeofday(&tv1,NULL);

    while(pConfig->scanTestTable() == 0){   
        if(psODBCTestData->isAvailableSelect == FALSE){
            continue;
        }
        LogMsgEx(fpLog, NONE,_T("\n"));
        LogMsgEx(fpLog, NONE,_T("----------------------------------------------------------------\n"));
        LogMsgEx(fpLog, TIME_STAMP, _T("begin select test......\n"));
        LogMsgEx(fpLog, NONE, _T("...............\n"));
        LogMsgEx(fpLog, NONE, _T("Test info:\n"));
        LogMsgEx(fpLog, NONE,_T("%s\n"), psStmtInfo->szSql);
        LogMsgEx(fpLog, NONE,_T("test times:%d\n"), psStmtInfo->testTimes);
        LogMsgEx(fpLog, NONE, _T("...............\n"));
        for(loop = 0; loop < psStmtInfo->testTimes; loop++){
#ifdef unixcli
            sleep(psStmtInfo->interval);
#else
            Sleep(psStmtInfo->interval * 1000);
#endif
            if(testSelect(pTestInfo, 
                      psStmtInfo, 
                      &psODBCTestData->mTableInfo
                      ) == 0){
                totalPass++;
            }
            else{
                totalFail++;
            }
        }
    };
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("Select test complete. Summary: test %s   time:%d ms\n"), 
                    (totalFail == 0) ? "pass" : "fail",
                    (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));
    delete pConfig;

    return (totalFail == 0) ? 0 : (-1);
}
int CTesSQL::doCleanupTable(TestInfo *pTestInfo)
{
    CConfig *pConfig = new CConfig;
    sODBCTestData *psODBCTestData = NULL;
    sTestTableInfo *psTestTableInfo = NULL;
    int totalPass, totalFail;
    int ret = 0;

    LogMsgEx(fpLog, SHORTTIMESTAMP, _T("Begin cleanup table test...\n"));
    if(pConfig == NULL){
        LogMsgEx(fpLog, NONE, _T("Failed to create configuration object.\n"));
        return -1;
    }
    psODBCTestData = pConfig->getTestData();
    psTestTableInfo = &psODBCTestData->mTableInfo;
    
    totalPass = 0;
    totalFail = 0;
    while(pConfig->scanTestTable() == 0){   
        if(psODBCTestData->isAvailableTable == FALSE){
            continue;
        } 
        ret = cleanupTableData(pTestInfo, psTestTableInfo->szTableName, fpLog);
        if(ret == -1){
            totalFail++;
            TEST_FAILED;
        }
        else if(ret == 0){
            totalPass++;
        }
    };
    LogMsgEx(fpLog, NONE, _T("\nSummary: pass cases:%d     fail cases:%d\n"), totalPass, totalFail);
    delete pConfig;
    
    return (totalFail == 0) ? 0 : (-1);
}
int CTesSQL::doSqlTest(TestInfo *pTestInfo, char *pCase)
{
    struct timeval tv1, tv2;
    int ret = -1;

    gettimeofday(&tv1,NULL);
    if(_tcscmp(_T("testStmt"), pCase) == 0){
        ret = doStmt(pTestInfo);
    }
    else if(_tcscmp(_T("testSqlFile"), pCase) == 0){
        ret = doSqlFile(pTestInfo);
    }
    else if(_tcscmp(_T("testSelect"), pCase) == 0){
        ret = doSelect(pTestInfo);
    }
    else if(_tcscmp(_T("cleanupTable"), pCase) == 0){
        ret = doCleanupTable(pTestInfo);
    }
    else{
        ret = -1;
    }
    
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("SQL test complete. Summary: test %s   time:%d ms\n\n"), 
                    (ret == 0) ? "pass" : "fail",
                    (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000));

    return 0;
}

