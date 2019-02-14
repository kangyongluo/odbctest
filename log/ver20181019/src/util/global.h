#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef unixcli

#else
#include <windows.h>
#include <tchar.h>
#endif
#include <assert.h>


typedef struct
{
    TCHAR szDataSource[128];
    TCHAR szUser[50];
    TCHAR szPassword[50];
    TCHAR szDatabase[50];
    TCHAR szCatalog[128];
    TCHAR szSchema[128];
    TCHAR szConnStr[128];
    BOOL isInitTable;
    int interval;
    BOOL isMultithreading;
    char szTable[50];
    BOOL isConnectInfo;
    BOOL isMultiTask;
    int testTimes;
}sTestConfig;

class CTestGlobal
{
    public:
        CTestGlobal(void);
        ~CTestGlobal();
    public:
        int init(void);
    public:
        sTestConfig mcfg;
        FILE *fpLog;
};

extern CTestGlobal gTestGlobal;

#endif/* __GLOBAL_H__ */
