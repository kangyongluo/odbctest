#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"



int exeSQLColAttribute(SQLHANDLE    hstmt, 
                            SQLSMALLINT colNum, 
                            SQLUSMALLINT descType,
                            sSQLColAttributeInfo *pColAttr)
{
    RETCODE retcode;
    
    if(hstmt == SQL_NULL_HANDLE){
        printf("%s %s %d]Invalid handle.", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    retcode = SQLColAttribute(hstmt, 
                            (SQLUSMALLINT)colNum, 
                            descType, 
                            pColAttr->szRgbDesc, 
                            (SQLSMALLINT)sizeof(pColAttr->szRgbDesc), 
                            &pColAttr->pcbDesc, 
                            &pColAttr->len);
	if (retcode != SQL_SUCCESS){
		return -1;
	}
    
	return 0;
}
int exeSQLColAttributes(SQLHANDLE    hstmt, 
                            SQLSMALLINT colNum, 
                            SQLUSMALLINT descType,
                            sSQLColAttributeInfo *pColAttr)
{
    RETCODE retcode;
    
    if(hstmt == SQL_NULL_HANDLE){
        printf("%s %s %d]Invalid handle.", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    retcode = SQLColAttributes(hstmt, 
                            (SQLUSMALLINT)colNum, 
                            descType, 
                            pColAttr->szRgbDesc, 
                            (SQLSMALLINT)sizeof(pColAttr->szRgbDesc), 
                            &pColAttr->pcbDesc, 
                            &pColAttr->len);
	if (retcode != SQL_SUCCESS){
		return -1;
	}
    
	return 0;
}

int doSQLColAttribute(TestInfo *pTestInfo, 
                            SQLSMALLINT colNum)
{
	SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
	SQLSMALLINT i;
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
    TCHAR *szDescTypeStr[30] = {
        _T("SQL_DESC_AUTO_UNIQUE_VALUE"),
        _T("SQL_DESC_CASE_SENSITIVE"),
        _T("SQL_DESC_COUNT"),
        _T("SQL_DESC_DISPLAY_SIZE"),
        _T("SQL_DESC_LENGTH"),
        _T("SQL_DESC_FIXED_PREC_SCALE"),
        _T("SQL_DESC_NULLABLE"),
        _T("SQL_DESC_PRECISION"),
        _T("SQL_DESC_SCALE"),
        _T("SQL_DESC_SEARCHABLE"),
        _T("SQL_DESC_TYPE"),
        _T("SQL_DESC_CONCISE_TYPE"),
        _T("SQL_DESC_UNSIGNED"),
        _T("SQL_DESC_UPDATABLE"),
        _T("SQL_DESC_NAME"),
        _T("SQL_DESC_TYPE_NAME"),
        _T("SQL_DESC_SCHEMA_NAME"),
        _T("SQL_DESC_CATALOG_NAME"),
        _T("SQL_DESC_TABLE_NAME"),
        _T("SQL_DESC_LABEL")
    };  
    sSQLColAttributeInfo mAttrInfo;
    
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    hstmt = pTestInfo->hstmt;

    if(henv == SQL_NULL_HANDLE ||
        hdbc == SQL_NULL_HANDLE ||
        hstmt == SQL_NULL_HANDLE){
        printf("[%s %s %d]Invalid handle.", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    for(i = 0; i < (sizeof(szDescType) / sizeof(szDescType[0])); i++){
        printf("SQLColAttribute ---> %s\n\n", szDescTypeStr[i]);
        memset(&mAttrInfo, 0, sizeof(mAttrInfo));
		if(exeSQLColAttribute(hstmt,
                            i,
                            szDescType[i],
                            &mAttrInfo
                            ) != 0){
			printf("SQLColAttribute fail\n");
			LogAllErrors(henv, hdbc, hstmt);
            continue;
		}
		else{
			printf("%d,%d,%s\n", mAttrInfo.pcbDesc, mAttrInfo.len, mAttrInfo.szRgbDesc);
		}
	}
    
	return 0;
}
int doSQLColAttributes(TestInfo *pTestInfo, 
                            SQLSMALLINT colNum)
{
	SQLHANDLE 	henv = SQL_NULL_HANDLE;
 	SQLHANDLE 	hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE	hstmt = SQL_NULL_HANDLE;
	SQLSMALLINT i;

    TCHAR *szDescTypeStr[30] = {
        _T("SQL_COLUMN_AUTO_INCREMENT"),
        _T("SQL_COLUMN_CASE_SENSITIVE"),
        _T("SQL_COLUMN_COUNT"),// Numberic
        _T("SQL_COLUMN_DISPLAY_SIZE"),// Numberic
        _T("SQL_COLUMN_LENGTH"),// Numberic
        _T(" SQL_COLUMN_MONEY"),
        _T("SQL_COLUMN_NULLABLE"),// 
        _T("SQL_COLUMN_PRECISION"),//
        _T("SQL_COLUMN_SCALE"),// 
        _T("SQL_COLUMN_SEARCHABLE"),//
        _T(" SQL_COLUMN_TYPE"),// Numeric
        _T("SQL_COLUMN_UNSIGNED"),//
        _T("SQL_COLUMN_UPDATABLE"),//
        _T("SQL_COLUMN_NAME"),
        _T("SQL_COLUMN_TYPE_NAME"),
        _T("SQL_COLUMN_OWNER_NAME"),
        _T("SQL_COLUMN_QUALIFIER_NAME"),
        _T("SQL_COLUMN_TABLE_NAME"),
        _T("SQL_COLUMN_LABEL")
    };
    UWORD   szDescType[] = {
        SQL_COLUMN_AUTO_INCREMENT,
        SQL_COLUMN_CASE_SENSITIVE,
        SQL_COLUMN_COUNT,// Numberic
        SQL_COLUMN_DISPLAY_SIZE,// Numberic
        SQL_COLUMN_LENGTH,// Numberic
        SQL_COLUMN_MONEY,
        SQL_COLUMN_NULLABLE,// 
        SQL_COLUMN_PRECISION,//
        SQL_COLUMN_SCALE,// 
        SQL_COLUMN_SEARCHABLE,//
        SQL_COLUMN_TYPE,// Numeric
        SQL_COLUMN_UNSIGNED,//
        SQL_COLUMN_UPDATABLE,//
        SQL_COLUMN_NAME,
        SQL_COLUMN_TYPE_NAME,
        SQL_COLUMN_OWNER_NAME,
        SQL_COLUMN_QUALIFIER_NAME,
        SQL_COLUMN_TABLE_NAME,
        SQL_COLUMN_LABEL
    };  
    sSQLColAttributeInfo mAttrInfo;
    
    henv = pTestInfo->henv;
    hdbc = pTestInfo->hdbc;
    hstmt = pTestInfo->hstmt;

    if(henv == SQL_NULL_HANDLE ||
        hdbc == SQL_NULL_HANDLE ||
        hstmt == SQL_NULL_HANDLE){
        printf("[%s %s %d]Invalid handle.", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    for(i = 0; i < (sizeof(szDescType) / sizeof(szDescType[0])); i++){
        printf("SQLColAttributes ---> %s\n\n", szDescTypeStr[i]);
        memset(&mAttrInfo, 0, sizeof(mAttrInfo));
		if(exeSQLColAttributes(hstmt,
                            i,
                            szDescType[i],
                            &mAttrInfo
                            ) != 0){
			printf("SQLColAttributes fail\n");
			LogAllErrors(henv, hdbc, hstmt);
            continue;
		}
		else{
			printf("%d,%d,%s\n", mAttrInfo.pcbDesc, mAttrInfo.pfDesc, mAttrInfo.szRgbDesc);
		}
	}
    
	return 0;
}

