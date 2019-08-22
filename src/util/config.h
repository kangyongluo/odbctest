#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "util.h"

#define ODBC_TEST_CFG_FILE     "./config.ini"
class CConfig
{
    public:
        CConfig(void);
        ~CConfig();
    public:
        int readTable(char *pSection, sTestTableInfo *psTestTableInfo);
        int readLoadDirectInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readRowsetInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readAtExecInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readLobUpdateInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readSQL(char *pSection, sSqlStmt *psStmtInfo);
        int readSQLFileInfo(char *pSection, sSqlStmt *psStmtInfo);
        int readSQLBigObject(char *pSection, sSqlStmt *psStmtInfo);
        int readSQLFile(char *pSection, sSqlStmt *psStmtInfo);
    private:
        
};

#define MAX_LEN_BYTES_OF_LINE 1024
#ifdef __cplusplus
extern "C" {
#endif

int getConfigString(const char *pSectionName,// section name
            				const char *pKeyName,        // key name
            				const char *pDefault,        // default string
            				char *pReturnedString,  // destination buffer
            				unsigned int nSize,              // size of destination buffer
            				const char *pFileName        // initialization file name
            				);
int getConfigBool(const char *pSectionName,// section name
            				const char *pKeyName,        // key name
            				const char *pDefault,        // default string
            				BOOL *pReturnedBool,  // destination buffer
            				const char *pFileName        // initialization file name
            				);
int getConfigInt(const char *pSectionName,// section name
            				const char *pKeyName,        // key name
            				const char *pDefault,        // default string
            				int *pReturnedInt,  // destination buffer
            				const char *pFileName        // initialization file name
            				);
#ifdef unixcli
#define GetPrivateProfileStringEx GetPrivateProfileString
#else
int GetPrivateProfileStringEx(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            char *pReturnedString,  // destination buffer
                            unsigned int nSize,              // size of destination buffer
                            const char *pFileName        // initialization file name
                            );
#endif

#ifdef __cplusplus
}
#endif

#endif /*__CONFIG_H__*/
