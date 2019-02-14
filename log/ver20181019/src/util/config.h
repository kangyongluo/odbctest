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
        sODBCTestData *getTestData(void);
        int getTableName(char *ddl, char *name, int size);
        int init(char *pFile);
        int scanTestSqlFile(void);
        int scanTestTable(void);
        void reset(void);
    private:
        int readTable(char *pSection, sTestTableInfo *psTestTableInfo);
        int readLoadInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readRowsetInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readLobInfo(char *pSection, sLoadDataInfo *psLoadDataInfo);
        int readStmt(char *pSection, sSqlStmt *psStmtInfo);
        int readSelect(char *pSection, sSqlStmt *psStmtInfo);
        int readSqlFile(char *pSection, sSqlStmt *psSqlFileInfo);
        int findSQL(FILE *fp, char *sql, unsigned int nsize);
        void setDefaultTable(void);
    private:
        FILE *mFpSql;
        int mFd;
        int mFdSqlData;
        int numTableSet;
        int iSqlStat;
        int mSqlSet;
        sODBCTestData *pODBCTestData;
        BOOL isTableDefault;
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
#ifdef __cplusplus
}
#endif

#endif /*__CONFIG_H__*/
