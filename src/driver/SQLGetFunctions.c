#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"
#include "../util/util.h"


int SQLGetFunctionsEx(TestInfo *pTestInfo, 
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported,
                            FILE *fpLog)
{
    int ret = 0;
    RETCODE retcode;
 	SQLHANDLE henv = SQL_NULL_HANDLE;
 	SQLHANDLE hdbc = SQL_NULL_HANDLE;
 	SQLHANDLE hstmt = SQL_NULL_HANDLE;
    TestInfo sTestInfo;
    struct timeval tv1, tv2;
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
        LogMsgEx(fpLog, NONE, _T("Unable to connect\n"));
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, _T("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n"), __LINE__);
        LogMsgEx(fpLog, NONE,_T("Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n"));
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,_T("SQLAllocStmt hstmt fail, line %d\n"), __LINE__);
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    
    LogMsgEx(fpLog, NONE, _T("connect info......\n"));
    addInfoSessionEx(hstmt, fpLog);

    LogMsgEx(fpLog, TIME_STAMP, _T("SQLGetFunctions(hdbc, %d, %s)\n"), 
                                                            FunctionId, 
                                                            (Supported == NULL) ? _T("NULL") : _T("Supported"));
    gettimeofday(&tv1,NULL);
    retcode = SQLGetFunctions((SQLHANDLE)hdbc, FunctionId, Supported);
    if(retcode != SQL_SUCCESS){
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("call SQLGetFunctions: %d (ms) FunctionId:%d, %s\n"), 
                                (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000), 
                                FunctionId,
                                (Supported == NULL) ? _T("NULL") : _T("Supported"));
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return ret;
}
int SQLGetFunctionsEx_(SQLHANDLE henv,
                            SQLHANDLE hdbc,
                            SQLHANDLE hstmt,
                            SQLUSMALLINT FunctionId, 
                            SQLUSMALLINT *Supported,
                            FILE *fpLog)
{
    int ret = 0;
    RETCODE retcode;
    struct timeval tv1, tv2;

    LogMsgEx(fpLog, NONE, _T("SQLGetFunctions(hdbc, %d, %s)\n"), FunctionId, (Supported == NULL) ? _T("NULL") : _T("Supported"));
    gettimeofday(&tv1,NULL);
    retcode = SQLGetFunctions((SQLHANDLE)hdbc, FunctionId, Supported);
    if(retcode != SQL_SUCCESS){
        LogAllErrorsEx(fpLog, henv,hdbc,hstmt);
        ret = -1;
    }
    gettimeofday(&tv2,NULL);
    LogMsgEx(fpLog, TIME_STAMP, _T("call SQLGetFunctions: %d (ms) FunctionId:%d, %s\n"), 
                                (tv2.tv_sec*1000 + tv2.tv_usec/1000) - (tv1.tv_sec*1000 + tv1.tv_usec/1000), 
                                FunctionId,
                                (Supported == NULL) ? _T("NULL") : _T("Supported"));
    
    return ret;
}


