#ifndef __TEST_SQL_H__
#define __TEST_SQL_H__

#include "../util/util.h"
#include "../util/global.h"

#define CONFIG_LONG_COLUMN_MAX  4
#define CONFIG_SELECT_STMT_MAX      10

enum{
    DDL_NULL = 0,
    DDL_SELECT,
    DDL_LOAD,
    DDL_FILE
};



#define CONFIG_LOAD_COLUMN_MAX  64
#define CONFIG_LOAD_DATA_SIZE   128


class CTesSQL
{
    public:
        CTesSQL(void);
        CTesSQL(TCHAR *szKey);
        ~CTesSQL();
    public:
        int checkColAttribute(TestInfo *pTestInfo, 
                                  sSqlStmt *psStmt,
                                  sTestTableInfo *psTestTableInfo,
                                  SQLSMALLINT colNum
                                  );
        int checkColumns(TestInfo *pTestInfo, 
                                  sSqlStmt *psStmt,
                                  sTestTableInfo *psTestTableInfo,
                                  SQLSMALLINT colNum
                                  );
        //When the result set of the test is relatively large, for example, query the blob table.
        int testSelect(TestInfo *pTestInfo, 
                      sSqlStmt *psStmt,
                      sTestTableInfo *psTestTableInfo
                      );
        //When the result set of the test is relatively small, you can use this interface.
        int testSQL(TestInfo *pTestInfo, 
                      sSqlStmt *psSqlStmt, 
                      sTestTableInfo *psTestTableInfo);
        int doStmt(TestInfo *pTestInfo);
        int doSqlFile(TestInfo *pTestInfo);
        int doSqlTable(TestInfo *pTestInfo);
        int doSelect(TestInfo *pTestInfo);
        int doCleanupTable(TestInfo *pTestInfo);
        int doSqlTest(TestInfo *pTestInfo, char *pCase);
    private:
        int mBeginVal;
        TCHAR szLoadBuf[CONFIG_LOAD_COLUMN_MAX][CONFIG_LOAD_DATA_SIZE];
        BOOL isBindOk;
        FILE *fpLog;
    public:
        sTestConfig *pGlobalCfg;
};
#endif /*__TEST_SQL_H__*/
