#ifndef __TEST_SQL_H__
#define __TEST_SQL_H__

#include "../util/util.h"
#include "../util/global.h"

#define CONFIG_LONG_COLUMN_MAX  4
#define CONFIG_SELECT_STMT_MAX      10

typedef struct {
    TCHAR sz16M[CONFIG_LONG_COLUMN_MAX][16777218];
    TCHAR sz200K[CONFIG_LONG_COLUMN_MAX][204802];
    TCHAR szBuf[CONFIG_SQL_COLUMNS_MAX][256];
    SQLLEN len[CONFIG_SQL_COLUMNS_MAX];
} insertDataPara;

enum{
    DDL_NULL = 0,
    DDL_SELECT,
    DDL_LOAD,
    DDL_FILE
};

enum{
    DDL_LOAD_TYPE_INSERT = 0,
    DDL_LOAD_TYPE_ROWSET,
    DDL_LOAD_TYPE_BLOB
};

#define CONFIG_LOAD_COLUMN_MAX  64
#define CONFIG_LOAD_DATA_SIZE   128

#define CONFIG_BATCH_SIZE    5
#define CONDIG_NUM_BATCH     10



typedef struct
{
    TestInfo *pTestInfo;
    sTestTableInfo *pTableInfo;
    sLoadDataInfo mLoadDataInfo;
    int retcode;
    int loadType;
    void *args;
    FILE *fpLog;
}loadThreadPara;

#define CONFIG_LOAD_TYPE_MAX    5
#define CONFIG_LOAD_THREAD_MAX  100

class CTesSQL
{
    public:
        CTesSQL(void);
        CTesSQL(TCHAR *szKey);
        ~CTesSQL();
    public:
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
        int doInsert(TestInfo *pTestInfo);
        int doSelect(TestInfo *pTestInfo);
        int doRowset(TestInfo *pTestInfo);
        int doLobLoad(TestInfo *pTestInfo);
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
