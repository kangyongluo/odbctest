#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"

static BOOL isTypeName(SQLSMALLINT sqlType, TCHAR *pTypeName)
{
    BOOL ret = FALSE;
    
    switch (sqlType)
   {
       case SQL_CHAR: 
            if(_tcscmp(pTypeName, _T("CHAR")) == 0) ret = TRUE; 
            break;
       case SQL_VARCHAR: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_LONGVARCHAR: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_WCHAR: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_WVARCHAR: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_WLONGVARCHAR: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_DECIMAL: 
            if(_tcscmp(pTypeName, _T("DECIMAL")) == 0) ret = TRUE;
            if(_tcscmp(pTypeName, _T("DECIMAL UNSIGNED")) == 0) ret = TRUE;
            break;
       case SQL_NUMERIC: 
            if(_tcscmp(pTypeName, _T("DECIMAL")) == 0) ret = TRUE;
            if(_tcscmp(pTypeName, _T("DECIMAL UNSIGNED")) == 0) ret = TRUE; 
            break;
       case SQL_SMALLINT: 
            if(_tcscmp(pTypeName, _T("SMALLINT")) == 0) ret = TRUE;
            if(_tcscmp(pTypeName, _T("SMALLINT UNSIGNED")) == 0) ret = TRUE;
            break;
       case SQL_INTEGER: 
            if(_tcscmp(pTypeName, _T("INTEGER UNSIGNED")) == 0) ret = TRUE;
            if(_tcscmp(pTypeName, _T("INTEGER")) == 0) ret = TRUE; 
            break;
       case SQL_REAL: 
            if(_tcscmp(pTypeName, _T("REAL")) == 0) ret = TRUE; 
            break;
       case SQL_FLOAT: 
            if(_tcscmp(pTypeName, _T("DOUBLE PRECISION")) == 0) ret = TRUE; 
            break;
       case SQL_DOUBLE: 
            if(_tcscmp(pTypeName, _T("DOUBLE PRECISION")) == 0) ret = TRUE; 
            break;
       case SQL_BIT: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_TINYINT: 
            if(_tcscmp(pTypeName, _T("TINYINT")) == 0) ret = TRUE;
            if(_tcscmp(pTypeName, _T("TINYINT UNSIGNED")) == 0) ret = TRUE; 
            break;
       case SQL_BIGINT: 
            if(_tcscmp(pTypeName, _T("BIGINT")) == 0) ret = TRUE;
            if(_tcscmp(pTypeName, _T("BIGINT UNSIGNED")) == 0) ret = TRUE;
            break;
       case SQL_BINARY: 
            if(_tcscmp(pTypeName, _T("BINARY")) == 0) ret = TRUE; 
            break;
       case SQL_VARBINARY: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_LONGVARBINARY: 
            if(_tcscmp(pTypeName, _T("VARCHAR")) == 0) ret = TRUE; 
            break;
       case SQL_TYPE_DATE: 
            if(_tcscmp(pTypeName, _T("DATE")) == 0) ret = TRUE; 
            break;
       case SQL_TYPE_TIME: 
            if(_tcscmp(pTypeName, _T("TIME")) == 0) ret = TRUE; 
            break;
       case SQL_TYPE_TIMESTAMP: 
            if(_tcscmp(pTypeName, _T("TIMESTAMP")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_MONTH: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_YEAR: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_YEAR_TO_MONTH: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_HOUR: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_MINUTE: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_SECOND: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY_TO_HOUR: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY_TO_MINUTE: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY_TO_SECOND: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_HOUR_TO_MINUTE: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_HOUR_TO_SECOND: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_MINUTE_TO_SECOND: 
            if(_tcscmp(pTypeName, _T("INTERVAL")) == 0) ret = TRUE; 
            break;
       default:
            break;
	}

    return ret;
}
static TCHAR *sqlTypeToSqltypeName(SQLSMALLINT sqlType, TCHAR *pTypeName)
{
    switch (sqlType)
   {
       case SQL_CHAR: 
            _stprintf(pTypeName, _T("CHAR")); 
            break;
       case SQL_VARCHAR: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_LONGVARCHAR: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_WCHAR: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_WVARCHAR: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_WLONGVARCHAR: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_DECIMAL: 
            _stprintf(pTypeName, _T("DECIMAL"));
            break;
       case SQL_NUMERIC: 
            _stprintf(pTypeName, _T("DECIMAL"));
            break;
       case SQL_SMALLINT: 
            _stprintf(pTypeName, _T("SMALLINT"));
            break;
       case SQL_INTEGER: 
            _stprintf(pTypeName, _T("INTEGER UNSIGNED"));
            break;
       case SQL_REAL: 
            _stprintf(pTypeName, _T("REAL")); 
            break;
       case SQL_FLOAT: 
            _stprintf(pTypeName, _T("DOUBLE PRECISION")); 
            break;
       case SQL_DOUBLE: 
            _stprintf(pTypeName, _T("DOUBLE PRECISION")); 
            break;
       case SQL_BIT: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_TINYINT: 
            _stprintf(pTypeName, _T("TINYINT"));
            break;
       case SQL_BIGINT: 
            _stprintf(pTypeName, _T("BIGINT"));
            break;
       case SQL_BINARY: 
            _stprintf(pTypeName, _T("BINARY")); 
            break;
       case SQL_VARBINARY: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_LONGVARBINARY: 
            _stprintf(pTypeName, _T("VARCHAR")); 
            break;
       case SQL_TYPE_DATE: 
            _stprintf(pTypeName, _T("DATE")); 
            break;
       case SQL_TYPE_TIME: 
            _stprintf(pTypeName, _T("TIME")); 
            break;
       case SQL_TYPE_TIMESTAMP: 
            _stprintf(pTypeName, _T("TIMESTAMP")); 
            break;
       case SQL_INTERVAL_MONTH: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_YEAR: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_YEAR_TO_MONTH: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_DAY: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_HOUR: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_MINUTE: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_SECOND: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_DAY_TO_HOUR: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_DAY_TO_MINUTE: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_DAY_TO_SECOND: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_HOUR_TO_MINUTE: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_HOUR_TO_SECOND: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       case SQL_INTERVAL_MINUTE_TO_SECOND: 
            _stprintf(pTypeName, _T("INTERVAL")); 
            break;
       default:
            break;
	}

    return pTypeName;
}

int SQLColumnsEx(TestInfo *pTestInfo,
                            sTestTableInfo *psTestTableInfo,
                            TCHAR *pCatalog, 
                            TCHAR *pSchema, 
                            TCHAR *pTable, 
                            TCHAR *pColumnName,
                            FILE *fpLog)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT lenCatalog, lenSchema, lenTable, lenColumnName;
    TCHAR   szBuf[128];
    TCHAR   szCatalog[128];
	TCHAR   szSchema[128];
	TCHAR   szTable[128];
	TCHAR   szColname[128];
	SWORD oColDataType;
	TCHAR   szColTypeName[128];
	SQLSMALLINT oColPrec;
	SQLSMALLINT oColLen;
	SQLSMALLINT oColScale;
	SQLSMALLINT oColRadix;
	SQLSMALLINT oColNullable;
	TCHAR   szRemark[128];
	SQLLEN  oTableQualifierlen; 
	SQLLEN  oTableOwnerlen;
	SQLLEN  oTableNamelen;
	SQLLEN  oColNamelen;
	SQLLEN  oColDataTypelen;
	SQLLEN  oColTypeNamelen;
	SQLLEN  oColPreclen;
	SQLLEN  oColLenlen;
	SQLLEN  oColScalelen;
	SQLLEN  oColRadixlen;
	SQLLEN  oColNullablelen;
	SQLLEN  oRemarklen;
    struct timeval tv1, tv2, tv3;
    int intervalTimes = 0;
    
    int i, cols;
    SQLULEN sSqlMode[2] = {SQL_MODE_READ_WRITE, SQL_MODE_READ_ONLY};
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
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
    
    LogMsgEx(fpLog, NONE, _T("connect info......\n"));
    addInfoSessionEx(hstmt, fpLog);
    
    SQLSetConnectOption((SQLHANDLE)hdbc,SQL_ACCESS_MODE,SQL_MODE_READ_WRITE);
    for(i = 0; i < 2; i++){
        LogMsgEx(fpLog, TIME_STAMP, _T("SQLSetConnectOption((SQLHANDLE)hdbc, SQL_ACCESS_MODE, %s)\n"),
                (i == 0) ? _T("SQL_MODE_READ_WRITE") : _T("SQL_MODE_READ_ONLY"));
        SQLSetConnectOption((SQLHANDLE)hdbc, SQL_ACCESS_MODE, sSqlMode[i]);
        LogMsgEx(fpLog, TIME_STAMP, _T(""));
        LogMsgEx(fpLog, NONE, _T("SQLColumns(hstmt,"));
        if(pCatalog == NULL){
            LogMsgEx(fpLog, NONE, _T("NULL, 0, "));
            lenCatalog = 0;
        }
        else{
            lenCatalog = (SQLSMALLINT)_tcslen(pCatalog);
            LogMsgEx(fpLog, NONE, _T("\"%s\", %d, "), pCatalog, lenCatalog);
        }
        if(pSchema == NULL){
            LogMsgEx(fpLog, NONE, _T("NULL, 0, "));
            lenSchema = 0;
        }
        else{
            lenSchema = (SQLSMALLINT)_tcslen(pSchema);
            LogMsgEx(fpLog, NONE, _T("\"%s\", %d, "), pSchema, lenSchema);
        }
        if(pTable == NULL){
            LogMsgEx(fpLog, NONE, _T("NULL, 0, "));
            lenTable = 0;
        }
        else{
            lenTable = (SQLSMALLINT)_tcslen(pTable);
            LogMsgEx(fpLog, NONE, _T("\"%s\", %d, "), pTable, lenTable);
        }
        if(pColumnName == NULL){
            LogMsgEx(fpLog, NONE, _T("NULL, 0, "));
            lenColumnName = 0;
        }
        else{
            lenColumnName = (SQLSMALLINT)_tcslen(pColumnName);
            LogMsgEx(fpLog, NONE, _T("\"%s\", %d, "), pColumnName, lenColumnName);
        }
        LogMsgEx(fpLog, NONE, _T("\"%\", 1)\n"));
        gettimeofday(&tv1,NULL);
        intervalTimes = 0;
        retcode = SQLColumns(hstmt, 
                             (SQLTCHAR*)pCatalog,
                             (SWORD)lenCatalog,
                             (SQLTCHAR*)pSchema,
                             (SWORD)lenSchema,
                             (SQLTCHAR*)pTable,
                             (SWORD)lenTable,
                             (SQLTCHAR*)pColumnName,
                             (SWORD)lenColumnName);
        if(SQL_SUCCESS == retcode){
            retcode = SQLBindCol(hstmt,1, SQL_C_TCHAR, szCatalog, sizeof(szCatalog)/sizeof(TCHAR), &oTableQualifierlen);
            if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 1 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,2,SQL_C_TCHAR, szSchema, sizeof(szSchema)/sizeof(TCHAR),&oTableOwnerlen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 2 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }

			retcode = SQLBindCol(hstmt,3,SQL_C_TCHAR, szTable, sizeof(szTable)/sizeof(TCHAR),&oTableNamelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 3 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,4,SQL_C_TCHAR, szColname, sizeof(szColname)/sizeof(TCHAR), &oColNamelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 4 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,5,SQL_C_SHORT, &oColDataType, 0,&oColDataTypelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 5 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,6,SQL_C_TCHAR, szColTypeName,sizeof(szColTypeName)/sizeof(TCHAR),&oColTypeNamelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 6 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,7,SQL_C_LONG, &oColPrec, 0,&oColPreclen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 7 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,8,SQL_C_LONG,&oColLen,0,&oColLenlen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 7 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,9,SQL_C_SHORT,&oColScale,0,&oColScalelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 8 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,10,SQL_C_SHORT,&oColRadix,0,&oColRadixlen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 10 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,11,SQL_C_SHORT,&oColNullable,0,&oColNullablelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 11 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,12,SQL_C_TCHAR,szRemark,sizeof(szRemark)/sizeof(TCHAR),&oRemarklen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, _T("SQLBindCol 12 fail.line %d\n"), __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
            else{
                cols = 0;
                while (retcode == SQL_SUCCESS)
				{
					_tcscpy(szCatalog,_T(""));
					_tcscpy(szSchema,_T(""));
					_tcscpy(szTable,_T(""));
					_tcscpy(szColname,_T(""));
					oColDataType = 0;
					_tcscpy(szColTypeName,_T(""));
					oColPrec = 0;
					oColLen = 0;
					oColScale = 0;
					oColRadix = 0;
					oColNullable = 0;
					_tcscpy(szRemark,_T(""));
                    
					retcode = SQLFetch(hstmt);
					if(retcode != SQL_SUCCESS && cols == 0)
					{
					    LogMsgEx(fpLog, NONE, _T("SQLFetch fail and no data found.\n"));
						LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
						ret = -1;
                        break;
					}
                    else if(retcode != SQL_SUCCESS)
					{
						break;
					}
                    gettimeofday(&tv2,NULL);
                    if(cols == 0){
                        LogMsgEx(fpLog, NONE, _T("---------------------------\n"));
                        LogMsgEx(fpLog, TIME_STAMP, _T("Check return result set:\n"));
                        LogMsgEx(fpLog, NONE,_T("colCatalog:%s\n"),szCatalog);
                    }
                    if(_tcscmp(szCatalog, pTestInfo->Catalog) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),pTestInfo->Catalog, szCatalog);
                        ret = -1;
                    }
                    if(cols == 0)
                        LogMsgEx(fpLog, NONE, _T("Schema:%s\n"), szSchema);
                    if(_tcscmp(szSchema, pTestInfo->Schema) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),pTestInfo->Schema, szSchema);
                        ret = -1;
                    }
                    if(cols == 0)
                        LogMsgEx(fpLog, NONE, _T("TableName:%s\n"), szTable);
                    if((pTable != NULL) && (_tcslen(pTable) > 0)){
                        if(_tcscmp(szTable, pTable) != 0) 
                        {
                            LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),pTable, szTable);
                            ret = -1;
                        }
                    }
                    else if(_tcscmp(szTable, psTestTableInfo->szTableName) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),psTestTableInfo->szTableName,szTable);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, _T("check columns %d ......\n"), cols + 1);
                    LogMsgEx(fpLog, NONE, _T("ColName:%s\n"), szColname);
                    if(_tcscmp(szColname, psTestTableInfo->szColName[cols]) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),psTestTableInfo->szColName[cols], szColname);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, _T("data type:%d\n"), oColDataType);
                    if(oColDataType != psTestTableInfo->sqlType[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"),psTestTableInfo->sqlType[cols], oColDataType);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, _T("ColTypeName:%s\n"), szColTypeName);
                    if(!isTypeName(psTestTableInfo->sqlType[cols], szColTypeName)) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),
                                        sqlTypeToSqltypeName(psTestTableInfo->sqlType[cols], szBuf),
                                        szColTypeName);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, _T("ColPrec:%d\n"), oColPrec);
                    if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColPrec != 0)){
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"), 0, oColPrec);
                        ret = -1;
                    }
                    else if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColPrec == 0)){
                        ;
                    }
                    else if(oColPrec != psTestTableInfo->columnSize[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"),psTestTableInfo->columnSize[cols],oColPrec);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, _T("ColLen:%d\n"), oColLen);
                    if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColLen != 0)){
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"),0,oColLen);
                        ret = -1;
                    }
                    else if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColPrec == 0)){
                        ;
                    }
                    else if(oColLen != psTestTableInfo->columnSize[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"),psTestTableInfo->columnSize[cols],oColLen);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, _T("ColScale:%d\n"), oColScale);
                    if(oColScale != psTestTableInfo->decimalDigits[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"),psTestTableInfo->decimalDigits[cols],oColScale);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE,_T("ColRadix:%d\n"), oColRadix);
                    LogMsgEx(fpLog, NONE, _T("ColNullable:%d\n"), oColNullable);
                    if(oColNullable != 1) 
                    {
                        LogMsgEx(fpLog, NONE,_T("\texpect: %d and actual: %d are not matched\n"), 1, oColNullable);
                        ret = -1;
                    }
                    gettimeofday(&tv3,NULL);
                    intervalTimes += (tv3.tv_sec*1000 + tv3.tv_usec/1000) - (tv2.tv_sec*1000 + tv2.tv_usec/1000);
                    cols++;
                }
                gettimeofday(&tv2,NULL);
                LogMsgEx(fpLog, NONE, _T("-----------------------------------------------------------\n"));
                LogMsgEx(fpLog, TIME_STAMP, _T("Execution SQLColumns: %d (ms)   table:%s\n"), 
                                (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000) - intervalTimes, 
                                pTable);
                LogMsgEx(fpLog, NONE, _T("-----------------------------------------------------------\n"));
            }
        }
        else{
            LogMsgEx(fpLog, NONE, _T("SQLColumns fail.\n"));
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        
        SQLFreeStmt(hstmt,SQL_UNBIND);
		SQLFreeStmt(hstmt,SQL_CLOSE);
    }
    
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}

