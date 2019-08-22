#ifndef __TEST_CASE_H__
#define __TEST_CASE_H__

#include "../util/util.h"

#ifdef __cplusplus
extern "C" {
#endif


enum{
    DDL_LOAD_TYPE_BINDPARA = 0,
    DDL_LOAD_TYPE_PUTDATA,
    DDL_LOAD_TYPE_ROWSET,
    DDL_LOAD_TYPE_BLOB
};


#define CONFIG_BATCH_SIZE    5
#define CONDIG_NUM_BATCH     10


#define CONFIG_LOAD_TYPE_MAX    5
#define CONFIG_LOAD_THREAD_MAX  100

extern int testSQLColumns(testCaseInfo info);
extern int testSQLTables(testCaseInfo info);
extern int testLoadAtExec(testCaseInfo info);
extern int testLoadDirect(testCaseInfo info);
extern int testRowset(testCaseInfo info);
extern int testLobUpdate(testCaseInfo info);

#ifdef __cplusplus
}
#endif
#endif /*__TEST_CASE_H__*/
