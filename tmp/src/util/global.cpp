#include "config.h"
#include "global.h"

CTestGlobal gTestGlobal;

CTestGlobal::CTestGlobal(void)
{
    init();
}
CTestGlobal::~CTestGlobal()
{
    if (fpLog) fclose(fpLog);
}
int CTestGlobal::init(void)
{
    fpLog = openLog("test_odbc");
    memset(&mcfg, 0, sizeof(mcfg));

    getConfigString(
                    "config", 
                    "CATALOG", 
                    "TRAFODION", 
                    mcfg.szCatalog, 
                    sizeof(mcfg.szCatalog), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(
                    "config", 
                    "DSN", 
                    "traf", 
                    mcfg.szDataSource, 
                    sizeof(mcfg.szDataSource), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(
                    "config", 
                    "user", 
                    "trafodion", 
                    mcfg.szUser, 
                    sizeof(mcfg.szUser), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(
                    "config", 
                    "password", 
                    "traf123", 
                    mcfg.szPassword, 
                    sizeof(mcfg.szPassword), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(
                    "config", 
                    "schema", 
                    "ODBCTEST", 
                    mcfg.szSchema, 
                    sizeof(mcfg.szSchema), 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(
                    "config", 
                    "isMultiTask", 
                    "FALSE", 
                    &mcfg.isMultiTask, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(
                    "config", 
                    "test_times", 
                    "1", 
                    &mcfg.testTimes, 
                    ODBC_TEST_CFG_FILE);
    getConfigString(
                    "config", 
                    "test_table", 
                    "", 
                    mcfg.szTable, 
                    sizeof(mcfg.szTable), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(
                    "config", 
                    "connect_string", 
                    "", 
                    mcfg.szConnStr, 
                    sizeof(mcfg.szConnStr), 
                    ODBC_TEST_CFG_FILE);

    return 0;
}

