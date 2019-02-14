#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"


void printBindData(sBindParamAttr bindParamAttr, sBindParamStruct *pBindParamS)
{
    int cols;
    DATE_STRUCT *pDateS;
    TIME_STRUCT *pTimeS;
    TIMESTAMP_STRUCT *pTimestampS;
    //SQL_NUMERIC_STRUCT *pNumeric;
    SQL_INTERVAL_STRUCT *pInterval;

    if(pBindParamS->addr == NULL){
        return;
    }
    for(cols = 0; cols < bindParamAttr.totalCols; cols++){
        switch(bindParamAttr.sqlCType[cols]){
        case SQL_C_TCHAR:
        case SQL_C_BINARY:
        case SQL_C_WCHAR:
            printf("cols:%d values:%s\n", 
                    cols + 1, 
                    pBindParamS->pColData[cols]);
            break;
        case SQL_C_BIT:
            printf("cols:%d values:%d\n", 
                cols + 1, 
                *((SQLCHAR *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_STINYINT:
        case SQL_C_UTINYINT:
            printf("cols:%d values:%d\n", 
                cols + 1, 
                *((SQLCHAR *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_SSHORT:
        case SQL_C_USHORT:
            printf("cols:%d values:%d\n", 
            cols + 1, 
            *((SQLSMALLINT *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_SLONG:
        case SQL_C_ULONG:
            printf("cols:%d values:%d\n", 
                cols + 1, 
                *((SQLINTEGER *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_SBIGINT:
        case SQL_C_UBIGINT:
            printf("cols:%d values:%d\n", 
                cols + 1, 
                *((SQLBIGINT *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_FLOAT:
            printf("cols:%d values:%f\n", 
                cols + 1, 
                *((SQLREAL *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_DOUBLE:
            printf("cols:%d values:%f\n", 
                cols + 1, 
                *((SQLDOUBLE *)pBindParamS->pColData[cols]));
            break;
        case SQL_C_TYPE_DATE:
            pDateS = (DATE_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d-%d-%d\n", 
                                                cols + 1,
                                                pDateS->year, 
                                                pDateS->month,
                                                pDateS->day
                                                );
            break;
        case SQL_C_TYPE_TIME:
            pTimeS = (TIME_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d:%d:%d\n", 
                                                cols + 1,
                                                pTimeS->hour,
                                                pTimeS->minute,
                                                pTimeS->second
                                                );
            break;
        case SQL_C_TYPE_TIMESTAMP:
            pTimestampS = (TIMESTAMP_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d-%d-%d %d:%d:%d\n", 
                                                cols + 1,
                                                pTimestampS->year, 
                                                pTimestampS->month,
                                                pTimestampS->day,
                                                pTimestampS->hour,
                                                pTimestampS->minute,
                                                pTimestampS->second
                                                );
            break;
        case SQL_INTERVAL_YEAR:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d\n", cols + 1, pInterval->intval.year_month.year);
            break;
        case SQL_INTERVAL_MONTH:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d\n", cols + 1, pInterval->intval.year_month.month);
            break;
        case SQL_INTERVAL_DAY:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d\n", cols + 1, pInterval->intval.day_second.day);
            break;
        case SQL_INTERVAL_HOUR:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d\n", cols + 1, pInterval->intval.day_second.hour);
            break;
        case SQL_INTERVAL_MINUTE:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d\n", cols + 1, pInterval->intval.day_second.minute);
            break;
        case SQL_INTERVAL_SECOND:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d\n", cols + 1, pInterval->intval.day_second.second);
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d-%d-%d\n", 
                                    cols + 1, 
                                    pInterval->intval.year_month.year,
                                    pInterval->intval.year_month.month,
                                    pInterval->intval.day_second.day
                                    );
            break;
        case SQL_INTERVAL_DAY_TO_HOUR:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values: %d %d:%d:%d\n", 
                                    cols + 1, 
                                    pInterval->intval.day_second.day,
                                    pInterval->intval.day_second.hour,
                                    0,
                                    0
                                    );
            break;
        case SQL_INTERVAL_DAY_TO_MINUTE:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values: %d %d:%d:%d\n", 
                                    cols + 1, 
                                    pInterval->intval.day_second.day,
                                    pInterval->intval.day_second.hour,
                                    pInterval->intval.day_second.minute,
                                    0
                                    );
            break;
        case SQL_INTERVAL_DAY_TO_SECOND:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values: %d %d:%d:%d\n", 
                                    cols + 1, 
                                    pInterval->intval.day_second.day,
                                    pInterval->intval.day_second.hour,
                                    pInterval->intval.day_second.minute,
                                    pInterval->intval.day_second.second
                                    );
            break;
        case SQL_INTERVAL_HOUR_TO_MINUTE:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d:%d:%d\n", 
                                    cols + 1, 
                                    pInterval->intval.day_second.hour,
                                    pInterval->intval.day_second.minute,
                                    0
                                    );
            break;
        case SQL_INTERVAL_HOUR_TO_SECOND:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d:%d:%d\n", 
                                    cols + 1, 
                                    pInterval->intval.day_second.hour,
                                    pInterval->intval.day_second.minute,
                                    pInterval->intval.day_second.second
                                    );
            break;
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            pInterval = (SQL_INTERVAL_STRUCT*)pBindParamS->pColData[cols];
            printf("cols:%d values:%d:%d:%d\n", 
                                    cols + 1, 
                                    0,
                                    pInterval->intval.day_second.minute,
                                    pInterval->intval.day_second.second
                                    );
            break;
        case SQL_C_NUMERIC:
            printf("cols:%d values:numeric\n", cols + 1);
            break;
        case SQL_C_GUID:
            printf("cols:%d values:guid\n", cols + 1);
            break;
        default:
            printf("cols:%d values:%s..............\n", cols + 1, &pBindParamS->pColData[cols]);
            break;
        }
    }
}

int setBindParamData(sBindParamAttr bindParamAttr, sBindParamStruct *pBindParamS, int beginVal, int batchNum)
{
    int ret = 0;
    SQLINTEGER cols, i;
    DATE_STRUCT *pDateS;
    TIME_STRUCT *pTimeS;
    TIMESTAMP_STRUCT *pTimestampS;
    SQL_NUMERIC_STRUCT *pNumeric;
    SQL_INTERVAL_STRUCT *pInterval;
    TCHAR szCrc[10] = {0};
    TCHAR *pData = NULL;
    unsigned int crc = 0;
    FILE *fpLob = NULL;
    BOOL isLobCol = FALSE;
    int num;
    unsigned char *pStartAddr = NULL;
    unsigned char *pColData[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN *pDataLen[CONFIG_SQL_COLUMNS_MAX];

    if(pBindParamS->addr == NULL || batchNum <= 0){
        printf("Pleass init bindparameter info.file:%s function:%s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    pStartAddr = pBindParamS->addr;
    for(num = 0; num < batchNum; num++){
        for(cols = 0; cols < bindParamAttr.totalCols; cols++){
            pColData[cols] = &pStartAddr[num * pBindParamS->batchSize + pBindParamS->offsetAddr[cols]];
            pDataLen[cols] = (SQLLEN *)(&pStartAddr[num * pBindParamS->batchSize + pBindParamS->offsetLen + cols * sizeof(SQLLEN)]);
        }
        for(cols = 0; cols < bindParamAttr.totalCols; cols++){
            pData = (TCHAR *)pColData[cols];
            *pDataLen[cols] = SQL_NTS;
            switch(bindParamAttr.sqlCType[cols]){
            case SQL_C_TCHAR:
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
                        fpLob = fopen(bindParamAttr.szLobFile, "rb+");
                        if(fpLob){
                            *pDataLen[cols] = fread(pData, 1, bindParamAttr.columnSize[cols], fpLob);
                            fclose(fpLob);
                            printf("col:%d load lob file:%s size=%d bytes\n", cols + 1, bindParamAttr.szLobFile, *pDataLen[cols]);
                            if(*pDataLen[cols] <= 0){
                                *pDataLen[cols] = SQL_NULL_DATA;
                            }
                        }
                        else{
                            isLobCol = FALSE;
                        }
                    }
                    else if(isLobCol){
                        *pDataLen[cols] = SQL_NULL_DATA;
                    }
                }
                if(isLobCol != TRUE){
                    setSqlData(pData, 
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
                setSqlData(pData, 
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
        case SQL_C_TCHAR:
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
    pBindParam->batchSize = totalSize;
    pBindParam->totalBatch = batchNum;
    pBindParam->addr = (unsigned char *)calloc(1, totalSize * batchNum);
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
static int bindLoadParam(SQLHANDLE hstmt, sBindParamAttr *pBindParamAttr, sBindParamStruct *pBindParam)
{
    int ret = 0;
    int numCol;
    RETCODE retcode;

    for(numCol = 0; numCol < pBindParamAttr->totalCols; numCol++){
        if(numCol >= CONFIG_SQL_COLUMNS_MAX){
            printf("The column width defined by the table is out of range. The largest column value is %d\n", CONFIG_SQL_COLUMNS_MAX);
            ret = -1;
            break;
        }
        switch(pBindParamAttr->sqlCType[numCol]){
        case SQL_C_TCHAR:
        case SQL_C_BINARY:
        case SQL_C_WCHAR:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                pBindParamAttr->actualDataSize[numCol][0], 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol], 
                                (pBindParamAttr->actualDataSize[numCol][0] <= -1) ? 0 : pBindParamAttr->actualDataSize[numCol][0], 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_BIT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLCHAR), 
                                pBindParam->pDataLen[numCol]);
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
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLTCHAR), 
                                pBindParam->pDataLen[numCol]);
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
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLSMALLINT), 
                                pBindParam->pDataLen[numCol]);
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
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLINTEGER), 
                                pBindParam->pDataLen[numCol]);
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
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLBIGINT), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_FLOAT:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0, 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLREAL), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_DOUBLE:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                0,  
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQLDOUBLE), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_TYPE_DATE:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_DATE_STRUCT), 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol], 
                                sizeof(SQL_DATE_STRUCT), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_TYPE_TIME:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_TIME_STRUCT), 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQL_TIME_STRUCT), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_TYPE_TIMESTAMP:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_TIMESTAMP_STRUCT), 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol], 
                                sizeof(SQL_TIMESTAMP_STRUCT), 
                                pBindParam->pDataLen[numCol]);
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
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQL_INTERVAL_STRUCT), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_NUMERIC:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                sizeof(SQL_NUMERIC_STRUCT), 
                                pBindParamAttr->actualDataSize[numCol][1], 
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                sizeof(SQL_NUMERIC_STRUCT), 
                                pBindParam->pDataLen[numCol]);
            break;
        case SQL_C_GUID:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                pBindParamAttr->actualDataSize[numCol][0], 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol], 
                                pBindParamAttr->columnSize[numCol], 
                                pBindParam->pDataLen[numCol]);
            break;
        default:
            retcode = SQLBindParameter(hstmt, 
                                (numCol + 1), 
                                pBindParamAttr->inputOutputType, 
                                pBindParamAttr->sqlCType[numCol], 
                                pBindParamAttr->sqlType[numCol], 
                                pBindParamAttr->actualDataSize[numCol][0], 
                                0, 
                                (SQLPOINTER )pBindParam->pColData[numCol],
                                pBindParamAttr->columnSize[numCol], 
                                pBindParam->pDataLen[numCol]);
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
    TCHAR szBuf[256] = {0};
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
    TCHAR szCrc[10] = {0};
    
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
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
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    _stprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.actualDataSize[numCol][0] > 16777216){
            LogMsgEx(fpLog, NONE, _T("The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n"), 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LogMsgEx(fpLog, NONE, _T("prepare bind parameter data fail. line %d\n"), __LINE__);
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare load......\n"));
    LogMsgEx(fpLog, TIME_STAMP, _T("type:%s begin value:%d batch:%d num of batch:%d\n"),
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadParam->mTable.szCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadParam->mTable.szCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    LogMsgEx(fpLog, NONE, _T("SQLPrepare -- > SQLBindParameter --> SQLExecute.\n"));
    _stprintf(szBuf, _T("%s INTO %s VALUES("), 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
    {
        _tcscat(szBuf, _T("?"));
        if(cols < (mBindParamAttr.totalCols - 1))
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
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        isBlobCol = FALSE;
        if(mBindParamAttr.isLobTable){
            for(i = 0; i < mBindParamAttr.numLobCols; i++){
                if(numCol == pLoadParam->mTable.idLobCols[i]){
                    isBlobCol = TRUE;
                    break;
                }
            }
        }
        if(mBindParamAttr.actualDataSize[numCol][0] <= -1){
            *mBindParam.pDataLen[numCol] = SQL_NULL_DATA;
        }
        else if((isBlobCol == TRUE) && (lobSize > 0)){
            /*if(lobSize > mBindParamAttr.columnSize[numCol]){
                lobSize = mBindParamAttr.columnSize[numCol];
            }*/
            *mBindParam.pDataLen[numCol] = lobSize;
        }
        else{
            *mBindParam.pDataLen[numCol] = SQL_NTS;
        }
    }
    if(bindLoadParam(hstmt, &mBindParamAttr, &mBindParam) != 0){
        LogMsgEx(fpLog, NONE, _T("Call SQLBindParameter fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    
    LogMsgEx(fpLog, NONE, _T("Start insert...\n"));
    beginVal = pLoadParam->beginVal;
    totalInsert = 0;
    for(i = 0; i < (pLoadParam->totalBatch * pLoadParam->batchSize); i++, beginVal++)
    {
        if((i < 3000) || ((i > 3000) && (i % 1000 == 0))){
            LogMsgEx(fpLog, NONE, _T("insert row %d\n"), i + 1);
        }
        setBindParamData(mBindParamAttr, &mBindParam, beginVal, 1);
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
                                                                            pLoadParam->szInsertType,
                                                                            pLoadParam->mTable.szTableName,
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
    _stprintf(szBuf, _T("SELECT COUNT(*) FROM %s\n"), pLoadParam->mTable.szTableName);
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
    if(totalRows < (pLoadParam->totalBatch * pLoadParam->batchSize))
    {
        ret = -1;
        goto TEST_INSERT_FAIL;
    }
    LogMsgEx(fpLog, NONE, _T("%s: expect rows:%d actual rows:%d\n"), 
                (ret == 0) ? _T("Test pass") : _T("Test fail"),
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
    TCHAR szBuf[512] = {0};
    int rows = 0;
    int beginVal = 0;
    int cols;
    int dataSize = 0;
    int lobSize = 0;
    int maxLobSize = 0;
    FILE *fpBlob = NULL;
    PTR pToken;
    unsigned int crc = 0;
    TCHAR szCrc[10] = {0};
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
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
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
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    _stprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        if(mBindParamAttr.actualDataSize[cols][0] > 16777216){
            LogMsgEx(fpLog, NONE, _T("The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n"), 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LogMsgEx(fpLog, NONE, _T("prepare bind parameter data fail. line %d\n"), __LINE__);
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare load......\n"));
    LogMsgEx(fpLog, TIME_STAMP, _T("type:%s begin value:%d batch:%d num of batch:%d\n"),
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadParam->mTable.szCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadParam->mTable.szCqd[i], SQL_NTS);
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
    _stprintf(szBuf, _T("%s INTO %s VALUES("), 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
    {
        _tcscat(szBuf, _T("?"));
        if(cols < (mBindParamAttr.totalCols - 1))
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
        goto LOAD_EXEC_PUT_DATA_FAIL;
    }
    ret = 0;
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
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
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail:line = %d\n"), cols + 1, __LINE__);
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
            gettimeofday(&tv1,NULL);
            retcode = SQLExecute(hstmt);
            if (retcode != SQL_NEED_DATA){
                LogMsgEx(fpLog, NONE, _T("SQLExecute fail:rows:%d id:%d line = %d\n"), rows, beginVal, __LINE__);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            gettimeofday(&tv2,NULL);
            if((rows < 3000) || ((rows > 3000) && (rows % 1000 == 0))){
                LogMsgEx(fpLog, NONE, _T("insert row %d\n"), rows + 1);
            }
            setBindParamData(mBindParamAttr, &mBindParam, beginVal, 1);
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
                                break;
                            }
                        }
                    }
                    if(isBlobCol && (lobSize > 0) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
                        pLobData = mBindParam.pColData[cols];
                        for(offset = 0; offset < lobSize; ){
                            dataBatch = lobSize - offset;
                            if(dataBatch >= CONFIG_BLOB_SIZE) dataBatch = CONFIG_BLOB_SIZE;
                            if(dataBatch > pLoadParam->lenPutBatchSize) dataBatch = pLoadParam->lenPutBatchSize;
                            if(dataBatch == 0) dataBatch = 4096;
                            LogMsgEx(fpLog, NONE, _T("lob size:%d bytes offset:%d, put size:%d bytes\n"), lobSize, offset, dataBatch);
                            retcode = SQLPutData(hstmt, &pLobData[offset], dataBatch);
                            if (retcode != SQL_SUCCESS){
                                LogMsgEx(fpLog, NONE, _T("SQLPutData for lob fail !\n"));
                                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                break;
                            }
                            offset += dataBatch;
                        }
                    }
                    else{
                        if(mBindParamAttr.actualDataSize[cols][0] <= -1){
                            retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NULL_DATA);
                        }
                        else{
                            retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NTS);
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
            if(cols >= mBindParamAttr.totalCols){
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
                                                                                beginVal,
                                                                                pLoadParam->szInsertType,
                                                                                pLoadParam->mTable.szTableName,
                                                                                exeTimes - initDataTime,
                                                                                1,
                                                                                initDataTime);
        }
    }
    LogMsgEx(fpLog, NONE, _T("complete insert...,total rows:%d\n"), rows);
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
    TCHAR szBuf[512] = {0};
    int rows = 0;
    int beginVal = 0;
    int cols;
    int dataSize = 0;
    int lobSize = 0;
    int maxLobSize = 0;
    FILE *fpBlob = NULL;
    PTR pToken;
    unsigned int crc = 0;
    TCHAR szCrc[10] = {0};
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
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
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
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    _stprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++){
        if(mBindParamAttr.actualDataSize[cols][0] > 16777216){
            LogMsgEx(fpLog, NONE, _T("The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n"), 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LogMsgEx(fpLog, NONE, _T("prepare bind parameter data fail. line %d\n"), __LINE__);
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare load......\n"));
    LogMsgEx(fpLog, TIME_STAMP, _T("type:%s begin value:%d batch:%d num of batch:%d\n"),
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    pLoadParam->batchSize,
                                    pLoadParam->totalBatch
                                    );
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadParam->mTable.szCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadParam->mTable.szCqd[i], SQL_NTS);
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
              _T("SQL_LEN_DATA_AT_EXEC"));
    _stprintf(szBuf, _T("%s INTO %s VALUES("), 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
    {
        _tcscat(szBuf, _T("?"));
        if(cols < (mBindParamAttr.totalCols - 1))
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
        else{
            if(mBindParamAttr.actualDataSize[cols][0] <= 0){
                *mBindParam.pDataLen[cols] = 32;
            }
            else{
                //
            }
        }
        mBindParam.lenTextSize[cols] = SQL_LEN_DATA_AT_EXEC(*mBindParam.pDataLen[cols]);
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
            LogMsgEx(fpLog, NONE, _T("SQLBindParameter %d fail:line = %d\n"), cols + 1, __LINE__);
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
            gettimeofday(&tv1,NULL);
            retcode = SQLExecute(hstmt);
            if (retcode != SQL_NEED_DATA){
                LogMsgEx(fpLog, NONE, _T("SQLExecute fail:rows:%d id:%d line = %d\n"), rows, beginVal, __LINE__);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                ret = -1;
                break;
            }
            gettimeofday(&tv2,NULL);
            if((rows < 3000) || ((rows > 3000) && (rows % 1000 == 0))){
                LogMsgEx(fpLog, NONE, _T("insert row %d\n"), rows + 1);
            }
            setBindParamData(mBindParamAttr, &mBindParam, beginVal, 1);
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
                                break;
                            }
                        }
                    }
                    if(isBlobCol && (lobSize > 0) && (mBindParamAttr.actualDataSize[cols][0] >= 0)){
                        pLobData = mBindParam.pColData[cols];
                        for(offset = 0; offset < lobSize; ){
                            dataBatch = lobSize - offset;
                            if(dataBatch >= CONFIG_BLOB_SIZE) dataBatch = CONFIG_BLOB_SIZE;
                            if(dataBatch > pLoadParam->lenPutBatchSize) dataBatch = pLoadParam->lenPutBatchSize;
                            if(dataBatch == 0) dataBatch = 4096;
                            LogMsgEx(fpLog, NONE, _T("lob size:%d bytes offset:%d, put size:%d bytes\n"), lobSize, offset, dataBatch);
                            retcode = SQLPutData(hstmt, &pLobData[offset], dataBatch);
                            if (retcode != SQL_SUCCESS){
                                LogMsgEx(fpLog, NONE, _T("SQLPutData for lob fail !\n"));
                                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                                break;
                            }
                            offset += dataBatch;
                        }
                    }
                    else{
                        if(mBindParamAttr.actualDataSize[cols][0] <= -1){
                            retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NULL_DATA);
                        }
                        else{
                            retcode = SQLPutData(hstmt, mBindParam.pColData[cols], SQL_NTS);
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
            if(cols >= mBindParamAttr.totalCols){
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
                                                                                beginVal,
                                                                                pLoadParam->szInsertType,
                                                                                pLoadParam->mTable.szTableName,
                                                                                exeTimes - initDataTime,
                                                                                1,
                                                                                initDataTime);
        }
    }
    LogMsgEx(fpLog, NONE, _T("complete insert...,total rows:%d\n"), rows);
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
    SQLINTEGER i, numCol, cols;
    int beginVal = 0;
    TCHAR szBuf[256] = {0};
    int totalRows = 0;
    SQLLEN len;
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    int updateInt = -1;
    TCHAR szUpdateStr[128] = {0};
    int updateWhere = 0;
    BOOL isLobCol = FALSE;
    PTR pToken;
    #define CONFIG_LOB_UPDATE_BUFFER_SIZE   4096
    TCHAR szLobData[CONFIG_LOB_UPDATE_BUFFER_SIZE + 2] = {0};
    unsigned int lobDataLen, lobDataOffset;
    FILE *fpLob;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;

    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
    //init
    mBindParamAttr.inputOutputType = pLoadParam->inputOutputType;
    mBindParamAttr.totalCols = pLoadParam->mTable.columNum;
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    _stprintf(mBindParamAttr.szLobFile, _T("%s"), pLoadParam->mTable.szLobFile);
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
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    _stprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
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
        LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, 1) != 0){
        LogMsgEx(fpLog, NONE, _T("prepare bind parameter data fail. line %d\n"), __LINE__);
        return -1;
    }
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;

    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadParam->mTable.szCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadParam->mTable.szCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    //prepare...
    _stprintf(szBuf, _T("%s INTO %s VALUES("), 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++)
    {
        _tcscat(szBuf, _T("?"));
        if(numCol < (mBindParamAttr.totalCols - 1))
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
        FullDisconnectEx(fpLog, &mTestInfo);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(pLoadParam->mTable.actualDataSize[numCol][0] <= -1){
            *mBindParam.pDataLen[numCol] = SQL_NULL_DATA;
        }
        else{
            *mBindParam.pDataLen[numCol] = SQL_NTS;
        }
    }
    //bindparameter...
    if(bindLoadParam(hstmt, &mBindParamAttr, &mBindParam) != 0){
        LogMsgEx(fpLog, NONE, _T("Call SQLBindParameter fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_LOB_UPDATE_FAIL;
    }
    
    mBindParamAttr.isLobTable = FALSE;
    setBindParamData(mBindParamAttr, &mBindParam, pLoadParam->beginVal, 1);
    printBindData(mBindParamAttr, &mBindParam);
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.sqlCType[numCol] == SQL_C_SLONG ||
            mBindParamAttr.sqlCType[numCol] == SQL_C_ULONG){
            updateInt = *((int *)(mBindParam.pColData[numCol]));
            updateWhere = numCol;
            break;
        }        
    }
    if(updateInt == -1){
        for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
            if(mBindParamAttr.sqlCType[numCol] == SQL_C_SBIGINT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_UBIGINT){
                updateInt = *((int *)(mBindParam.pColData[numCol]));
                updateWhere = numCol;
                break;
            }        
        }
    }
    if(updateInt == -1){
        for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
            if(mBindParamAttr.sqlCType[numCol] == SQL_C_USHORT ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_SSHORT){
                updateInt = *((unsigned short *)(mBindParam.pColData[numCol]));
                updateWhere = numCol;
                break;
            }        
        }
    }
    if(updateInt == -1){
        for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
            if((mBindParamAttr.sqlCType[numCol] == SQL_C_TCHAR ||
                mBindParamAttr.sqlCType[numCol] == SQL_C_CHAR) &&
                (mBindParamAttr.actualDataSize[numCol][0] < sizeof(szUpdateStr))){
                _stprintf(szUpdateStr, _T("%s"), mBindParam.pColData[numCol]);
                updateWhere = numCol;
                break;
            }        
        }
    }
    //execute...
    retcode = SQLExecute(hstmt);
    if (retcode != SQL_SUCCESS)
    {
        LogMsgEx(fpLog, NONE, _T("SQLExecute fail.line=%d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_LOB_UPDATE_FAIL;
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    if((mBindParamAttr.isLobTable) &&
        (updateInt != -1 || _tcslen(szUpdateStr) > 0)){
        isLobCol = FALSE;
        memcpy(mBindParamAttr.actualDataSize, pLoadParam->mTable.actualDataSize, sizeof(mBindParamAttr.actualDataSize));
        for(numCol = 0; numCol < pLoadParam->mTable.columNum; numCol++){
            for(cols = 0; cols < pLoadParam->mTable.numLobCols; cols++){
                if(cols == pLoadParam->mTable.idLobCols[cols]){
                    isLobCol = TRUE;
                    break;
                }
            }
            if(isLobCol){
                isLobCol = FALSE;
                if(updateInt != -1){
                    _stprintf(szBuf, _T("select %s from %s where %s=%d"), 
                                    pLoadParam->mTable.szColName[numCol],
                                    pLoadParam->mTable.szTableName,
                                    pLoadParam->mTable.szColName[updateWhere],
                                    updateInt
                                    );
                }
                else{
                    _stprintf(szBuf, _T("select %s from %s where %s=%s"), 
                                    pLoadParam->mTable.szColName[numCol],
                                    pLoadParam->mTable.szTableName,
                                    pLoadParam->mTable.szColName[updateWhere],
                                    szUpdateStr
                                    );
                }
                retcode = SQLExecDirect(hstmt, (SQLCHAR *)szBuf, SQL_NTS);
                if (retcode != SQL_SUCCESS){
                    LogMsgEx(fpLog, NONE, _T("select fail.line=%d\n"), __LINE__);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    goto LOAD_LOB_UPDATE_FAIL;
                }
                retcode = SQLFetch(hstmt);
                if (retcode != SQL_SUCCESS){
                    LogMsgEx(fpLog, NONE, _T("SQLFetch fail.line=%d\n"), __LINE__);
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    goto LOAD_LOB_UPDATE_FAIL;
                }
                while (retcode == SQL_SUCCESS){
                    retcode = SQLGetData(hstmt, 1, SQL_C_TCHAR, szBuf, sizeof(szBuf), &len);
                    retcode = SQLFetch(hstmt);
                }
                //Update data by "LOBUPDATE"
                retcode = SQLExecDirect(hstmt, (SQLCHAR *)"LOBUPDATE", SQL_NTS);
                if (retcode == SQL_NEED_DATA){
                    LogMsgEx(fpLog, NONE, _T("lob update success\n"));
                }
                else if (retcode == SQL_ERROR){
                    LogMsgEx(fpLog, NONE, _T("lob update error\n"));
                    LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                    ret = -1;
                    goto LOAD_LOB_UPDATE_FAIL;
                }
                else{
                    LogMsgEx(fpLog, NONE, _T("lob update failed , rc = %d\n"), retcode);
                }
                retcode = SQLParamData(hstmt,&pToken);
                lobDataLen = mBindParamAttr.actualDataSize[numCol][0];
                lobDataOffset = 0;
                if(retcode == SQL_NEED_DATA){
                    fpLob = NULL;
                    if(pLoadParam->mTable.isLobFile){
                        fpLob = fopen(pLoadParam->mTable.szLobFile, "rb+");
                    }
                    do{
                        len = ((lobDataLen - lobDataOffset) > CONFIG_LOB_UPDATE_BUFFER_SIZE) ? CONFIG_LOB_UPDATE_BUFFER_SIZE : (lobDataLen - lobDataOffset);
                        if(fpLob){
                            len = fread(szLobData, 1, len, fpLob);
                        }
                        else{
                            fillData(szLobData, len);
                        }
                        if(len == 0) break;
                        retcode = SQLPutData(hstmt, szLobData, len);
                        if (retcode == SQL_SUCCESS){
                            LogMsgEx(fpLog, NONE, _T("SQLPutData fail. line %d\n"), __LINE__);
                            ret = -1;
                            goto LOAD_LOB_UPDATE_FAIL;
                        }
                        lobDataOffset += (unsigned int)len;
                    }while(retcode == SQL_SUCCESS);
                    if(fpLob) fclose(fpLob);
                    fpLob = NULL;
                    retcode = SQLParamData(hstmt,&pToken);
                    if (retcode == SQL_SUCCESS){
                        LogMsgEx(fpLog, NONE, _T("SQLParamData fail. line %d\n"), __LINE__);
                        ret = -1;
                        goto LOAD_LOB_UPDATE_FAIL;
                    }
                }
            }
        }
    }
    else{
        LogMsgEx(fpLog, NONE, _T("No lob columns found to be updated.\n"));
    }
LOAD_LOB_UPDATE_FAIL:    
    FullDisconnectEx(fpLog, &mTestInfo);
    deleteBindParamStruct(&mBindParam);
    
    return ret;
}
//SQLPrepare --- SQLBindParameter --- SQLExecute
int loadRowset(sLoadParamInfo *pLoadParam)
{
    RETCODE retcode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    SQLUSMALLINT sParamStatus[6] = {0};
    SQLINTEGER iParamsProcessed = 0;
    SQLLEN RowsFetched = 0;
    SQLLEN nBindOffset = 0;
    SQLINTEGER i, numCol, cols;
    int beginVal = 0;
    TCHAR szBuf[256] = {0};
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
    TCHAR szCrc[10] = {0};
    lobSize = 0;
    
    FILE *fpLog = pLoadParam->fpLog;
    TestInfo mTestInfo;
    sBindParamStruct mBindParam;
    sBindParamAttr mBindParamAttr;
    
    memset(&mBindParam, 0, sizeof(mBindParam));
    memset(&mBindParamAttr, 0, sizeof(mBindParamAttr));
    memcpy(&mTestInfo, &pLoadParam->mTestInfo, sizeof(TestInfo));
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
    }
    mBindParamAttr.isLobTable = pLoadParam->mTable.isLobTable;
    mBindParamAttr.isLobFile = pLoadParam->mTable.isLobFile;
    _stprintf(mBindParamAttr.szLobFile, "%s", pLoadParam->mTable.szLobFile);
    mBindParamAttr.numLobCols = pLoadParam->mTable.numLobCols;
    memcpy(mBindParamAttr.idLobCols, pLoadParam->mTable.idLobCols, sizeof(mBindParamAttr.idLobCols));
    if(mBindParamAttr.isLobTable){
        lobSize = calcFileSize(mBindParamAttr.szLobFile);
    }
    if(mBindParamAttr.totalCols >= CONFIG_SQL_COLUMNS_MAX){
        LogMsgEx(fpLog, NONE, _T("The column width defined by the table is out of range. The largest column value is %d\n"), CONFIG_SQL_COLUMNS_MAX);
        return -1;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        if(mBindParamAttr.actualDataSize[numCol][0] > 16777216){
            LogMsgEx(fpLog, NONE, _T("The data loaded is too large and the maximum value is %d.Please use \"LOBUPDATE\" mode.\n"), 16777216);
            return -1;
        }
    }
    if(newBindParamStruct(mBindParamAttr, &mBindParam, pLoadParam->batchSize) != 0){
        LogMsgEx(fpLog, NONE, _T("prepare bind parameter data fail. line %d\n"), __LINE__);
        return -1;
    }
    
    LogMsgEx(fpLog, TIME_STAMP, _T("Prepare load......\n"));
    LogMsgEx(fpLog, TIME_STAMP, _T("type:%s begin value:%d batch:%d num of batch:%d\n"),
                                    pLoadParam->szInsertType,
                                    pLoadParam->beginVal,
                                    mBindParam.batchSize,
                                    mBindParam.totalBatch
                                    );
    _stprintf(szBuf, _T("DSN=%s;UID=%s;PWD=%s;%s"), 
                        mTestInfo.DataSource, 
                        mTestInfo.UserID, 
                        mTestInfo.Password, 
                        pLoadParam->szConnStr);
    LogMsgEx(fpLog, NONE, _T("%s\n"), szBuf);
    if(!FullConnectWithOptionsEx(&mTestInfo, CONNECT_ODBC_VERSION_3, pLoadParam->szConnStr)){
        LogMsgEx(fpLog, NONE, _T("connect fail: line %d\n"), __LINE__);
		LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        deleteBindParamStruct(&mBindParam);
        return -1;
    }
    henv = mTestInfo.henv;
    hdbc = mTestInfo.hdbc;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &mTestInfo);
            deleteBindParamStruct(&mBindParam);
            return -1;
        }
    }
    mTestInfo.hstmt = hstmt;
    addInfoSessionEx(hstmt, fpLog);

    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(_tcslen(pLoadParam->mTable.szCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, _T("%s\n"), pLoadParam->mTable.szCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)pLoadParam->mTable.szCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, _T("set cqd fail !\n"));
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    LogMsgEx(fpLog, NONE, _T("SQLPrepare -- > SQLBindParameter --> SQLExecute.\n"));
    _stprintf(szBuf, _T("%s INTO %s VALUES("), 
                        pLoadParam->szInsertType, 
                        pLoadParam->mTable.szTableName);
    for(cols = 0; cols < mBindParamAttr.totalCols; cols++)
    {
        _tcscat(szBuf, _T("?"));
        if(cols < (mBindParamAttr.totalCols - 1))
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
        goto LOAD_ROWSET_FAIL;
    }
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER)(pLoadParam->batchSize * pLoadParam->totalBatch), 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)pLoadParam->batchSize, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR, sParamStatus, 0);
    retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &iParamsProcessed, 0);
   
    if (!SQL_SUCCEEDED(retcode))
    {
        LogMsgEx(fpLog, NONE, _T("SQLSetStmtAttr  fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    for(numCol = 0; numCol < mBindParamAttr.totalCols; numCol++){
        isBlobCol = FALSE;
        for(i = 0; i < mBindParamAttr.numLobCols; i++){
            if(numCol == pLoadParam->mTable.idLobCols[i]){
                isBlobCol = (pLoadParam->mTable.isLobTable == TRUE) ? TRUE : FALSE;
                break;
            }
        }
        if(mBindParamAttr.actualDataSize[numCol][0] <= -1){
            *mBindParam.pDataLen[numCol] = SQL_NULL_DATA;
        }
        else if((isBlobCol) && (lobSize > 0)){
            /*if(lobSize > mBindParamAttr.columnSize[numCol]){
                lobSize = mBindParamAttr.columnSize[numCol];
            }*/
            *mBindParam.pDataLen[numCol] = lobSize;
        }
        else{
            *mBindParam.pDataLen[numCol] = SQL_NTS;
        }
    }
    if(bindLoadParam(hstmt, &mBindParamAttr, &mBindParam) != 0){
        LogMsgEx(fpLog, NONE, _T("Call SQLBindParameter fail.\n"));
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    
    LogMsgEx(fpLog, NONE, _T("Start rowset...\n"));
    beginVal = pLoadParam->beginVal;
    totalInsert = 0;
    for(i = 0; i < pLoadParam->totalBatch; i++, beginVal+=pLoadParam->batchSize)
    {
        if((i < 3000) || ((i > 3000) && (i % 1000 == 0))){
            LogMsgEx(fpLog, NONE, _T("num of batch %d\n"), i + 1);
        }
        setBindParamData(mBindParamAttr, &mBindParam, beginVal, pLoadParam->batchSize);
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
                                                                            pLoadParam->szInsertType,
                                                                            pLoadParam->mTable.szTableName,
                                                                            exeTimes, 
                                                                            pLoadParam->batchSize, 
                                                                            i);
        totalInsert += pLoadParam->batchSize;
    }
    LogMsgEx(fpLog, NONE, _T("Total insert rows %d\n"), totalInsert);
    if(totalInsert == 0){
        ret = -1;
        goto LOAD_ROWSET_FAIL;
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
        goto LOAD_ROWSET_FAIL;
    }
    _stprintf(szBuf, _T("SELECT COUNT(*) FROM %s\n"), pLoadParam->mTable.szTableName);
    LogMsgEx(fpLog, NONE,_T("%s\n"), szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
    {
        LogMsgEx(fpLog, NONE, _T("SQLExecDirect fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    SQLFetch(hstmt);
    if(totalRows < (pLoadParam->totalBatch * pLoadParam->batchSize))
    {
        ret = -1;
        goto LOAD_ROWSET_FAIL;
    }
    LogMsgEx(fpLog, NONE, _T("%s: expect rows:%d actual rows:%d\n"), 
                (ret == 0) ? _T("Test pass") : _T("Test fail"),
                (pLoadParam->totalBatch * pLoadParam->batchSize), 
                totalRows);

LOAD_ROWSET_FAIL:
    FullDisconnectEx(fpLog, &mTestInfo);
    deleteBindParamStruct(&mBindParam);
        
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
        LogMsgEx(pLoadParam->fpLog, NONE, _T("[file:%s func:%s line %d]:Unknow load type. \n"), __FILE__, __FUNCTION__, __LINE__);
    }
    return ret;
}


