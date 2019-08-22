#include "config.h"
#include "global.h"

CTestGlobal gTestGlobal;

CTestGlobal::CTestGlobal(void)
{
    int i;

    mTotalSection = 0;
    for(i = 0 ; i < CONFIG_CFG_SECTION_NUM; i++)
        szSection[i] = NULL;
    initSection();
    init();
}
CTestGlobal::~CTestGlobal()
{
    int i;
    
    if (fpLog) fclose(fpLog);

    for(i = 0 ; i < CONFIG_CFG_SECTION_NUM; i++)
        if(szSection[i]) free(szSection[i]);
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
                    "SEABASE", 
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
void CTestGlobal::initSection(void)
{
    FILE *fp;
    char szBuf[1024] = {0};
    int len;
    char *pTok;
    char *szCfgSet[CONFIG_SECTION_SET_MAX];
    int i, numSection;
    char matchFlg = 0;
   
    numSection = 0;
    for(i = 0; i < CONFIG_SECTION_SET_MAX; i++) szCfgSet[i] = NULL;
    
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString("config",
                    "case_set",
                    "",
                    szBuf,
                    sizeof(szBuf) - 1,
                    ODBC_TEST_CFG_FILE
                    );
    numSection = 0;
    pTok = strtok(szBuf, ";");
    while(pTok){
        if(numSection >= CONFIG_SECTION_SET_MAX) break;
        szCfgSet[numSection++] = strdup(pTok);
        pTok = strtok(NULL, ";");
    }
    
    mTotalSection = 0;
    fp = fopen(ODBC_TEST_CFG_FILE, "r");
    if(fp){
        memset(szBuf, 0, sizeof(szBuf));
        while(fgets(szBuf, sizeof(szBuf) - 1, fp)){
            len = strlen(szBuf);
            if(szBuf[len - 1]== 0x0d || szBuf[len - 1]== 0x0a){
                szBuf[len - 1] = 0x00;
                len -=1;
                if(szBuf[len - 1]== 0x0d || szBuf[len - 1]== 0x0a){
                    szBuf[len - 1] = 0x00;
                    len -=1;
                }
            }
            if(len > 2){
                if(szBuf[0] == '[' && szBuf[len - 1] == ']'){
                    szBuf[len - 1] = 0x00;
                    if(numSection > 0){
                        matchFlg = 0;
                        for(i = 0; i < numSection; i++){
                            if(strcmp(&szBuf[1], szCfgSet[i]) == 0){
                                szSection[mTotalSection++] = strdup(szCfgSet[i]);
                                matchFlg = 1;
                                break;
                            }
                        }
                        if(matchFlg == 0){
                            for(i = 0; i < numSection; i++){
                                if(sectionMatch(szCfgSet[i], &szBuf[1])){
                                    szSection[mTotalSection++] = strdup(&szBuf[1]);
                                    break;
                                }
                            }
                        }
                    }
                    else{
                        szSection[mTotalSection++] = strdup(&szBuf[1]);
                    }
                }
            }
            if(mTotalSection >= CONFIG_CFG_SECTION_NUM) break;          
        }
        fclose(fp);

        for(i = 0; i < numSection; i++){
            if(szCfgSet[i]){
                free(szCfgSet[i]);
                szCfgSet[i] = NULL;
            }
        }
    }
}
int CTestGlobal::sectionMatch(char *src, char *dest)
{   
    int pos;
    
    if(strstr(src, "*") == NULL && strcmp(src, dest) != 0) return 0;
    if(strstr(src, "*") == NULL && strcmp(src, dest) == 0) return 1;
    if(*src == '*' && strlen(src) == 1) return 1;

    if(*src == '*' && (strcmp(src+1, dest) == 0)) return 1;
    pos = strlen(src);
    if(src[pos - 1]== '*' && (strncmp(src, dest, pos - 1) == 0)) return 1;
    
    return 0;
}
