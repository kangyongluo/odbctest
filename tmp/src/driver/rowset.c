#include "odbcdriver.h"
#include "../util/util.h"


int rowset(TestInfo *pTestInfo, 
                      sTestTableInfo *psTestTableInfo,
                      sLoadDataInfo *pLoadInfo, 
                      TCHAR *pConnStrEx,
                      FILE *fpLog)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLUSMALLINT sParamStatus[6] = {0};
    SQLINTEGER iParamsProcessed = 0;
    SQLLEN RowsFetched = 0;
    SQLLEN nBindOffset = 0;
    int i, numBatch, numCol;
    int beginVal = 0;
    TCHAR szBuf[256] = {0};
    int totalRows = 0;
    int totalInsert;
    SQLLEN len;
    int ret = 0;
    TestInfo sTestInfo;
    struct timeval tv1, tv2;
    int exeTimes = 0;

    int structSize = 0;
    sOffsetRowsetInfo mOffset;

    mOffset.data[0] = 0;
    for(i = 0; i < psTestTableInfo->columNum; i++)
    {
        if(psTestTableInfo->sqlType[i] == SQL_CHAR ||
            psTestTableInfo->sqlType[i] == SQL_VARCHAR ||
            psTestTableInfo->sqlType[i] == SQL_LONGVARCHAR){
            structSize += (psTestTableInfo->columnSize[i] + psTestTableInfo->columnSize[i] % 4 + 4);
        }
        else{
            structSize += 64;
        }
        mOffset.data[i + 1] = structSize;
    }
    for(i = 0; i < psTestTableInfo->columNum; i++)
    {
        mOffset.len[i] = structSize + (i * sizeof(SQLLEN)) + 4;
    }
    structSize += psTestTableInfo->columNum * sizeof(SQLLEN) + 4;
    char *pRowsetPtr = (char *)malloc(structSize * pLoadInfo->batchSize);

    if(pRowsetPtr == NULL){
        LogMsgEx(fpLog, NONE, _T("Prepare rowset data fail...\n"));
        return -1;
    }
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare rowset...\n"));
    LogMsgEx(fpLog, TIME_STAMP, _T("type:%s begin value:%d batch:%d num of batch:%d\n"),
                                    pLoadInfo->szInsertType,
                                    pLoadInfo->beginVal,
                                    pLoadInfo->batchSize,
                                    pLoadInfo->totalBatch
                                    );
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pConnStrEx);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pConnStrEx)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        if(pRowsetPtr){
            free(pRowsetPtr);
        }
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
            if(pRowsetPtr){
                free(pRowsetPtr);
            }
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadInfo->szLoadCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadInfo->szLoadCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadInfo->szLoadCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    _stprintf(szBuf, _T("%s INTO %s VALUES("), pLoadInfo->szInsertType, psTestTableInfo->szTableName);
    for(i = 0; i < psTestTableInfo->columNum; i++)
    {
        _tcscat(szBuf, _T("?"));
        if(i < (psTestTableInfo->columNum - 1))
        {
            _tcscat(szBuf, _T(","));
        }
    }
    _tcscat(szBuf, _T(")"));
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    retcode = SQLPrepare(hstmt, (SQLTCHAR *)szBuf , SQL_NTS);
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, _T("SQLPrepare  fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER) structSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)pLoadInfo->batchSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR, sParamStatus, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &iParamsProcessed, 0);
   
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, _T("SQLSetStmtAttr  fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }

    for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
        if(numCol >= CONFIG_SQL_COLUMNS_MAX){
            LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
            ret = -1;
            goto TEST_ROW_SET_FAIL;
        }
        if(psTestTableInfo->dataWidth[numCol][0] == -1){
            *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])) = SQL_NULL_DATA;
        }
        else{
            *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])) = SQL_NTS;
        }
        retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                SQL_PARAM_INPUT, 
                                SQL_C_TCHAR, 
                                psTestTableInfo->sqlType[numCol], 
                                psTestTableInfo->columnSize[numCol], 
                                0, 
                                (SQLPOINTER )&pRowsetPtr[mOffset.data[numCol]], 
                                psTestTableInfo->columnSize[numCol], 
                                ((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])));
        if (!SQL_SUCCEEDED(retcode))
        {
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail.\n"), numCol + 1);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto TEST_ROW_SET_FAIL;
        }
    }
    LogMsgEx(fpLog, NONE, _T("Start rowset...\n"));
    totalInsert = 0;
    for(i = 0; i < pLoadInfo->totalBatch; i++)
    {
        memset(pRowsetPtr, 0, structSize * pLoadInfo->batchSize);
        for(numBatch = 0; numBatch < pLoadInfo->batchSize; numBatch++){
            beginVal = pLoadInfo->beginVal + (i * pLoadInfo->batchSize) + numBatch;
            for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
                *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol] + numBatch * structSize])) = SQL_NTS;
                setSqlData(&pRowsetPtr[mOffset.data[numCol] + numBatch * structSize],
                                psTestTableInfo->dataWidth[numCol][0],
                                beginVal,
                                psTestTableInfo->sqlType[numCol],
                                psTestTableInfo->dataWidth[numCol][0],
                                (SQLSMALLINT)psTestTableInfo->dataWidth[numCol][1]);
            }
        }
        LogMsgEx(fpLog, NONE, _T("Loading %d...\n"), i + 1);
        gettimeofday(&tv1,NULL);
        retcode = SQLExecute(hstmt);
        if (retcode != SQL_SUCCESS)
        {
            LogMsgEx(fpLog, NONE, _T("SQLExecute fail.line=%d\n"), __LINE__);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            break;
        }
        gettimeofday(&tv2,NULL);
        exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        LogMsgEx(fpLog, NONE, _T("SQLExecute rowset: insert:%s table:%s %d ms batch:%d num:%d\n"), 
                                                                            pLoadInfo->szInsertType,
                                                                            psTestTableInfo->szTableName,
                                                                            exeTimes, 
                                                                            pLoadInfo->batchSize, 
                                                                            i);
        totalInsert += pLoadInfo->batchSize;
    }
    LogMsgEx(fpLog, NONE, _T("Total insert rows %d\n"), totalInsert);
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);
   
    LogMsgEx(fpLog, NONE, _T("check total insert\n"));
    totalRows = 0;
    retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &totalRows, sizeof(totalRows), &len);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLBindCol fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }
    _stprintf(szBuf, _T("SELECT COUNT(*) FROM %s\n"), psTestTableInfo->szTableName);
    LogMsgEx(fpLog, NONE,_T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }
    SQLFetch(hstmt);
    
    if(totalRows < (pLoadInfo->batchSize * pLoadInfo->totalBatch))
    {
        ret = -1;
    }
    LogMsgEx(fpLog, NONE, _T("%s: expect rows:%d actual rows:%d\n"), 
                                                        (ret == 0) ? _T("Test pass") : _T("Test fail"), 
                                                        (pLoadInfo->batchSize * pLoadInfo->totalBatch), 
                                                        totalRows);
TEST_ROW_SET_FAIL:
    FullDisconnectEx(fpLog, &sTestInfo);
    
    if(pRowsetPtr){
        free(pRowsetPtr);
    }
    
    return ret;
}
int rowsetEx(TestInfo *pTestInfo, 
                      sTestTableInfo *psTestTableInfo,
                      sLoadDataInfo *pLoadInfo, 
                      TCHAR *pConnStrEx,
                      FILE *fpLog)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLUSMALLINT sParamStatus[6] = {0};
    SQLINTEGER iParamsProcessed = 0;
    SQLLEN RowsFetched = 0;
    SQLLEN nBindOffset = 0;
    int i, numBatch, numCol;
    int beginVal = 0;
    TCHAR szBuf[256] = {0};
    int totalRows = 0;
    int totalInsert;
    SQLLEN len;
    int ret = 0;
    TestInfo sTestInfo;
    struct timeval tv1, tv2;
    int exeTimes = 0;
    
    int structSize = 0;
    sOffsetRowsetInfo mOffset;
    BOOL isBlobCol = FALSE;
    int lobSize = 0;
    FILE *fpBlob = NULL;
    int cols, nbytes;

    lobSize = 0;
    if((psTestTableInfo->isBlobTable == TRUE) && 
        (strlen(pLoadInfo->szBlobFile) > 0)){
        fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
        if(fpBlob){
            fseek(fpBlob, 0, SEEK_END);
            lobSize = ftell(fpBlob);
            fseek(fpBlob, 0, SEEK_SET);
            fclose(fpBlob);
            fpBlob = NULL;
        }
    }
    
    mOffset.data[0] = 0;
    for(i = 0; i < psTestTableInfo->columNum; i++)
    {
        if(psTestTableInfo->sqlType[i] == SQL_CHAR ||
            psTestTableInfo->sqlType[i] == SQL_VARCHAR ||
            psTestTableInfo->sqlType[i] == SQL_LONGVARCHAR){
            structSize += (psTestTableInfo->columnSize[i] + psTestTableInfo->columnSize[i] % 4 + 12);
        }
        else{
            structSize += 64;
        }
        mOffset.data[i + 1] = structSize;
    }
    for(i = 0; i < psTestTableInfo->columNum; i++)
    {
        mOffset.len[i] = structSize + (i * sizeof(SQLLEN)) + 4;
    }
    structSize += psTestTableInfo->columNum * sizeof(SQLLEN) + 4;
    char *pRowsetPtr = (char *)malloc(structSize * pLoadInfo->batchSize);

    if(pRowsetPtr == NULL){
        LogMsgEx(fpLog, NONE, _T("Prepare rowset data fail...\n"));
        return -1;
    }
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare rowset...\n"));
    LogMsgEx(fpLog, TIME_STAMP, _T("type:%s begin value:%d batch:%d num of batch:%d\n"),
                                    pLoadInfo->szInsertType,
                                    pLoadInfo->beginVal,
                                    pLoadInfo->batchSize,
                                    pLoadInfo->totalBatch
                                    );
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pConnStrEx);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pConnStrEx)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        if(pRowsetPtr){
            free(pRowsetPtr);
        }
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
            if(pRowsetPtr){
                free(pRowsetPtr);
            }
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadInfo->szLoadCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadInfo->szLoadCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadInfo->szLoadCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    _stprintf(szBuf, _T("%s INTO %s VALUES("), pLoadInfo->szInsertType, psTestTableInfo->szTableName);
    for(i = 0; i < psTestTableInfo->columNum; i++)
    {
        _tcscat(szBuf, _T("?"));
        if(i < (psTestTableInfo->columNum - 1))
        {
            _tcscat(szBuf, _T(","));
        }
    }
    _tcscat(szBuf, _T(")"));
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    retcode = SQLPrepare(hstmt, (SQLTCHAR *)szBuf , SQL_NTS);
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, _T("SQLPrepare  fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER) structSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)pLoadInfo->batchSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR, sParamStatus, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &iParamsProcessed, 0);
   
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, _T("SQLSetStmtAttr  fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }

    for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
        if(numCol >= CONFIG_SQL_COLUMNS_MAX){
            LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
            ret = -1;
            goto TEST_ROW_SET_FAIL;
        }
        isBlobCol = FALSE;
        for(i = 0; i < psTestTableInfo->numBlobCols; i++){
            if(numCol == psTestTableInfo->idBlobCols[i]){
                isBlobCol = (psTestTableInfo->isBlobTable == TRUE) ? TRUE : FALSE;
                break;
            }
        }
        if(psTestTableInfo->dataWidth[numCol][0] == -1){
            *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])) = SQL_NULL_DATA;
        }
        else if((isBlobCol) && (lobSize > 0)){
            if(lobSize > psTestTableInfo->columnSize[numCol]){
                lobSize = psTestTableInfo->columnSize[numCol];
            }
            *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])) = lobSize;
        }
        else{
            *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])) = SQL_NTS;
        }
        retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                SQL_PARAM_INPUT, 
                                SQL_C_TCHAR, 
                                psTestTableInfo->sqlType[numCol], 
                                psTestTableInfo->columnSize[numCol], 
                                0, 
                                (SQLPOINTER )&pRowsetPtr[mOffset.data[numCol]], 
                                psTestTableInfo->columnSize[numCol], 
                                ((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol]])));
        if (!SQL_SUCCEEDED(retcode))
        {
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail.\n"), numCol + 1);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto TEST_ROW_SET_FAIL;
        }
    }
    LogMsgEx(fpLog, NONE, _T("Start rowset...\n"));
    totalInsert = 0;
    for(i = 0; i < pLoadInfo->totalBatch; i++)
    {
        memset(pRowsetPtr, 0, structSize * pLoadInfo->batchSize);
        for(numBatch = 0; numBatch < pLoadInfo->batchSize; numBatch++){
            beginVal = pLoadInfo->beginVal + (i * pLoadInfo->batchSize) + numBatch;
            for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
                isBlobCol = FALSE;
                for(cols = 0; cols < psTestTableInfo->numBlobCols; cols++){
                    if(numCol == psTestTableInfo->idBlobCols[cols]){
                        isBlobCol = (psTestTableInfo->isBlobTable == TRUE) ? TRUE : FALSE;
                        break;
                    }
                }
                if(psTestTableInfo->dataWidth[numCol][0] == -1){
                    *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol] + numBatch * structSize])) = SQL_NULL_DATA;
                    continue;
                }
                else if((isBlobCol == TRUE) && (lobSize > 0)){
                    *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol] + numBatch * structSize])) = SQL_NULL_DATA;
                    fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
                    if(fpBlob){
                        nbytes = fread(&pRowsetPtr[mOffset.data[numCol] + numBatch * structSize], 
                                        1, 
                                        lobSize, 
                                        fpBlob);
                        *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol] + numBatch * structSize])) = nbytes;
                        fclose(fpBlob);
                        fpBlob = NULL;
                    }
                    LogMsgEx(fpLog, NONE, _T("Read %d bytes from %s\n"), nbytes, pLoadInfo->szBlobFile);
                }
                else{
                    *((SQLLEN *)(&pRowsetPtr[mOffset.len[numCol] + numBatch * structSize])) = SQL_NTS;
                    setSqlData(&pRowsetPtr[mOffset.data[numCol] + numBatch * structSize],
                                    psTestTableInfo->dataWidth[numCol][0],
                                    beginVal,
                                    psTestTableInfo->sqlType[numCol],
                                    psTestTableInfo->dataWidth[numCol][0],
                                    (SQLSMALLINT)psTestTableInfo->dataWidth[numCol][1]);
                }
            }
        }
        LogMsgEx(fpLog, NONE, _T("Loading %d...\n"), i + 1);
        gettimeofday(&tv1,NULL);
        retcode = SQLExecute(hstmt);
        if (retcode != SQL_SUCCESS)
        {
            LogMsgEx(fpLog, NONE, _T("SQLExecute fail.line=%d\n"), __LINE__);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            break;
        }
        gettimeofday(&tv2,NULL);
        exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        LogMsgEx(fpLog, NONE, _T("SQLExecute rowset: insert:%s table:%s %d ms batch:%d num:%d\n"), 
                                                                            pLoadInfo->szInsertType,
                                                                            psTestTableInfo->szTableName,
                                                                            exeTimes, 
                                                                            pLoadInfo->batchSize, 
                                                                            i);
        totalInsert += pLoadInfo->batchSize;
    }
    LogMsgEx(fpLog, NONE, _T("Total insert rows %d\n"), totalInsert);
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);
   
    LogMsgEx(fpLog, NONE, _T("check total insert\n"));
    totalRows = 0;
    retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &totalRows, sizeof(totalRows), &len);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLBindCol fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }
    _stprintf(szBuf, _T("SELECT COUNT(*) FROM %s\n"), psTestTableInfo->szTableName);
    LogMsgEx(fpLog, NONE,_T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_ROW_SET_FAIL;
    }
    SQLFetch(hstmt);
    
    if(totalRows < (pLoadInfo->batchSize * pLoadInfo->totalBatch))
    {
        ret = -1;
    }
    LogMsgEx(fpLog, NONE, _T("%s: expect rows:%d actual rows:%d\n"), 
                                                        (ret == 0) ? _T("Test pass") : _T("Test fail"), 
                                                        (pLoadInfo->batchSize * pLoadInfo->totalBatch), 
                                                        totalRows);
TEST_ROW_SET_FAIL:
    FullDisconnectEx(fpLog, &sTestInfo);
    
    if(pRowsetPtr){
        free(pRowsetPtr);
    }
    
    return ret;
}


