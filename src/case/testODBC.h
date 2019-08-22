#ifndef __TEST_ODBC_H__
#define __TEST_ODBC_H__


#include "../util/util.h"

#define CONFIG_MAX_TEST_CASE 256

typedef struct caseThreadPara_
{
    int testTimes;
    int interval;
    void *args;
    cfgSectionInfo sectionInfo;
    char szThreadId[16];
}caseThreadPara;


#define CONFIG_TOTAL_THREAD_NUM     64
#define CONFIG_TEST_CASES_MAX     20



#define CONFIG_ODBCTEST_CASE_SQL                    "testSQL"
#define CONFIG_ODBCTEST_CASE_SQL_BIG_OBJECT        "testSQLBigObject"
#define CONFIG_ODBCTEST_CASE_CLEANUP                "cleanupTable"
#define CONFIG_ODBCTEST_CASE_LOAD_DIRECT            "testLoadDirect"
#define CONFIG_ODBCTEST_CASE_LOAD_AT_EXEC           "testLoadAtExec"
#define CONFIG_ODBCTEST_CASE_LOB_UPDATE             "testLobUpdate"
#define CONFIG_ODBCTEST_CASE_ROWSET                 "testRowset"
#define CONFIG_ODBCTEST_CASE_SQLCOLUMNS             "SQLColumns"
#define CONFIG_ODBCTEST_CASE_SQLTABLES              "SQLTables"
#define CONFIG_ODBCTEST_CASE_SQL_FILE              "testSQLFile"


class CTestODBC
{
    public:
        CTestODBC(void);
        ~CTestODBC();
    public:
        void runCases(void);
        void runCasesSerial(void);
        void doLoadData(testCaseInfo caseInfo);
        void doSQLBigObject(testCaseInfo caseInfo);
        void doSQL(testCaseInfo caseInfo);
        void doCleanUpTable(testCaseInfo caseInfo);
        void doSQLColumns(testCaseInfo caseInfo);
        void doSQLTables(testCaseInfo caseInfo);
        void doSqlFile(testCaseInfo caseInfo);
        void doCase(cfgSectionInfo section);
    private:

};


#endif /*__TEST_ODBC_H__*/
