#ifndef __TEST_CASE_H__
#define __TEST_CASE_H__
#ifdef __cplusplus
extern "C" {
#endif

extern PassFail testSQLColumns(TestInfo *pTestInfo);
extern PassFail testSQLTables(TestInfo *pTestInfo);

#ifdef __cplusplus
}
#endif
#endif /*__TEST_CASE_H__*/
