#ifndef __TEST_SQL_H__
#define __TEST_SQL_H__

#include "../util/util.h"
#include "../util/global.h"

#define CONFIG_LONG_COLUMN_MAX  4
#define CONFIG_SELECT_STMT_MAX      10


typedef struct sqlResultSet_
{
    unsigned int id;
    int sqlTime;
    int sqlPrepareTime;
    int queryTime;
    int fetchTime;
    unsigned int rowsSecond;
    unsigned int microSecondsRows;
    unsigned int crcTime;
    unsigned int extraTime;
    unsigned int totalRows;
    unsigned int totalCols;
    SQLLEN minLen;
    SQLLEN maxLen;
    SQLSMALLINT sqlCType;
    char *szSQL;
    int ret;
}sqlResultSet;

class CTesSQL
{
    public:
        CTesSQL(void);
        CTesSQL(char *szKey);
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
        int testSqlBigObject(TestInfo *pTestInfo, 
                      sSqlStmt *psStmt,
                      sTestTableInfo *psTestTableInfo
                      );
        //When the result set of the test is relatively small, you can use this interface.
        int testSQL(TestInfo *pTestInfo, 
                      sSqlStmt *psSqlStmt);
        FILE *fileLogs(void);
    private:
        void putResultset(FILE *fpLog, sqlResultSet *resultset, BOOL head);
    private:
        FILE *fpLog;
    public:
        sTestConfig *pGlobalCfg;
};
#endif /*__TEST_SQL_H__*/
