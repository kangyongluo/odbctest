#include "../util/util.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../driver/odbcdriver.h"
#include "../case/testCase.h"

#define CONFIG_SQL_TABLES_VIEW_TABLE_HEAD                  "VIEW"

typedef struct sResultSQLTables_
{
    char szSQLTables[256];
    int ret;
    int timeSQLTables;
    int timeFetch;
    char szCatalog[128];
	char szSchema[128];
	char szTable[128];
	char szTableType[32];
    char szRemark[128];
}sResultSQLTables;

typedef struct SQLTablesParameterInfo_
{
    char szCatalog[64];
    char szSchema[64];
    char szTable[64];
    char szTableType[64];
    char *catalog;
    char *schema;
    char *table;
    char *tableType;
    SQLSMALLINT lenCatalog;
    SQLSMALLINT lenSchema;
    SQLSMALLINT lenTable;
    SQLSMALLINT lenTableType;
}SQLTablesParameterInfo;

static FILE *fpLog = NULL;
static BOOL gSQLTablesResultAll = FALSE;

static void LogMsg(unsigned int option, char *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    
    if(fpLog == NULL){
        fpLog = openLog("SQLTables");
    }
    if(fpLog != NULL){
        if(option == SHORTTIMESTAMP || option == TIME_STAMP){
            fprintf(fpLog, "%s", timeStr);
        }
        va_start(argsList,format);\
        vfprintf(fpLog, format, argsList);
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
static int initReportSQLTables(void)
{
    fprintf(fpLog, "%-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-10s %-160s\n", 
                "Test Name",
                "Catalog",
                "Schema",
                "Table Name",
                "Table Type",
                "Remarks",
                "time",
                "fetch",
                "result",
                "SQLTables"
                );

    return 0;
}

static int reportSQLTables(sResultSQLTables resultReport)
{
    fprintf(fpLog, "%-16s %-16s %-16s %-16s %-16s %-16s %-16d %-16d %-10s %-160s\n",
        "SQLTables",
        resultReport.szCatalog,
        resultReport.szSchema,
        resultReport.szTable,
        resultReport.szTableType,
        resultReport.szRemark,
        resultReport.timeSQLTables,
        resultReport.timeFetch,
        (resultReport.ret == 0) ? "pass" : "fail",
        resultReport.szSQLTables
        );

    return 0;
}
static int paraSQLTablesText(char *text, SQLTablesParameterInfo *parameter)
{
    char *start, *end, *strText;
#define CONFIG_SQLTABLES_FILE_NUM  13
    char *pFile[CONFIG_SQLTABLES_FILE_NUM] = {NULL};
    char szBuf[128] = {0};
    int num = 0;
    int i;
    
    if(text == NULL) return -1;
    strText = strdup(text);
    if(strText == NULL) return -1;
    
    parameter->catalog = NULL;
    parameter->schema= NULL;
    parameter->table = NULL;
    parameter->tableType = NULL;
    parameter->szCatalog[0] = 0;
    parameter->szSchema[0] = 0;
    parameter->szTable[0] = 0;
    parameter->szTableType[0] = 0;

    parameter->lenCatalog = 0;
    parameter->lenSchema = 0;
    parameter->lenTable = 0;
    parameter->lenTableType = 0;
    num = 0;
    start = strtok(strText, ",");
    while(start){
        if(num >= CONFIG_SQLTABLES_FILE_NUM) break;
        pFile[num++] = start;
        start = strtok(NULL, ",");
    }
    if(num < 9){
        free(strText);
        return -1;
    }
    
    if(strstr(pFile[0], "SQLTables") == NULL){
        free(strText);
        return -1;
    }
    if(strstr(pFile[1], "null") == NULL && strstr(pFile[1], "NULL") == NULL){
        start = end = pFile[1];
        end += strlen(pFile[1]) - 1;
        while(start){
                if((*start != ' ') && (*start != '"')) break;
                start++;
        }
        while(end){
                if(end == start) break;
                if(*end == '"'){
                        *end = 0;
                        break;
                }
                else if(*end != ' ') break;
                end--;
        }
        sprintf(parameter->szCatalog, "%s", start);
        parameter->catalog = parameter->szCatalog;
    }
    if(strstr(pFile[2], "SQL_NTS")){
        parameter->lenCatalog = SQL_NTS;
    }
    else{
        parameter->lenCatalog = strlen(parameter->szCatalog);
    }
    if(strstr(pFile[3], "null") == NULL && strstr(pFile[3], "NULL") == NULL){
        start = end = pFile[3];
        end += strlen(pFile[3]) - 1;
        while(start){
                if((*start != ' ') && (*start != '"')) break;
                start++;
        }
        while(end){
                if(end == start) break;
                if(*end == '"'){
                        *end = 0;
                        break;
                }
                else if(*end != ' ') break;
                end--;
        }
        sprintf(parameter->szSchema, "%s", start);
        parameter->schema = parameter->szSchema;
    }
    if(strstr(pFile[4], "SQL_NTS")){
        parameter->lenSchema = SQL_NTS;
    }
    else{
        parameter->lenSchema = strlen(parameter->szSchema);
    }
    if(strstr(pFile[5], "null") == NULL && strstr(pFile[5], "NULL") == NULL){
        start = end = pFile[5];
        end += strlen(pFile[5]) - 1;
        while(start){
                if((*start != ' ') && (*start != '"')) break;
                start++;
        }
        while(end){
                if(end == start) break;
                if(*end == '"'){
                        *end = 0;
                        break;
                }
                else if(*end != ' ') break;
                end--;
        }
        sprintf(parameter->szTable, "%s", start);
        parameter->table = parameter->szTable;
    }
    if(strstr(pFile[6], "SQL_NTS")){
        parameter->lenTable = SQL_NTS;
    }
    else{
        parameter->lenTable = strlen(parameter->szTable);
    }
    
    if(strstr(pFile[7], "null") == NULL && strstr(pFile[7], "NULL") == NULL){
        for(i = 7; i < num; i++){
            strcat(szBuf, pFile[i]);
            if(i < (num - 1)) strcat(szBuf, ",");
        }
        start = end = szBuf;
        end += strlen(szBuf) - 1;
        while(start){
                if((*start != ' ') && (*start != '"')) break;
                start++;
        }
        while(end){
                if(end == start) break;
                if(*end == '"'){
                        *end = 0;
                        break;
                }
                else if(*end != ' ') break;
                end--;
        }
        sprintf(parameter->szTableType, "%s", start);
        parameter->tableType = parameter->szTableType;
    }
    if(strstr(pFile[num - 1], "SQL_NTS")){
        parameter->lenTableType = SQL_NTS;
    }
    else{
        parameter->lenTableType = strlen(parameter->szTableType);
    }

    free(strText);
    
    return 0;
}
int runSQLTables(TestInfo *pTestInfo,
                    SQLTablesParameterInfo tablesPara,
                    FILE *fpLog)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    TCHAR *pCatalog, *pSchema,*pTable, *pTableType;
    SQLLEN lenOfCatalog, lenOfSchema, lenOfTable, lenOfTableType, lenOfRemark;
    TCHAR szCatalog[128];
	TCHAR szSchema[128];
	TCHAR szTable[128];
    TCHAR szTableType[128];
    TCHAR szRemark[128];
    char szTmp[128];
    int rows = 0;
    struct timeval tv1, tv2;
    sResultSQLTables mReport;

    memset(&mReport, 0, sizeof(sResultSQLTables));
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
    LogMsgEx(fpLog, NONE, "connect info:\n");
    addInfoSessionEx(hstmt, fpLog);
    LogMsgEx(fpLog, TIME_STAMP, "SQLTables(hstmt,");
    strcat(mReport.szSQLTables, "SQLTables(hstmt,");
    if(tablesPara.catalog == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0, ");
        strcat(mReport.szSQLTables, "NULL, 0, ");
    }
    else{
        if(tablesPara.lenCatalog != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", tablesPara.catalog, tablesPara.lenCatalog);
            sprintf(szTmp, "\"%s\", %d, ", tablesPara.catalog, tablesPara.lenCatalog);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s, ", tablesPara.catalog, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s, ", tablesPara.catalog, "SQL_NTS");
        }
        
        strcat(mReport.szSQLTables, szTmp);
    }
    if(tablesPara.schema == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0, ");
        strcat(mReport.szSQLTables, "NULL, 0, ");
    }
    else{
        if(tablesPara.lenSchema != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", tablesPara.schema, tablesPara.lenSchema);
            sprintf(szTmp, "\"%s\", %d, ", tablesPara.schema, tablesPara.lenSchema);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s, ", tablesPara.schema, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s, ", tablesPara.schema, "SQL_NTS");
        }
        strcat(mReport.szSQLTables, szTmp);
    }
    if(tablesPara.table == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0, ");
        strcat(mReport.szSQLTables, "NULL, 0, ");
    }
    else{
        if(tablesPara.lenTable != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", tablesPara.table, tablesPara.lenTable);
            sprintf(szTmp, "\"%s\", %d, ", tablesPara.table, tablesPara.lenTable);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s, ", tablesPara.table, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s, ", tablesPara.table, "SQL_NTS");
        }
        strcat(mReport.szSQLTables, szTmp);
    }
    if(tablesPara.tableType == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0)\n");
        strcat(mReport.szSQLTables, "NULL, 0)");
    }
    else{
        if(tablesPara.lenTableType != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d)\n", tablesPara.tableType, tablesPara.lenTableType);
            sprintf(szTmp, "\"%s\", %d)", tablesPara.tableType, tablesPara.lenTableType);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s)\n", tablesPara.tableType, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s)", tablesPara.tableType, "SQL_NTS");
        }
        strcat(mReport.szSQLTables, szTmp);
    }
#ifdef UNICODE
    pCatalog = NULL;
    pSchema = NULL;
    pTable = NULL;
    pTableType = NULL;
    if(tablesPara.catalog){
        charToTChar(tablesPara.catalog, szCatalog);
        pCatalog = szCatalog;
    }
    if(tablesPara.schema){
        charToTChar(tablesPara.schema, szSchema);
        pSchema = szSchema;
    }
    if(tablesPara.table){
        charToTChar(tablesPara.table, szTable);
        pTable = szTable;
    }
    if(tablesPara.tableType){
        charToTChar(tablesPara.tableType, szTableType);
        pTableType = szTableType;
    }
#else
    pCatalog = tablesPara.catalog;
    pSchema = tablesPara.schema;
    pTable = tablesPara.table;
    pTableType = tablesPara.tableType;
#endif
    gettimeofday(&tv1,NULL);
    retcode = SQLTables(hstmt, 
                         (SQLTCHAR*)pCatalog,
                         (SWORD)tablesPara.lenCatalog,
                         (SQLTCHAR*)pSchema,
                         (SWORD)tablesPara.lenSchema,
                         (SQLTCHAR*)pTable,
                         (SWORD)tablesPara.lenTable,
                         (SQLTCHAR*)pTableType,
                         (SWORD)tablesPara.lenTableType);
    gettimeofday(&tv2,NULL);
    mReport.timeSQLTables = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
    if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, "execute SQLTables fail. line %d\n", __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        FullDisconnectEx(fpLog, &sTestInfo);
        return -1;
    }
   	retcode = SQLBindCol(hstmt, 1, SQL_C_TCHAR, szCatalog, sizeof(szCatalog) / sizeof(TCHAR), &lenOfCatalog);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, "SQLBindCol 1 fail.line %d\n", __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    retcode = SQLBindCol(hstmt, 2, SQL_C_TCHAR, szSchema, sizeof(szSchema) / sizeof(TCHAR), &lenOfSchema);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, "SQLBindCol 2 fail.line %d\n", __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
	retcode = SQLBindCol(hstmt, 3, SQL_C_TCHAR, szTable, sizeof(szTable) / sizeof(TCHAR),&lenOfTable);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, "SQLBindCol 3 fail.line %d\n", __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    retcode = SQLBindCol(hstmt, 4, SQL_C_TCHAR, szTableType, sizeof(szTableType)/sizeof(TCHAR),&lenOfTableType);
    if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, "SQLBindCol 4 fail.line %d\n", __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
	retcode = SQLBindCol(hstmt, 5, SQL_C_TCHAR,szRemark, sizeof(szRemark)/sizeof(TCHAR), &lenOfRemark);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, "SQLBindCol 5 fail.line %d\n", __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    szCatalog[0] = 0x00;
    szSchema[0] = 0x00;
    szTable[0] = 0x00;
    szTableType[0] = 0x00;
    szRemark[0] = 0x00;
	while (retcode == SQL_SUCCESS)
	{
	    gettimeofday(&tv1,NULL);
		retcode = SQLFetch(hstmt);
        gettimeofday(&tv2,NULL);
        mReport.timeFetch = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        
		if(retcode != SQL_SUCCESS && rows == 0)
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
        if((rows >= 1) && (gSQLTablesResultAll == FALSE)){
                    
            LogMsgEx(fpLog, TIME_STAMP, "Ignore more result set checksums!!!\n");
            break;
        }
        gettimeofday(&tv2,NULL);
        mReport.timeFetch = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
        if(rows == 0){
            LogMsgEx(fpLog, NONE, "---------------------------\n");
            LogMsgEx(fpLog, TIME_STAMP, "Check return result set:\n");
            LogMsgEx(fpLog, NONE,"colCatalog:%s\n",szCatalog);
        }
        tcharToChar(szCatalog, szTmp);
        if(tablesPara.catalog){
            if((strcmp(szTmp, tablesPara.catalog) != 0) && (strcmp(tablesPara.catalog, "%") != 0)) 
            {
                LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n", tablesPara.catalog, szTmp);
                ret = -1;
            }
        }
        tcharToChar(szSchema, szTmp);
        if(tablesPara.schema){
            if((strcmp(szTmp, tablesPara.schema) != 0) && (strcmp(tablesPara.schema, "%") != 0)) 
            {
                LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",tablesPara.schema, szTmp);
                ret = -1;
            }
        }
        tcharToChar(szTable, szTmp);
        if(tablesPara.table){
            if((strcmp(szTmp, tablesPara.table) != 0) && (strcmp(tablesPara.table, "%") != 0)) 
            {
                LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",tablesPara.table, szTmp);
                ret = -1;
            }
        }
        tcharToChar(szRemark, szTmp);
        LogMsgEx(fpLog, NONE, "Remark:%s\n", szTmp);
        LogMsgEx(fpLog, NONE, "------------------------------------\n");
        rows++;
        tcharToChar(szCatalog, szTmp);
        sprintf(mReport.szCatalog, "%s", szCatalog);
        tcharToChar(szSchema, szTmp);
        sprintf(mReport.szSchema, "%s", szTmp);
        tcharToChar(szTable, szTmp);
        sprintf(mReport.szTable, "%s", szTmp);
        tcharToChar(szTableType, szTmp);
        sprintf(mReport.szTableType, "%s", szTmp);
        tcharToChar(szRemark, szTmp);
        sprintf(mReport.szRemark, "%s", szTmp);
        mReport.ret = ret;
        reportSQLTables(mReport);
	}

    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}

int testSQLTables(testCaseInfo info)
{
    CConfig *pConfig = new CConfig;
    char *start, *end;
    SQLTablesParameterInfo mTablesPara;
    char *pTok;
    int len;
    int numCase, idCase;
#define CONFIG_SQLTABLES_CASES     20
    char *pTables[CONFIG_SQLTABLES_CASES] = {NULL};
    int totalPass, totalFail;
    int i;
    char szTmp[64] = {0};
    int testTime = 0, testCount = 0;
    char szStmt[2048] = {0};
    int digit;

    fpLog = INIT_LOGS("SQLTables");
    TESTCASE_BEGINW("begin SQLTables test...\n");
    if(pConfig == NULL){
        LogMsg(NONE, "Failed to create configuration object.\n");
        closeLog(fpLog);
        return -1;
    }

    totalPass = 0;
    totalFail = 0;
    sSqlStmt *pSqlStmt = new sSqlStmt;
    if(pSqlStmt == NULL){
        delete pConfig;
        return -1;
    }
    if(pConfig->readSQL(info.mSection.szSection, pSqlStmt) != 0){
        LogMsg(NONE, "Failed to read section %s.\n", info.mSection.szSection);
        delete pConfig;
        delete pSqlStmt;
        return -1;
    }
    LogMsg(NONE, "------------------------------------------------------------------------------------------------\n");
    LogMsg(NONE, "%s\n", pSqlStmt->szSql);
    LogMsg(NONE, "------------------------------------------------------------------------------------------------\n");
    testTime = (pSqlStmt->testTimes > 1) ? pSqlStmt->testTimes : 1;
    digit = pSqlStmt->range[0];
    for(testCount = 0; testCount < testTime; testCount++){
        memset(szStmt, 0, sizeof(szStmt));
        start = pSqlStmt->szSql;
        end = strstr(start, "_digit");
        if(end == NULL){
            sprintf(szStmt, "%s", pSqlStmt->szSql);
        }
        else{
            i = 0;
            sprintf(szTmp, "%d", digit);
            while(end){ 
                len = end - start;
                memcpy(&szStmt[i], start, len);
                strcat(szStmt, szTmp);
                i += strlen(szTmp);
                i += (SQLSMALLINT)len;
                start = end + strlen("_digit");
                end = strstr(start, "_digit");
            }
            strcat(szStmt, start);
        }
        if(digit < pSqlStmt->range[1]) digit++;
        else digit = pSqlStmt->range[0];
        numCase = 0;
        pTok = strtok(szStmt, ";");
        while(pTok){
            if(numCase >= CONFIG_SQLTABLES_CASES) break;
            pTables[numCase++] = pTok;
            pTok = strtok(NULL, ";");
        }
        if(totalFail == 0 && totalPass == 0) initReportSQLTables();
        for(idCase = 0; idCase < numCase; idCase++){
            if(paraSQLTablesText(pTables[idCase], &mTablesPara) == 0){
                if(runSQLTables(&info.mTestInfo, 
                            mTablesPara,
                            fpLog) != 0){
                    totalFail++;
                }
                else{
                    totalPass++;
                }
            }
        }
    }
    LogMsg(NONE, "\nSummary: total pass cases:%d     total fail cases:%d\n", totalPass, totalFail);
    if(pConfig) delete pConfig;
    RELEASE_LOSG(fpLog);
    
    return 0;
}
