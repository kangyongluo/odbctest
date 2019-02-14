#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"


int SQLTablesEx(TestInfo *pTestInfo,
                    sTestTableInfo *psTestTableInfo,
                    TCHAR *pCatalog, 
                    TCHAR *pSchema, 
                    TCHAR *pTable, 
                    TCHAR *pTableType,
                    BOOL isView,
                    FILE *fpLog
                    )
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    SQLSMALLINT lenCatalog, lenSchema, lenTable, lenTableType;
    SQLLEN lenOfCatalog, lenOfSchema, lenOfTable, lenOfTableType, lenOfRemark;
    TCHAR szCatalog[128];
	TCHAR szSchema[128];
	TCHAR szTable[128];
    TCHAR szTableType[128];
    TCHAR szRemark[128];
    int cols = 0;
    struct timeval tv1, tv2;
    
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
    LogMsgEx(fpLog, TIME_STAMP, _T("SQLTables(hstmt,"));
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
    if(pTableType == NULL){
        LogMsgEx(fpLog, NONE, _T("NULL, 0)\n"));
        lenTableType = 0;
    }
    else{
        lenTableType = (SQLSMALLINT)_tcslen(pTableType);
        LogMsgEx(fpLog, NONE, _T("\"%s\", %d)\n"), pTableType, lenTableType);
    }
    gettimeofday(&tv1,NULL);
    retcode = SQLTables(hstmt, 
                         (SQLTCHAR*)pCatalog,
                         (SWORD)lenCatalog,
                         (SQLTCHAR*)pSchema,
                         (SWORD)lenSchema,
                         (SQLTCHAR*)pTable,
                         (SWORD)lenTable,
                         (SQLTCHAR*)pTableType,
                         (SWORD)lenTableType);
    if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, _T("execute SQLTables fail. line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        FullDisconnectEx(fpLog, &sTestInfo);
        return -1;
    }
   	retcode = SQLBindCol(hstmt, 1, SQL_C_TCHAR, szCatalog, sizeof(szCatalog) / sizeof(TCHAR), &lenOfCatalog);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, _T("SQLBindCol 1 fail.line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    retcode = SQLBindCol(hstmt, 2, SQL_C_TCHAR, szSchema, sizeof(szSchema) / sizeof(TCHAR), &lenOfSchema);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, _T("SQLBindCol 2 fail.line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
	retcode = SQLBindCol(hstmt, 3, SQL_C_TCHAR, szTable, sizeof(szTable) / sizeof(TCHAR),&lenOfTable);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, _T("SQLBindCol 3 fail.line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    retcode = SQLBindCol(hstmt, 4, SQL_C_TCHAR, szTableType, sizeof(szTableType)/sizeof(TCHAR),&lenOfTableType);
    if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, _T("SQLBindCol 4 fail.line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
	retcode = SQLBindCol(hstmt, 5, SQL_C_TCHAR,szRemark, sizeof(szRemark)/sizeof(TCHAR), &lenOfRemark);
	if(SQL_SUCCESS != retcode){
        LogMsgEx(fpLog, NONE, _T("SQLBindCol 5 fail.line %d\n"), __LINE__);
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
	while (retcode == SQL_SUCCESS)
	{
        szCatalog[0] = 0x00;
        szSchema[0] = 0x00;
        szTable[0] = 0x00;
        szTableType[0] = 0x00;
        szRemark[0] = 0x00;
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
        LogMsgEx(fpLog, NONE, _T("check ......\n"));
        LogMsgEx(fpLog, NONE,_T("colCatalog:%s\n"),szCatalog);
        if(_tcscmp(szCatalog, pTestInfo->Catalog) != 0) 
        {
            LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),pTestInfo->Catalog, szCatalog);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, _T("Schema:%s\n"), szSchema);
        if(_tcscmp(szSchema, pTestInfo->Schema) != 0) 
        {
            LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),pTestInfo->Schema, szSchema);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, _T("TableName:%s\n"), szTable);
        if(pTable != NULL){
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
        LogMsgEx(fpLog, NONE, _T("TableType:%s\n"), szTableType);
        if((isView == TRUE) && (_tcscmp(szTableType, _T("VIEW")) != 0)) 
        {
            LogMsgEx(fpLog, NONE,_T("\texpect: %s and actual: %s are not matched\n"),_T("VIEW"), szTableType);
            ret = -1;
        }
        LogMsgEx(fpLog, NONE, _T("Remark:%s\n"), szRemark);
        LogMsgEx(fpLog, NONE, _T("------------------------------------\n"));
        cols++;
	}
    gettimeofday(&tv2,NULL); 
    LogMsgEx(fpLog, TIME_STAMP, _T("Execution SQLTables: %d (ms)   table:%s       type:%s\n"),  
                (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000),
                pTable, 
                pTableType);
    LogMsgEx(fpLog, NONE, _T("------------------------------------\n"));
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}


