#ifndef __ODBC_DRIVER_H__
#define __ODBC_DRIVER_H__

#include "../util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_BLOB_SIZE        16777218


typedef struct {
    int data[CONFIG_SQL_COLUMNS_MAX];
    int len[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN lenDataSize[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN lenTextSize[CONFIG_SQL_COLUMNS_MAX];
} sOffsetRowsetInfo;

typedef struct {
    unsigned char *addr;
    unsigned char *pColData[CONFIG_SQL_COLUMNS_MAX];
    int offsetAddr[CONFIG_SQL_COLUMNS_MAX];
    int offsetLen;
    SQLLEN *pDataLen[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN lenTextSize[CONFIG_SQL_COLUMNS_MAX];
    int batchSize;
    int totalBatch;
} sBindParamStruct;

#define CONFIG_SQL_TYPE_INFO_RESULT_MAX    19
typedef struct
{
    TCHAR szTypeName[32];
    SQLSMALLINT sqlType;
    SQLINTEGER columnSize;
    TCHAR szTypeAttr[CONFIG_SQL_TYPE_INFO_RESULT_MAX][32];
}sSQLTypeInfo;

typedef struct
{
    TCHAR szRgbDesc[128];
    SQLSMALLINT pcbDesc;
    SQLSMALLINT pfDesc;
    SQLLEN len;
}sSQLColAttributeInfo;

typedef struct
{
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
}sSQLColumnsInfo;

extern void LogAllErrors(SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt
                  );

extern void LogAllErrorsEx(FILE *fpLog,
                  SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt
                  );
extern TrueFalse FullConnectWithOptions(TestInfo *pTestInfo, int Options);
extern TrueFalse FullConnectWithOptionsEx(TestInfo *pTestInfo, int Options, TCHAR *connStrEx);
extern int FullDisconnectEx(FILE *fpLog, TestInfo *pTestInfo);
extern int addInfoSessionEx(SQLHSTMT hstmt, FILE *fpLog);
extern int cleanupTableData(TestInfo *pTestInfo, TCHAR *szTableName, FILE *fpLog);

//SQLColumns
extern int SQLColumnsEx(TestInfo *pTestInfo,
                            sTestTableInfo *psTestTableInfo,
                            TCHAR *pCatalog, 
                            TCHAR *pSchema, 
                            TCHAR *pTable, 
                            TCHAR *pColumnName,
                            FILE *fpLog);
extern int exeSQLColumns(SQLHANDLE hstmt,
                        sSQLColumnsInfo *pColInfo,
                        TCHAR *pCatalog, 
                        TCHAR *pSchema, 
                        TCHAR *pTable, 
                        TCHAR *pColumnName);
//SQLTables
extern int SQLTablesEx(TestInfo *pTestInfo,
                        sTestTableInfo *psTestTableInfo,
                        TCHAR *pCatalog, 
                        TCHAR *pSchema, 
                        TCHAR *pTable, 
                        TCHAR *pTableType,
                        BOOL isView,
                        FILE *fpLog
                        );


//SQLGetFunctions
extern int SQLGetFunctionsEx(TestInfo *pTestInfo, 
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported,
                            FILE *fpLog);
extern int SQLGetFunctionsEx_(SQLHANDLE henv,
                            SQLHANDLE hdbc,
                            SQLHANDLE hstmt,
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported,
                            FILE *fpLog);


extern int exeSQLGetTypeInfo(SQLHANDLE	hstmt, 
                                SQLSMALLINT sqlType, 
                                sSQLTypeInfo *pSqlTypeInfo);

extern int exeSQLColAttribute(SQLHANDLE    hstmt, 
                            SQLSMALLINT colNum, 
                            SQLUSMALLINT descType,
                            sSQLColAttributeInfo *pColAttr);
extern int exeSQLColAttributes(SQLHANDLE    hstmt, 
                            SQLSMALLINT colNum, 
                            SQLUSMALLINT descType,
                            sSQLColAttributeInfo *pColAttr);

extern int doSQLColAttribute(TestInfo *pTestInfo, 
                            SQLSMALLINT colNum);
extern int doSQLColAttributes(TestInfo *pTestInfo, 
                            SQLSMALLINT colNum);
//
extern int loadData(sLoadParamInfo *pLoadParam);



#ifdef __cplusplus
}
#endif

#endif /* __ODBC_DRIVER_H__ */
