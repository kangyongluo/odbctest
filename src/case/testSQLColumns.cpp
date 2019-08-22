#include "../util/util.h"
#include "../util/config.h"
#include "../util/global.h"
#include "../driver/odbcdriver.h"
#include "../case/testCase.h"


typedef struct sResultSQLColumns_
{
    char szSQLColumns[256];
    int ret;
    int timeSQLColumns;
    int timeFetch;
    char szCatalog[128];
	char szSchema[128];
	char szTable[128];
	char szColName[128];
    char szTypeName[32];
    char szRemark[128];
    char szColumnDefault[64];
    char szIsNullable[64];
    char szSQLAccessMode[32];
    SQLINTEGER mColumnSize;
    SQLINTEGER mBufferLength;
    SQLINTEGER mCharOctetLength;
    SQLINTEGER mOrdinalPosition;
    SQLSMALLINT mDataType;
    SQLSMALLINT mDecimalDigits;
    SQLSMALLINT mNumPrecRadix;
    SQLSMALLINT mNullable;
    SQLSMALLINT mSQLDataType;
    SQLSMALLINT mDatetimeSubtypeCode;
}sResultSQLColumns;

typedef struct SQLColumnsParameterInfo_
{
    char szCatalog[64];
    char szSchema[64];
    char szTable[64];
    char szColumns[64];
    char *catalog;
    char *schema;
    char *table;
    char *columns;
    SQLSMALLINT lenCatalog;
    SQLSMALLINT lenSchema;
    SQLSMALLINT lenTable;
    SQLSMALLINT lenColumnName;
}SQLColumnsParameterInfo;

static FILE *fpLog = NULL;
static BOOL gSQLColumnsResultAll = FALSE;

static void LogMsg(unsigned int option, char *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    
    if(fpLog == NULL){
        fpLog = openLog("SQLColumns");
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
        printf("%s", timeStr);
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

static int initReportSQLColumns(void)
{
    fprintf(fpLog, "%-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-16s %-32s %-32s %-32s %-32s %-32s %-32s %-16s %-16s %-10s %-32s %-160s\n", 
                "Test Name",
                "Catalog",
                "Schema",
                "Table Name",
                "Column Name",
                "Data Type",
                "Type Name",
                "Column Size",
                "Buffer Length",
                "Decimal Digits",
                "Numeric Precision",
                "Nullable",
                "Remarks",
                "String Column Default",
                "SQL Data Type",
                "Datetime Sub Type Code",
                "Char Octet Length",
                "Ordinal Position",
                "String Is Nullable",
                "time",
                "fetch",
                "result",
                "mode",
                "SQLColumns"
                );

    return 0;
}

static int reportSQLColumns(sResultSQLColumns resultReport)
{
    fprintf(fpLog, "%-16s %-16s %-16s %-16s %-16s %-16d %-16s %-16d %-16d %-16d %-16d %-16d %-16s %-32s %-32d %-32d %-32d %-32d %-32s %-16d  %-16d %-10s %-32s %-160s\n",
        "SQLColumns",
        resultReport.szCatalog,
        resultReport.szSchema,
        resultReport.szTable,
        resultReport.szColName,
        resultReport.mDataType,
        resultReport.szTypeName,
        resultReport.mColumnSize,
        resultReport.mBufferLength,
        resultReport.mDecimalDigits,
        resultReport.mNumPrecRadix,
        resultReport.mNullable,
        resultReport.szRemark,
        resultReport.szColumnDefault,
        resultReport.mSQLDataType,
        resultReport.mDatetimeSubtypeCode,
        resultReport.mCharOctetLength,
        resultReport.mOrdinalPosition,
        resultReport.szIsNullable,
        resultReport.timeSQLColumns,
        resultReport.timeFetch,
        (resultReport.ret == 0) ? "pass" : "fail",
        resultReport.szSQLAccessMode,
        resultReport.szSQLColumns
        );

    return 0;
}
static int paraSQLColumnsText(char *text, SQLColumnsParameterInfo *parameter)
{
    char *start, *end, *strText;
#define CONFIG_SQLCOLUMNS_FILE_NUM  9
    char *pFile[CONFIG_SQLCOLUMNS_FILE_NUM] = {NULL};
    int num = 0;
    
    if(text == NULL) return -1;
    strText = strdup(text);
    if(strText == NULL) return -1;
    
    parameter->catalog = NULL;
    parameter->schema= NULL;
    parameter->table = NULL;
    parameter->columns = NULL;
    parameter->szCatalog[0] = 0;
    parameter->szSchema[0] = 0;
    parameter->szTable[0] = 0;
    parameter->szColumns[0] = 0;

    parameter->lenCatalog = 0;
    parameter->lenSchema = 0;
    parameter->lenTable = 0;
    parameter->lenColumnName = 0;
    num = 0;
    start = strtok(strText, ",");
    while(start){
        if(num >= CONFIG_SQLCOLUMNS_FILE_NUM) break;
        pFile[num++] = start;
        start = strtok(NULL, ",");
    }
    if(num < CONFIG_SQLCOLUMNS_FILE_NUM){
        free(strText);
        return -1;
    }
    if(strstr(pFile[0], "SQLColumns") == NULL){
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
        start = end = pFile[7];
        end += strlen(pFile[7]) - 1;
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
        sprintf(parameter->szColumns, "%s", start);
        parameter->columns = parameter->szColumns;
    }
    if(strstr(pFile[8], "SQL_NTS")){
        parameter->lenColumnName = SQL_NTS;
    }
    else{
        parameter->lenColumnName = strlen(parameter->szColumns);
    }

    free(strText);
    
    return 0;
}
static int runSQLColumns(TestInfo *pTestInfo,
                            SQLColumnsParameterInfo columnsPara,
                            SQLULEN accessMode,
                            FILE *fpLog)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    char szTmp[128];
    TCHAR   szCatalog[128];
	TCHAR   szSchema[128];
	TCHAR   szTable[128];
	TCHAR   szColumn[128];
    TCHAR szColumnDefault[128];
    TCHAR szIsNullable[128];
    TCHAR *pCatalog, *pSchema,*pTable, *pColumn;
	SWORD oColDataType;
	TCHAR   szColTypeName[128];
	SQLSMALLINT oColPrec;
	SQLSMALLINT oColLen;
	SQLSMALLINT oColScale;
	SQLSMALLINT oColRadix;
	SQLSMALLINT oColNullable;

    SQLINTEGER CharOctetLength;
    SQLINTEGER OrdinalPosition;

    SQLSMALLINT SQLDataType;
    SQLSMALLINT DatetimeSubtypeCode;

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
    SQLLEN lenColumnDefault;
    SQLLEN lenSQLDataType;
    SQLLEN lenDatetimeSubtypeCode;
    SQLLEN lenCharOctetLength;
    SQLLEN lenOrdinalPosition;
    SQLLEN lenIsNullable;
    struct timeval tv1, tv2, tv3;
    int intervalTimes = 0;
    sResultSQLColumns mReport;
    
    int rows = 0;

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
    
    LogMsgEx(fpLog, TIME_STAMP, "SQLSetConnectOption((SQLHANDLE)hdbc, SQL_ACCESS_MODE, %s)\n",
            (accessMode == SQL_MODE_READ_WRITE) ? "SQL_MODE_READ_WRITE" : "SQL_MODE_READ_ONLY");
    sprintf(mReport.szSQLAccessMode, "%s", (accessMode == SQL_MODE_READ_WRITE) ? "SQL_MODE_READ_WRITE" : "SQL_MODE_READ_ONLY");
    SQLSetConnectOption((SQLHANDLE)hdbc, 
                            SQL_ACCESS_MODE, 
                            (accessMode == SQL_MODE_READ_WRITE) ? SQL_MODE_READ_WRITE : SQL_MODE_READ_ONLY);
    LogMsgEx(fpLog, TIME_STAMP, "");
    LogMsgEx(fpLog, NONE, "SQLColumns(hstmt,");
    sprintf(mReport.szSQLColumns, "%s", "SQLColumns(hstmt,");
    if(columnsPara.catalog == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0, ");
        strcat(mReport.szSQLColumns, "NULL, 0, ");
    }
    else{
        if(columnsPara.lenCatalog != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", columnsPara.catalog, columnsPara.lenCatalog);
            sprintf(szTmp, "\"%s\", %d, ", columnsPara.catalog, columnsPara.lenCatalog);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s, ", columnsPara.catalog, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s, ", columnsPara.catalog, "SQL_NTS");
        }
        
        strcat(mReport.szSQLColumns, szTmp);
    }
    if(columnsPara.schema == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0, ");
        strcat(mReport.szSQLColumns, "NULL, 0, ");
    }
    else{
        if(columnsPara.lenSchema != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", columnsPara.schema, columnsPara.lenSchema);
            sprintf(szTmp, "\"%s\", %d, ", columnsPara.schema, columnsPara.lenSchema);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s, ", columnsPara.schema, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s, ", columnsPara.schema, "SQL_NTS");
        }
        strcat(mReport.szSQLColumns, szTmp);
    }
    if(columnsPara.table == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0, ");
        strcat(mReport.szSQLColumns, "NULL, 0, ");
    }
    else{
        if(columnsPara.lenTable != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d, ", columnsPara.table, columnsPara.lenTable);
            sprintf(szTmp, "\"%s\", %d, ", columnsPara.table, columnsPara.lenTable);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s, ", columnsPara.table, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s, ", columnsPara.table, "SQL_NTS");
        }
        strcat(mReport.szSQLColumns, szTmp);
    }
    if(columnsPara.columns == NULL){
        LogMsgEx(fpLog, NONE, "NULL, 0)\n");
        strcat(mReport.szSQLColumns, "NULL, 0)");
    }
    else{
        if(columnsPara.lenColumnName != SQL_NTS){
            LogMsgEx(fpLog, NONE, "\"%s\", %d)\n", columnsPara.columns, columnsPara.lenColumnName);
            sprintf(szTmp, "\"%s\", %d)", columnsPara.columns, columnsPara.lenColumnName);
        }
        else{
            LogMsgEx(fpLog, NONE, "\"%s\", %s)\n", columnsPara.columns, "SQL_NTS");
            sprintf(szTmp, "\"%s\", %s)", columnsPara.columns, "SQL_NTS");
        }
        strcat(mReport.szSQLColumns, szTmp);
    }
#ifdef UNICODE
    pCatalog = NULL;
    pSchema = NULL;
    pTable = NULL;
    pColumn = NULL;
    if(columnsPara.catalog){
        charToTChar(columnsPara.catalog, szCatalog);
        pCatalog = szCatalog;
    }
    if(columnsPara.schema){
        charToTChar(columnsPara.schema, szSchema);
        pSchema = szSchema;
    }
    if(columnsPara.table){
        charToTChar(columnsPara.table, szTable);
        pTable = szTable;
    }
    if(columnsPara.columns){
        charToTChar(columnsPara.columns, szColumn);
        pColumn = szColumn;
    }
#else
    pCatalog = columnsPara.catalog;
    pSchema = columnsPara.schema;
    pTable = columnsPara.table;
    pColumn = columnsPara.columns;
#endif
    gettimeofday(&tv1,NULL);
    intervalTimes = 0;
    retcode = SQLColumns(hstmt, 
                         (SQLTCHAR*)pCatalog,
                         (SWORD)columnsPara.lenCatalog,
                         (SQLTCHAR*)pSchema,
                         (SWORD)columnsPara.lenSchema,
                         (SQLTCHAR*)pTable,
                         (SWORD)columnsPara.lenTable,
                         (SQLTCHAR*)pColumn,
                         (SWORD)columnsPara.lenColumnName);
    gettimeofday(&tv2,NULL);
    mReport.timeSQLColumns = (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000);
    
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
		retcode = SQLBindCol(hstmt,4,SQL_C_TCHAR, szColumn, sizeof(szColumn)/sizeof(TCHAR), &oColNamelen);
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
        retcode = SQLBindCol(hstmt, 13, SQL_C_CHAR, szColumnDefault, sizeof(szColumnDefault), &lenColumnDefault);
        if(SQL_SUCCESS != retcode){
            LogMsgEx(fpLog, NONE, "SQLBindCol 13 fail.line %d\n", __LINE__);
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        retcode = SQLBindCol(hstmt, 14, SQL_C_SSHORT, &SQLDataType, 0, &lenSQLDataType);
        if(SQL_SUCCESS != retcode){
            LogMsgEx(fpLog, NONE, "SQLBindCol 14 fail.line %d\n", __LINE__);
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        retcode = SQLBindCol(hstmt, 15, SQL_C_SSHORT, &DatetimeSubtypeCode, 0, &lenDatetimeSubtypeCode);
        if(SQL_SUCCESS != retcode){
            LogMsgEx(fpLog, NONE, "SQLBindCol 15 fail.line %d\n", __LINE__);
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        retcode = SQLBindCol(hstmt, 16, SQL_C_SLONG, &CharOctetLength, 0, &lenCharOctetLength);
        if(SQL_SUCCESS != retcode){
            LogMsgEx(fpLog, NONE, "SQLBindCol 16 fail.line %d\n", __LINE__);
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        retcode = SQLBindCol(hstmt, 17, SQL_C_SLONG, &OrdinalPosition, 0, &lenOrdinalPosition);
        if(SQL_SUCCESS != retcode){
            LogMsgEx(fpLog, NONE, "SQLBindCol 17 fail.line %d\n", __LINE__);
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        retcode = SQLBindCol(hstmt, 18, SQL_C_CHAR, szIsNullable, sizeof(szIsNullable), &lenIsNullable);
        if(SQL_SUCCESS != retcode){
            LogMsgEx(fpLog, NONE, "SQLBindCol 18 fail.line %d\n", __LINE__);
            LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
            ret = -1;
        }
        else{
            rows = 0;
            szCatalog[0] = 0;
            szSchema[0] = 0;
            szTable[0] = 0;
            szColumn[0] = 0;
            szColTypeName[0] = 0;
			oColDataType = 0;
			oColPrec = 0;
			oColLen = 0;
			oColScale = 0;
			oColRadix = 0;
			oColNullable = 0;

            szColumnDefault[0] = 0;
            SQLDataType = 0;
            DatetimeSubtypeCode = 0;
            CharOctetLength = 0;
            OrdinalPosition = 0;
            szRemark[0] = 0;
            while (retcode == SQL_SUCCESS)
			{
			    gettimeofday(&tv1,NULL);
				retcode = SQLFetch(hstmt);
				if(retcode != SQL_SUCCESS && rows == 0)
				{
				    LogMsgEx(fpLog, NONE, "%s:%d SQLFetch fail and no data found.\n", __FILE__, __LINE__);
					LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
					ret = -1;
                    break;
				}
                else if(retcode != SQL_SUCCESS)
				{
					break;
				}
                if((rows >= 1) && (gSQLColumnsResultAll == FALSE)){
                    
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
                if(columnsPara.catalog){
                    if((strcmp(szTmp, columnsPara.catalog) != 0) && (strcmp(columnsPara.catalog, "%") != 0)) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n", columnsPara.catalog, szTmp);
                        ret = -1;
                    }
                }
                tcharToChar(szSchema, szTmp);
                if(columnsPara.schema){
                    if((strcmp(szTmp, columnsPara.schema) != 0) && (strcmp(columnsPara.schema, "%") != 0)) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",columnsPara.schema, szTmp);
                        ret = -1;
                    }
                }
                tcharToChar(szTable, szTmp);
                if(columnsPara.table){
                    if((strcmp(szTmp, columnsPara.table) != 0) && (strcmp(columnsPara.table, "%") != 0)) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",columnsPara.table, szTmp);
                        ret = -1;
                    }
                }
                tcharToChar(szColumn, szTmp);
                if(columnsPara.columns){
                    if((strcmp(szTmp, columnsPara.columns) != 0) && (strcmp(columnsPara.columns, "%") != 0)) 
                    {
                        LogMsgEx(fpLog, NONE,"\texpect: %s and actual: %s are not matched\n",columnsPara.columns, szTmp);
                        ret = -1;
                    }
                }
                gettimeofday(&tv3,NULL);
                intervalTimes += (tv3.tv_sec*1000 + tv3.tv_usec/1000) - (tv2.tv_sec*1000 + tv2.tv_usec/1000);
                rows++;
                tcharToChar(szCatalog, szTmp);
                sprintf(mReport.szCatalog, "%s", szTmp);
                tcharToChar(szSchema, szTmp);
                sprintf(mReport.szSchema, "%s", szTmp);
                tcharToChar(szTable, szTmp);
                sprintf(mReport.szTable, "%s", szTmp);
                tcharToChar(szColumn, szTmp);
                sprintf(mReport.szColName, "%s", szTmp);
                mReport.mDataType = oColDataType;
                tcharToChar(szColTypeName, szTmp);
                sprintf(mReport.szTypeName, "%s", szTmp);
                mReport.mColumnSize = oColPrec;
                mReport.mBufferLength = oColLen;
                mReport.mDecimalDigits = oColScale;
                mReport.mNumPrecRadix = oColRadix;
                mReport.mNullable = oColNullable;
                tcharToChar(szRemark, szTmp);
                sprintf(mReport.szRemark, "%s", szTmp);
                tcharToChar(szColumnDefault, szTmp);
                sprintf(mReport.szColumnDefault, "%s", szTmp);
                mReport.mSQLDataType = SQLDataType;
                mReport.mDatetimeSubtypeCode = DatetimeSubtypeCode;
                mReport.mCharOctetLength = CharOctetLength;
                mReport.mOrdinalPosition = OrdinalPosition;
                tcharToChar(szIsNullable, szTmp);
                sprintf(mReport.szIsNullable, "%s", szTmp);
                mReport.ret = ret;
                reportSQLColumns(mReport);
            }
            
        }
    }
    else{
        LogMsgEx(fpLog, NONE, "SQLColumns fail.\n");
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}
int testSQLColumns(testCaseInfo info)
{
    CConfig *pConfig = new CConfig;
    char *start, *end;
    SQLColumnsParameterInfo mColumnsPara;
    char *pTok;
    int len;
    int iCase, numCase, idCase;
#define CONFIG_SQLCOLUMNS_CASES     20
    char *pColumns[CONFIG_SQLCOLUMNS_CASES] = {NULL};
    int totalPass, totalFail;
    int i;
    char szTmp[64] = {0};
    int testTime = 0, testCount = 0;
    char szStmt[1024] = {0};
    SQLULEN accessMode[2] = {SQL_MODE_READ_ONLY, SQL_MODE_READ_WRITE};
    int digit;

    fpLog = INIT_LOGS("SQLColumns");
    TESTCASE_BEGINW("begin SQLColumns test...\n");
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
            if(numCase >= CONFIG_SQLCOLUMNS_CASES) break;
            pColumns[numCase++] = pTok;
            pTok = strtok(NULL, ";");
        }
        if(totalFail == 0 && totalPass == 0) initReportSQLColumns();
        for(idCase = 0; idCase < numCase; idCase++){
            if(paraSQLColumnsText(pColumns[idCase], &mColumnsPara) == 0){
                for(iCase = 0; iCase < 2; iCase++){
                    if(runSQLColumns(&info.mTestInfo, 
                                mColumnsPara,
                                accessMode[iCase],
                                fpLog) != 0){
                        totalFail++;
                    }
                    else{
                        totalPass++;
                    }
                }
            }
        }
    }
    LogMsg(NONE, "\nSummary: total pass cases:%d     total fail cases:%d\n", totalPass, totalFail);
    
    if(pConfig) delete pConfig;
    if(pSqlStmt) delete pSqlStmt;
    RELEASE_LOSG(fpLog);
    
    return 0;
}

