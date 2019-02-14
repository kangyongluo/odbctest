#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"


int SQLPutDataSQLLenDataAtExec(TestInfo *pTestInfo, 
                                      sTestTableInfo *psTestTableInfo,
                                      sLoadDataInfo *pLoadInfo,
                                      TCHAR *pConnStrEx,
                                      FILE *fpLog)
{
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TCHAR szBuf[512] = {0};
    TestInfo sTestInfo;
    int rows = 0;
    int rowVal = pLoadInfo->beginVal;
    int cols;
    int dataSize = 0;
    int lobSize = 0;
    int maxLobSize = 0;
    FILE *fpBlob = NULL;
    PTR pToken;
    unsigned int crc = 0;
    TCHAR szCrc[10] = {0};
    int tmplen;
    int ret = 0;
    int structSize = 0;
    sOffsetRowsetInfo mOffset;
    SQLINTEGER i;
    struct timeval tv1, tv2, tv3, tv4;
    int exeTimes = 0;
    char szPutBuf[512];
    int offset = 0, dataBatch = 0;
    BOOL isBlobCol = FALSE;
    int calcCrcTime = 0;

    int lenLob = 0;

    unsigned char *pLoadPara = (unsigned char *)malloc(CONFIG_BLOB_SIZE + 4);
    if(pLoadPara ==  NULL){
        LogMsgEx(fpLog, NONE, _T("Malloc load buffer fail...\n"));
        return -1;
    }

    memset(pLoadPara, 0, CONFIG_BLOB_SIZE + 2);
    if((psTestTableInfo->isBlobTable == TRUE) && 
        (strlen(pLoadInfo->szBlobFile) > 0) && 
        (pLoadInfo->isloadCrc)){
        lobSize = 0;
        fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
        if(fpBlob){
            fseek(fpBlob, 0, SEEK_END);
            lobSize = ftell(fpBlob);
            fseek(fpBlob, 0, SEEK_SET);
        }
        if(lobSize >= CONFIG_BLOB_SIZE){
            LogMsgEx(fpLog, NONE, _T("The lob file is too large, ignoring the CRC check.\n"));
            memset(szCrc, 0, sizeof(szCrc));
        }
        else if(lobSize > 0){
            tmplen = fread(pLoadPara, 1, lobSize, fpBlob);
            crc = calcCRC32((unsigned char *)&pLoadPara[8], lobSize);
            hexToAscii((unsigned char *)&crc, (unsigned char *)szCrc, sizeof(crc));
            lenLob = lobSize + 8;
            
        }
        else{
            memset(szCrc, 0, sizeof(szCrc));
        }
        if(fpBlob){
            fclose(fpBlob);
            fpBlob = NULL;
        }
    }
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    _stprintf(szPutBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pConnStrEx);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szPutBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pConnStrEx)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        if(pLoadPara) free(pLoadPara);
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
            if(pLoadPara) free(pLoadPara);
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
    LogMsgEx(fpLog, NONE, _T("SQLPrepare -- > SQLBindParameter --> SQLPutData. type:%s\n"),
              _T("SQL_LEN_DATA_AT_EXEC"));
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
    retcode = SQLPrepare(hstmt, (SQLCHAR *)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsgEx(fpLog, NONE, _T("SQLPrepare fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        FullDisconnectEx(fpLog, &sTestInfo);
        if(pLoadPara) free(pLoadPara);
        return -1;
    }
    ret = 0;
    for(cols = 0; cols < psTestTableInfo->columNum; cols++){
        if(psTestTableInfo->sqlType[cols] == SQL_CHAR ||
            psTestTableInfo->sqlType[cols] == SQL_VARCHAR ||
            psTestTableInfo->sqlType[cols] == SQL_LONGVARCHAR ||
            psTestTableInfo->sqlType[cols] == SQL_BINARY){
            mOffset.lenDataSize[cols] = psTestTableInfo->dataWidth[cols][0];
            if(mOffset.lenDataSize[cols] < 0) mOffset.lenDataSize[cols] = 0;
        }
        else{
            mOffset.lenDataSize[cols] = 256;
        }
        mOffset.lenTextSize[cols] = SQL_LEN_DATA_AT_EXEC(mOffset.lenDataSize[cols]);
        retcode = SQLBindParameter(hstmt,
                                    cols + 1,
                                    SQL_PARAM_INPUT, 
                                    SQL_C_TCHAR, 
                                    psTestTableInfo->sqlType[cols], 
                                    mOffset.lenDataSize[cols],
                                    0,
                                    (void *)pLoadPara,
                                    0,
                                    &(mOffset.lenTextSize[cols])
                                    );
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail:line = %d\n"), cols + 1, __LINE__);
            break;
        }
        LogMsgEx(fpLog, NONE, _T("SQLBindParameter:Bytes Remaining : %i, "), (int)mOffset.lenDataSize[cols]);
        LogMsgEx(fpLog, NONE, _T("SQLBindParameter:LenOrIndArray : %i\n"), (int)mOffset.lenTextSize[cols]);
    }    
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        for(rows = 0; rows < (pLoadInfo->batchSize * pLoadInfo->totalBatch); rows++,rowVal++){
            calcCrcTime = 0;
            gettimeofday(&tv1,NULL);
            memset(pLoadPara, 0, CONFIG_BLOB_SIZE);
            retcode = SQLExecute(hstmt);
            if (retcode != SQL_NEED_DATA){
                LogMsgEx(fpLog, NONE, _T("Call SQLExecute fail:rows:%d id:%d line = %d\n"), rows, rowVal, __LINE__);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            retcode = SQLParamData(hstmt,&pToken);
            if (retcode != SQL_NEED_DATA){
                LogMsgEx(fpLog, NONE, _T("Call SQLParamData fail:rows:%d id:%d line = %d\n"), rows, rowVal, __LINE__);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            if((rows < 3000) || ((rows > 3000) && (rows % 1000 == 0))){
                LogMsgEx(fpLog, NONE, _T("insert row %d\n"), rows + 1);
            }
            for(cols = 0; cols < psTestTableInfo->columNum; cols++){
                isBlobCol = FALSE;
                gettimeofday(&tv3,NULL);
                for(i = 0; i < psTestTableInfo->numBlobCols; i++){
                    if(cols == psTestTableInfo->idBlobCols[i]){
                        isBlobCol = (psTestTableInfo->isBlobTable == TRUE) ? TRUE : FALSE;
                        break;
                    }
                }
                if((isBlobCol == FALSE) && (psTestTableInfo->dataWidth[cols][0] >= 0)){
                    setSqlData((TCHAR *)pLoadPara,
                                psTestTableInfo->dataWidth[cols][0],
                                rowVal,
                                psTestTableInfo->sqlType[cols],
                                psTestTableInfo->dataWidth[cols][0],
                                (SQLSMALLINT)psTestTableInfo->dataWidth[cols][1]);
                    LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NTS); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, _tcslen((char *)pLoadPara));
                    retcode = SQLPutData(hstmt, pLoadPara, SQL_NTS);
                }
                else if(psTestTableInfo->dataWidth[cols][0] == -1){
                    LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NULL_DATA); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, -1);
                    retcode = SQLPutData(hstmt, pLoadPara, SQL_NULL_DATA);
                }
                else if(isBlobCol){
                    LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., %d); ---> rows:%d col:%d\n"), lobSize, rows, cols + 1);
                    gettimeofday(&tv4,NULL);
                    calcCrcTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                    gettimeofday(&tv3,NULL);
                    fpBlob = NULL;
                    lobSize = 0;
                    if(strlen(pLoadInfo->szBlobFile) > 0){
                        fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
                        if(fpBlob){
                            fseek(fpBlob, 0, SEEK_END);
                            lobSize = ftell(fpBlob);
                            fseek(fpBlob, 0, SEEK_SET);
                            LogMsgEx(fpLog, NONE, _T("Lob file:%s size:%d bytes\n"), pLoadInfo->szBlobFile, lobSize);
                        }
                    }
                    if(lobSize == 0) lobSize = psTestTableInfo->dataWidth[cols][0];
                    if(lobSize == -1){
                        gettimeofday(&tv4,NULL);
                        LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NULL_DATA); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, -1);
                        calcCrcTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                        retcode = SQLPutData(hstmt, pLoadPara, SQL_NULL_DATA);
                        lobSize = 0;
                    }
                    for(offset = 0; offset < lobSize; ){
                        dataBatch = lobSize - offset;
                        if(dataBatch >= CONFIG_BLOB_SIZE) dataBatch = CONFIG_BLOB_SIZE;
                        if(dataBatch > pLoadInfo->putDataBatch) dataBatch = pLoadInfo->putDataBatch;
                        if(dataBatch == 0) dataBatch = 4096;
                        if(fpBlob){
                            if((offset == 0) && (strlen(szCrc) > 0)){
                                LogMsgEx(fpLog, NONE, _T("Add CRC to lob header......\n"));
                                memcpy(pLoadPara, szCrc, 8);
                                lobSize +=8;
                                if(((dataBatch + 8) < CONFIG_BLOB_SIZE) && (dataBatch < pLoadInfo->putDataBatch)){
                                    tmplen = fread(&pLoadPara[8], 1, dataBatch, fpBlob);
                                }
                                else{
                                    tmplen = fread(&pLoadPara[8], 1, dataBatch - 8, fpBlob);
                                }
                                dataBatch = tmplen + 8;
                            }
                            else{
                                tmplen = fread(&pLoadPara[offset], 1, dataBatch, fpBlob);
                                dataBatch = tmplen;
                            }
                            if(tmplen <= 0) break;
                            
                        }
                        else{
                            LogMsgEx(fpLog, NONE, _T("Generate random lob data......\n"));
                            setSqlData((TCHAR *)(&pLoadPara[offset]),
                                                dataBatch,
                                                0,
                                                SQL_CHAR,
                                                0,
                                                0);
                        }                            
                        LogMsgEx(fpLog, NONE, _T("SQL_NEED_DATA Call SQLPutData : size:%d nbytes:%d, batch:%d\n"), lobSize, offset, dataBatch);
                        gettimeofday(&tv4,NULL);
                        calcCrcTime += (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                        retcode = SQLPutData(hstmt, &pLoadPara[offset], dataBatch);
                        offset += dataBatch;
                    }
                    if(fpBlob){
                        fclose(fpBlob);
                        fpBlob = NULL;
                    }
                    LogMsgEx(fpLog, NONE, _T("Final Call - SQLPutData: size:%d nbytes:%d\n"), lobSize, offset);
                    if (retcode != SQL_SUCCESS){
                        LogMsgEx(fpLog, NONE, _T("SQLPutData for lob fail !\n"));
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        break;
                    }
                }
                else{
                    LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NULL_DATA); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, -1);
                    retcode = SQLPutData(hstmt, pLoadPara, SQL_NULL_DATA);
                }
                retcode = SQLParamData(hstmt, &pToken);
                if ( (retcode != SQL_SUCCESS) &&
                                 (retcode != SQL_SUCCESS_WITH_INFO) &&
                                 (retcode != SQL_NEED_DATA) ) {
                    LogMsgEx(fpLog, NONE, _T("SQLParamData fail !\n"));
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
                    ret = -1;
                    break;
                }

            }
            gettimeofday(&tv2,NULL);
            exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            LogMsgEx(fpLog, NONE, _T("Lob test: row:%d val:%d insert:%s table:%s times:%d (ms) batch:%d (row) read data:%d ms\n"),
                                                                                i,
                                                                                rowVal,
                                                                                pLoadInfo->szInsertType,
                                                                                psTestTableInfo->szTableName,
                                                                                exeTimes - calcCrcTime,
                                                                                1,
                                                                                calcCrcTime);
        }
    }
    LogMsgEx(fpLog, NONE, _T("complete call SQLPutData...,total rows:%d\n"), rows);
    
    FullDisconnectEx(fpLog, &sTestInfo);
    if(pLoadPara) free(pLoadPara);
    
    return ret;
}
int SQLPutDataSQLDataAtExec(TestInfo *pTestInfo, 
                                  sTestTableInfo *psTestTableInfo,
                                  sLoadDataInfo *pLoadInfo,
                                  TCHAR *pConnStrEx,
                                  FILE *fpLog)
{
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TCHAR szBuf[512] = {0};
    TestInfo sTestInfo;
    SQLLEN len;
    int rows = 0;
    int rowVal = pLoadInfo->beginVal;
    int cols;
    int dataSize = 0;
    int lobSize = 0;
    int maxLobSize = 0;
    FILE *fpBlob = NULL;
    PTR pToken;
    unsigned int crc = 0;
    TCHAR szCrc[10] = {0};
    int tmplen;
    int ret = 0;
    SQLINTEGER i;
    struct timeval tv1, tv2, tv3, tv4;
    int exeTimes = 0;
    char szPutBuf[512];
    int offset = 0, dataBatch = 0;
    BOOL isBlobCol = FALSE;
    int calcCrcTime = 0;

    unsigned char *pLoadPara = (unsigned char *)malloc(CONFIG_BLOB_SIZE + 2);
    if(pLoadPara ==  NULL){
        LogMsgEx(fpLog, NONE, _T("Malloc load buffer fail...\n"));
        return -1;
    }

    memset(pLoadPara, 0, CONFIG_BLOB_SIZE + 2);
    if((psTestTableInfo->isBlobTable == TRUE) && 
        (strlen(pLoadInfo->szBlobFile) > 0) && 
        (pLoadInfo->isloadCrc)){
        lobSize = 0;
        fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
        if(fpBlob){
            fseek(fpBlob, 0, SEEK_END);
            lobSize = ftell(fpBlob);
            fseek(fpBlob, 0, SEEK_SET);
        }
        if(lobSize >= CONFIG_BLOB_SIZE){
            LogMsgEx(fpLog, NONE, _T("The lob file is too large, ignoring the CRC check.\n"));
            memset(szCrc, 0, sizeof(szCrc));
        }
        else if(lobSize > 0){
            tmplen = fread(pLoadPara, 1, lobSize, fpBlob);
            crc = calcCRC32((unsigned char *)&pLoadPara[8], lobSize);
            hexToAscii((unsigned char *)&crc, (unsigned char *)szCrc, sizeof(crc));
        }
        else{
            memset(szCrc, 0, sizeof(szCrc));
        }
        if(fpBlob){
            fclose(fpBlob);
            fpBlob = NULL;
        }
    }
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    _stprintf(szPutBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pConnStrEx);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szPutBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pConnStrEx)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        if(pLoadPara) free(pLoadPara);
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
            if(pLoadPara) free(pLoadPara);
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
    LogMsgEx(fpLog, NONE, _T("SQLPrepare -- > SQLBindParameter --> SQLPutData.  type:%s\n"),
              _T("SQL_DATA_AT_EXEC"));
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
    retcode = SQLPrepare(hstmt, (SQLCHAR *)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsgEx(fpLog, NONE, _T("SQLPrepare fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        FullDisconnectEx(fpLog, &sTestInfo);
        if(pLoadPara) free(pLoadPara);
        return -1;
    }
    ret = 0;
    for(cols = 0; cols < psTestTableInfo->columNum; cols++){
        len = SQL_DATA_AT_EXEC;
        retcode = SQLBindParameter(hstmt,
                                    cols + 1,
                                    SQL_PARAM_INPUT, 
                                    SQL_C_TCHAR, 
                                    psTestTableInfo->sqlType[cols], 
                                    0,
                                    0,
                                    NULL, 
                                    0, 
                                    &len
                                    );
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail:line = %d\n"), cols + 1, __LINE__);
            break;
        }
    }    
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        for(rows = 0; rows < (pLoadInfo->batchSize * pLoadInfo->totalBatch); rows++,rowVal++){
            calcCrcTime = 0;
            gettimeofday(&tv1,NULL);
            retcode = SQLExecute(hstmt);
            if (retcode != SQL_NEED_DATA){
                LogMsgEx(fpLog, NONE, _T("SQLExecute fail:rows:%d id:%d line = %d\n"), rows, rowVal, __LINE__);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            if((rows < 3000) || ((rows > 3000) && (rows % 1000 == 0))){
                LogMsgEx(fpLog, NONE, _T("insert row %d\n"), rows + 1);
            }
            for(cols = 0; cols < psTestTableInfo->columNum; cols++){
                retcode = SQLParamData(hstmt,&pToken);
                if(retcode == SQL_NEED_DATA){
                    isBlobCol = FALSE;
                    gettimeofday(&tv3,NULL);
                    for(i = 0; i < psTestTableInfo->numBlobCols; i++){
                        if(cols == psTestTableInfo->idBlobCols[i]){
                            isBlobCol = (psTestTableInfo->isBlobTable == TRUE) ? TRUE : FALSE;
                            break;
                        }
                    }
                    if(isBlobCol == FALSE){
                        if(psTestTableInfo->dataWidth[cols][0] == -1){
                            gettimeofday(&tv4,NULL);
                            calcCrcTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                            retcode = SQLPutData(hstmt, pLoadPara, SQL_NULL_DATA);
                            LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NULL_DATA); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, 0);
                        }
                        else{
                            memset(pLoadPara, 0, CONFIG_BLOB_SIZE);
                            setSqlData((TCHAR *)pLoadPara,
                                                psTestTableInfo->dataWidth[cols][0],
                                                rowVal,
                                                psTestTableInfo->sqlType[cols],
                                                psTestTableInfo->dataWidth[cols][0],
                                                (SQLSMALLINT)psTestTableInfo->dataWidth[cols][1]);
                            LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NTS); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, _tcslen((char *)pLoadPara));
                            gettimeofday(&tv4,NULL);
                            calcCrcTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                            retcode = SQLPutData(hstmt, pLoadPara, SQL_NTS);
                        }
                    }
                    else{
                        gettimeofday(&tv4,NULL);
                        calcCrcTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                        gettimeofday(&tv3,NULL);
                        fpBlob = NULL;
                        lobSize = 0;
                        if(strlen(pLoadInfo->szBlobFile) > 0){
                            fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
                            if(fpBlob){
                                fseek(fpBlob, 0, SEEK_END);
                                lobSize = ftell(fpBlob);
                                fseek(fpBlob, 0, SEEK_SET);
                            }
                        }
                        if(lobSize == 0) lobSize = psTestTableInfo->dataWidth[cols][0];
                        LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., %d); ---> rows:%d col:%d\n"), lobSize, rows, cols + 1);
                        if(lobSize == -1){
                            gettimeofday(&tv4,NULL);
                            calcCrcTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                            retcode = SQLPutData(hstmt, pLoadPara, SQL_NULL_DATA);
                            LogMsgEx(fpLog, NONE, _T("SQLPutData(hstmt, ..., SQL_NULL_DATA); ---> rows:%d col:%d len:%d\n"), rows, cols + 1, 0);
                            lobSize = 0;
                        }
                        for(offset = 0; offset < lobSize; ){
                            dataBatch = lobSize - offset;
                            if(dataBatch >= CONFIG_BLOB_SIZE) dataBatch = CONFIG_BLOB_SIZE;
                            if(dataBatch > pLoadInfo->putDataBatch) dataBatch = pLoadInfo->putDataBatch;
                            if(dataBatch == 0) dataBatch = 4096;
                            if(fpBlob){
                                if((offset == 0) && (strlen(szCrc) > 0)){
                                    LogMsgEx(fpLog, NONE, _T("Add CRC to lob header......\n"));
                                    memcpy(pLoadPara, szCrc, 8);
                                    lobSize +=8;
                                    if(((dataBatch + 8) < CONFIG_BLOB_SIZE) && (dataBatch < pLoadInfo->putDataBatch)){
                                        tmplen = fread(&pLoadPara[8], 1, dataBatch, fpBlob);
                                    }
                                    else{
                                        tmplen = fread(&pLoadPara[8], 1, dataBatch - 8, fpBlob);
                                    }
                                    dataBatch = tmplen + 8;
                                }
                                else{
                                    tmplen = fread(pLoadPara, 1, dataBatch, fpBlob);
                                    dataBatch = tmplen;
                                }
                                if(tmplen <= 0) break;
                                
                            }
                            else{
                                LogMsgEx(fpLog, NONE, _T("Generate random lob data......\n"));
                                setSqlData((TCHAR *)pLoadPara,
                                                    dataBatch,
                                                    0,
                                                    SQL_CHAR,
                                                    0,
                                                    0);
                            }                            
                            LogMsgEx(fpLog, NONE, _T("lob:offset:%d, put size:%d\n"), offset, dataBatch);
                            gettimeofday(&tv4,NULL);
                            calcCrcTime += (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                            retcode = SQLPutData(hstmt, pLoadPara, dataBatch);
                            if (retcode != SQL_SUCCESS){
                                LogMsgEx(fpLog, NONE, _T("SQLPutData for lob fail !\n"));
                                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                break;
                            }
                            offset += dataBatch;
                        }
                        if(fpBlob){
                            fclose(fpBlob);
                            fpBlob = NULL;
                        }
                    }
                    if (retcode != SQL_SUCCESS){
                        LogMsgEx(fpLog, NONE, _T("SQLPutData  fail: row:%d, columns:%d\n"), rows, cols + 1);
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        break;
                    }
                }
                else{
                    LogMsgEx(fpLog, NONE, _T("SQLParamData  fail: row:%d, columns:%d\n"), rows, cols);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    break;
                }
            }
            if(cols >= psTestTableInfo->columNum){
                retcode = SQLParamData(hstmt, &pToken);
                if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                    LogMsgEx(fpLog, NONE, _T("SQLPutData  fail: row:%d, columns:%d\n"), rows, cols);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                }
            }
            gettimeofday(&tv2,NULL);
            exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            LogMsgEx(fpLog, NONE, _T("Lob test: row:%d val:%d insert:%s table:%s times:%d (ms) batch:%d (row) read data:%d ms\n"),
                                                                                i,
                                                                                rowVal,
                                                                                pLoadInfo->szInsertType,
                                                                                psTestTableInfo->szTableName,
                                                                                exeTimes - calcCrcTime,
                                                                                1,
                                                                                calcCrcTime);
        }
    }
    LogMsgEx(fpLog, NONE, _T("complete insert...,total rows:%d\n"), rows);
    
    FullDisconnectEx(fpLog, &sTestInfo);
    if(pLoadPara) free(pLoadPara);
    
    return ret;
}
int SQLBindParameterSqlNts(TestInfo *pTestInfo, 
                                  sTestTableInfo *psTestTableInfo,
                                  sLoadDataInfo *pLoadInfo, 
                                  TCHAR *pConnStrEx,
                                  FILE *fpLog)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLINTEGER i, numCol;
    int beginVal = 0;
    TCHAR szBuf[256] = {0};
    int totalRows = 0;
    int totalInsert;
    SQLLEN len;
    int ret = 0;
    TestInfo sTestInfo;
    struct timeval tv1, tv2;
    int exeTimes = 0;
    TCHAR *pData = NULL;
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
    char *pLoadPara = (char *)malloc(structSize);

    if(pLoadPara == NULL){
        LogMsgEx(fpLog, NONE, _T("Prepare load data fail...\n"));
        return -1;
    }
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare load......\n"));
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
        if(pLoadPara) free(pLoadPara);
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
            if(pLoadPara) free(pLoadPara);
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
    LogMsgEx(fpLog, NONE, _T("SQLPrepare -- > SQLBindParameter --> SQLExecute. type:%s\n"),
              _T("SQL_NTS,SQL_NULL_DATA"));
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
        goto TEST_INSERT_FAIL;
    }
    for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
        if(numCol >= CONFIG_SQL_COLUMNS_MAX){
            LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
            ret = -1;
            goto TEST_INSERT_FAIL;
        }
        if(psTestTableInfo->dataWidth[numCol][0] == -1){
            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NULL_DATA;
        }
        else{
            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NTS;
        }
        retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                SQL_PARAM_INPUT, 
                                SQL_C_TCHAR, 
                                psTestTableInfo->sqlType[numCol], 
                                psTestTableInfo->columnSize[numCol], 
                                0, 
                                (SQLPOINTER )&pLoadPara[mOffset.data[numCol]], 
                                psTestTableInfo->columnSize[numCol], 
                                ((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])));
        if (!SQL_SUCCEEDED(retcode))
        {
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail.\n"), numCol + 1);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto TEST_INSERT_FAIL;
        }
    }
    LogMsgEx(fpLog, NONE, _T("Start insert...\n"));
    beginVal = pLoadInfo->beginVal;
    totalInsert = 0;
    for(i = 0; i < (pLoadInfo->totalBatch * pLoadInfo->batchSize); i++, beginVal++)
    {
        if((i < 3000) || ((i > 3000) && (i % 1000 == 0))){
            LogMsgEx(fpLog, NONE, _T("insert row %d\n"), i + 1);
        }
        memset(pLoadPara, 0, structSize);
        for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
            if(psTestTableInfo->dataWidth[numCol][0] == -1){
                *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NULL_DATA;
                continue;
            }
            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NTS;
            setSqlData(&pLoadPara[mOffset.data[numCol]],
                            psTestTableInfo->dataWidth[numCol][0],
                            beginVal,
                            (SQLSMALLINT)psTestTableInfo->sqlType[numCol],
                            psTestTableInfo->dataWidth[numCol][0],
                            psTestTableInfo->dataWidth[numCol][1]);
        }
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
        LogMsgEx(fpLog, NONE, _T("Load test: row:%d val:%d insert:%s table:%s %d (ms) batch:%d (row)\n"),
                                                                            i,
                                                                            beginVal,
                                                                            pLoadInfo->szInsertType,
                                                                            psTestTableInfo->szTableName,
                                                                            exeTimes, 
                                                                            1);
        totalInsert += 1;
    }
    LogMsgEx(fpLog, NONE, _T("Total insert rows %d\n"), totalInsert);
    if(totalInsert == 0){
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
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
        goto TEST_INSERT_FAIL;
    }
    _stprintf(szBuf, _T("SELECT COUNT(*) FROM %s\n"), psTestTableInfo->szTableName);
    LogMsgEx(fpLog, NONE,_T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    SQLFetch(hstmt);
    if(totalRows < (pLoadInfo->totalBatch * pLoadInfo->batchSize))
    {
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    LogMsgEx(fpLog, NONE, _T("%s: expect rows:%d actual rows:%d\n"), 
                (ret == 0) ? _T("Test pass") : _T("Test fail"),
                (pLoadInfo->totalBatch * pLoadInfo->batchSize), 
                totalRows);
TEST_INSERT_FAIL:
    FullDisconnectEx(fpLog, &sTestInfo);
    
    if(pLoadPara) free(pLoadPara);
    
    return ret;

}
int SQLBindParameterSQLLen(TestInfo *pTestInfo, 
                                  sTestTableInfo *psTestTableInfo,
                                  sLoadDataInfo *pLoadInfo, 
                                  TCHAR *pConnStrEx,
                                  FILE *fpLog)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLINTEGER i, numCol, cols;
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
    unsigned int crc = 0;
    TCHAR szCrc[10] = {0};

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
    unsigned char *pLoadPara = (unsigned char *)malloc(structSize);
    unsigned char *pData = NULL;
    int nbytes;

    if(pLoadPara == NULL){
        LogMsgEx(fpLog, NONE, _T("Prepare load data fail...\n"));
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare load......\n"));
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
        if(pLoadPara) free(pLoadPara);
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
            if(pLoadPara) free(pLoadPara);
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
    LogMsgEx(fpLog, NONE, _T("SQLPrepare -- > SQLBindParameter --> SQLExecute. type:%s\n"),
              _T("SQL_LEN_DATA_AT_EXEC"));
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
        goto TEST_INSERT_FAIL;
    }
    
    for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
        if(numCol >= CONFIG_SQL_COLUMNS_MAX){
            LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
            ret = -1;
            goto TEST_INSERT_FAIL;
        }
        isBlobCol = FALSE;
        for(i = 0; i < psTestTableInfo->numBlobCols; i++){
            if(numCol == psTestTableInfo->idBlobCols[i]){
                isBlobCol = (psTestTableInfo->isBlobTable == TRUE) ? TRUE : FALSE;
                break;
            }
        }
        if(psTestTableInfo->dataWidth[numCol][0] == -1){
            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NULL_DATA;
        }
        else if((isBlobCol) && (lobSize > 0)){
            if(lobSize > psTestTableInfo->columnSize[numCol]){
                lobSize = psTestTableInfo->columnSize[numCol];
            }
            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = lobSize;
        }
        else{
            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NTS;
        }
        retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                SQL_PARAM_INPUT, 
                                SQL_C_TCHAR, 
                                psTestTableInfo->sqlType[numCol], 
                                psTestTableInfo->columnSize[numCol], 
                                0, 
                                (SQLPOINTER )&pLoadPara[mOffset.data[numCol]], 
                                psTestTableInfo->columnSize[numCol], 
                                ((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])));
        if (!SQL_SUCCEEDED(retcode))
        {
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail.\n"), numCol + 1);
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto TEST_INSERT_FAIL;
        }
    }
    LogMsgEx(fpLog, NONE, _T("Start insert...\n"));
    beginVal = pLoadInfo->beginVal;
    totalInsert = 0;
    for(i = 0; i < (pLoadInfo->totalBatch * pLoadInfo->batchSize); i++, beginVal++)
    {
        if((i < 3000) || ((i > 3000) && (i % 1000 == 0))){
            LogMsgEx(fpLog, NONE, _T("insert row %d\n"), i + 1);
        }
        memset(pLoadPara, 0, structSize);
        for(numCol = 0; numCol < psTestTableInfo->columNum; numCol++){
            isBlobCol = FALSE;
            for(cols = 0; cols < psTestTableInfo->numBlobCols; cols++){
                if(numCol == psTestTableInfo->idBlobCols[cols]){
                    isBlobCol = (psTestTableInfo->isBlobTable == TRUE) ? TRUE : FALSE;
                    break;
                }
            }
            if(psTestTableInfo->dataWidth[numCol][0] == -1){
                *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NULL_DATA;
                continue;
            }
            else if((isBlobCol == TRUE) && (lobSize > 0)){
                crc = 0;
                LogMsgEx(fpLog, NONE, _T("Read lob file:%s\n"), pLoadInfo->szBlobFile);
                fpBlob = fopen(pLoadInfo->szBlobFile, "rb+");
                if(fpBlob){
                    if(pLoadInfo->isloadCrc){
                        pData = &pLoadPara[mOffset.data[numCol]];
                        nbytes = fread(&pData[8], 1, lobSize, fpBlob);
                        *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = (SQLLEN)(nbytes + 8);
                        crc = calcCRC32((unsigned char *)&pData[8], nbytes);
                        hexToAscii((unsigned char *)&crc, (unsigned char *)szCrc, sizeof(crc));
                        memcpy(pData, szCrc, 8);
                    }
                    else{
                        nbytes = fread(&pLoadPara[mOffset.data[numCol]], 1, lobSize, fpBlob);
                        *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = (SQLLEN)nbytes;
                    }
                    fclose(fpBlob);
                    fpBlob = NULL;
                }
                LogMsgEx(fpLog, NONE, _T("Read %d bytes from %s crc=%x\n"), 
                            *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])),
                            pLoadInfo->szBlobFile,
                            crc);
            }
            else{
                *((SQLLEN *)(&pLoadPara[mOffset.len[numCol]])) = SQL_NTS;
                setSqlData((TCHAR *)(&pLoadPara[mOffset.data[numCol]]),
                                psTestTableInfo->dataWidth[numCol][0],
                                beginVal,
                                (SQLSMALLINT)psTestTableInfo->sqlType[numCol],
                                psTestTableInfo->dataWidth[numCol][0],
                                psTestTableInfo->dataWidth[numCol][1]);
            }
        }
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
        LogMsgEx(fpLog, NONE, _T("Load test: row:%d val:%d insert:%s table:%s %d (ms) batch:%d (row)\n"),
                                                                            i,
                                                                            beginVal,
                                                                            pLoadInfo->szInsertType,
                                                                            psTestTableInfo->szTableName,
                                                                            exeTimes, 
                                                                            1);
        totalInsert += 1;
    }
    LogMsgEx(fpLog, NONE, _T("Total insert rows %d\n"), totalInsert);
    if(totalInsert == 0){
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
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
        goto TEST_INSERT_FAIL;
    }
    _stprintf(szBuf, _T("SELECT COUNT(*) FROM %s\n"), psTestTableInfo->szTableName);
    LogMsgEx(fpLog, NONE,_T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    SQLFetch(hstmt);
    if(totalRows < (pLoadInfo->totalBatch * pLoadInfo->batchSize))
    {
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    LogMsgEx(fpLog, NONE, _T("%s: expect rows:%d actual rows:%d\n"), 
                (ret == 0) ? _T("Test pass") : _T("Test fail"),
                (pLoadInfo->totalBatch * pLoadInfo->batchSize), 
                totalRows);
TEST_INSERT_FAIL:
    FullDisconnectEx(fpLog, &sTestInfo);
    
    if(pLoadPara) free(pLoadPara);
    
    return ret;

}

