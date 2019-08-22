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
    char szDataSource[128];
    char szUser[50];
    char szPassword[50];
    char szDatabase[50];
    char szCatalog[128];
    char szSchema[128];
    char szConnStr[128];
    BOOL isInitTable;
    int interval;
    BOOL isMultithreading;
    int totalThreads;
    char szTable[50];
    BOOL isConnectInfo;
    BOOL isMultiTask;
    int testTimes;
}sTestConfig;
#define  CONFIG_CFG_SECTION_NUM     256
#define CONFIG_SECTION_SET_MAX     50

class CTestGlobal
{
    public:
        CTestGlobal(void);
        ~CTestGlobal();
    public:
        int init(void);
    private:
        void initSection(void);
        int sectionMatch(char *src, char *dest);
    public:
        char *szSection[CONFIG_CFG_SECTION_NUM];
        int mTotalSection;
    public:
        sTestConfig mcfg;
        FILE *fpLog;
};

extern CTestGlobal gTestGlobal;

#endif/* __GLOBAL_H__ */
