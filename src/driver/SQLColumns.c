#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"



int SQLColumnsEx(TestInfo *pTestInfo,
                            sTestTableInfo *psTestTableInfo,
                            char *pCatalog, 
                            char *pSchema, 
                            char *pTable, 
                            char *pColumnName,
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
        LogMsgEx(fpLog, NONE, "Unable to connect\n");
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n", __LINE__);
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail, line %d\n", __LINE__);
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    
    LogMsgEx(fpLog, NONE, "connect info......\n");
    addInfoSessionEx(hstmt, fpLog);
    
    SQLSetConnectOption((SQLHANDLE)hdbc,SQL_ACCESS_MODE,SQL_MODE_READ_WRITE);
    for(i = 0; i < 2; i++){
        LogMsgEx(fpLog, TIME_STAMP, "SQLSetConnectOption((SQLHANDLE)hdbc, SQL_ACCESS_MODE, %s)\n",
                (i == 0) ? "SQL_MODE_READ_WRITE" : "SQL_MODE_READ_ONLY");
        SQLSetConnectOption((SQLHANDLE)hdbc, SQL_ACCESS_MODE, sSqlMode[i]);
        LogMsgEx(fpLog, TIME_STAMP, "");
        LogMsgEx(fpLog, NONE, "SQLColumns(hstmt,");
        if(pCatalog == NULL){
            LogMsgEx(fpLog, NONE, "NULL, 0, ");
            lenCatalog = 0;
        }
        else{
            lenCatalog = (SQLSMALLINT)strlen(pCatalog);
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", pCatalog, lenCatalog);
        }
        if(pSchema == NULL){
            LogMsgEx(fpLog, NONE, "NULL, 0, ");
            lenSchema = 0;
        }
        else{
            lenSchema = (SQLSMALLINT)strlen(pSchema);
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", pSchema, lenSchema);
        }
        if(pTable == NULL){
            LogMsgEx(fpLog, NONE, "NULL, 0, ");
            lenTable = 0;
        }
        else{
            lenTable = (SQLSMALLINT)strlen(pTable);
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", pTable, lenTable);
        }
        if(pColumnName == NULL){
            LogMsgEx(fpLog, NONE, "NULL, 0, ");
            lenColumnName = 0;
        }
        else{
            lenColumnName = (SQLSMALLINT)strlen(pColumnName);
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", pColumnName, lenColumnName);
        }
        LogMsgEx(fpLog, NONE, "\"%\", 1)\n");
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
                LogMsgEx(fpLog, NONE, "SQLBindCol 1 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,2,SQL_C_TCHAR, szSchema, sizeof(szSchema)/sizeof(TCHAR),&oTableOwnerlen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 2 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }

			retcode = SQLBindCol(hstmt,3,SQL_C_TCHAR, szTable, sizeof(szTable)/sizeof(TCHAR),&oTableNamelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 3 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,4,SQL_C_TCHAR, szColname, sizeof(szColname)/sizeof(TCHAR), &oColNamelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 4 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,5,SQL_C_SHORT, &oColDataType, 0,&oColDataTypelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 5 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,6,SQL_C_TCHAR, szColTypeName,sizeof(szColTypeName)/sizeof(TCHAR),&oColTypeNamelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 6 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,7,SQL_C_LONG, &oColPrec, 0,&oColPreclen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 7 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,8,SQL_C_LONG,&oColLen,0,&oColLenlen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 7 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,9,SQL_C_SHORT,&oColScale,0,&oColScalelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 8 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,10,SQL_C_SHORT,&oColRadix,0,&oColRadixlen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 10 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,11,SQL_C_SHORT,&oColNullable,0,&oColNullablelen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 11 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
			retcode = SQLBindCol(hstmt,12,SQL_C_TCHAR,szRemark,sizeof(szRemark)/sizeof(TCHAR),&oRemarklen);
			if(SQL_SUCCESS != retcode){
                LogMsgEx(fpLog, NONE, "SQLBindCol 12 fail.line %d\n", __LINE__);
                LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
                ret = -1;
            }
            else{
                cols = 0;
                while (retcode == SQL_SUCCESS)
				{
					strcpy(szCatalog,"");
					strcpy(szSchema,"");
					strcpy(szTable,"");
					strcpy(szColname,"");
					oColDataType = 0;
					strcpy(szColTypeName,"");
					oColPrec = 0;
					oColLen = 0;
					oColScale = 0;
					oColRadix = 0;
					oColNullable = 0;
					strcpy(szRemark,"");
                    
					retcode = SQLFetch(hstmt);
					if(retcode != SQL_SUCCESS && cols == 0)
					{
					    LogMsgEx(fpLog, NONE, "SQLFetch fail and no data found.\n");
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
                        LogMsgEx(fpLog, NONE, "---------------------------\n");
                        LogMsgEx(fpLog, TIME_STAMP, "Check return result set:\n");
                        LogMsgEx(fpLog, NONE,"colCatalog:%s\n",szCatalog);
                    }
                    if(strcmp(szCatalog, pTestInfo->Catalog) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",pTestInfo->Catalog, szCatalog);
                        ret = -1;
                    }
                    if(cols == 0)
                        LogMsgEx(fpLog, NONE, "Schema:%s\n", szSchema);
                    if(strcmp(szSchema, pTestInfo->Schema) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",pTestInfo->Schema, szSchema);
                        ret = -1;
                    }
                    if(cols == 0)
                        LogMsgEx(fpLog, NONE, "TableName:%s\n", szTable);
                    if((pTable != NULL) && (strlen(pTable) > 0)){
                        if(strcmp(szTable, pTable) != 0) 
                        {
                            LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",pTable, szTable);
                            ret = -1;
                        }
                    }
                    else if(strcmp(szTable, psTestTableInfo->szTableName) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",psTestTableInfo->szTableName,szTable);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, "check columns %d ......\n", cols + 1);
                    LogMsgEx(fpLog, NONE, "ColName:%s\n", szColname);
                    if(strcmp(szColname, psTestTableInfo->szColName[cols]) != 0) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",psTestTableInfo->szColName[cols], szColname);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, "data type:%d\n", oColDataType);
                    if(oColDataType != psTestTableInfo->sqlType[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->sqlType[cols], oColDataType);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, "ColTypeName:%s\n", szColTypeName);
                    if(!isTypeName(psTestTableInfo->sqlType[cols], szColTypeName)) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",
                                        sqlTypeToSqltypeName(psTestTableInfo->sqlType[cols], szBuf),
                                        szColTypeName);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, "ColPrec:%d\n", oColPrec);
                    if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColPrec != 0)){
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n", 0, oColPrec);
                        ret = -1;
                    }
                    else if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColPrec == 0)){
                        ;
                    }
                    else if(oColPrec != psTestTableInfo->columnSize[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->columnSize[cols],oColPrec);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, "ColLen:%d\n", oColLen);
                    if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColLen != 0)){
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",0,oColLen);
                        ret = -1;
                    }
                    else if((psTestTableInfo->sqlType[cols] == SQL_CHAR || psTestTableInfo->sqlType[cols] == SQL_VARCHAR) && 
                        (oColPrec == 0)){
                        ;
                    }
                    else if(oColLen != psTestTableInfo->columnSize[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->columnSize[cols],oColLen);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE, "ColScale:%d\n", oColScale);
                    if(oColScale != psTestTableInfo->decimalDigits[cols]) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->decimalDigits[cols],oColScale);
                        ret = -1;
                    }
                    LogMsgEx(fpLog, NONE,"ColRadix:%d\n", oColRadix);
                    LogMsgEx(fpLog, NONE, "ColNullable:%d\n", oColNullable);
                    if(oColNullable != 1) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n", 1, oColNullable);
                        ret = -1;
                    }
                    gettimeofday(&tv3,NULL);
                    intervalTimes += (tv3.tv_sec*1000 + tv3.tv_usec/1000) - (tv2.tv_sec*1000 + tv2.tv_usec/1000);
                    cols++;
                }
                gettimeofday(&tv2,NULL);
                LogMsgEx(fpLog, NONE, "-----------------------------------------------------------\n");
                LogMsgEx(fpLog, TIME_STAMP, "Execution SQLColumns: %d (ms)   table:%s\n", 
                                (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000) - intervalTimes, 
                                pTable);
                LogMsgEx(fpLog, NONE, "-----------------------------------------------------------\n");
            }
        }
        else{
            LogMsgEx(fpLog, NONE, "SQLColumns fail.\n");
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        
        SQLFreeStmt(hstmt,SQL_UNBIND);
		SQLFreeStmt(hstmt,SQL_CLOSE);
    }
    
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}
int exeSQLColumns(SQLHANDLE hstmt,
                        sSQLColumnsInfo *pColInfo,
                        char *pCatalog, 
                        char *pSchema, 
                        char *pTable, 
                        char *pColumnName)
{
    int ret = 0;
    RETCODE retcode;
    SQLSMALLINT lenCatalog, lenSchema, lenTable, lenColumnName;
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
    

    if(hstmt == SQL_NULL_HANDLE){
        printf("[%s %s %d]Invalid handle.", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    if(pCatalog == NULL){
        lenCatalog = 0;
    }
    else{
        lenCatalog = (SQLSMALLINT)strlen(pCatalog);
    }
    if(pSchema == NULL){
        lenSchema = 0;
    }
    else{
        lenSchema = (SQLSMALLINT)strlen(pSchema);
    }
    if(pTable == NULL){
        lenTable = 0;
    }
    else{
        lenTable = (SQLSMALLINT)strlen(pTable);
    }
    if(pColumnName == NULL){
        lenColumnName = 0;
    }
    else{
        lenColumnName = (SQLSMALLINT)strlen(pColumnName);
    }

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
        retcode = SQLBindCol(hstmt,
                            1, 
                            SQL_C_TCHAR, 
                            pColInfo->szCatalog, 
                            sizeof(pColInfo->szCatalog)/sizeof(TCHAR), 
                            &oTableQualifierlen);
        if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                            2,
                            SQL_C_TCHAR, 
                            pColInfo->szSchema, 
                            sizeof(pColInfo->szSchema)/sizeof(TCHAR),
                            &oTableOwnerlen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }

		retcode = SQLBindCol(hstmt,
                                3,
                                SQL_C_TCHAR, 
                                pColInfo->szTable, 
                                sizeof(pColInfo->szTable)/sizeof(TCHAR),
                                &oTableNamelen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                4,
                                SQL_C_TCHAR, 
                                pColInfo->szColname, 
                                sizeof(pColInfo->szColname)/sizeof(TCHAR), 
                                &oColNamelen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                5,
                                SQL_C_SHORT, 
                                &pColInfo->oColDataType, 
                                0,
                                &oColDataTypelen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                6,
                                SQL_C_TCHAR, 
                                pColInfo->szColTypeName,
                                sizeof(pColInfo->szColTypeName)/sizeof(TCHAR),
                                &oColTypeNamelen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                7,
                                SQL_C_LONG, 
                                &pColInfo->oColPrec, 
                                0,
                                &oColPreclen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                8,
                                SQL_C_LONG,
                                &pColInfo->oColLen,
                                0,
                                &oColLenlen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                9,
                                SQL_C_SHORT,
                                &pColInfo->oColScale,
                                0,
                                &oColScalelen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                10,
                                SQL_C_SHORT,
                                &pColInfo->oColRadix,
                                0,
                                &oColRadixlen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                11,
                                SQL_C_SHORT,
                                &pColInfo->oColNullable,
                                0,
                                &oColNullablelen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
		retcode = SQLBindCol(hstmt,
                                12,
                                SQL_C_TCHAR,
                                pColInfo->szRemark,
                                sizeof(pColInfo->szRemark)/sizeof(TCHAR),
                                &oRemarklen);
		if(SQL_SUCCESS != retcode){
            SQLFreeStmt(hstmt,SQL_UNBIND);
	        SQLFreeStmt(hstmt,SQL_CLOSE);
            return -1;
        }
        while (SQLFetch(hstmt) == SQL_SUCCESS) {
            
        }
        SQLFreeStmt(hstmt,SQL_UNBIND);
	    SQLFreeStmt(hstmt,SQL_CLOSE);
    }
    else{
        ret = -1;
    }
        
    return ret;
}

