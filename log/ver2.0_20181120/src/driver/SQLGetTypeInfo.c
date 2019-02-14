#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"


int exeSQLGetTypeInfo(SQLHANDLE hstmt, 
                            SQLSMALLINT sqlType, 
                            sSQLTypeInfo *pSqlTypeInfo)
{
    RETCODE retcode;
    int i;
    SQLLEN len;

    if(hstmt == SQL_NULL_HANDLE) return -1;

    retcode = SQLGetTypeInfo(hstmt, sqlType);
    if (retcode != SQL_SUCCESS){
        printf("[file %s line %d] end test\n", __FILE__, __LINE__);
		return -1;
	}
    retcode = SQLBindCol(hstmt, 1, SQL_C_TCHAR,
	                         (SQLPOINTER)pSqlTypeInfo->szTypeName,
	                         (SQLLEN) sizeof(pSqlTypeInfo->szTypeName), &len);
    if (retcode != SQL_SUCCESS){
        SQLFreeStmt(hstmt, SQL_UNBIND);
        SQLFreeStmt(hstmt, SQL_CLOSE);
		return -1;
	}
    retcode = SQLBindCol(hstmt, 2, SQL_C_SHORT,
                         (SQLPOINTER) &pSqlTypeInfo->sqlType,
                         (SQLLEN) sizeof(pSqlTypeInfo->sqlType), &len);
    if (retcode != SQL_SUCCESS){
        SQLFreeStmt(hstmt, SQL_UNBIND);
        SQLFreeStmt(hstmt, SQL_CLOSE);
		return -1;
	}
    retcode = SQLBindCol(hstmt, 3, SQL_C_ULONG,
                         (SQLPOINTER) &pSqlTypeInfo->columnSize,
                         (SQLLEN) sizeof(pSqlTypeInfo->columnSize), &len);
    if (retcode != SQL_SUCCESS){
        SQLFreeStmt(hstmt, SQL_UNBIND);
        SQLFreeStmt(hstmt, SQL_CLOSE);
		return -1;
	}
    for(i = 4; i <= CONFIG_SQL_TYPE_INFO_RESULT_MAX; i++){
        retcode = SQLBindCol(hstmt, i, SQL_C_TCHAR,
                         (SQLPOINTER) pSqlTypeInfo->szTypeAttr[i - 4],
                         (SQLLEN) sizeof(pSqlTypeInfo->szTypeAttr[i - 4]), &len);
        if (retcode != SQL_SUCCESS){
            SQLFreeStmt(hstmt, SQL_UNBIND);
            SQLFreeStmt(hstmt, SQL_CLOSE);
			return -1;
		}
    }
    while (SQLFetch(hstmt) == SQL_SUCCESS) {
        
    }
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);

    return 0;
}
