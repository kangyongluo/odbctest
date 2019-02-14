#ifndef __TEST_ODBC_H__
#define __TEST_ODBC_H__


#include "../util/util.h"

#ifdef __cplusplus
//extern "C" {
#endif

#define CONFIG_MAX_TEST_CASE 256

typedef struct testThreadPara_
{
    TestInfo mTestInfo;
    char szTestCase[32];
    TCHAR szDdl[512];
    int testTimes;
    int interval;
}testThreadPara;
typedef struct testTaskInfo_
{
    char szTestName[32];
    BOOL isTest;
    char szSql[512];
    int testTimes;
}testTaskInfo;

typedef struct testCase_
{
    TCHAR szCase[32];
    PassFail (*run)(TestInfo *);
}testCase;
#define CONFIG_TOTAL_THREAD_NUM     64
class CTestODBC
{
    public:
        CTestODBC(void);
        ~CTestODBC();
    public:
        void runTest(void);
    private:
        testThreadPara mtestThreadPara;
#ifdef unixcli
        pthread_t mPth[CONFIG_TOTAL_THREAD_NUM];
#else
        HANDLE mPth[CONFIG_TOTAL_THREAD_NUM];
#endif
        BOOL mThStat[CONFIG_TOTAL_THREAD_NUM];
};




#ifdef __cplusplus
//}
#endif

#endif /*__TEST_ODBC_H__*/
