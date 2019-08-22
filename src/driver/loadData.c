#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"
#ifdef UNICODE
#ifdef unixcli
#include <iconv.h>
#include "unicode/ucnv.h"
#endif/*unixcli*/
#endif/*UNICODE*/


typedef struct loadResult_
{
    int row;
    int total;
    int beginVal;
    int batchSize;
    int numOfBatch;
    char szInsertType[32];
    char szTableName[64];
    int timeInit;
    int timePrepare;
    int timeExecute;
    int timeQuery;
    int timeFetch;
    char szDataType[32];
    unsigned int sizeLob;
    char szParaSQLBind[32];
    SQLSMALLINT sqlCType;
    char szQuery[512];
    int ret;
}loadResult;

static void putLoadResultset(FILE *fpLog, loadResult *resultset, BOOL head)
{
    char szBuf[512] = {0};
    char szTmp[64] = {0};
    unsigned int option = PRINT_OFF;
    
    if(head == TRUE){
        memset(szBuf, 0, sizeof(szBuf));
        strcat(szBuf, "|   row   |");
        strcat(szBuf, "  begin value |");
        strcat(szBuf, "      insert      |");
        strcat(szBuf, "       table        |");
        strcat(szBuf, "   execute(ms)  |");
        strcat(szBuf, "  batch(row) |");
        strcat(szBuf, "  num of batch |");
        strcat(szBuf, "   SQLBindParameter   |");
        strcat(szBuf, "   lob:c type   |");
        strcat(szBuf, " lob:data type |");
        strcat(szBuf, " lob:data length |");
        strcat(szBuf, "  result  |");
        strcat(szBuf, "  query  |");
        LogMsgEx(fpLog, NONE, "%s\n", szBuf);
        option = 0;
    }
    else{
#ifdef unixcli
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\brow:%d/%d  %s", 
                resultset->row, 
                resultset->total,
                (resultset->ret == 0) ? "pass" : "fail");
        fflush(stdout);
#endif
    }
    LogMsgEx(fpLog, NONE | option, "  %-14d %-14d %-16s %-20s %-16d %-12d %-12d %-24s %-14s %-16s %-16d %-10s %s\n",
                                                resultset->row,
                                                resultset->beginVal,
                                                resultset->szInsertType,
                                                resultset->szTableName,
                                                resultset->timeExecute, 
                                                resultset->batchSize,
                                                resultset->numOfBatch,
                                                resultset->szParaSQLBind,
                                                SQLCTypeToChar(resultset->sqlCType, szTmp),
                                                resultset->szDataType,
                                                resultset->sizeLob,
                                                (resultset->ret == 0) ? "passed" : "failed",
                                                resultset->szQuery);
}

void printBindData(sBindParamAttr bindParamAttr, 
                        sBindParamStruct *pBindParamS, 
                        int batchNum)
{
    int cols;
    DATE_STRUCT *pDateS;
    TIME_STRUCT *pTimeS;
    TIMESTAMP_STRUCT *pTimestampS;
    //SQL_NUMERIC_STRUCT *pNumeric;
    SQL_INTERVAL_STRUCT *pInterval;
    int num;
    unsigned char *pStartAddr = NULL;
    unsigned char *pColData[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN *pDataLen[CONFIG_SQL_COLUMNS_MAX];
    
    if(pBindParamS->addr == NULL || batchNum <= 0){
        printf("Pleass init bindparameter info.file:%s function:%s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    pStartAddr = pBindParamS->addr;
    for(num = 0; num < batchNum; num++){
        for(cols = 0; cols < bindParamAttr.totalCols; cols++){
            pColData[cols] = &pStartAddr[num * pBindParamS->structSize + pBindParamS->offsetAddr[cols]];
            pDataLen[cols] = (SQLLEN *)(&pStartAddr[num * pBindParamS->structSize + pBindParamS->offsetLen + cols * sizeof(SQLLEN)]);
        }
        for(cols = 0; cols < bindParamAttr.totalCols; cols++){
            switch(bindParamAttr.sqlCType[cols]){
            case SQL_C_CHAR:
            case SQL_C_BINARY:
            case SQL_C_WCHAR:
                printf("rows:%d cols:%d len = %d values:%s\n", 
                        num + 1, 
                        cols + 1, 
                        *pDataLen[cols],
                        pColData[cols]);
                break;
            case SQL_C_BIT:
                printf("rows:%d cols:%d len = %d values:%d\n", 
                    num + 1, 
                    cols + 1, 
                    *pDataLen[cols],
                    *((SQLCHAR *)pColData[cols]));
                break;
            case SQL_C_STINYINT:
            case SQL_C_UTINYINT:
                printf("rows:%d cols:%d len = %d values:%d\n",
                    num + 1, 
                    cols + 1, 
                    *pDataLen[cols],
                    *((SQLCHAR *)pColData[cols]));
                break;
            case SQL_C_SSHORT:
            case SQL_C_USHORT:
                printf("rows:%d cols:%d len = %d values:%d\n", 
                num + 1, 
                cols + 1, 
                *pDataLen[cols],
                *((SQLSMALLINT *)pColData[cols]));
                break;
            case SQL_C_SLONG:
            case SQL_C_ULONG:
                printf("rows:%d cols:%d len = %d values:%d\n", 
                    num + 1, 
                    cols + 1, 
                    *pDataLen[cols],
                    *((SQLINTEGER *)pColData[cols]));
                break;
            case SQL_C_SBIGINT:
            case SQL_C_UBIGINT:
                printf("rows:%d cols:%d len = %d values:%d\n",
                    num + 1, 
                    cols + 1, 
                    *pDataLen[cols],
                    *((SQLBIGINT *)pColData[cols]));
                break;
            case SQL_C_FLOAT:
                printf("rows:%d cols:%d len = %d values:%f\n", 
                    num + 1, 
                    cols + 1, 
                    *pDataLen[cols],
                    *((SQLREAL *)pColData[cols]));
                break;
            case SQL_C_DOUBLE:
                printf("rows:%d cols:%d len = %d values:%f\n", 
                    num + 1, 
                    cols + 1, 
                    *((SQLDOUBLE *)pColData[cols]));
                break;
            case SQL_C_TYPE_DATE:
                pDateS = (DATE_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d-%d-%d\n", 
                                                    num + 1, 
                                                    cols + 1,
                                                    *pDataLen[cols],
                                                    pDateS->year, 
                                                    pDateS->month,
                                                    pDateS->day
                                                    );
                break;
            case SQL_C_TYPE_TIME:
                pTimeS = (TIME_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d:%d:%d\n", 
                                                    num + 1, 
                                                    cols + 1,
                                                    *pDataLen[cols],
                                                    pTimeS->hour,
                                                    pTimeS->minute,
                                                    pTimeS->second
                                                    );
                break;
            case SQL_C_TYPE_TIMESTAMP:
                pTimestampS = (TIMESTAMP_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d-%d-%d %d:%d:%d\n",
                                                    num + 1, 
                                                    cols + 1,
                                                    *pDataLen[cols],
                                                    pTimestampS->year, 
                                                    pTimestampS->month,
                                                    pTimestampS->day,
                                                    pTimestampS->hour,
                                                    pTimestampS->minute,
                                                    pTimestampS->second
                                                    );
                break;
            case SQL_INTERVAL_YEAR:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d\n", num + 1, cols + 1, *pDataLen[cols], pInterval->intval.year_month.year);
                break;
            case SQL_INTERVAL_MONTH:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d\n", num + 1, cols + 1, *pDataLen[cols], pInterval->intval.year_month.month);
                break;
            case SQL_INTERVAL_DAY:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d\n", num + 1, cols + 1, *pDataLen[cols], pInterval->intval.day_second.day);
                break;
            case SQL_INTERVAL_HOUR:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d\n", num + 1, cols + 1, *pDataLen[cols], pInterval->intval.day_second.hour);
                break;
            case SQL_INTERVAL_MINUTE:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d\n", num + 1, cols + 1, *pDataLen[cols], pInterval->intval.day_second.minute);
                break;
            case SQL_INTERVAL_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d\n", num + 1, cols + 1, *pDataLen[cols], pInterval->intval.day_second.second);
                break;
            case SQL_INTERVAL_YEAR_TO_MONTH:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d-%d-%d\n", 
                                        num + 1, 
                                        cols + 1, 
                                        *pDataLen[cols],
                                        pInterval->intval.year_month.year,
                                        pInterval->intval.year_month.month,
                                        pInterval->intval.day_second.day
                                        );
                break;
            case SQL_INTERVAL_DAY_TO_HOUR:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values: %d %d:%d:%d\n",
                                        num + 1, 
                                        cols + 1,
                                        *pDataLen[cols],
                                        pInterval->intval.day_second.day,
                                        pInterval->intval.day_second.hour,
                                        0,
                                        0
                                        );
                break;
            case SQL_INTERVAL_DAY_TO_MINUTE:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values: %d %d:%d:%d\n", 
                                        num + 1, 
                                        cols + 1, 
                                        *pDataLen[cols],
                                        pInterval->intval.day_second.day,
                                        pInterval->intval.day_second.hour,
                                        pInterval->intval.day_second.minute,
                                        0
                                        );
                break;
            case SQL_INTERVAL_DAY_TO_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values: %d %d:%d:%d\n", 
                                        num + 1, 
                                        cols + 1, 
                                        *pDataLen[cols],
                                        pInterval->intval.day_second.day,
                                        pInterval->intval.day_second.hour,
                                        pInterval->intval.day_second.minute,
                                        pInterval->intval.day_second.second
                                        );
                break;
            case SQL_INTERVAL_HOUR_TO_MINUTE:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d:%d:%d\n", 
                                        num + 1, 
                                        cols + 1, 
                                        *pDataLen[cols],
                                        pInterval->intval.day_second.hour,
                                        pInterval->intval.day_second.minute,
                                        0
                                        );
                break;
            case SQL_INTERVAL_HOUR_TO_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d:%d:%d\n", 
                                        num + 1, 
                                        cols + 1, 
                                        *pDataLen[cols],
                                        pInterval->intval.day_second.hour,
                                        pInterval->intval.day_second.minute,
                                        pInterval->intval.day_second.second
                                        );
                break;
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pColData[cols];
                printf("rows:%d cols:%d len = %d values:%d:%d:%d\n", 
                                        num + 1, 
                                        cols + 1, 
                                        *pDataLen[cols],
                                        0,
                                        pInterval->intval.day_second.minute,
                                        pInterval->intval.day_second.second
                                        );
                break;
            case SQL_C_NUMERIC:
                printf("rows:%d cols:%d len = %d values:numeric\n", num + 1, cols + 1, *pDataLen[cols]);
                break;
            case SQL_C_GUID:
                printf("rows:%d cols:%d len = %d values:guid\n", num + 1, cols + 1, *pDataLen[cols]);
                break;
            default:
                printf("rows:%d cols:%d len = %d values:%s..............\n", num + 1, cols + 1, *pDataLen[cols], pColData[cols]);
                break;
            }
        }
    }
}

int setBindParamData(sBindParamAttr bindParamAttr, 
                            sBindParamStruct *pBindParamS, 
                            int beginVal, 
                            int batchNum, 
                            FILE *fpLog)
{
    int ret = 0;
    SQLINTEGER cols, i;
    DATE_STRUCT *pDateS;
    TIME_STRUCT *pTimeS;
    TIMESTAMP_STRUCT *pTimestampS;
    SQL_NUMERIC_STRUCT *pNumeric;
    SQL_INTERVAL_STRUCT *pInterval;
    unsigned char szCrc[10] = {0};
    unsigned char *pData = NULL;
    unsigned int crc = 0;
    FILE *fpLob = NULL;
    BOOL isLobCol = FALSE;
    int num;
    unsigned char *pStartAddr = NULL;
    SQLTCHAR *pColData[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN *pDataLen[CONFIG_SQL_COLUMNS_MAX];
    char szCharset[32] = {0};
    int charsetFlgLen = 0;

    if(pBindParamS->addr == NULL || batchNum <= 0){
        printf("Pleass init bindparameter info.file:%s function:%s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    pStartAddr = pBindParamS->addr;
    for(num = 0; num < batchNum; num++,beginVal++){
        for(cols = 0; cols < bindParamAttr.totalCols; cols++){
            pColData[cols] = &pStartAddr[num * pBindParamS->structSize + pBindParamS->offsetAddr[cols]];
            pDataLen[cols] = (SQLLEN *)(&pStartAddr[num * pBindParamS->structSize + pBindParamS->offsetLen + cols * sizeof(SQLLEN)]);
        }
        for(cols = 0; cols < bindParamAttr.totalCols; cols++){
            pData = (unsigned char *)pColData[cols];
            *pDataLen[cols] = SQL_NTS;
            switch(bindParamAttr.sqlCType[cols]){
            case SQL_C_CHAR:
            case SQL_C_BINARY:
            case SQL_C_WCHAR:
                isLobCol = FALSE;
                if(bindParamAttr.isLobTable){
                    for(i = 0; i < bindParamAttr.numLobCols; i++){
                        if(cols == bindParamAttr.idLobCols[i]){
                            isLobCol = (bindParamAttr.isLobTable == TRUE) ? TRUE : FALSE;
                            break;
                        }
                    }
                    if(isLobCol && (bindParamAttr.actualDataSize[cols][0] >= 0)){
                        getFileEncoding(bindParamAttr.szLobFile, szCharset);
                        if(strcmp(szCharset, ENCODING_UCS2_BIG_ENDIAN) == 0){
                            charsetFlgLen = 2;
                        }
                        else if(strcmp(szCharset, ENCODING_UCS2_LITTLE_ENDIAN) == 0){
                            charsetFlgLen = 2;
                        }
                        else if(strcmp(szCharset, ENCODING_UTF8) == 0){
                            charsetFlgLen = 3;
                        }
                        fpLob = fopen(bindParamAttr.szLobFile, "rb+");
                        if(fpLob){
                            if(charsetFlgLen > 0) fread(szCrc, 1, charsetFlgLen, fpLob);
                            *pDataLen[cols] = fread(pData, 1, bindParamAttr.columnSize[cols], fpLob);
                            fclose(fpLob);
#if 0
#ifdef UNICODE
#if unixcli
                            UConverter* convSrc;
                            UErrorCode status = U_ZERO_ERROR;
                            int lenConv = 0;
                            TCHAR *szTChar = NULL;

                            szTChar = malloc(*pDataLen[cols] * sizeof(TCHAR) + 2);
                            if(szTChar != NULL){
                                LogMsgEx(fpLog, NONE, "ucnv_open(\"%s\", &status)\n", bindParamAttr.szCharsetOfFile);
                                convSrc = ucnv_open(bindParamAttr.szCharsetOfFile, &status);
                                if (!U_SUCCESS(status)){
                                        LogMsgEx(fpLog, NONE, "[%s:%d]Call ucnv_open fail.Charset of file is %s\n", __FILE__, __LINE__, bindParamAttr.szCharsetOfFile);
                                }
                                else{
                                    printf("src of data:%s\n", pData);
                                    lenConv= ucnv_toUChars(convSrc, szTChar, *pDataLen[cols], pData, *pDataLen[cols], &status);
                                    if(lenConv > 0){
                                        LogMsgEx(fpLog, NONE, "len = %d lenconv=%d\n", *pDataLen[cols], lenConv);
                                        memcpy(pData, szTChar, lenConv * sizeof(TCHAR));
                                        *pDataLen[cols] = lenConv * 2;
                                        int kk;
                                        for(kk = 0; kk < lenConv * sizeof(TCHAR); kk++)
                                            printf("0x%x ", pData[kk]);
                                            printf("\n");
                                    }
                                    else{
                                        LogMsgEx(fpLog, NONE, "Call ucnv_toUChars fail!!!\n");
                                    }
                                }
                                ucnv_close(convSrc);
                                free(szTChar);
                            }
#else

#endif /*unixcli*/
#endif /*UNICODE*/
#endif
                            LogMsgEx(fpLog, NONE, "col:%d load lob file:%s size=%d bytes\n", cols + 1, bindParamAttr.szLobFile, *pDataLen[cols]);
                            if(*pDataLen[cols] <= 0){
                                *pDataLen[cols] = SQL_NULL_DATA;
                            }
                        }
                        else{
                            LogMsgEx(fpLog, NONE, "col:%d load lob strings data. size=%d bytes\n", cols + 1, bindParamAttr.actualDataSize[cols][0]);
                            isLobCol = FALSE;
                            *pDataLen[cols] = (bindParamAttr.paraValueLen[cols] == SQL_DEFAULT) ? bindParamAttr.actualDataSize[cols][0] : SQL_NTS;
                        }
                    }
                    else if(isLobCol){
                        *pDataLen[cols] = SQL_NULL_DATA;
                    }
                }
                if(isLobCol != TRUE){
                    setSqlData((TCHAR *)pData, 
                            bindParamAttr.actualDataSize[cols][0], 
                            beginVal, 
                            bindParamAttr.sqlType[cols],
                            bindParamAttr.actualDataSize[cols][0],
                            bindParamAttr.actualDataSize[cols][1]
                            );
                    
                }
                break;
            case SQL_C_BIT:
                *((SQLCHAR *)pData) = beginVal % 2;
                break;
            case SQL_C_STINYINT:
            case SQL_C_UTINYINT:
                *((SQLCHAR *)pData) = (beginVal <= 127) ? beginVal : (beginVal % 127);
                break;
            case SQL_C_SSHORT:
            case SQL_C_USHORT:
                *((SQLSMALLINT *)pData) = (beginVal <= 32767) ? beginVal : (beginVal % 127);
                break;
            case SQL_C_SLONG:
            case SQL_C_ULONG:
                *((SQLBIGINT *)pData) = (beginVal <= 2147483647) ? beginVal : (beginVal % 127);
                break;
            case SQL_C_SBIGINT:
            case SQL_C_UBIGINT:
                *((SQLBIGINT *)pData) = (beginVal <= 2147483647) ? beginVal : (beginVal % 127);
                break;
            case SQL_C_FLOAT:
                *((SQLCHAR *)pData) = beginVal * 1.0;
                break;
            case SQL_C_DOUBLE:
                *((SQLCHAR *)pData) = beginVal * 1.0;
                break;
            case SQL_C_TYPE_DATE:
                pDateS = (DATE_STRUCT*)pData;
                pDateS->year = beginVal;
                pDateS->month = 1;
                pDateS->day = 1;
                break;
            case SQL_C_TYPE_TIME:
                pTimeS = (TIME_STRUCT*)pData;
                pTimeS->hour = beginVal % 23;
                pTimeS->minute = beginVal % 59;
                pTimeS->second = beginVal % 59;
                break;
            case SQL_C_TYPE_TIMESTAMP:
                pTimestampS = (TIMESTAMP_STRUCT*)pData;
                pTimestampS->year = 1972 + beginVal % 8027;
                pTimestampS->month = 1;
                pTimestampS->day = 1;
                pTimestampS->hour = beginVal % 23;
                pTimestampS->minute = beginVal % 59;
                pTimestampS->second = beginVal % 59;
                break;
            case SQL_INTERVAL_YEAR:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.year_month.year = beginVal;
                break;
            case SQL_INTERVAL_MONTH:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.year_month.month = beginVal % 11;
                break;
            case SQL_INTERVAL_DAY:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.day = beginVal;
                break;
            case SQL_INTERVAL_HOUR:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.hour = beginVal % 11;
                break;
            case SQL_INTERVAL_MINUTE:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.minute = beginVal % 59;
                break;
            case SQL_INTERVAL_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.second = beginVal % 59;
                break;
            case SQL_INTERVAL_YEAR_TO_MONTH:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.year_month.year = beginVal;
                pInterval->intval.year_month.month = beginVal % 11;
                break;
            case SQL_INTERVAL_DAY_TO_HOUR:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.day = beginVal;
                pInterval->intval.day_second.hour = beginVal % 11;
                break;
            case SQL_INTERVAL_DAY_TO_MINUTE:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.day = beginVal;
                pInterval->intval.day_second.hour = beginVal % 11;
                pInterval->intval.day_second.minute = beginVal % 59;
                break;
            case SQL_INTERVAL_DAY_TO_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.day = beginVal;
                pInterval->intval.day_second.hour = beginVal % 11;
                pInterval->intval.day_second.minute = beginVal % 59;
                pInterval->intval.day_second.second = beginVal % 59;
                break;
            case SQL_INTERVAL_HOUR_TO_MINUTE:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.hour = beginVal % 11;
                pInterval->intval.day_second.minute = beginVal % 59;
                break;
            case SQL_INTERVAL_HOUR_TO_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.hour = beginVal % 11;
                pInterval->intval.day_second.minute = beginVal % 59;
                pInterval->intval.day_second.second = beginVal % 59;
                break;
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                pInterval = (SQL_INTERVAL_STRUCT*)pData;
                pInterval->intval.day_second.minute = beginVal % 59;
                pInterval->intval.day_second.second = beginVal % 59;
                break;
            case SQL_C_NUMERIC:
                pNumeric = (SQL_NUMERIC_STRUCT*)pData;
                getRandNumericEx(bindParamAttr.actualDataSize[cols][0], bindParamAttr.actualDataSize[cols][1], pNumeric, beginVal * 1.0);
                break;
            case SQL_C_GUID:
                break;
            default:
                setSqlData((TCHAR *)pData, 
                            bindParamAttr.actualDataSize[cols][0], 
                            beginVal, 
                            bindParamAttr.sqlType[cols],
                            bindParamAttr.actualDataSize[cols][0],
                            bindParamAttr.actualDataSize[cols][1]
                            );
                break;
            }
        }
    }
    
    return 0;
}

static int newBindParamStruct(sBindParamAttr bindParamAttr, sBindParamStruct *pBindParam, int batchNum)
{
    int ret = 0;
    int cols;
    int totalSize = 0;
    SQLLEN tmpLen[CONFIG_SQL_COLUMNS_MAX] = {0};
    
    for(cols = 0; cols < bindParamAttr.totalCols; cols++){
        pBindParam->offsetAddr[cols] = totalSize;
        if(bindParamAttr.actualDataSize[cols][0] <= -1){
            tmpLen[cols] = SQL_NULL_DATA;
        }
        switch(bindParamAttr.sqlCType[cols]){
        case SQL_C_CHAR:
        case SQL_C_BINARY:
        case SQL_C_WCHAR:
            if(bindParamAttr.sqlType[cols] == SQL_CHAR ||
                bindParamAttr.sqlType[cols] == SQL_VARCHAR ||
                bindParamAttr.sqlType[cols] == SQL_LONGVARCHAR ||
                bindParamAttr.sqlType[cols] == SQL_BINARY ||
                bindParamAttr.sqlType[cols] == SQL_VARBINARY ||
                bindParamAttr.sqlType[cols] == SQL_LONGVARBINARY
                ){
                totalSize += bindParamAttr.columnSize[cols] + bindParamAttr.columnSize[cols] % 4 + 4;
                if(bindParamAttr.actualDataSize[cols][0] >= 0){
                    tmpLen[cols] = bindParamAttr.columnSize[cols];
                }
            }
            else{
                totalSize += 64;
                if(bindParamAttr.actualDataSize[cols][0] >= 0){
                    tmpLen[cols] = 64;
                }
            }
            break;
        case SQL_C_BIT:
            totalSize += sizeof(SQLCHAR) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLCHAR);
            }
            break;
        case SQL_C_STINYINT:
        case SQL_C_UTINYINT:
            totalSize += sizeof(SQLTCHAR) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLTCHAR);
            }
            break;
        case SQL_C_SSHORT:
        case SQL_C_USHORT:
            totalSize += sizeof(SQLSMALLINT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLSMALLINT);
            }
            break;
        case SQL_C_SLONG:
        case SQL_C_ULONG:
            totalSize += sizeof(SQLINTEGER) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLINTEGER);
            }
            break;
        case SQL_C_SBIGINT:
        case SQL_C_UBIGINT:
            totalSize += sizeof(SQLBIGINT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLBIGINT);
            }
            break;
        case SQL_C_FLOAT:
            totalSize += sizeof(SQLREAL) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLREAL);
            }
            break;
        case SQL_C_DOUBLE:
            totalSize += sizeof(SQLDOUBLE) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLDOUBLE);
            }
            break;
        case SQL_C_TYPE_DATE:
            totalSize += sizeof(SQL_DATE_STRUCT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQL_DATE_STRUCT);
            }
            break;
        case SQL_C_TYPE_TIME:
            totalSize += sizeof(SQL_TIME_STRUCT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQL_TIME_STRUCT);
            }
            break;
        case SQL_C_TYPE_TIMESTAMP:
            totalSize += sizeof(SQL_TIMESTAMP_STRUCT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQL_TIMESTAMP_STRUCT);
            }
            break;
        case SQL_INTERVAL_YEAR:
        case SQL_INTERVAL_MONTH:
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_YEAR_TO_MONTH:
        case SQL_INTERVAL_DAY_TO_HOUR:
        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            totalSize += sizeof(SQL_INTERVAL_STRUCT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQL_INTERVAL_STRUCT);
            }
            break;
        case SQL_C_NUMERIC:
            totalSize += sizeof(SQL_NUMERIC_STRUCT) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQL_NUMERIC_STRUCT);
            }
            break;
        case SQL_C_GUID:
            totalSize += sizeof(SQLGUID) + 4;
            if(bindParamAttr.actualDataSize[cols][0] >= 0){
                tmpLen[cols] = sizeof(SQLGUID);
            }
            break;
        default:
            if(bindParamAttr.sqlType[cols] == SQL_CHAR ||
                bindParamAttr.sqlType[cols] == SQL_VARCHAR ||
                bindParamAttr.sqlType[cols] == SQL_LONGVARCHAR ||
                bindParamAttr.sqlType[cols] == SQL_BINARY ||
                bindParamAttr.sqlType[cols] == SQL_VARBINARY ||
                bindParamAttr.sqlType[cols] == SQL_LONGVARBINARY
                ){
                totalSize += bindParamAttr.columnSize[cols] + bindParamAttr.columnSize[cols] % 4 + 4;
                if(bindParamAttr.actualDataSize[cols][0] >= 0){
                    tmpLen[cols] = bindParamAttr.columnSize[cols];
                }
            }
            else{
                totalSize += 64;
                if(bindParamAttr.actualDataSize[cols][0] >= 0){
                    tmpLen[cols] = 64;
                }
            }
            break;
        }
    }
    totalSize += 4;
    pBindParam->offsetLen = totalSize;
    totalSize += bindParamAttr.totalCols * sizeof(SQLLEN);
    if(batchNum == 0) batchNum = 1;
    pBindParam->structSize = totalSize;
    pBindParam->totalStruct = batchNum;
    pBindParam->addr = (unsigned char *)calloc(1, totalSize * batchNum + 8);
    if(pBindParam->addr == NULL){
        printf("[%s %s line:%d]malloc fail.\n", __FILE__, __FUNCTION__, __LINE__);
        ret = -1;
    }
    else{
        for(cols = 0; cols < bindParamAttr.totalCols; cols++)
        {
            pBindParam->pColData[cols] = &pBindParam->addr[pBindParam->offsetAddr[cols]];
            pBindParam->pDataLen[cols] = (SQLLEN *)(&pBindParam->addr[pBindParam->offsetLen + cols * sizeof(SQLLEN)]);
            *pBindParam->pDataLen[cols] = tmpLen[cols];
        }
    }

    return ret;
}
static int deleteBindParamStruct(sBindParamStruct *pBindParam)
{
    if(pBindParam->addr){
        free(pBindParam->addr);
        pBindParam->addr = NULL;
    }
    return 0;
}
static int bindLoadParam(SQLHANDLE hstmt, 
                                sBindParamAttr *pBindParamAttr, 
                                sBindParamStruct *pBindParam,
                                FILE *fpLog)
{
    int ret = 0;
    int numCol;
    RETCODE retcode;
    SQLPOINTER rgbValue;
    SQLLEN *pLen;
    char szBuf[64];
    char szTmp[64];
    char szData[64];
    unsigned char *pStartAddr = pBindParam->addr;

    for(numCol = 0; numCol < pBindParamAttr->totalCols; numCol++){
        if(numCol >= CONFIG_SQL_COLUMNS_MAX){
            printf("The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
            ret = -1;
            break;
        }
        rgbValue = &pStartAddr[pBindParam->offsetAddr[numCol]];
        pLen = (SQLLEN *)(&pStartAddr[pBindParam->offsetLen + numCol * sizeof(SQLLEN)]); 
        if(*pLen == SQL_NTS){
            sprintf(szData, "%s", "SQL_NTS");
        }
        else if(*pLen == SQL_NULL_DATA){
            sprintf(szData, "%s", "SQL_NULL_DATA");
        }
        else if(*pLen > 0){
            sprintf(szData, "%d", *pLen);
        }
        else{
            sprintf(szData, "%s", "SQL_NTS");
        }
        LogMsgEx(fpLog, NONE, "SQLBindParameter(hstmt, %d, %s, %s, %s, ..., %s)\n", 
                            numCol + 1,
                            (pBindParamAttr->inputOutputType == SQL_PARAM_INPUT_OUTPUT) ? "SQL_PARAM_INPUT_OUTPUT" : "SQL_PARAM_INPUT",
                            SQLCTypeToChar(pBindParamAttr->sqlCType[numCol], szBuf),
                            SQLTypeToChar(pBindParamAttr->sqlType[numCol], szTmp),
                            szData
                            );
        switch(pBindParamAttr->sqlCType[numCol]){
        case SQL_C_CHAR:
        case SQL_C_BINARY:
        case SQL_C_WCHAR:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                pBindParamAttr->actualDataSize[numCol][0], 
                                0, 
                                rgbValue, 
                                (pBindParamAttr->actualDataSize[numCol][0] <= -1) ? 0 : pBindParamAttr->actualDataSize[numCol][0], 
                                pLen);
            break;
        case SQL_C_BIT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                rgbValue,
                                sizeof(SQLCHAR), 
                                pLen);
            break;
        case SQL_C_STINYINT:
        case SQL_C_UTINYINT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                rgbValue,
                                sizeof(SQLTCHAR), 
                                pLen);
            break;
        case SQL_C_SSHORT:
        case SQL_C_USHORT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                rgbValue,
                                sizeof(SQLSMALLINT), 
                                pLen);
            break;
        case SQL_C_SLONG:
        case SQL_C_ULONG:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                rgbValue,
                                sizeof(SQLINTEGER), 
                                pLen);
            break;
        case SQL_C_SBIGINT:
        case SQL_C_UBIGINT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                rgbValue,
                                sizeof(SQLBIGINT), 
                                pLen);
            break;
        case SQL_C_FLOAT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                rgbValue,
                                sizeof(SQLREAL), 
                                pLen);
            break;
        case SQL_C_DOUBLE:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0,  
                                0, 
                                rgbValue,
                                sizeof(SQLDOUBLE), 
                                pLen);
            break;
        case SQL_C_TYPE_DATE:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_DATE_STRUCT), 
                                0, 
                                rgbValue, 
                                sizeof(SQL_DATE_STRUCT), 
                                pLen);
            break;
        case SQL_C_TYPE_TIME:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_TIME_STRUCT), 
                                0, 
                                rgbValue,
                                sizeof(SQL_TIME_STRUCT), 
                                pLen);
            break;
        case SQL_C_TYPE_TIMESTAMP:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_TIMESTAMP_STRUCT), 
                                0, 
                                rgbValue,
                                sizeof(SQL_TIMESTAMP_STRUCT), 
                                pLen);
            break;
        case SQL_INTERVAL_YEAR:
        case SQL_INTERVAL_MONTH:
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_YEAR_TO_MONTH:
        case SQL_INTERVAL_DAY_TO_HOUR:
        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_INTERVAL_STRUCT), 
                                0, 
                                rgbValue,
                                sizeof(SQL_INTERVAL_STRUCT), 
                                pLen);
            break;
        case SQL_C_NUMERIC:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_NUMERIC_STRUCT), 
                                pBindParamAttr->actualDataSize[numCol][1], 
                                rgbValue,
                                sizeof(SQL_NUMERIC_STRUCT), 
                                pLen);
            break;
        case SQL_C_GUID:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                pBindParamAttr->actualDataSize[numCol][0], 
                                0, 
                                rgbValue, 
                                pBindParamAttr->columnSize[numCol], 
                                pLen);
            break;
        default:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                pBindParamAttr->actualDataSize[numCol][0], 
                                0, 
                                rgbValue,
                                pBindParamAttr->columnSize[numCol], 
                                pLen);
            break;
        }
        if(retcode != SQL_SUCCESS){
            ret = -1;
            break;
        }
    }
    
    return ret;
}

//SQLPrepare --- SQLBindParameter(...,string_len) --- SQLExecute
int loadExecDirect(sLoadParamInfo *pLoadParam)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLINTEGER i, numCol, cols;
    int beginVal = 0;
    char szBuf[256] = {0};
    char szTmp[256] = {0};
    TCHAR szQuery[256] = {0};
    int totalRows = 0;
    int totalInsert;
    SQLLEN len;
    int ret = 0;
    struct timeval tv1, tv2;
    int exeTimes = 0;
    BOOL isBlobCol = FALSE;
    int lobSize = 0;
    FILE *fpBlob = NULL;
    unsigned int crc = 0;
    unsigned char szCrc[10] = {0};
    char szCharset[32] = {0};
    
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;
    SQLSMALLINT lobCType = SQL_C_TCHAR;
    SQLLEN lenLob = 0, dataTypeLob = SQL_NTS;
    loadResult mloadResult;
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
    memset(&mloadResult, 0, sizeof(mloadResult));
    //init
    mBindParamAttr.inputOutputType = pLoadParam->inputOutputType;
    mBindParamAttr.totalCols = pLoadParam->mTable.columNum;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        mBindParamAttr.sqlCType[cols] = pLoadParam->mTable.sqlCType[cols];
        mBindParamAttr.sqlType[cols] = pLoadParam->mTable.sqlType[cols];
        mBindParamAttr.columnSize[cols] = pLoadParam->mTable.columnSize[cols];
        mBindParamAttr.decimalDigits[cols] = pLoadParam->mTable.decimalDigits[cols];
        mBindParamAttr.actualDataSize[cols][0] = pLoadParam->mTable.actualDataSize[cols][0];
        mBindParamAttr.actualDataSize[cols][1] = pLoadParam->mTable.actualDataSize[cols][1];
        mBindParamAttr.lenType[cols] = pLoadParam->lenType[cols];
        mBindParamAttr.paraValueLen[cols] = pLoadParam->mTable.paraValueLen[cols];
        sprintf(mBindParamAttr.szColName[cols], pLoadParam->mTable.szColName[cols]);
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    sprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    sprintf(mBindParamAttr.szCharsetOfFile, "%s", pLoadParam->mTable.szCharsetOfLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
        getFileEncoding(mBindParamAttr.szLobFile, szCharset);
        if(strcmp(szCharset, ENCODING_UCS2_BIG_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UCS2_LITTLE_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UTF8) == 0){
            lobSize -= 3;
        }
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LOG_HEAD(pLoadParam->fpLog);
        LogMsgEx(fpLog, NONE, "The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.actualDataSize[numCol][0] > 16777216){
            LOG_HEAD(pLoadParam->fpLog);
            LogMsgEx(fpLog, NONE, "The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n", 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "prepare bind parameter data fail.\n");
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, "Prepare load......\n");
    LogMsgEx(fpLog, TIME_STAMP, "type:%s begin value:%d batch:%d num of batch:%d\n",
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    sprintf(szBuf, "DSN=%s;UID=%s;PWD=%s;%s", 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr, fpLog)){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "connect fail.\n");
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail.\n");
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail.\n");
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", pLoadParam->mTable.szCqd[i]);
            charToTChar(pLoadParam->mTable.szCqd[i], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    
    LogMsgEx(fpLog, NONE, "SQLPrepare -- > SQLBindParameter --> SQLExecute.\n");
    if(strcmp(pLoadParam->szInsertType, "INSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "insert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT USING LOAD") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert using load") == 0){
        sprintf(szBuf, "%s INTO %s VALUES(", 
                            pLoadParam->szInsertType, 
                            pLoadParam->mTable.szTableName);
        for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
        {
            strcat(szBuf, "?");
            if(cols < (mBindParamAttr.totalCols - 1))
            {
                strcat(szBuf, ",");
            }
        }
        strcat(szBuf, ")");
    }
    else if(strcmp(pLoadParam->szInsertType, "UPDATE") == 0 ||
        strcmp(pLoadParam->szInsertType, "update") == 0
        ){
        sprintf(szBuf, "UPDATE %s set ", pLoadParam->szTable);
        for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
        {
            memset(szTmp, 0, sizeof(szTmp));
            sprintf(szTmp, "%s=?", mBindParamAttr.szColName[cols]);
            if(cols < (mBindParamAttr.totalCols - 1))
            {
                strcat(szTmp, ",");
            }
            else{
                if(strlen(pLoadParam->szInsertWhere) > 0)
                    strcat(szTmp, "  ");
            }
            strcat(szBuf, szTmp);
        }
        strcat(szBuf, pLoadParam->szInsertWhere);
    }
    else if(strcmp(pLoadParam->szInsertType, "DELETE") == 0 ||
        strcmp(pLoadParam->szInsertType, "delete") == 0
        ){
        sprintf(szBuf, "DELETE FROM %s WHERE ", pLoadParam->szTable);
        for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
        {
            sprintf(szTmp, "%s=?", mBindParamAttr.szColName[cols]);
            if(cols < (mBindParamAttr.totalCols - 1))
            {
                strcat(szTmp, ",");
            }
            else{
                if(strlen(pLoadParam->szInsertWhere) > 0)
                    strcat(szTmp, "  ");
            }
            strcat(szBuf, szTmp);
        }
        strcat(szBuf, pLoadParam->szInsertWhere);
    }
    else{
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE,"Unknown insertion type:%s\n", pLoadParam->szInsertType);
        FullDisconnectEx(fpLog, &mTestInfo);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    sprintf(mloadResult.szQuery, "%s", szBuf);
    LogMsgEx(fpLog, NONE, "SQLPrepare(hstmt, \"%s\", SQL_NTS)\n", szBuf);
    charToTChar(szBuf, szQuery);
    retcode = SQLPrepare(hstmt, (SQLTCHAR *)szQuery , SQL_NTS);
    if (!SQL_SUCCEEDED(retcode))
    {
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "SQLPrepare  fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        isBlobCol = FALSE;
        if(mBindParamAttr.isLobTable){
            for(i = 0; i < mBindParamAttr.numLobCols; i++){
                if(numCol == pLoadParam->mTable.idLobCols[i]){
                    isBlobCol = TRUE;
                    lobCType = pLoadParam->mTable.sqlCType[numCol];
                    if(lobSize > 0) lenLob = lobSize;
                    else lenLob = pLoadParam->mTable.actualDataSize[numCol][0];
                    break;
                }
            }
        }
        if(mBindParamAttr.actualDataSize[numCol][0] == SQL_NULL_DATA){
            *mBindParam.pDataLen[numCol] = SQL_NULL_DATA;
            if(isBlobCol == TRUE) dataTypeLob = SQL_NULL_DATA;
        }
        else if((isBlobCol == TRUE) && (lobSize > 0)){
            //load lob file...
            /*if(lobSize > mBindParamAttr.columnSize[numCol]){
                lobSize = mBindParamAttr.columnSize[numCol];
            }*/
            *mBindParam.pDataLen[numCol] = lobSize;
            dataTypeLob = lobSize;
        }
        else if((isBlobCol == TRUE) && (mBindParamAttr.actualDataSize[numCol][0] >= 0)){
            //Load lob string with the actual length
            *mBindParam.pDataLen[numCol] = (mBindParamAttr.paraValueLen[numCol] == SQL_DEFAULT) ? mBindParamAttr.actualDataSize[numCol][0] : SQL_NTS;
            dataTypeLob = (mBindParamAttr.paraValueLen[numCol] == SQL_DEFAULT) ? 0 : SQL_NTS;
        }
        else{
            *mBindParam.pDataLen[numCol] = SQL_NTS;
        }
    }
    if(bindLoadParam(hstmt, &mBindParamAttr, &mBindParam, fpLog) != 0){
        LogMsgEx(fpLog, NONE, "Call SQLBindParameter fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    
    LogMsgEx(fpLog, NONE, "Start %s...\n", pLoadParam->szInsertType);
    
    beginVal = pLoadParam->beginVal;
    totalInsert = 0;
    mloadResult.total = pLoadParam->totalBatch * pLoadParam->batchSize;
    for(i = 0; i < (pLoadParam->totalBatch * pLoadParam->batchSize); i++, beginVal++)
    {
        setBindParamData(mBindParamAttr, 
                        &mBindParam, 
                        beginVal, 
                        1, 
                        (i == 0) ? fpLog : NULL);
        gettimeofday(&tv1,NULL);
        if(i == 0) LogMsgEx(fpLog, NONE, "SQLExecute(hstmt)\n");
        retcode = SQLExecute(hstmt);
        if (retcode != SQL_SUCCESS)
        {
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLExecute fail.\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
        }
        gettimeofday(&tv2,NULL);
        exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        
        mloadResult.row = i;
        mloadResult.beginVal = beginVal;
        if(dataTypeLob == SQL_NULL_DATA) sprintf(mloadResult.szDataType, "%s", "SQL_NULL_DATA");
        else if(dataTypeLob == SQL_NTS) sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
        else if((dataTypeLob == 0) && (lobSize == 0)) sprintf(mloadResult.szDataType, "%s", "string");
        else if(dataTypeLob > 0) sprintf(mloadResult.szDataType, "%s(%s)", "hex", szCharset);
        else sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
        mloadResult.batchSize = 1;
        mloadResult.numOfBatch = i + 1;
        sprintf(mloadResult.szInsertType, "%s", pLoadParam->szInsertType);
        sprintf(mloadResult.szTableName, "%s", pLoadParam->mTable.szTableName);
        mloadResult.timeExecute = exeTimes;
        mloadResult.sizeLob = lenLob;
        if(dataTypeLob == SQL_NULL_DATA) sprintf(mloadResult.szParaSQLBind, "%s", "SQL_NULL_DATA");
        else if(dataTypeLob == SQL_NTS) sprintf(mloadResult.szParaSQLBind, "%s", "SQL_NTS");
        else if((dataTypeLob == 0) && (lobSize == 0)) sprintf(mloadResult.szParaSQLBind, "%s", "string");
        else sprintf(mloadResult.szParaSQLBind, "%s", "SQL_NTS");
        mloadResult.sqlCType = lobCType;
        mloadResult.ret = ret;
        putLoadResultset(fpLog, &mloadResult, (i == 0) ? TRUE : FALSE);
        if(ret != 0)break;
        totalInsert += 1;
    }
    LogMsgEx(fpLog, NONE, "Total insert rows %d\n", totalInsert);
    if(totalInsert == 0){
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);

    if(strcmp(pLoadParam->szInsertType, "INSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "insert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT USING LOAD") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert using load") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPDATE") == 0 ||
        strcmp(pLoadParam->szInsertType, "update") == 0
        ){
        LogMsgEx(fpLog, NONE, "check total insert\n");
        totalRows = 0;
        retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &totalRows, sizeof(totalRows), &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
        {
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLBindCol fail.\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto TEST_INSERT_FAIL;
        }
        /*retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"set FETCHSIZE 1", SQL_NTS);
        if(retcode != SQL_SUCCESS){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "set cqd fail !\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
        }*/
        sprintf(szBuf, "SELECT COUNT(*) FROM %s\n", pLoadParam->mTable.szTableName);
        LogMsgEx(fpLog, NONE,"%s\n", szBuf);
        charToTChar(szBuf, szQuery);
        retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
        {
            LogMsgEx(fpLog, NONE, "SQLExecDirect fail.\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto TEST_INSERT_FAIL;
        }
        SQLFetch(hstmt);
        if(totalRows < (pLoadParam->totalBatch * pLoadParam->batchSize))
        {
            ret = -1;
        }
    }
    
    LogMsgEx(fpLog, NONE, "%s: expect rows:%d actual rows:%d\n", 
                (ret == 0) ? "Test pass" : "Test fail",
                (pLoadParam->totalBatch * pLoadParam->batchSize), 
                totalRows);
TEST_INSERT_FAIL:
    FullDisconnectEx(fpLog, &mTestInfo);
    deleteBindParamStruct(&mBindParam);
        
    return ret;
}
//SQLPrepare --- SQLBindParameter(...,SQL_DATA_AT_EXEC) --- SQLExecute --- SQLPutData
int loadExecPutData(sLoadParamInfo *pLoadParam)
{
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    char szBuf[512] = {0};
    char szTmp[64];
    TCHAR szQuery[256] = {0};
    int rows = 0;
    int beginVal = 0;
    int cols;
    int dataSize = 0;
    int lobSize = 0;
    int maxLobSize = 0;
    FILE *fpBlob = NULL;
    PTR pToken;
    unsigned int crc = 0;
    unsigned char szCrc[10] = {0};
    int ret = 0;
    SQLINTEGER i;
    struct timeval tv1, tv2, tv3;
    int exeTimes = 0;
    int offset = 0, dataBatch = 0;
    BOOL isBlobCol = FALSE;
    int initDataTime = 0;
    unsigned char *pLobData = NULL;
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;
    SQLSMALLINT lobCType = SQL_C_TCHAR;
    SQLLEN lenLob = 0, dataTypeLob = SQL_NTS;
    char szCharset[32] = {0};
    loadResult mloadResult;
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
    memset(&mloadResult, 0, sizeof(mloadResult));
    //init
    mBindParamAttr.inputOutputType = pLoadParam->inputOutputType;
    mBindParamAttr.totalCols = pLoadParam->mTable.columNum;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        mBindParamAttr.sqlCType[cols] = pLoadParam->mTable.sqlCType[cols];
        mBindParamAttr.sqlType[cols] = pLoadParam->mTable.sqlType[cols];
        mBindParamAttr.columnSize[cols] = pLoadParam->mTable.columnSize[cols];
        mBindParamAttr.decimalDigits[cols] = pLoadParam->mTable.decimalDigits[cols];
        mBindParamAttr.actualDataSize[cols][0] = pLoadParam->mTable.actualDataSize[cols][0];
        mBindParamAttr.actualDataSize[cols][1] = pLoadParam->mTable.actualDataSize[cols][1];
        mBindParamAttr.lenType[cols] = pLoadParam->lenType[cols];
        mBindParamAttr.paraValueLen[cols] = pLoadParam->mTable.paraValueLen[cols];
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    sprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    sprintf(mBindParamAttr.szCharsetOfFile, "%s", pLoadParam->mTable.szCharsetOfLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
        getFileEncoding(mBindParamAttr.szLobFile, szCharset);
        if(strcmp(szCharset, ENCODING_UCS2_BIG_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UCS2_LITTLE_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UTF8) == 0){
            lobSize -= 3;
        }
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LogMsgEx(fpLog, NONE, "The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        if(mBindParamAttr.actualDataSize[cols][0] > 16777216){
            LogMsgEx(fpLog, NONE, "The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n", 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LogMsgEx(fpLog, NONE, "prepare bind parameter data fail.\n");
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, "Prepare load......\n");
    LogMsgEx(fpLog, TIME_STAMP, "type:%s begin value:%d batch:%d num of batch:%d\n",
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    sprintf(szBuf, "DSN=%s;UID=%s;PWD=%s;%s", 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr, fpLog)){
        LogMsgEx(fpLog, NONE, "connect fail.\n");
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail.\n");
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail.\n");
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", pLoadParam->mTable.szCqd[i]);
            charToTChar(pLoadParam->mTable.szCqd[i], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    LogMsgEx(fpLog, NONE, "SQLPrepare -- > SQLBindParameter --> SQLPutData.  type:%s\n",
              "SQL_DATA_AT_EXEC");
    sprintf(szBuf, "%s INTO %s VALUES(", 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
    {
        strcat(szBuf, "?");
        if(cols < (mBindParamAttr.totalCols - 1))
        {
            strcat(szBuf, ",");
        }
    }
    strcat(szBuf, ")");
    sprintf(mloadResult.szQuery, "%s", szBuf);
    LogMsgEx(fpLog, NONE, "SQLPrepare(hstmt, \"%s\", SQL_NTS)\n", szBuf);
    retcode = SQLPrepare(hstmt, (SQLTCHAR *)szBuf , SQL_NTS);
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, "SQLPrepare  fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_EXEC_PUT_DATA_FAIL;
    }
    ret = 0;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        LogMsgEx(fpLog, NONE, "SQLBindParameter(hstmt, %d, SQL_PARAM_INPUT, %s, %s, ...)\n", 
                            cols + 1,
                            SQLCTypeToChar(mBindParamAttr.sqlCType[cols], szBuf),
                            SQLTypeToChar(mBindParamAttr.sqlType[cols], szTmp)
                            );
        *mBindParam.pDataLen[cols] = SQL_DATA_AT_EXEC;
        retcode = SQLBindParameter(hstmt,
                                    cols + 1,
                                    SQL_PARAM_INPUT, 
                                    mBindParamAttr.sqlCType[cols], 
                                    mBindParamAttr.sqlType[cols], 
                                    0,
                                    0,
                                    NULL, 
                                    0, 
                                    mBindParam.pDataLen[cols]
                                    );
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLBindParameter %d fail.\n", cols + 1);
            break;
        }
    }    
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        beginVal = pLoadParam->beginVal;
        for(rows = 0; rows < (pLoadParam->totalBatch * pLoadParam->batchSize); rows++,beginVal++){
            initDataTime = 0;
            if(rows == 0) LogMsgEx(fpLog, NONE, "SQLExecute(hstmt)\n");
            gettimeofday(&tv1,NULL);
            retcode = SQLExecute(hstmt);
            if (retcode != SQL_NEED_DATA){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "SQLExecute fail:rows:%d id:%d\n", rows + 1, beginVal);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            gettimeofday(&tv2,NULL);
            setBindParamData(mBindParamAttr, 
                            &mBindParam, 
                            beginVal, 
                            1, 
                            (rows == 0) ? fpLog : NULL);
            for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
                *mBindParam.pDataLen[cols] = SQL_DATA_AT_EXEC;
            }
            gettimeofday(&tv3,NULL);
            initDataTime += (tv3.tv_sec*1000 + tv3.tv_usec/1000) - (tv2.tv_sec*1000 + tv2.tv_usec/1000);
            for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
                retcode = SQLParamData(hstmt,&pToken);
                if(retcode == SQL_NEED_DATA){
                    isBlobCol = FALSE;
                    if(mBindParamAttr.isLobTable){
                        for(i = 0; i < mBindParamAttr.numLobCols; i++){
                            if(cols == mBindParamAttr.idLobCols[i]){
                                isBlobCol = TRUE;
                                lobCType = pLoadParam->mTable.sqlCType[cols];
                                if(lobSize > 0) lenLob = lobSize;
                                else lenLob = pLoadParam->mTable.actualDataSize[cols][0];
                                break;
                            }
                        }
                    }
                    if(isBlobCol && (lobSize > 0) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
                        dataTypeLob = lobSize;
                        if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, pLobData, %d)  lob size:%d bytes\n", cols + 1, lobSize, lobSize);
                        pLobData = mBindParam.pColData[cols];
                        for(offset = 0; offset < lobSize; ){
                            dataBatch = lobSize - offset;
                            if(dataBatch >= CONFIG_BLOB_SIZE) dataBatch = CONFIG_BLOB_SIZE;
                            if(dataBatch > pLoadParam->lenPutBatchSize) dataBatch = pLoadParam->lenPutBatchSize;
                            if(dataBatch == 0) dataBatch = 4096;
                            if(rows == 0) LogMsgEx(fpLog, NONE, "lob size:%d bytes offset:%d, put size:%d bytes\n", lobSize, offset, dataBatch);
                            retcode = SQLPutData(hstmt, &pLobData[offset], dataBatch);
                            if (retcode != SQL_SUCCESS){
                                LOG_HEAD(fpLog);
                                LogMsgEx(fpLog, NONE, "SQLPutData for lob fail !\n");
                                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                break;
                            }
                            offset += dataBatch;
                        }
                    }
                    else{
                        if(mBindParamAttr.actualDataSize[cols][0] <= -1){
                            if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, NULL, SQL_NULL_DATA)\n", cols + 1);
                            retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NULL_DATA);
                            if(isBlobCol){ 
                                dataTypeLob = SQL_NULL_DATA;
                                lenLob = -1;
                            }
                        }
                        else{
                            if(isBlobCol && (mBindParamAttr.paraValueLen[cols] == SQL_DEFAULT) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
                                dataTypeLob = 0;
                                if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, pStr, %d)\n", cols + 1, mBindParamAttr.actualDataSize[cols][0]);
                                retcode = SQLPutData(hstmt, mBindParam.pColData[cols], mBindParamAttr.actualDataSize[cols][0]);
                            }
                            else{
                                if(isBlobCol) dataTypeLob = SQL_NTS;
                                if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, pStr, SQL_NTS)\n", cols + 1);
                                retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NTS);
                            }
                        }    
                    }
                    if (retcode != SQL_SUCCESS){
                        LOG_HEAD(fpLog);
                        LogMsgEx(fpLog, NONE, "SQLPutData  fail: row:%d, columns:%d\n", rows + 1, cols + 1);
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        break;
                    }
                }
                else{
                    LogMsgEx(fpLog, NONE, "SQLParamData  fail: row:%d, columns:%d\n", rows + 1, cols + 1);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    break;
                }
            }
            if(cols >= mBindParamAttr.totalCols){
                retcode = SQLParamData(hstmt, &pToken);
                if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "SQLPutData  fail: row:%d, columns:%d\n", rows + 1, cols = 1);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                }
            }
            gettimeofday(&tv2,NULL);
            exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            mloadResult.row = rows + 1;
            mloadResult.beginVal = beginVal;
            if(dataTypeLob == SQL_NULL_DATA) sprintf(mloadResult.szDataType, "%s", "SQL_NULL_DATA");
            else if(dataTypeLob == SQL_NTS) sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
            else if((dataTypeLob == 0) && (lobSize == 0)) sprintf(mloadResult.szDataType, "%s", "string");
            else if(dataTypeLob > 0) sprintf(mloadResult.szDataType, "%s(%s)", "hex", szCharset);
            else sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
            mloadResult.batchSize = 1;
            mloadResult.numOfBatch = rows + 1;
            sprintf(mloadResult.szInsertType, "%s", pLoadParam->szInsertType);
            sprintf(mloadResult.szTableName, "%s", pLoadParam->mTable.szTableName);
            mloadResult.timeInit = initDataTime;
            mloadResult.timeExecute = exeTimes - initDataTime;
            mloadResult.sizeLob = lenLob;
            sprintf(mloadResult.szParaSQLBind, "%s", "SQL_DATA_AT_EXEC");
            mloadResult.sqlCType = lobCType;
            mloadResult.ret = ret;
            putLoadResultset(fpLog, &mloadResult, (rows == 0) ? TRUE : FALSE);
        }
    }
    LogMsgEx(fpLog, NONE, "complete insert...,total rows:%d\n", rows);
LOAD_EXEC_PUT_DATA_FAIL:    
    FullDisconnectEx(fpLog, &mTestInfo);
    deleteBindParamStruct(&mBindParam);
    
    return ret;
}
//SQLPrepare --- SQLBindParameter(...,SQL_LEN_DATA_AT_EXEC) --- SQLExecute --- SQLPutData
int loadExecPutLenData(sLoadParamInfo *pLoadParam)
{
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    char szBuf[512] = {0};
    TCHAR szQuery[256] = {0};
    int rows = 0;
    int beginVal = 0;
    int cols;
    int dataSize = 0;
    int lobSize = 0;
    int maxLobSize = 0;
    FILE *fpBlob = NULL;
    PTR pToken;
    unsigned int crc = 0;
    unsigned char szCrc[10] = {0};
    int ret = 0;
    SQLINTEGER i;
    struct timeval tv1, tv2, tv3;
    int exeTimes = 0;
    int offset = 0, dataBatch = 0;
    BOOL isBlobCol = FALSE;
    int initDataTime = 0;
    unsigned char *pLobData = NULL;
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;
    char szTmp[64];
    SQLSMALLINT lobCType = SQL_C_TCHAR;
    SQLLEN lenLob = 0, dataTypeLob = SQL_NTS;
    char szCharset[32] = {0};
    loadResult mloadResult;
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
    memset(&mloadResult, 0, sizeof(mloadResult));
    //init
    mBindParamAttr.inputOutputType = pLoadParam->inputOutputType;
    mBindParamAttr.totalCols = pLoadParam->mTable.columNum;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        mBindParamAttr.sqlCType[cols] = pLoadParam->mTable.sqlCType[cols];
        mBindParamAttr.sqlType[cols] = pLoadParam->mTable.sqlType[cols];
        mBindParamAttr.columnSize[cols] = pLoadParam->mTable.columnSize[cols];
        mBindParamAttr.decimalDigits[cols] = pLoadParam->mTable.decimalDigits[cols];
        mBindParamAttr.actualDataSize[cols][0] = pLoadParam->mTable.actualDataSize[cols][0];
        mBindParamAttr.actualDataSize[cols][1] = pLoadParam->mTable.actualDataSize[cols][1];
        mBindParamAttr.lenType[cols] = pLoadParam->lenType[cols];
        mBindParamAttr.paraValueLen[cols] = pLoadParam->mTable.paraValueLen[cols];
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    sprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    sprintf(mBindParamAttr.szCharsetOfFile, "%s", pLoadParam->mTable.szCharsetOfLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
        getFileEncoding(mBindParamAttr.szLobFile, szCharset);
        if(strcmp(szCharset, ENCODING_UCS2_BIG_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UCS2_LITTLE_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UTF8) == 0){
            lobSize -= 3;
        }
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        if(mBindParamAttr.actualDataSize[cols][0] > 16777216){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n", 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "prepare bind parameter data fail.\n");
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, "Prepare load......\n");
    LogMsgEx(fpLog, TIME_STAMP, "type:%s begin value:%d batch:%d num of batch:%d\n",
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    sprintf(szBuf, "DSN=%s;UID=%s;PWD=%s;%s", 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr, fpLog)){
        LogMsgEx(fpLog, NONE, "connect fail.\n");
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail.\n");
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail.\n");
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", pLoadParam->mTable.szCqd[i]);
            charToTChar(pLoadParam->mTable.szCqd[i], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    LogMsgEx(fpLog, NONE, "SQLPrepare -- > SQLBindParameter --> SQLPutData.  type:%s\n",
              "SQL_LEN_DATA_AT_EXEC");
    sprintf(szBuf, "%s INTO %s VALUES(", 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
    {
        strcat(szBuf, "?");
        if(cols < (mBindParamAttr.totalCols - 1))
        {
            strcat(szBuf, ",");
        }
    }
    strcat(szBuf, ")");
    sprintf(mloadResult.szQuery, "%s", szBuf);
    LogMsgEx(fpLog, NONE, "SQLPrepare(hstmt, \"%s\", SQL_NTS)\n", szBuf);
    retcode = SQLPrepare(hstmt, (SQLTCHAR *)szBuf , SQL_NTS);
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, "SQLPrepare  fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_EXEC_PUT_DATA_FAIL;
    }
    ret = 0;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        isBlobCol = FALSE;
        if(mBindParamAttr.isLobTable){
            for(i = 0; i < mBindParamAttr.numLobCols; i++){
                if(cols == pLoadParam->mTable.idLobCols[i]){
                    isBlobCol = TRUE;
                    break;
                }
            }
        }
        if((mBindParamAttr.isLobTable) && (lobSize > 0)){
            *mBindParam.pDataLen[cols] = lobSize;
        }
        else if((isBlobCol == TRUE) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
            //Load lob string with the actual length
            *mBindParam.pDataLen[cols] = (mBindParamAttr.paraValueLen[cols] == SQL_DEFAULT) ? mBindParamAttr.actualDataSize[cols][0] : SQL_NTS;
            if(mBindParamAttr.actualDataSize[cols][0] != lobSize){
                *mBindParam.pDataLen[cols] = lobSize;
            }
        }
        else{
            if(mBindParamAttr.actualDataSize[cols][0] <= 0){
                *mBindParam.pDataLen[cols] = 32;
            }
            else{
                //
                *mBindParam.pDataLen[cols] = SQL_NTS;
            }
        }
        mBindParam.lenTextSize[cols] = SQL_LEN_DATA_AT_EXEC(*mBindParam.pDataLen[cols]);
        LogMsgEx(fpLog, NONE, "SQLBindParameter(hstmt, %d, SQL_PARAM_INPUT, %s, %s, ...)\n", 
                            cols + 1,
                            SQLCTypeToChar(mBindParamAttr.sqlCType[cols], szBuf),
                            SQLTypeToChar(mBindParamAttr.sqlType[cols], szTmp)
                            );
        retcode = SQLBindParameter(hstmt,
                                    cols + 1,
                                    SQL_PARAM_INPUT, 
                                    mBindParamAttr.sqlCType[cols], 
                                    mBindParamAttr.sqlType[cols], 
                                    *mBindParam.pDataLen[cols],
                                    0,
                                    (void *)mBindParam.pColData[cols],
                                    0,
                                    &(mBindParam.lenTextSize[cols])
                                    );
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLBindParameter %d fail.\n", cols + 1);
            break;
        }
    }    
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        beginVal = pLoadParam->beginVal;
        for(rows = 0; rows < (pLoadParam->totalBatch * pLoadParam->batchSize); rows++,beginVal++){
            initDataTime = 0;
            if(rows == 0) LogMsgEx(fpLog, NONE, "SQLExecute(hstmt)\n");
            gettimeofday(&tv1,NULL);
            retcode = SQLExecute(hstmt);
            if (retcode != SQL_NEED_DATA){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "SQLExecute fail:rows:%d id:%d\n", rows + 1, beginVal);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            gettimeofday(&tv2,NULL);
            setBindParamData(mBindParamAttr, 
                            &mBindParam, 
                            beginVal, 
                            1, 
                            (rows == 0) ? fpLog : NULL);
            gettimeofday(&tv3,NULL);
            initDataTime += (tv3.tv_sec*1000 + tv3.tv_usec/1000) - (tv2.tv_sec*1000 + tv2.tv_usec/1000);
            for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
                retcode = SQLParamData(hstmt,&pToken);
                if(retcode == SQL_NEED_DATA){
                    isBlobCol = FALSE;
                    if(mBindParamAttr.isLobTable){
                        for(i = 0; i < mBindParamAttr.numLobCols; i++){
                            if(cols == mBindParamAttr.idLobCols[i]){
                                isBlobCol = TRUE;
                                lobCType = pLoadParam->mTable.sqlCType[cols];
                                if(lobSize > 0) lenLob = lobSize;
                                else lenLob = pLoadParam->mTable.actualDataSize[cols][0];
                                break;
                            }
                        }
                    }
                    if(isBlobCol && (lobSize > 0) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
                        dataTypeLob = lobSize;
                        if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, pLobData, %d)  lob size:%d bytes\n", cols + 1, lobSize, lobSize);
                        pLobData = mBindParam.pColData[cols];
                        for(offset = 0; offset < lobSize; ){
                            dataBatch = lobSize - offset;
                            if(dataBatch >= CONFIG_BLOB_SIZE) dataBatch = CONFIG_BLOB_SIZE;
                            if(dataBatch > pLoadParam->lenPutBatchSize) dataBatch = pLoadParam->lenPutBatchSize;
                            if(dataBatch == 0) dataBatch = 4096;
                            if(rows == 0) LogMsgEx(fpLog, NONE, "lob size:%d bytes offset:%d, put size:%d bytes\n", lobSize, offset, dataBatch);
                            retcode = SQLPutData(hstmt, &pLobData[offset], dataBatch);
                            if (retcode != SQL_SUCCESS){
                                LOG_HEAD(fpLog);
                                LogMsgEx(fpLog, NONE, "SQLPutData for lob fail !\n");
                                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                break;
                            }
                            offset += dataBatch;
                        }
                    }
                    else{
                        if(mBindParamAttr.actualDataSize[cols][0] <= -1){
                            if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, NULL, SQL_NULL_DATA)\n", cols + 1);
                            retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NULL_DATA);
                            if(isBlobCol){ 
                                dataTypeLob = SQL_NULL_DATA;
                                lenLob = -1;
                            }
                        }
                        else{
                            if(isBlobCol && (mBindParamAttr.paraValueLen[cols] == SQL_DEFAULT) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
                                dataTypeLob = 0;
                                if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, pStr, %d)\n", cols + 1, mBindParamAttr.actualDataSize[cols][0]);
                                retcode = SQLPutData(hstmt, mBindParam.pColData[cols], mBindParamAttr.actualDataSize[cols][0]);
                            }
                            else{
                                if(isBlobCol) dataTypeLob = SQL_NTS;
                                if(rows == 0) LogMsgEx(fpLog, NONE, "columns:%d SQLPutData(hstmt, pStr, SQL_NTS)\n", cols + 1);
                                retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NTS);
                            }
                        }  
                    }
                    if (retcode != SQL_SUCCESS){
                        LOG_HEAD(fpLog);
                        LogMsgEx(fpLog, NONE, "SQLPutData  fail: row:%d, columns:%d\n", rows + 1, cols + 1);
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        break;
                    }
                }
                else{
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "SQLParamData  fail: row:%d, columns:%d\n", rows + 1, cols + 1);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    break;
                }
            }
            if(cols >= mBindParamAttr.totalCols){
                retcode = SQLParamData(hstmt, &pToken);
                if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "SQLPutData  fail: row:%d, columns:%d\n", rows + 1, cols + 1);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                }
            }
            gettimeofday(&tv2,NULL);
            exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
           
            mloadResult.row = rows + 1;
            mloadResult.beginVal = beginVal;
            if(dataTypeLob == SQL_NULL_DATA) sprintf(mloadResult.szDataType, "%s", "SQL_NULL_DATA");
            else if(dataTypeLob == SQL_NTS) sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
            else if((dataTypeLob == 0) && (lobSize == 0)) sprintf(mloadResult.szDataType, "%s", "string");
            else if(dataTypeLob > 0) sprintf(mloadResult.szDataType, "%s(%s)", "hex", szCharset);
            else sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
            mloadResult.batchSize = 1;
            mloadResult.numOfBatch = rows + 1;
            sprintf(mloadResult.szInsertType, "%s", pLoadParam->szInsertType);
            sprintf(mloadResult.szTableName, "%s", pLoadParam->mTable.szTableName);
            mloadResult.timeInit = initDataTime;
            mloadResult.timeExecute = exeTimes - initDataTime;
            mloadResult.sizeLob = lenLob;
            sprintf(mloadResult.szParaSQLBind, "%s", "SQL_LEN_DATA_AT_EXEC");
            mloadResult.sqlCType = lobCType;
            mloadResult.ret = ret;
            putLoadResultset(fpLog, &mloadResult, (rows == 0) ? TRUE : FALSE);
        }
    }
    LogMsgEx(fpLog, NONE, "complete insert...,total rows:%d\n", rows);
LOAD_EXEC_PUT_DATA_FAIL:    
    FullDisconnectEx(fpLog, &mTestInfo);
    deleteBindParamStruct(&mBindParam);
    
    return ret;
}
//If you insert more than 16M data, use this method, otherwise the efficiency is very low.
//SQLPrepare --- SQLBindParameter --- SQLExecute --- SQLPutData
int loadLobUpdate(sLoadParamInfo *pLoadParam)
{
    int ret = 0;
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLINTEGER i, numCol, cols, idLob;
    int beginVal = 0;
    char szBuf[256] = {0};
    TCHAR szQuery[256] = {0};
    int totalRows = 0;
    SQLLEN len;
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    char szUpdateStr[128] = {0};
    int updateWhere = -1;
    BOOL isLobCol = FALSE;
    PTR pToken;
    #define CONFIG_LOB_UPDATE_BUFFER_SIZE   102400
    unsigned char *pLobData = NULL;
    unsigned int lobDataLen, lobDataOffset;
    FILE *fpLob;
    sBindParamAttr mBindParamAttr;
    unsigned int crc = 0;
    unsigned int crc1 = 0;

    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
    //init
    mBindParamAttr.inputOutputType = pLoadParam->inputOutputType;
    mBindParamAttr.totalCols = pLoadParam->mTable.columNum;
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    sprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        mBindParamAttr.sqlCType[cols] = pLoadParam->mTable.sqlCType[cols];
        mBindParamAttr.sqlType[cols] = pLoadParam->mTable.sqlType[cols];
        mBindParamAttr.columnSize[cols] = pLoadParam->mTable.columnSize[cols];
        mBindParamAttr.decimalDigits[cols] = pLoadParam->mTable.decimalDigits[cols];
        mBindParamAttr.actualDataSize[cols][0] = pLoadParam->mTable.actualDataSize[cols][0];
        mBindParamAttr.actualDataSize[cols][1] = pLoadParam->mTable.actualDataSize[cols][1];
        mBindParamAttr.lenType[cols] = pLoadParam->lenType[cols];
        mBindParamAttr.paraValueLen[cols] = pLoadParam->mTable.paraValueLen[cols];
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    sprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    sprintf(mBindParamAttr.szCharsetOfFile, "%s", pLoadParam->mTable.szCharsetOfLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    for(numCol = 0; numCol < pLoadParam->mTable.columNum; numCol++){
        for(cols = 0; cols < pLoadParam->mTable.numLobCols; cols++){
            if((cols == pLoadParam->mTable.idLobCols[cols]) && 
                (mBindParamAttr.actualDataSize[numCol][0] > CONFIG_LOB_UPDATE_BUFFER_SIZE)){
                mBindParamAttr.actualDataSize[numCol][0] = CONFIG_LOB_UPDATE_BUFFER_SIZE;
                break;
            }
        }
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    sprintf(szBuf, "DSN=%s;UID=%s;PWD=%s;%s", 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr, fpLog)){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "connect fail.\n");
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail.\n");
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail.\n");
            FullDisconnectEx(fpLog, &mTestInfo);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", pLoadParam->mTable.szCqd[i]);
            charToTChar(pLoadParam->mTable.szCqd[i], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    //prepare...
    pLobData = (TCHAR *)malloc(CONFIG_LOB_UPDATE_BUFFER_SIZE * 3);
    if(pLobData == NULL){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "malloc fail.\n");
        FullDisconnectEx(fpLog, &mTestInfo);
        return -1;
    }
    memset(pLobData, 0, CONFIG_LOB_UPDATE_BUFFER_SIZE + 2);
    
    sprintf(pLobData, "%s into %s values(", 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    isLobCol = FALSE;
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.isLobTable){
            for(cols = 0; cols < pLoadParam->mTable.numLobCols; cols++){
                if(numCol == pLoadParam->mTable.idLobCols[cols]){
                    isLobCol = TRUE;
                    break;
                }
            }
        }
        if(isLobCol){
            isLobCol = FALSE;
            strcat(pLobData, "?");
        }
        else{
            setSqlData(szBuf, 
                sizeof(szBuf) - 1,
                pLoadParam->beginVal,
                mBindParamAttr.sqlType[numCol],
                (mBindParamAttr.actualDataSize[numCol][0] > 10) ? 10 : mBindParamAttr.actualDataSize[numCol][0],
                mBindParamAttr.actualDataSize[numCol][1]
                );
            if(mBindParamAttr.sqlCType[numCol] == SQL_C_SLONG ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_ULONG ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_LONG ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_SBIGINT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_UBIGINT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_BIT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_STINYINT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_UTINYINT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_TINYINT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_SSHORT || 
                mBindParamAttr.sqlCType[numCol] == SQL_C_USHORT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_SHORT
                )
                {
                strcat(pLobData, szBuf);
            }
            else{
                strcat(pLobData, "'");
                strcat(pLobData, szBuf);
                strcat(pLobData, "'");
            }
            if(updateWhere < 0){
                if(mBindParamAttr.sqlCType[numCol] == SQL_C_SLONG ||
                    mBindParamAttr.sqlCType[numCol] == SQL_C_ULONG){
                    updateWhere = numCol; 
                    sprintf(szUpdateStr, "%s", szBuf);
                }
                if(mBindParamAttr.sqlCType[numCol] == SQL_C_SBIGINT ||
                    mBindParamAttr.sqlCType[numCol] == SQL_C_UBIGINT){
                    updateWhere = numCol;
                    sprintf(szUpdateStr, "%s", szBuf);
                } 
                if((mBindParamAttr.sqlCType[numCol] == SQL_C_TCHAR ||
                    mBindParamAttr.sqlCType[numCol] == SQL_C_CHAR) &&
                    (mBindParamAttr.actualDataSize[numCol][0] < sizeof(szUpdateStr))){
                    sprintf(szUpdateStr, "%s", szBuf);
                    updateWhere = numCol;
                } 
            }
            
        }
        if(numCol < (mBindParamAttr.totalCols - 1))
        {
            strcat(pLobData, ",");
        }
    }
    strcat(pLobData, ")");
    LogMsgEx(fpLog, NONE, "%s\n", pLobData);
    retcode = SQLPrepare(hstmt, (SQLCHAR *)pLobData, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "SQLPrepare fail.\n");
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_LOB_UPDATE_FAIL;
    }
    isLobCol = FALSE;
    idLob = 1;
    sprintf(szBuf, "lobdata=%d", pLoadParam->beginVal);
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.isLobTable){
            for(cols = 0; cols < pLoadParam->mTable.numLobCols; cols++){
                if(numCol == pLoadParam->mTable.idLobCols[cols]){
                    isLobCol = TRUE;
                    break;
                }
            }
        }
        if(isLobCol){
            len = strlen(szBuf);
            isLobCol = FALSE;
            retcode = SQLBindParameter(hstmt, 
                                idLob, 
                                SQL_PARAM_INPUT_OUTPUT, 
                                SQL_C_TCHAR, 
                                SQL_VARCHAR, 
                                0, 
                                0, 
                                szBuf, 
                                sizeof(szBuf), 
                                &len);
            if(retcode != SQL_SUCCESS){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "Call SQLBindParameter fail. col:%d\n", idLob);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                goto LOAD_LOB_UPDATE_FAIL;
            }
            idLob++;
        }
        
    }
    //execute...
    retcode = SQLExecute(hstmt);
    if (retcode != SQL_SUCCESS)
    {
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "SQLExecute fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_LOB_UPDATE_FAIL;
    }
    LogMsgEx(fpLog, NONE, "do lob update......\n");
    //SQLExecDirect(hstmt, (SQLTCHAR*)"set FETCHSIZE 1"), SQL_NTS);
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    if((mBindParamAttr.isLobTable) &&
        (updateWhere >= 0) && 
        strlen(szUpdateStr) > 0){
        isLobCol = FALSE;
        for(numCol = 0; numCol < pLoadParam->mTable.columNum; numCol++){
            for(cols = 0; cols < pLoadParam->mTable.numLobCols; cols++){
                if(numCol == pLoadParam->mTable.idLobCols[cols]){
                    mBindParamAttr.actualDataSize[numCol][0] = pLoadParam->mTable.actualDataSize[numCol][0];
                    mBindParamAttr.actualDataSize[numCol][1] = pLoadParam->mTable.actualDataSize[numCol][1];
                    isLobCol = TRUE;
                    break;
                }
            }
            if(isLobCol){
                isLobCol = FALSE;
                
                sprintf(szBuf, "select %s from %s where %s = %s", 
                                    pLoadParam->mTable.szColName[numCol],
                                    pLoadParam->mTable.szTableName,
                                    pLoadParam->mTable.szColName[updateWhere],
                                    szUpdateStr
                                    );
                LogMsgEx(fpLog, NONE, "%s\n", szBuf);
                retcode = SQLExecDirect(hstmt, (SQLCHAR *)szBuf, SQL_NTS);
                if (retcode != SQL_SUCCESS){
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "select fail.\n");
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    goto LOAD_LOB_UPDATE_FAIL;
                }
                LogMsgEx(fpLog, NONE, "%s\n", "SQLFetch");
                retcode = SQLFetch(hstmt);
                if (retcode != SQL_SUCCESS){
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "SQLFetch fail.\n");
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    goto LOAD_LOB_UPDATE_FAIL;
                }
                while (retcode == SQL_SUCCESS){
                    memset(szBuf, 0, sizeof(szBuf));
                    retcode = SQLGetData(hstmt, 1, SQL_C_TCHAR, pLobData, CONFIG_LOB_UPDATE_BUFFER_SIZE, &len);
                    LogMsgEx(fpLog, NONE, "len = %d value:%s\n", len, pLobData);
                    retcode = SQLFetch(hstmt);
                }
                //Update data by "LOBUPDATE"
                LogMsgEx(fpLog, NONE, "call %s\n", "SQLExecDirect(hstmt, \"LOBUPDATE\", SQL_NTS)");
                retcode = SQLExecDirect(hstmt, (SQLCHAR *)"LOBUPDATE", SQL_NTS);
                if (retcode == SQL_NEED_DATA){
                    LogMsgEx(fpLog, NONE, "call \"LOBUPDATE\" success.\n");
                }
                else if (retcode == SQL_ERROR){
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "call \"LOBUPDATE\" fail.\n");
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    goto LOAD_LOB_UPDATE_FAIL;
                }
                else{
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "lob update failed , rc = %d\n", retcode);
                }

                retcode = SQLParamData(hstmt,&pToken);
                lobDataLen = mBindParamAttr.actualDataSize[numCol][0];
                lobDataOffset = 0;
                if(retcode == SQL_NEED_DATA){
                    lobDataLen = mBindParamAttr.actualDataSize[numCol][0];
                    lobDataOffset = 0;
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "actualDataSize = %d %d numCol=%d\n", lobDataLen, mBindParamAttr.actualDataSize[numCol][1], numCol);
                    fpLob = NULL;
                    if(pLoadParam->mTable.isLobFile){
                        lobDataLen = calcFileSize(pLoadParam->mTable.szLobFile);
                        if(lobDataLen <= 0){
                            lobDataLen = mBindParamAttr.actualDataSize[numCol][0];
                        }
                        fpLob = fopen(pLoadParam->mTable.szLobFile, "rb+");
                    }
                    do{
                        len = ((lobDataLen - lobDataOffset) > CONFIG_LOB_UPDATE_BUFFER_SIZE) ? CONFIG_LOB_UPDATE_BUFFER_SIZE : (lobDataLen - lobDataOffset);
                        if(fpLob){
                            len = fread(pLobData, 1, len, fpLob);
                            LogMsgEx(fpLog, NONE, "read %s: nbytes=%d bytes\n", pLoadParam->mTable.szLobFile, len);
                        }
                        else{
                            fillData(pLobData, len);
                            LogMsgEx(fpLog, NONE, "SQLPutData: len=%d values:%s\n", len, pLobData);
                        }
                        if(len == 0) break;
                        LogMsgEx(fpLog, NONE, "total:%d bytes offset:%d bytes  put:%d bytes\n", lobDataLen, lobDataOffset, len);
                        retcode = SQLPutData(hstmt, pLobData, len);
                        if (retcode != SQL_SUCCESS){
                            LOG_HEAD(fpLog);
                            LogMsgEx(fpLog, NONE, "SQLPutData fail.\n");
                            ret = -1;
                            goto LOAD_LOB_UPDATE_FAIL;
                        }
                        lobDataOffset += (unsigned int)len;
                        break;
                    }while(retcode == SQL_SUCCESS);
                    if(fpLob) fclose(fpLob);
                    fpLob = NULL;
                    retcode = SQLParamData(hstmt,&pToken);
                    if (retcode != SQL_SUCCESS){
                        LOG_HEAD(fpLog);
                        LogMsgEx(fpLog, NONE, "SQLParamData fail.\n");
                        ret = -1;
                        goto LOAD_LOB_UPDATE_FAIL;
                    }
                }
                else{
                    LOG_HEAD(fpLog);
                    LogMsgEx(fpLog, NONE, "SQLParamData fail. expect:SQL_NEED_DATA actual:%d\n", retcode);
                    ret = -1;
                }
            }
        }
        LogMsgEx(fpLog, NONE, "checkout lob update......\n");
        //check result
        if(ret == 0){
            //SQLFreeStmt(hstmt,SQL_UNBIND);
            //SQLFreeStmt(hstmt,SQL_CLOSE);
            //SQLExecDirect(hstmt, (SQLTCHAR*)"set FETCHSIZE 1"), SQL_NTS);
            for(numCol = 0; numCol < pLoadParam->mTable.columNum; numCol++){
                for(cols = 0; cols < pLoadParam->mTable.numLobCols; cols++){
                    if(numCol == pLoadParam->mTable.idLobCols[cols]){
                        isLobCol = TRUE;
                        break;
                    }
                }
                if(isLobCol){
                    isLobCol = FALSE;
                    sprintf(szBuf, "select %s from %s where %s = %s", 
                                        pLoadParam->mTable.szColName[numCol],
                                        pLoadParam->mTable.szTableName,
                                        pLoadParam->mTable.szColName[updateWhere],
                                        szUpdateStr
                                        );
                    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
                    retcode = SQLExecDirect(hstmt, (SQLCHAR *)szBuf, SQL_NTS);
                    if (retcode != SQL_SUCCESS){
                        LOG_HEAD(fpLog);
                        LogMsgEx(fpLog, NONE, "select fail.\n");
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        goto LOAD_LOB_UPDATE_FAIL;
                    }
                    retcode = SQLFetch(hstmt);
                    if (retcode != SQL_SUCCESS){
                        LOG_HEAD(fpLog);
                        LogMsgEx(fpLog, NONE, "SQLFetch fail.\n");
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        goto LOAD_LOB_UPDATE_FAIL;
                    }
                    lobDataLen = mBindParamAttr.actualDataSize[numCol][0];
                    if(pLoadParam->mTable.isLobFile){
                        lobDataLen = calcFileSize(pLoadParam->mTable.szLobFile);
                    }
                    if(lobDataLen <= 0){
                        lobDataLen = mBindParamAttr.actualDataSize[numCol][0];
                    }
                    while (retcode == SQL_SUCCESS){
                        fpLob = fopen(pLoadParam->mTable.szLobFile, "r");
                        if(fpLob){
                            fclose(fpLob);
                            fpLob = fopen("lobupdate.jpg", "wb+");
                        }
                        lobDataOffset = 0;
                        do{
                            len = 0;
                            LogMsgEx(fpLog, NONE, "call SQLGetData buffser size=%d bytes\n", CONFIG_LOB_UPDATE_BUFFER_SIZE);
                            memset(pLobData, 0, sizeof(pLobData));
                            retcode = SQLGetData(hstmt, 1, SQL_C_TCHAR, pLobData, CONFIG_LOB_UPDATE_BUFFER_SIZE, &len);
                            if (retcode != SQL_SUCCESS && retcode != SQL_NO_DATA_FOUND && retcode != SQL_SUCCESS_WITH_INFO) {
                                if(lobDataOffset == 0){
                                    LOG_HEAD(fpLog);
                                    LogMsgEx(fpLog, NONE, "Call SQLGetData fail.\n");
                                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                    ret = -1;
                                }
                            }else if(retcode == SQL_NO_DATA_FOUND){
                                if(lobDataOffset == 0){
                                    LOG_HEAD(fpLog);
                                    LogMsgEx(fpLog, NONE, "no data found.\n");
                                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                    ret = -1;
                                }
                                break;
                            }
                            else if (retcode == SQL_ERROR){
                                LOG_HEAD(fpLog);
                                LogMsgEx(fpLog, NONE, "call SQLGetData fail.\n");
                                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                ret = -1;
                                break;
                            }
                            LogMsgEx(fpLog, NONE, "total:%d bytes offset:%d bytes  get:%d bytes\n", lobDataLen, lobDataOffset, len);
                            lobDataOffset += (unsigned int)len;
                            if(fpLob){
                                fwrite(pLobData, len, 1, fpLob);
                            }
                            else if(len > 8){
                                LogMsgEx(fpLog, NONE, "len = %d values:%s\n", len, pLobData);
                                crc = calcCRC32((unsigned char *)pLobData, len - 8);
                                crc1 = 0;
                                asciiToHex((unsigned char *)&pLobData[len - 9], (unsigned char *)&crc1, 8);
                                LogMsgEx(fpLog, NONE, "columns:%d expect:0x%x  actual:0x%x\n", 
                                                    numCol + 1, crc, crc1);
                                if(crc != crc1){
                                    LOG_HEAD(fpLog);
                                    LogMsgEx(fpLog, NONE, "columns:%d check crc fail, expect:0x%x  actual:0x%x\n", 
                                                    numCol + 1, crc, crc1);
                                    ret = -1;
                                }
                            }
                            if(lobDataOffset >= lobDataLen){
                                break;
                            }
                        }while(retcode != SQL_NO_DATA_FOUND);
                        if(fpLob){
                            fclose(fpLob);
                        }
                        retcode = SQLFetch(hstmt);
                    }
                }
            }
        }
    }
    else{
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "No lob columns found to be updated.\n");
    }
LOAD_LOB_UPDATE_FAIL: 

    FullDisconnectEx(fpLog, &mTestInfo);
    if(pLobData) free(pLobData);
    
    return ret;
}
//SQLPrepare --- SQLBindParameter --- SQLExecute
int loadRowset(sLoadParamInfo *pLoadParam)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLUSMALLINT *pParamStatus = NULL;
#ifdef unixcli
    //linux:It may be a bug that you have to define this variable to complete the test.
    SQLUSMALLINT mpParamStatus[2] = {0};
#endif
    SQLULEN iParamsProcessed = 0;
    SQLLEN RowsFetched = 0;
    SQLLEN nBindOffset = 0;
    SQLINTEGER i, numCol, cols;
    int beginVal = 0;
    char szBuf[512] = {0};
    char szTmp[256] = {0};
    TCHAR szQuery[256] = {0};
    int totalRows = 0;
    int totalInsert;
    SQLLEN len;
    int ret = 0;
    struct timeval tv1, tv2;
    int exeTimes = 0;
    BOOL isBlobCol = FALSE;
    int lobSize = 0;
    FILE *fpBlob = NULL;
    unsigned int crc = 0;
    unsigned char szCrc[10] = {0};
    lobSize = 0;
    SQLSMALLINT lobCType = SQL_C_TCHAR;
    SQLLEN lenLob = 0, dataTypeLob = SQL_NTS;
    char szCharset[32] = {0};
    
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;
    loadResult mloadResult;
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
    memset(&mloadResult, 0, sizeof(mloadResult));
    //init
    mBindParamAttr.inputOutputType = pLoadParam->inputOutputType;
    mBindParamAttr.totalCols = pLoadParam->mTable.columNum;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        mBindParamAttr.sqlCType[cols] = pLoadParam->mTable.sqlCType[cols];
        mBindParamAttr.sqlType[cols] = pLoadParam->mTable.sqlType[cols];
        mBindParamAttr.columnSize[cols] = pLoadParam->mTable.columnSize[cols];
        mBindParamAttr.decimalDigits[cols] = pLoadParam->mTable.decimalDigits[cols];
        mBindParamAttr.actualDataSize[cols][0] = pLoadParam->mTable.actualDataSize[cols][0];
        mBindParamAttr.actualDataSize[cols][1] = pLoadParam->mTable.actualDataSize[cols][1];
        mBindParamAttr.lenType[cols] = pLoadParam->lenType[cols];
        mBindParamAttr.paraValueLen[cols] = pLoadParam->mTable.paraValueLen[cols];
        sprintf(mBindParamAttr.szColName[cols], pLoadParam->mTable.szColName[cols]);
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    sprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    sprintf(mBindParamAttr.szCharsetOfFile, "%s", pLoadParam->mTable.szCharsetOfLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
        getFileEncoding(mBindParamAttr.szLobFile, szCharset);
        if(strcmp(szCharset, ENCODING_UCS2_BIG_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UCS2_LITTLE_ENDIAN) == 0){
            lobSize -= 2;
        }
        else if(strcmp(szCharset, ENCODING_UTF8) == 0){
            lobSize -= 3;
        }
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.actualDataSize[numCol][0] > 16777216){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n", 16777216);
            return -1;
        }
    }
    pParamStatus = (SQLUSMALLINT *)calloc(1, sizeof(SQLUSMALLINT) * (pLoadParam->batchSize + 1));
    if(pParamStatus == NULL){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "prepare bind parameter data fail. line %d\n");
        return -1;
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, pLoadParam->batchSize) != 0){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "prepare bind parameter data fail. line %d\n");
        if(pParamStatus) free(pParamStatus);
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, "Prepare load......\n");
    LogMsgEx(fpLog, TIME_STAMP, "type:%s begin value:%d batch size:%d total batch:%d\n",
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    sprintf(szBuf, "DSN=%s;UID=%s;PWD=%s;%s", 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr, fpLog)){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "connect fail.\n");
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        if(pParamStatus) free(pParamStatus);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail.\n");
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail\n");
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            if(pParamStatus) free(pParamStatus);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;
    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", pLoadParam->mTable.szCqd[i]);
            charToTChar(pLoadParam->mTable.szCqd[i], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LOG_HEAD(fpLog);
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    LogMsgEx(fpLog, NONE, "SQLPrepare -- > SQLBindParameter --> SQLExecute.\n");
    if(strcmp(pLoadParam->szInsertType, "INSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "insert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT USING LOAD") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert using load") == 0
        ){
        sprintf(szBuf, "%s INTO %s VALUES(", 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
        for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
        {
            strcat(szBuf, "?");
            if(cols < (mBindParamAttr.totalCols - 1))
            {
                strcat(szBuf, ",");
            }
        }
        strcat(szBuf, ")");
    }
    else if(strcmp(pLoadParam->szInsertType, "UPDATE") == 0 ||
        strcmp(pLoadParam->szInsertType, "update") == 0
        ){
        sprintf(szBuf, "UPDATE %s set ", pLoadParam->szTable);
        for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
        {
            memset(szTmp, 0, sizeof(szTmp));
            sprintf(szTmp, "%s=?", mBindParamAttr.szColName[cols]);
            if(cols < (mBindParamAttr.totalCols - 1))
            {
                strcat(szTmp, ",");
            }
            else{
                if(strlen(pLoadParam->szInsertWhere) > 0)
                    strcat(szTmp, "  ");
            }
            strcat(szBuf, szTmp);
        }
        strcat(szBuf, pLoadParam->szInsertWhere);
    }
    else if(strcmp(pLoadParam->szInsertType, "DELETE") == 0 ||
        strcmp(pLoadParam->szInsertType, "delete") == 0
        ){
        sprintf(szBuf, "DELETE FROM %s WHERE ", pLoadParam->szTable);
        for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
        {
            sprintf(szTmp, "%s=?", mBindParamAttr.szColName[cols]);
            if(cols < (mBindParamAttr.totalCols - 1))
            {
                strcat(szTmp, ",");
            }
            else{
                if(strlen(pLoadParam->szInsertWhere) > 0)
                    strcat(szTmp, "  ");
            }
            strcat(szBuf, szTmp);
        }
        strcat(szBuf, pLoadParam->szInsertWhere);
    }
    else{
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE,"Unknown insertion type:%s\n", pLoadParam->szInsertType);
        FullDisconnectEx(fpLog, &mTestInfo);
        deleteBindParamStruct(&mBindParam);
        if(pParamStatus) free(pParamStatus);
        return -1;
    }
    sprintf(mloadResult.szQuery, "%s", szBuf);
    LogMsgEx(fpLog, NONE, "SQLPrepare(hstmt, \"%s\", SQL_NTS)\n", szBuf);
    retcode = SQLPrepare(hstmt, (SQLTCHAR *)szBuf , SQL_NTS);
    if (!SQL_SUCCEEDED(retcode))
    {
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "SQLPrepare  fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER)mBindParam.structSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)pLoadParam->batchSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR, pParamStatus, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &iParamsProcessed, 0);
   
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, "SQLSetStmtAttr  fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        isBlobCol = FALSE;
        for(i = 0; i < mBindParamAttr.numLobCols; i++){
            if(numCol == pLoadParam->mTable.idLobCols[i]){
                isBlobCol = (pLoadParam->mTable.isLobTable == TRUE) ? TRUE : FALSE;
                lobCType = pLoadParam->mTable.sqlCType[numCol];
                if(lobSize > 0) lenLob = lobSize;
                else lenLob = pLoadParam->mTable.actualDataSize[numCol][0];
                break;
            }
        }
        if(mBindParamAttr.actualDataSize[numCol][0] == SQL_NULL_DATA){
            *mBindParam.pDataLen[numCol] = SQL_NULL_DATA;
            if(isBlobCol == TRUE) dataTypeLob = SQL_NULL_DATA;
        }
        else if((isBlobCol) && (lobSize > 0)){
            /*if(lobSize > mBindParamAttr.columnSize[numCol]){
                lobSize = mBindParamAttr.columnSize[numCol];
            }*/
            *mBindParam.pDataLen[numCol] = lobSize;
            dataTypeLob = lobSize;
        }
        else if((isBlobCol) && (mBindParamAttr.actualDataSize[numCol][0] >= 0)){
            *mBindParam.pDataLen[numCol] = (mBindParamAttr.paraValueLen[numCol] == SQL_DEFAULT) ? mBindParamAttr.actualDataSize[numCol][0] : SQL_NTS;
            dataTypeLob = (mBindParamAttr.paraValueLen[numCol] == SQL_DEFAULT) ? 0 : SQL_NTS;
        }
        else{
            *mBindParam.pDataLen[numCol] = SQL_NTS;
        }
    }
    if(bindLoadParam(hstmt, &mBindParamAttr, &mBindParam, fpLog) != 0){
        LOG_HEAD(fpLog);
        LogMsgEx(fpLog, NONE, "Call SQLBindParameter fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    LogMsgEx(fpLog, NONE, "Start rowset...\n");
    
    beginVal = pLoadParam->beginVal;
    totalInsert = 0;
    for(i = 0; i < pLoadParam->totalBatch; i++, beginVal+=pLoadParam->batchSize)
    {
        setBindParamData(mBindParamAttr, 
                        &mBindParam, 
                        beginVal, 
                        pLoadParam->batchSize, 
                        (i == 0) ? fpLog : NULL);
        //printBindData(mBindParamAttr, &mBindParam, pLoadParam->batchSize);
        if(i == 0) LogMsgEx(fpLog, NONE, "SQLExecute(hstmt)\n");
        gettimeofday(&tv1,NULL);
        retcode = SQLExecute(hstmt);
        if (retcode != SQL_SUCCESS)
        {
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLExecute fail\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
        }
        gettimeofday(&tv2,NULL);
        exeTimes = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);

        mloadResult.row = beginVal;
        mloadResult.beginVal = beginVal;
        if(dataTypeLob == SQL_NULL_DATA) sprintf(mloadResult.szDataType, "%s", "SQL_NULL_DATA");
        else if(dataTypeLob == SQL_NTS) sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
        else if((dataTypeLob == 0) && (lobSize == 0)) sprintf(mloadResult.szDataType, "%s", "string");
        else if(dataTypeLob > 0) sprintf(mloadResult.szDataType, "%s(%s)", "hex", szCharset);
        else sprintf(mloadResult.szDataType, "%s", "SQL_NTS");
        mloadResult.batchSize = pLoadParam->batchSize;
        mloadResult.numOfBatch = i;
        sprintf(mloadResult.szInsertType, "%s", pLoadParam->szInsertType);
        sprintf(mloadResult.szTableName, "%s", pLoadParam->mTable.szTableName);
        mloadResult.timeInit = 0;
        mloadResult.timeExecute = exeTimes;
        mloadResult.sizeLob = lenLob;
        if(dataTypeLob == SQL_NULL_DATA) sprintf(mloadResult.szParaSQLBind, "%s", "SQL_NULL_DATA");
        else if(dataTypeLob == SQL_NTS) sprintf(mloadResult.szParaSQLBind, "%s", "SQL_NTS");
        else if((dataTypeLob == 0) && (lobSize == 0)) sprintf(mloadResult.szParaSQLBind, "%s", "string");
        else sprintf(mloadResult.szParaSQLBind, "%s", "SQL_NTS");
        mloadResult.sqlCType = lobCType;
        mloadResult.ret = ret;
        putLoadResultset(fpLog, &mloadResult, (i == 0) ? TRUE : FALSE);
        if(ret != 0) break;
        totalInsert += pLoadParam->batchSize;
    }
    LogMsgEx(fpLog, NONE, "Total insert rows: %d\n", totalInsert);
    if(totalInsert == 0){
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }


    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);

    if(strcmp(pLoadParam->szInsertType, "INSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "insert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPSERT USING LOAD") == 0 ||
        strcmp(pLoadParam->szInsertType, "upsert using load") == 0 ||
        strcmp(pLoadParam->szInsertType, "UPDATE") == 0 ||
        strcmp(pLoadParam->szInsertType, "update") == 0
        ){
        LogMsgEx(fpLog, NONE, "check total insert\n");
        totalRows = 0;
        retcode = SQLBindCol(hstmt, 1, SQL_C_ULONG, &totalRows, sizeof(totalRows), &len);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
        {
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLBindCol fail.\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto LOAD_ROWSET_FAIL;
        }
        sprintf(szBuf, "SELECT COUNT(*) FROM %s\n", pLoadParam->mTable.szTableName);
        LogMsgEx(fpLog, NONE,"%s\n", szBuf);
        retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
        {
            LOG_HEAD(fpLog);
            LogMsgEx(fpLog, NONE, "SQLExecDirect fail.\n");
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            ret = -1;
            goto LOAD_ROWSET_FAIL;
        }
        SQLFetch(hstmt);
        if(totalRows < (pLoadParam->totalBatch * pLoadParam->batchSize))
        {
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, "%s: expect rows:%d actual rows:%d\n", 
                    (ret == 0) ? "Test pass" : "Test fail",
                    (pLoadParam->totalBatch * pLoadParam->batchSize), 
                    totalRows);
    }

LOAD_ROWSET_FAIL:
    FullDisconnectEx(fpLog, &mTestInfo);
    deleteBindParamStruct(&mBindParam);
    if(pParamStatus) free(pParamStatus);
    
    return ret;
}

int loadData(sLoadParamInfo *pLoadParam)
{
    int ret = -1;

    if(pLoadParam->loadType == BINDPARAM_EXECUTE_DIRECT){
        //SQLPrepare --- SQLBindParameter --- SQLExecute
        ret = loadExecDirect(pLoadParam);
    }
    else if(pLoadParam->loadType == BINDPARAM_SQL_PUT_DATA){
        //SQLPrepare --- SQLBindParameter --- SQLExecute --- SQLPutData
        if(pLoadParam->putType == PUT_LEN_DATA_AT_EXEC){
            ret = loadExecPutLenData(pLoadParam);
        }
        else if(pLoadParam->putType == PUT_DATA_LOB_UPDATE){
            //When loading data is greater than 16M, use this method.
            ret = loadLobUpdate(pLoadParam);
        }
        else{
            ret = loadExecPutData(pLoadParam);
        }
    }
    else if(pLoadParam->loadType == BINDPARAM_ROWSET){
        //row set
        ret = loadRowset(pLoadParam);
    }
    else{
        LOG_HEAD(pLoadParam->fpLog);
        LogMsgEx(pLoadParam->fpLog, NONE, "Unknow load type. \n");
    }
    return ret;
}


