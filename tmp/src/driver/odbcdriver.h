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

//SQLBindParameter SQLPutData
extern int SQLBindParameterSqlNts(TestInfo *pTestInfo, 
                                      sTestTableInfo *psTestTableInfo,
                                      sLoadDataInfo *pLoadInfo, 
                                      TCHAR *pConnStrEx,
                                      FILE *fpLog);
extern int SQLBindParameterSQLLen(TestInfo *pTestInfo, 
                                  sTestTableInfo *psTestTableInfo,
                                  sLoadDataInfo *pLoadInfo, 
                                  TCHAR *pConnStrEx,
                                  FILE *fpLog);

extern int SQLPutDataSQLDataAtExec(TestInfo *pTestInfo, 
                                  sTestTableInfo *psTestTableInfo,
                                  sLoadDataInfo *pLoadInfo,
                                  TCHAR *pConnStrEx,
                                  FILE *fpLog);
extern int SQLPutDataSQLLenDataAtExec(TestInfo *pTestInfo, 
                                      sTestTableInfo *psTestTableInfo,
                                      sLoadDataInfo *pLoadInfo,
                                      TCHAR *pConnStrEx,
                                      FILE *fpLog);
//row set
extern int rowset(TestInfo *pTestInfo, 
                      sTestTableInfo *psTestTableInfo,
                      sLoadDataInfo *pLoadInfo, 
                      TCHAR *pConnStrEx,
                      FILE *fpLog);
extern int rowsetEx(TestInfo *pTestInfo, 
                      sTestTableInfo *psTestTableInfo,
                      sLoadDataInfo *pLoadInfo, 
                      TCHAR *pConnStrEx,
                      FILE *fpLog);

//SQLGetFunctions
extern int SQLGetFunctionsEx(TestInfo *pTestInfo, 
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported,
                            FILE *fpLog);
int SQLGetFunctionsEx_(SQLHANDLE henv,
                            SQLHANDLE hdbc,
                            SQLHANDLE hstmt,
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported,
                            FILE *fpLog);

//



#ifdef __cplusplus
}
#endif

#endif /* __ODBC_DRIVER_H__ */
