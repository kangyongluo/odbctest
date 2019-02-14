#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#ifdef unixcli
#include <unistd.h>
#include <sys/time.h>
#else
#include <windows.h>
#include <tchar.h>
#endif
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <assert.h>
#ifdef unixcli
#include <signal.h>
#include <pthread.h>   
#include <ctype.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern FILE *gLogFp;


#ifdef unixcli
#ifndef TCHAR
#define TCHAR	char//SQLTCHAR
#endif
#endif

typedef  struct tabTestInfo
{
    SQLHANDLE henv;
    SQLHDBC hdbc;
    SQLHANDLE hstmt;
    TCHAR DataSource[128];
    TCHAR Server[128];
    TCHAR Port[10 * 2];
    TCHAR UserID[128];
    TCHAR Password[50];
    TCHAR Database[50];
    TCHAR Catalog[128];
    TCHAR Schema[128];
}TestInfo;

#ifdef unixcli

//#ifndef UNIXODBC
//typedef char BOOL;
//#endif /*UNIXODBC*/


#ifndef UNICODE


typedef char CHAR;

#define _tprintf    printf
#define _ftprintf   fprintf
#define _vftprintf  vfprintf
#define _sprintf    printf
#define _stprintf   sprintf

/*** String operation functions ***/

#define _tcslen			strlen
#define _tcsdup			strdup
#define _tcscspn        strcspn
#define _tcstok         strtok
#define _tcsstr         strstr
#define _tcsncpy        strncpy
#define _stprintf		sprintf
#define _tcscpy			strcpy
#define _tcscat			strcat
#define _tcsicmp		strcmp
#define _strcasecmp		strcasecmp
#define _tcscmp			strcmp
#define _tcsncmp		strncmp
#define _tfopen         fopen
#define _tfclose	fclose
#define _tcsftime   strftime    
#define TFILE		FILE
#define __T(x)		x

#else
#define __T(x)		(SQLTCHAR *)(L##x)
#endif

#else
#define snprintf _snprintf
#ifdef _MSC_VER
#define strcasecmp stricmp
#define strncasecmp  strnicmp 
#endif /* _MSC_VER */

//#define _tfopen         fopen
#define _tfclose          fclose
#define TFILE             FILE
#ifdef UNICODE
#define _tcsicmp        wcscmp
#define _strcasecmp     wcscmp
#define _tcscmp         wcscmp
#define __T(x)		L##x
#else
#define _tcsicmp        _stricmp
#define _strcasecmp     _stricmp
#define _tcscmp         strcmp

#endif /*UNICODE */
#endif /*unixcli*/
#define _T(x)		__T(x)

/*
** Valid Values for LogMsg() <Options> parameter
** These can be easily used to make the LogMsg function display standard things
** like a time stamp and/or a standard error prefix like "***ERROR:" instead of
** you having to build those things.  More than one option may be used by
** ORing or adding options.  Example1: ERRMSG | TIME_STAMP	 Example 2: ERRMSG + LINES
*/ 
#define NONE            0x00     /* Don't use any options.  Equivalent to using LogPrintf() */
#define ERRMSG          0x01     /* Prefix the <ErrorString> to the front of the message */
#define TIME_STAMP      0x02     /* Display a timestamp on the line before the message */
#define LINEBEFORE      0x04     /* Display separating lines before and after the message */
#define LINEAFTER       0x08     /* Display a separating line after the message */
#define ENDLINE         LINEAFTER/* Some older programs used to use ENDLINE instead of LINEAFTER */
#define INTERNALERRMSG  0x10     /* Prefix ***INTERNAL ERROR: to the fromt of the message */
#define SHORTTIMESTAMP  0x20     /* Prefix a shorter timestamp to the front of the message */
#define INFO			0x30	 /* Will print only if debugMode = on */

typedef enum PassFail {PASSED, FAILED} PassFail;

#define TEST_DECLARE  //do{;}while(0)
#define TEST_INIT  //do{;}while(0)
#define TESTCASE_BEGIN(x) //LogMsg(SHORTTIMESTAMP+LINEBEFORE, x)
#define TESTCASE_BEGINW(x) //LogMsg(SHORTTIMESTAMP+LINEBEFORE, x)
#define TEST_FAILED  //LogMsg(SHORTTIMESTAMP, _T("<Test Fail>\n"))
#define TESTCASE_END  do{;}while(0)
#define TEST_RETURN  return PASSED
#define TESTCASE_LOG(x) //LogMsg(SHORTTIMESTAMP+LINEBEFORE,x)


#define CONNECT_AUTOCOMMIT_OFF        0x01    // Default is on.
#define CONNECT_ODBC_VERSION_2        0x02
#define CONNECT_ODBC_VERSION_3        0x04

#define MAX_SQLSTRING_LEN 1000
#define STATE_SIZE 6
#define MAX_CONNECT_STRING 256
#define TRUE 1
#define FALSE 0
#define MAX_STRING_SIZE            500

#define NULL_STRING  '\0'

#define CONFIG_BINDPARA_SQL_TYPE_DEFAULT

#define CONFIG_DATA_TYPE_MAX    256
#define CONFIG_DATA_CHAR_SIZE   512
#define CONFIG_STMT_BUF_MAX    20480

typedef short TrueFalse;                         
#define SQL_BOOLEAN 13

#define STACK_SIZE (20*1024*1024)

#define CLEANUP_KNOW_ERROR

#define CONFIG_SQL_COLUMNS_MAX    256
#define CONFIG_NUM_CQD_SIZE     3

#define CONFIG_TEST_NUM_RESULT      10
#define CONFIG_TEST_RESULT_SIZE     128
#define CONFIG_TEST_RESULT_ROWS     5
typedef struct
{
    TCHAR szResult[CONFIG_TEST_NUM_RESULT][CONFIG_TEST_RESULT_SIZE];
    int numOfResult;
}sTestResultData;

typedef struct{
    char szTableName[50];
    SQLSMALLINT sqlCType[CONFIG_SQL_COLUMNS_MAX];
    SQLSMALLINT sqlType[CONFIG_SQL_COLUMNS_MAX];
    SQLINTEGER columnSize[CONFIG_SQL_COLUMNS_MAX];
    SQLSMALLINT decimalDigits[CONFIG_SQL_COLUMNS_MAX];
    SQLINTEGER dataWidth[CONFIG_SQL_COLUMNS_MAX][2];
    char szColName[CONFIG_SQL_COLUMNS_MAX][64];
    SQLINTEGER columNum;
    char szCqd[CONFIG_NUM_CQD_SIZE][64];
    BOOL isBlobTable;
    int idBlobCols[10];
    int numBlobCols;
} sTestTableInfo;
typedef struct
{
    char szSqlFile[64];
    char szSql[2048];
    RETCODE retcode;
    BOOL isIgnoreFail;
    BOOL isCheckResult;
    BOOL isResultNull;
    BOOL isCalcCrc;
    sTestResultData mResult[CONFIG_TEST_RESULT_ROWS];
    int testTimes;
    int interval;
    BOOL isSaveFile;
    char szSaveFile[32];
    int saveColId;
    char szStmtCqd[CONFIG_NUM_CQD_SIZE][50];
}sSqlStmt;
typedef struct
{
    int beginVal;
    int batchSize;
    int totalBatch;
    char szInsertType[256];
    int threads;
    char szBlobFile[64];
    char szLoadCqd[CONFIG_NUM_CQD_SIZE][50];
    int putDataBatch;
    SQLLEN putDataType;
    BOOL isloadCrc;
}sLoadDataInfo;


typedef struct
{
    sTestTableInfo mTableInfo;
    sLoadDataInfo mLoadDataInfo;
    sLoadDataInfo mRowsetInfo;
    sLoadDataInfo mLobInfo;
    sSqlStmt mStmtInfo;
    sSqlStmt mSelectInfo;
    sSqlStmt mSqlFileInfo;
    BOOL isAvailableTable;
    BOOL isAvailableLoad;
    BOOL isAvailableRowset;
    BOOL isAvailableStmt;
    BOOL isAvailableSelect;
    BOOL isAvailableSqlFile;
    BOOL isAvailableLob;
} sODBCTestData;


extern TCHAR *SQLTypeToChar(SWORD SQLType, TCHAR *buf);
extern TCHAR *SQLCTypeToChar(SWORD CType, TCHAR *buf);

#ifndef unixcli
extern int gettimeofday(struct timeval *tp, void *tzp);
#endif



extern FILE *openLog(TCHAR *headKey);
extern int closeLog(FILE *fp);

extern void invertUint32(unsigned int *dBuf,unsigned int *srcBuf);
extern void invertUint8(unsigned char *dBuf,unsigned char *srcBuf);
extern unsigned int calcCRC32(unsigned char *puchMsg, unsigned int usDataLen);
extern void hexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen);
extern void asciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen);

extern void fillData(TCHAR *buf, int size);
extern void setSqlData(TCHAR *pSqlData, 
                  int size, 
                  int beginVal,
                  SQLSMALLINT sqlType,
                  SQLULEN columnSize, 
                  SQLSMALLINT decimalDigits
                  );
                  


extern void LogMsgEx(FILE *fp, unsigned int option, TCHAR *format, ...);
extern void LogMsgGlobal(unsigned int option, TCHAR *format, ...);

#ifdef unixcli
#define CONFIG_DEBUG_PRINT
#endif

#define INIT_LOGS(x) openLog(x)
#define RELEASE_LOSG(x) closeLog(x)

#define postLogMsg(option, format,...) \
            do{\
                LogMsgGlobal(option, format, ##__VA_ARGS__);\
                }while(0)\



#ifdef __cplusplus
}
#endif

#endif /* __UTIL_H__ */