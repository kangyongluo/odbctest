#include "../util/util.h"
#include "../util/config.h"
#include "../case/testSQL.h"
#include "../driver/odbcdriver.h"


CTesSQL::CTesSQL(void)
{   
    fpLog = openLog("ddl");
    pGlobalCfg = &gTestGlobal.mcfg;
}
CTesSQL::CTesSQL(char *szKey)
{   
    char szBuf[128] = {0};

    sprintf(szBuf, "ddl_%s", szKey);
    fpLog = openLog(szBuf);
    pGlobalCfg = &gTestGlobal.mcfg;
}
FILE *CTesSQL::fileLogs(void)
{
    return fpLog;
}
CTesSQL::~CTesSQL()
{
    closeLog(fpLog);
}
int CTesSQL::checkColAttribute(TestInfo *pTestInfo, 
                                  sSqlStmt *psStmt,
                                  sTestTableInfo *psTestTableInfo,
                                  SQLSMALLINT colNum
                                  )
{
    SQLUSMALLINT    szDescType[] = {
					SQL_DESC_AUTO_UNIQUE_VALUE,
					SQL_DESC_CASE_SENSITIVE,
					SQL_DESC_COUNT,
					SQL_DESC_DISPLAY_SIZE,
					SQL_DESC_LENGTH,
					SQL_DESC_FIXED_PREC_SCALE,
					SQL_DESC_NULLABLE,
					SQL_DESC_PRECISION,
					SQL_DESC_SCALE,
					SQL_DESC_SEARCHABLE,
					SQL_DESC_TYPE,
					SQL_DESC_CONCISE_TYPE,
					SQL_DESC_UNSIGNED,
					SQL_DESC_UPDATABLE,
					SQL_DESC_NAME,
					SQL_DESC_TYPE_NAME,
					SQL_DESC_SCHEMA_NAME,
					SQL_DESC_CATALOG_NAME,
					SQL_DESC_TABLE_NAME,
					SQL_DESC_LABEL
					};
    char *szDescTypeStr[30] = {
        "SQL_DESC_AUTO_UNIQUE_VALUE",
        "SQL_DESC_CASE_SENSITIVE",
        "SQL_DESC_COUNT",
        "SQL_DESC_DISPLAY_SIZE",
        "SQL_DESC_LENGTH",
        "SQL_DESC_FIXED_PREC_SCALE",
        "SQL_DESC_NULLABLE",
        "SQL_DESC_PRECISION",
        "SQL_DESC_SCALE",
        "SQL_DESC_SEARCHABLE",
        "SQL_DESC_TYPE",
        "SQL_DESC_CONCISE_TYPE",
        "SQL_DESC_UNSIGNED",
        "SQL_DESC_UPDATABLE",
        "SQL_DESC_NAME",
        "SQL_DESC_TYPE_NAME",
        "SQL_DESC_SCHEMA_NAME",
        "SQL_DESC_CATALOG_NAME",
        "SQL_DESC_TABLE_NAME",
        "SQL_DESC_LABEL"
    };  
    SQLHANDLE henv = pTestInfo->henv;
    SQLHANDLE hdbc = pTestInfo->hdbc;
 	SQLHANDLE hstmt = pTestInfo->hstmt;
    sSQLColAttributeInfo mAttrInfo;
    int i;
    int ret = 0;
    
    if(hstmt == SQL_NULL_HANDLE){
        LogMsgEx(fpLog, NONE, "[%s %s %d]Invalid handle.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    for(i = 0; i < (sizeof(szDescType) / sizeof(szDescType[0])); i++){
        //LogMsgEx(fpLog, NONE, "SQLColAttribute ---> %s\n\n", szDescTypeStr[i]);
        memset(&mAttrInfo, 0, sizeof(mAttrInfo));
		if(exeSQLColAttribute(hstmt,
                            colNum + 1,
                            szDescType[i],
                            &mAttrInfo
                            ) != 0){
			LogMsgEx(fpLog, NONE, "SQLColAttribute fail\n");
			LogAllErrors(henv, hdbc, hstmt);
            ret = -1;
            continue;
		}
		else{
            if(szDescType[i] == SQL_DESC_NAME){
                if(strcmp(mAttrInfo.szRgbDesc, psTestTableInfo->szColName[colNum]) != 0){
                    LogMsgEx(fpLog, NONE, "SQL_DESC_NAME: cols:%d expect:%s actual:%s not match.\n", 
                                            colNum, 
                                            psTestTableInfo->szColName[colNum],
                                            mAttrInfo.szRgbDesc);
                    ret = -1;
                }
            }
            else if(szDescType[i] == SQL_DESC_TABLE_NAME){
                if(strcmp(mAttrInfo.szRgbDesc, psTestTableInfo->szTableName) != 0){
                    LogMsgEx(fpLog, NONE, "SQL_DESC_TABLE_NAME: cols:%d expect:%s actual:%s not match.\n", 
                                        colNum, 
                                        psTestTableInfo->szTableName,
                                        mAttrInfo.szRgbDesc);
                    ret = -1;
                }
            }
            else{
                LogMsgEx(fpLog, NONE, "%d,%d,%s\n", mAttrInfo.pcbDesc, mAttrInfo.len, mAttrInfo.szRgbDesc);
            }
		}
	}

    return ret;
}
int CTesSQL::checkColumns(TestInfo *pTestInfo, 
                                  sSqlStmt *psStmt,
                                  sTestTableInfo *psTestTableInfo,
                                  SQLSMALLINT colNum
                                  )
{
    SQLHANDLE henv = pTestInfo->henv;
    SQLHANDLE hdbc = pTestInfo->hdbc;
 	SQLHANDLE hstmt = pTestInfo->hstmt;
    sSQLColumnsInfo mColInfo;
    int ret = 0;
    char   szBuf[128];
    
    if(hstmt == SQL_NULL_HANDLE){
        LogMsgEx(fpLog, NONE, "[%s %s %d]Invalid handle.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    memset(&mColInfo, 0, sizeof(mColInfo));
    if(exeSQLColumns(hstmt, 
                        &mColInfo, 
                        pTestInfo->Catalog,
                        pTestInfo->Schema,
                        psTestTableInfo->szTableName,
                        psTestTableInfo->szColName[colNum]) != 0){
        LogMsgEx(fpLog, NONE, "call exeSQLColumns fail\n");
        LogAllErrors(henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        if(strcmp(mColInfo.szCatalog, pTestInfo->Catalog) != 0) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",pTestInfo->Catalog, mColInfo.szCatalog);
            ret = -1;
        }
        if(colNum == 0)
            LogMsgEx(fpLog, NONE, "Schema:%s\n", mColInfo.szSchema);
        if(strcmp(mColInfo.szSchema, pTestInfo->Schema) != 0) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",pTestInfo->Schema, mColInfo.szSchema);
            ret = -1;
        }
        if(colNum == 0)
            LogMsgEx(fpLog, NONE, "TableName:%s\n", mColInfo.szTable);
        if(strlen(psTestTableInfo->szTableName) > 0){
            if(strcmp(mColInfo.szTable, psTestTableInfo->szTableName) != 0) 
            {
                LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",psTestTableInfo->szTableName, mColInfo.szTable);
                ret = -1;
            }
        }
        else if(strcmp(mColInfo.szTable, psTestTableInfo->szTableName) != 0) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",psTestTableInfo->szTableName, mColInfo.szTable);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, "check columns %d ......\n", colNum+ 1);
        LogMsgEx(fpLog, NONE, "ColName:%s\n", mColInfo.szColname);
        if(strcmp(mColInfo.szColname, psTestTableInfo->szColName[colNum]) != 0) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",psTestTableInfo->szColName[colNum], mColInfo.szColname);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, "data type:%d\n", mColInfo.oColDataType);
        if(mColInfo.oColDataType != psTestTableInfo->sqlType[colNum]) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->sqlType[colNum], mColInfo.oColDataType);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, "ColTypeName:%s\n", mColInfo.szColTypeName);
        if(!isTypeName(psTestTableInfo->sqlType[colNum], mColInfo.szColTypeName)) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",
                            sqlTypeToSqltypeName(psTestTableInfo->sqlType[colNum], szBuf),
                            mColInfo.szColTypeName);
            ret = -1;
        }
        /*LogMsgEx(fpLog, NONE, "ColPrec:%d\n", mColInfo.oColPrec);
        if((psTestTableInfo->sqlType[colNum] == SQL_CHAR || psTestTableInfo->sqlType[colNum] == SQL_VARCHAR) && 
            (mColInfo.oColPrec != 0)){
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n", 0, mColInfo.oColPrec);
            ret = -1;
        }
        else if((psTestTableInfo->sqlType[colNum] == SQL_CHAR || psTestTableInfo->sqlType[colNum] == SQL_VARCHAR) && 
            (mColInfo.oColPrec == 0)){
            ;
        }
        else if(mColInfo.oColPrec != psTestTableInfo->columnSize[colNum]) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->columnSize[colNum],mColInfo.oColPrec);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, "ColLen:%d\n", mColInfo.oColLen);
        if((psTestTableInfo->sqlType[colNum] == SQL_CHAR || psTestTableInfo->sqlType[colNum] == SQL_VARCHAR) && 
            (mColInfo.oColLen != 0)){
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",0, mColInfo.oColLen);
            ret = -1;
        }
        else if((psTestTableInfo->sqlType[colNum] == SQL_CHAR || psTestTableInfo->sqlType[colNum] == SQL_VARCHAR) && 
            (mColInfo.oColPrec == 0)){
            ;
        }
        else if(mColInfo.oColLen != psTestTableInfo->columnSize[colNum]) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->columnSize[colNum], mColInfo.oColLen);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, "ColScale:%d\n", mColInfo.oColScale);
        if(mColInfo.oColScale != psTestTableInfo->decimalDigits[colNum]) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n",psTestTableInfo->decimalDigits[colNum], mColInfo.oColScale);
            ret = -1;
        }*/
        LogMsgEx(fpLog, NONE,"ColRadix:%d\n", mColInfo.oColRadix);
        LogMsgEx(fpLog, NONE, "ColNullable:%d\n", mColInfo.oColNullable);
        if(mColInfo.oColNullable != 1) 
        {
            LogMsgEx(fpLog, NONE,"\texpect: %d and actual: %d are not matched\n", 1, mColInfo.oColNullable);
            ret = -1;
        }
    }

    return ret;
}
int CTesSQL::testSqlBigObject(TestInfo *pTestInfo, 
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
    SQLLEN len;
    int idsql = 0;
    unsigned int crc = 0;
    unsigned int crc1 = 0;
    int initTime = 0;
    int ret = 0;
    char szBuf[512] = {0};
    unsigned char szHeadCharset[4] = {0};
    char szSelect[CONFIG_SELECT_STMT_MAX][256] = {0};
    char szSqlSelect[300] = {0};
    TCHAR szQuery[512] = {0};
    int totalSelect = 0;
    char *result;
    SQLINTEGER idCols = 0;
    unsigned int recvLen = CONFIG_GET_DATA_SIZE;
    sqlResultSet mResult;

    memset(&mResult, 0, sizeof(mResult));
    gettimeofday(&tv3,NULL);
    if((psStmt->lengRecv > 0) && (psStmt->lengRecv != recvLen)){
        recvLen = psStmt->lengRecv;
    }
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    
    sprintf(szBuf, "DSN=%s;UID=%s;PWD=%s;%s", 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pGlobalCfg->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pGlobalCfg->szConnStr, fpLog)){
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
   
    /*retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"set FETCHSIZE 1", SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsgEx(fpLog, NONE,"call \"set FETCHSIZE 1\" fail.\n");
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
    }*/

    LogMsgEx(fpLog, NONE, ".........................................\n");
    addInfoSessionEx(hstmt, fpLog);
    if(psStmt->isCheckColAttr){
        sSQLTypeInfo mTypeInfo;
        LogMsgEx(fpLog, NONE, "Call SQLGetTypeInfo to get type infomation......\n");
        if(psTestTableInfo->columNum == 0){
            LogMsgEx(fpLog, NONE, "Please configure parameter \"sql_type\", for example:sql_type   =SQL_VARCHAR\n");
        }
        for(idCols = 0; idCols < psTestTableInfo->columNum; idCols++){
            if(exeSQLGetTypeInfo(hstmt, psTestTableInfo->sqlType[idCols], &mTypeInfo) != 0){
                LogMsgEx(fpLog, NONE, "cols:%d call exeSQLGetTypeInfo fail.\n", idCols + 1);
                LogAllErrors(henv, hdbc, hstmt);
                ret = -1;
            }
            else{
                LogMsgEx(fpLog, NONE, "col:%d sql type:%s\n", idCols + 1, mTypeInfo.szTypeName);
                LogMsgEx(fpLog, NONE, "col:%d expect sql type:%d actual:%d\n", idCols + 1, psTestTableInfo->sqlType[idCols], mTypeInfo.sqlType);
                if(mTypeInfo.sqlType != psTestTableInfo->sqlType[idCols]){
                    LogMsgEx(fpLog, NONE, "Actual value does not match expected value\n");
                    ret = -1;
                }
                LogMsgEx(fpLog, NONE, "col:%d column size:%d\n", idCols + 1, mTypeInfo.columnSize);
                LogMsgEx(fpLog, NONE, "col:%d other attributes:\n", idCols + 1);
                for(i = 0; i < CONFIG_SQL_TYPE_INFO_RESULT_MAX; i++){
                    LogMsgEx(fpLog, NONE, "\t%s\n", mTypeInfo.szTypeAttr[i]);
                }
            }
        }
        LogMsgEx(fpLog, NONE, "Call SQLColumns to get columns infomation......\n");
        if(psTestTableInfo->columNum == 0){
            LogMsgEx(fpLog, NONE, "Please configure parameter \"table_name\" or \"sql_type\" or \"column_name\" , for example:column_name   =C1\n");
        }
        for(idCols = 0; idCols < psTestTableInfo->columNum; idCols++){
            if(checkColumns(&sTestInfo, 
                            psStmt, 
                            psTestTableInfo,
                            idCols) != 0){
                LogMsgEx(fpLog, NONE, "cols:%d check columns fail.\n", idCols + 1);
                ret = -1;
            }
        }
    }

    LogMsgEx(fpLog, NONE, ".........................................\n");
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(psStmt->szStmtCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", psStmt->szStmtCqd[i]);
            charToTChar(psStmt->szStmtCqd[i], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    pData = (unsigned char *)malloc(recvLen * sizeof(TCHAR) + 2);
    if(pData == NULL){
        LogMsgEx(fpLog, NONE, "malloc  fail: %d\n", __LINE__);
        FullDisconnectEx(fpLog, &sTestInfo);
        return -1;
    }
    gettimeofday(&tv4,NULL);
    initTime = (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
    memset(szSelect, 0, sizeof(szSelect));
    sprintf((char *)pData, "%s", psStmt->szSql);
    result = strtok((char *)pData, ";");
    totalSelect = 0;
    while( result != NULL ) {
        sprintf(szSelect[totalSelect++], result);
        result = strtok(NULL, ";");
        if(totalSelect >= CONFIG_SELECT_STMT_MAX) break;
    }

    for(idsql = 0; idsql < totalSelect; idsql++){
        sprintf(szSqlSelect, "%s", szSelect[idsql]);
        if(psStmt->isPrepare){
            LogMsgEx(fpLog, TIME_STAMP, "SQLPrepare(hstmt, \"%s\", SQL_NTS)\n", szSqlSelect);
        }
        else{
            LogMsgEx(fpLog, TIME_STAMP, "SQLExecDirect(hstmt, \"%s\", SQL_NTS)\n", szSqlSelect);
        }
        mResult.sqlPrepareTime = 0;
        gettimeofday(&tv1,NULL);
        if(psStmt->isPrepare){
            charToTChar(szSqlSelect, szQuery);
            retcode = SQLPrepare(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                gettimeofday(&tv2,NULL);
                mResult.sqlTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
                LogMsgEx(fpLog, TIME_STAMP, "Call prepare fail! exe time:%d (ms) stmt:%s\n", mResult.sqlTime, szSelect[idsql]);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                SQLFreeStmt(hstmt, SQL_UNBIND);
                SQLFreeStmt(hstmt, SQL_CLOSE);
                mResult.id = 1;
                mResult.fetchTime = 0;
                mResult.rowsSecond = 0;
                mResult.microSecondsRows = 0;
                mResult.totalCols = 0;
                mResult.ret = -1;
                mResult.sqlCType = psStmt->sqlCType[0];
                mResult.szSQL = szSqlSelect;
                putResultset(fpLog, &mResult, TRUE);
                ret = -1;
                continue;
            }
            gettimeofday(&tv2,NULL);
            mResult.sqlPrepareTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);;
            LogMsgEx(fpLog, TIME_STAMP, "SQLExecute(hstmt)\n");
            retcode = SQLExecute(hstmt);
            if(retcode != SQL_SUCCESS){
                gettimeofday(&tv2,NULL);
                mResult.sqlTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
                LogMsgEx(fpLog, TIME_STAMP, "Call SQLExecute fail: exe time:%d (ms)\n", mResult.sqlTime);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                SQLFreeStmt(hstmt, SQL_UNBIND);
                SQLFreeStmt(hstmt, SQL_CLOSE);
                ret = -1;
                
                mResult.id = 1;
                mResult.fetchTime = 0;
                mResult.rowsSecond = 0;
                mResult.microSecondsRows = 0;
                mResult.totalCols = 0;
                mResult.ret = ret;
                mResult.sqlCType = psStmt->sqlCType[0];
                mResult.szSQL = szSqlSelect;
                putResultset(fpLog, &mResult, TRUE);
                continue;
            }
        }
        else{
            charToTChar(szSelect[idsql], szQuery);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
            if(retcode != SQL_SUCCESS){
                gettimeofday(&tv2,NULL);
                mResult.sqlTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
                LogMsgEx(fpLog, NONE, "[%s:%d] Call SQLExecDirect fail: exe time:%d (ms)\n", 
                                            __FILE__,
                                            __LINE__,
                                            mResult.sqlTime);
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                SQLFreeStmt(hstmt, SQL_UNBIND);
                SQLFreeStmt(hstmt, SQL_CLOSE);
                ret = -1;
                
                mResult.id = 1;
                mResult.fetchTime = 0;
                mResult.rowsSecond = 0;
                mResult.microSecondsRows = 0;
                mResult.totalCols = 0;
                mResult.ret = ret;
                mResult.sqlCType = psStmt->sqlCType[0];
                mResult.szSQL = szSqlSelect;
                putResultset(fpLog, &mResult, TRUE);
                
                continue;
            }
        }
        gettimeofday(&tv2,NULL);
        mResult.queryTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        mResult.crcTime = 0;
        mResult.sqlTime = 0;
        mResult.extraTime = 0;
        mResult.totalRows = 0;
        cols = 0;
        mResult.minLen = 0;
        mResult.maxLen = 0;
        mResult.ret = 0;
        gettimeofday(&tv1,NULL);
        retcode = SQLFetch(hstmt);
        if(retcode == SQL_NO_DATA){
            gettimeofday(&tv2,NULL);
            mResult.fetchTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            LogMsgEx(fpLog, NONE, "[%s:%d] SQLFetch and no data found!!!\n", __FILE__, __LINE__); 
            ret = -1;
            mResult.id = 1;
            mResult.rowsSecond = 0;
            mResult.microSecondsRows = 0;
            mResult.totalCols = 0;
            mResult.ret = ret;
            mResult.sqlCType = psStmt->sqlCType[0];
            mResult.szSQL = szSqlSelect;
            putResultset(fpLog, &mResult, TRUE);
            continue;
        }
        retcode = SQLNumResultCols(hstmt, &cols);
        while (retcode == SQL_SUCCESS)
        {
            for (i = 1; i <= cols; i++)
            {
                gettimeofday(&tv3,NULL);
                gettimeofday(&tv4,NULL);
                if(psStmt->isCheckColAttr){
                    LogMsgEx(fpLog, NONE, "Call SQLColAttribute to get columns attribute......\n");
                    if(checkColAttribute(&sTestInfo, psStmt, psTestTableInfo, i - 1) != 0){
                        //LogMsgEx(fpLog, NONE, "cols:%d check column attribute fail.\n", i);
                    }
                }
                switch(psStmt->sqlCType[i-1]){
                case SQL_C_CHAR:
                    if(mResult.totalRows == 0) 
                        LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_CHAR, szBuf, %d, &len)\n", i, recvLen); 
                    gettimeofday(&tv4,NULL);
                case SQL_C_BINARY:
                    if((psStmt->sqlCType[i-1] == SQL_C_BINARY) && (mResult.totalRows == 0)) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_BINARY, szBuf, %d, &len)\n", i, recvLen);  
                case SQL_C_WCHAR:
                    if((psStmt->sqlCType[i-1] == SQL_C_WCHAR) && (mResult.totalRows == 0)) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_WCHAR, szBuf, %d, &len)\n", i, recvLen);  
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, recvLen, &len);
                    break;
                case SQL_C_BIT:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_BIT, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLCHAR), &len);
                    break;
                case SQL_C_STINYINT:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_STINYINT, ...)\n", i);
                case SQL_C_UTINYINT:
                    if((psStmt->sqlCType[i-1] == SQL_C_UTINYINT) && ( mResult.totalRows == 0)) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_UTINYINT, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLCHAR), &len);
                    break;
                case SQL_C_SSHORT:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_SSHORT, ...)\n", i);
                case SQL_C_USHORT:
                    if((psStmt->sqlCType[i-1] == SQL_C_USHORT) && ( mResult.totalRows == 0)) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_USHORT, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLSMALLINT), &len);
                    break;
                case SQL_C_SLONG:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_SLONG, ...)\n", i);
                case SQL_C_ULONG:
                    if((psStmt->sqlCType[i-1] == SQL_C_ULONG) && ( mResult.totalRows == 0)) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_ULONG, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLINTEGER), &len);
                    break;
                case SQL_C_SBIGINT:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_SBIGINT, ...)\n", i);
                case SQL_C_UBIGINT:
                    if((psStmt->sqlCType[i-1] == SQL_C_UBIGINT) && ( mResult.totalRows == 0)) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_UBIGINT, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLBIGINT), &len);
                    break;
                case SQL_C_FLOAT:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_FLOAT, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLREAL), &len);
                    break;
                case SQL_C_DOUBLE:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_DOUBLE, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQLDOUBLE), &len);
                    break;
                case SQL_C_TYPE_DATE:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_TYPE_DATE, ...)\n", i);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(DATE_STRUCT), &len);
                    break;
                case SQL_C_TYPE_TIME:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_TYPE_TIME, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(TIME_STRUCT), &len);
                    break;
                case SQL_C_TYPE_TIMESTAMP:
                    if( mResult.totalRows == 0) LogMsgEx(fpLog, TIME_STAMP, "SQLGetData(hstmt, %d, SQL_C_TYPE_TIMESTAMP, ...)\n", i);
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(TIMESTAMP_STRUCT), &len);
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
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, psStmt->sqlCType[i-1], (SQLPOINTER)pData, sizeof(SQL_INTERVAL_STRUCT), &len);
                    break;
                case SQL_C_NUMERIC:
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)pData, CONFIG_GET_DATA_SIZE - 1, &len);
                    break;
                case SQL_C_GUID:
                    gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)pData, CONFIG_GET_DATA_SIZE - 1, &len);
                    break;
                default:
                    if(mResult.totalRows == 0 && mResult.totalRows == 0) gettimeofday(&tv4,NULL);
                    retcode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)pData, recvLen, &len);
                    break;
                }
                mResult.extraTime += (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                if (retcode == SQL_SUCCESS){
                    if(mResult.maxLen == 0 && mResult.minLen == 0) mResult.maxLen = mResult.minLen = len;
                    if(mResult.maxLen <= len) mResult.maxLen = len;
                    else if(mResult.minLen > len) mResult.minLen = len;
                    //LogMsgEx(fpLog, NONE, "%s\n", pData);
                    gettimeofday(&tv3,NULL);
                    if((len > 10) &&
                        (psStmt->sqlCType[i - 1] == SQL_C_CHAR ||
                         psStmt->sqlCType[i - 1] == SQL_C_WCHAR ||
                         psStmt->sqlCType[i - 1] == SQL_C_BINARY)
                       ){
                        if((psStmt->isSaveFile) && (psStmt->saveColId[i - 1] == i)){
                            FILE *fp;
                            char szSaveFile[32] = {0};
                            if(i == 1){
                                sprintf(szSaveFile, "%s", psStmt->szSaveFile);
                            }
                            else{
                                sprintf(szSaveFile, "%s_%d", psStmt->szSaveFile, (i - 1));
                            }
                            fp = fopen(szSaveFile, "wb+");
                            if(fp){
                                if(strcmp(psStmt->szCharset, ENCODING_UCS2_BIG_ENDIAN) == 0){
                                    if(((psStmt->isCalcCrc == FALSE) && (pData[0] != 0xfe) && (pData[1] != 0xff)) ||
                                        ((psStmt->isCalcCrc == TRUE) && (pData[8] != 0xfe) && (pData[9] != 0xff))){
                                        szHeadCharset[0] = 0xfe;
                                        szHeadCharset[1] = 0xff;
                                        fwrite(szHeadCharset, 2, 1, fp);
                                    }
                                }
                                else if(strcmp(psStmt->szCharset, ENCODING_UCS2_LITTLE_ENDIAN) == 0){
                                    if(((psStmt->isCalcCrc == FALSE) && (pData[0] != 0xff) && (pData[1] != 0xfe)) ||
                                        ((psStmt->isCalcCrc == TRUE) && (pData[8] != 0xff) && (pData[9] != 0xfe))){
                                        szHeadCharset[0] = 0xff;
                                        szHeadCharset[1] = 0xfe;
                                        fwrite(szHeadCharset, 2, 1, fp);
                                    }
                                }
                                else if(strcmp(psStmt->szCharset, ENCODING_UTF8) == 0){
                                    if(((psStmt->isCalcCrc == FALSE) && (pData[0] != 0xef) && (pData[1] != 0xbb) && (pData[2] == 0xbf)) ||
                                        ((psStmt->isCalcCrc == TRUE) && (pData[8] != 0xef) && (pData[9] != 0xbb) && (pData[10] == 0xbf))){
                                        szHeadCharset[0] = 0xef;
                                        szHeadCharset[1] = 0xbb;
                                        szHeadCharset[1] = 0xbf;
                                        fwrite(szHeadCharset, 3, 1, fp);
                                    }
                                }
                                if(psStmt->isCalcCrc == FALSE){
                                    LogMsgEx(fpLog, NONE, "The size of %s is %d bytes.Charset is \"%s\"\n", szSaveFile, len, psStmt->szCharset);
                                    fwrite(pData, len, 1, fp);
                                }
                                else{
                                    LogMsgEx(fpLog, NONE, "The size of %s is %d bytes.Charset is \"%s\"\n", szSaveFile, len - 8, psStmt->szCharset);
                                    fwrite(&pData[8], len - 8, 1, fp);
                                }
                                fclose(fp);
                            }
                            else{
                                LogMsgEx(fpLog, NONE, "open % fail!\n", szSaveFile);
                            }
                        }
                        if(psStmt->isCalcCrc){
                            crc = calcCRC32(pData, len - 8);
                            crc1 = 0;
                            asciiToHex(&pData[len - 8], (unsigned char *)&crc1, 8);
                            if(crc != crc1){
                                LogMsgEx(fpLog, NONE, "row:%d columns:%d check crc fail, expect:0x%x  actual:0x%x, line = %d\n", 
                                                mResult.totalRows, i, crc, crc1, __LINE__);
                                ret = -1;
                            }
                        }
                        gettimeofday(&tv4,NULL);
                        mResult.crcTime += (tv4.tv_sec*1000 + tv4.tv_usec/1000) - (tv3.tv_sec*1000 + tv3.tv_usec/1000);
                    }
                    else if(len == -1){
                        LogMsgEx(fpLog, NONE, "Row:%d .The result of column %d is NULL\n", mResult.totalRows, i);
                    }
                }
                else if(retcode == SQL_NO_DATA){
                    LogMsgEx(fpLog, TIME_STAMP, "Row:%d .Column:%d Can not found data.\n", mResult.totalRows, i); 
                    ret = -1;
                    mResult.ret = -1;
                }
                else{
                    if(retcode == SQL_ERROR){
                        LogMsgEx(fpLog, TIME_STAMP, "Call SQLGetData fail! rows:%d column:%d th\n", mResult.totalRows, i); 
                        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                        ret = -1;
                        mResult.ret = -1;
                    }
                    //break;
                }
            }
#ifdef CLEANUP_KNOW_ERROR
            //bug:When the first column is empty, no data is returned, and then leads to core dump
            if((mResult.minLen == 0) && (mResult.maxLen == 0)){
                break;
            }
#endif
            mResult.totalRows++;
            retcode = SQLFetch(hstmt);
        }
        gettimeofday(&tv2,NULL);
        mResult.sqlTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        LogMsgEx(fpLog, TIME_STAMP, "SQLFreeStmt(hstmt, SQL_UNBIND)\n");
        SQLFreeStmt(hstmt, SQL_UNBIND);
        LogMsgEx(fpLog, TIME_STAMP, "SQLFreeStmt(hstmt, SQL_CLOSE)\n");
        SQLFreeStmt(hstmt, SQL_CLOSE);

        mResult.id = 1;
        mResult.fetchTime = (mResult.totalRows > 0) ? ((mResult.sqlTime - (mResult.crcTime + mResult.extraTime + mResult.queryTime + mResult.sqlPrepareTime))) : 0;
        mResult.rowsSecond = (mResult.sqlTime > mResult.crcTime) ? ((1000 * mResult.totalRows) / (mResult.sqlTime - mResult.crcTime)) : 0;
        mResult.microSecondsRows = (mResult.totalRows > 0) ? ((mResult.sqlTime - mResult.crcTime - mResult.extraTime) / mResult.totalRows) : 0;
        mResult.totalCols = cols;
        mResult.sqlCType = psStmt->sqlCType[0];
        mResult.szSQL = szSqlSelect;
        putResultset(fpLog, &mResult, FALSE);
    }
    
    if(pData) free(pData);
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}
int CTesSQL::testSQL(TestInfo *pTestInfo, sSqlStmt *psSqlStmt)
{
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    RETCODE retcode;
    TestInfo sTestInfo;
    struct timeval tv1, tv2, tvStart;
    SQLSMALLINT cols, i;
    SQLLEN len;
    int ret;
    char szSql[2048] = {0};
    char szTmp[64] = {0};
    TCHAR szQuery[2048] = {0};
    TCHAR szResult[CONFIG_TEST_RESULT_SIZE + 2] = {0};
    char szData[CONFIG_TEST_RESULT_SIZE + 2] = {0};
    char *result;
    sqlResultSet mResult;
    unsigned int options = 0;
   
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    memset(&mResult, 0, sizeof(mResult));
    gettimeofday(&tvStart,NULL);
    sprintf(szData, "DSN=%s;UID=%s;PWD=%s;%s", 
                        sTestInfo.DataSource, 
                        sTestInfo.UserID, 
                        sTestInfo.Password, 
                        pGlobalCfg->szConnStr);
    LogMsgEx(fpLog, NONE, "%s\n", szData);
    if(!FullConnectWithOptionsEx(&sTestInfo, CONNECT_ODBC_VERSION_3, pGlobalCfg->szConnStr, fpLog)){
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
    //SQLExecDirect(hstmt, (SQLTCHAR*)"set FETCHSIZE 1", SQL_NTS);
    LogMsgEx(fpLog, NONE, ".........................................\n");
    addInfoSessionEx(hstmt, fpLog);
    LogMsgEx(fpLog, NONE, ".........................................\n");
    ret = 0;
    
    for(i = 0; i < CONFIG_NUM_CQD_SIZE; i++){
        if(strlen(psSqlStmt->szStmtCqd[i]) > 0){
            LogMsgEx(fpLog, NONE, "%s\n", psSqlStmt->szStmtCqd[i]);
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)psSqlStmt->szStmtCqd[i], SQL_NTS);
            if(retcode != SQL_SUCCESS){
                LogMsgEx(fpLog, NONE, "set cqd fail !\n");
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt); 
            }
        }
        else{
            break;
        }
    }
    sprintf(szSql, "%s", psSqlStmt->szSql);
    result = strtok((char *)szSql, ";");
    do{
        LogMsgEx(fpLog, NONE, ".................................\n");
        LogMsgEx(fpLog, NONE, "sql:%s\n", result); 
        mResult.crcTime = 0;
        mResult.sqlTime = 0;
        mResult.extraTime = 0;
        mResult.totalRows = 0;
        cols = 0;
        mResult.minLen = 0;
        mResult.maxLen = 0;
        mResult.ret = 0;
        charToTChar(result, szQuery);
        gettimeofday(&tv1,NULL);
        retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
        gettimeofday(&tv2,NULL);
        mResult.queryTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        mResult.sqlTime = mResult.queryTime;
        mResult.szSQL = result;
        if(retcode != SQL_SUCCESS){
            ret = -1;
            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        }
        else{
            cols = 0;
            gettimeofday(&tv1,NULL);
            retcode = SQLFetch(hstmt);
            gettimeofday(&tv2,NULL);
            mResult.fetchTime += (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            retcode = SQLNumResultCols(hstmt, &cols);
            mResult.totalCols = cols;
            mResult.totalRows = 0;
            if(retcode != SQL_SUCCESS){
                LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
            }
            while (retcode == SQL_SUCCESS)
            {
                if(psSqlStmt->isCheckResult == TRUE){
                    if(mResult.totalRows == 0) options = 0;
                    else{
                        options = PRINT_OFF;
                        LogMsgEx(fpLog, NONE | options, "Result set of row %d\n", mResult.totalRows + 1);
                        LogMsgEx(fpLog, NONE | options, "---------------------------------------------\n");
                    }
                    for (i = 1; i <= cols; i++)
                    {
                        memset(szResult, 0, sizeof(szResult));
                        retcode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)szResult, sizeof(szResult) - 1, &len);
                        if (SQL_SUCCEEDED(retcode)){
                            if(mResult.maxLen == 0 && mResult.minLen == 0) mResult.maxLen = mResult.minLen = len;
                            if(mResult.maxLen <= len) mResult.maxLen = len;
                            else if(mResult.minLen > len) mResult.minLen = len;
                            tcharToChar(szResult, szData);
                            
                            LogMsgEx(fpLog, NONE | options, "rows:%d columns:%d value:%s\n", 
                                                    mResult.totalRows + 1,
                                                    i, 
                                                    szData);
                        }
                        else if(retcode == SQL_NO_DATA){
                            LogMsgEx(fpLog, TIME_STAMP, "Column:%d Can not found data.\n", i); 
                            ret = -1;
                        }
                        else if(i == 1){
                            LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
                            if(psSqlStmt->isResultNull == TRUE){
                                LogMsgEx(fpLog, NONE, "The expected result set is null.Ignore the error.\n");
                            }
                            else{
                                ret = -1;
                                LogMsgEx(fpLog, TIME_STAMP, "Can not get data...\n");
                            }
                            break;
                        }
                        else{
                            break;
                        }
                    }
                    if(options ==0) LogMsgEx(fpLog, NONE, "---------------------------------------------\n");
                }
                mResult.totalRows++;
                mResult.id++;
                mResult.sqlCType = SQL_C_TCHAR;
                mResult.rowsSecond = (mResult.sqlTime > mResult.crcTime) ? ((1000 * mResult.totalRows) / (mResult.sqlTime - mResult.crcTime)) : 0;
                mResult.microSecondsRows = (mResult.totalRows > 0) ? ((mResult.sqlTime - mResult.crcTime - mResult.extraTime) / mResult.totalRows) : 0;
                putResultset(fpLog, &mResult, (mResult.id == 1) ? TRUE : FALSE);
                gettimeofday(&tv1,NULL);
                retcode = SQLFetch(hstmt);
                gettimeofday(&tv2,NULL);
                mResult.fetchTime = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
            }
        }
        SQLFreeStmt(hstmt, SQL_UNBIND);
        LogMsgEx(fpLog, TIME_STAMP, "SQLFreeStmt(hstmt, SQL_CLOSE)\n");
        SQLFreeStmt(hstmt, SQL_CLOSE);
        result = strtok(NULL, ";");
    }while(result != NULL);
    
    LogMsgEx(fpLog, NONE, ".................................\n");
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}
void CTesSQL::putResultset(FILE *fpLog, sqlResultSet *resultset, BOOL head)
{
    char szBuf[512] = {0};
    unsigned int option = PRINT_OFF;
    
    if(head == TRUE){
        memset(szBuf, 0, sizeof(szBuf));
        strcat(szBuf, "| count |");
        strcat(szBuf, " sql(ms) |");
        strcat(szBuf, " prepare:(ms) |");
        strcat(szBuf, " query(ms) |");
        strcat(szBuf, " fetch(ms) |");
        strcat(szBuf, "   rows/s   |");
        strcat(szBuf, " ms/rows |");
        strcat(szBuf, "  crc(ms) |");
        strcat(szBuf, " extra(ms) |");
        strcat(szBuf, " total rows |");
        strcat(szBuf, " cols |");
        strcat(szBuf, " min(btyes) |");
        strcat(szBuf, " max(bytes) |");
        strcat(szBuf, "    c type    |");
        strcat(szBuf, " result |");
        strcat(szBuf, "     query     |");
        LogMsgEx(fpLog, NONE, "%s\n", szBuf);
        memset(szBuf, 0, sizeof(szBuf));
        option = 0;
    }
    else{
#ifdef unixcli
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\brow:%d  %s", 
                resultset->id, 
                (resultset->ret == 0) ? "pass" : "fail");
        fflush(stdout);
#endif
    }
    LogMsgEx(fpLog, NONE | option, "%-10d %-10d %-14d %-10d %-12d %-10d %-10d %-10d %-12d %-10d %-10d %-10d %-10d %-14s %-10s %s\n", 
                resultset->id,
                resultset->sqlTime,
                resultset->sqlPrepareTime,
                resultset->queryTime,
                resultset->fetchTime,
                resultset->rowsSecond,
                resultset->microSecondsRows,
                resultset->crcTime,
                resultset->extraTime,
                resultset->totalRows,
                resultset->totalCols,
                resultset->minLen,
                resultset->maxLen,
                SQLCTypeToChar(resultset->sqlCType, szBuf),
                (resultset->ret == 0) ? "passed" : "failed",
                resultset->szSQL);
}

