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
#include "utchar.h"

#ifdef __cplusplus
extern "C" {
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


//#ifndef UNIXODBC
//typedef char BOOL;
//#endif /*UNIXODBC*/

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
#define CONFIG_ROWS_RESULT_MAX   5
#define CONFIG_COLUMNS_RESULT_MAX  CONFIG_SQL_COLUMNS_MAX

#define CONFIG_TEST_NUM_RESULT      10
#define CONFIG_TEST_RESULT_SIZE     128
#define CONFIG_TEST_RESULT_ROWS     5
typedef struct
{
    char *szResult[CONFIG_ROWS_RESULT_MAX][CONFIG_COLUMNS_RESULT_MAX];
    SQLINTEGER totalCols;
    SQLINTEGER totalRows;
}sTestResultData;

#define CONFIG_ENDIAN_PRECISION_MAX    39
#define CONFIG_COLUMNS_LOB_MAX      10
typedef struct{
    char szTableName[64];
    SQLSMALLINT sqlCType[CONFIG_SQL_COLUMNS_MAX];
    SQLSMALLINT sqlType[CONFIG_SQL_COLUMNS_MAX];
    SQLINTEGER columnSize[CONFIG_SQL_COLUMNS_MAX];
    SQLSMALLINT decimalDigits[CONFIG_SQL_COLUMNS_MAX];
    SQLINTEGER actualDataSize[CONFIG_SQL_COLUMNS_MAX][2];
    char szColName[CONFIG_SQL_COLUMNS_MAX][64];
    SQLINTEGER columNum;
    char szCqd[CONFIG_NUM_CQD_SIZE][64];
    BOOL isLobTable;
    BOOL isLobFile;
    char szLobFile[64];
    SQLINTEGER idLobCols[CONFIG_COLUMNS_LOB_MAX];
    SQLINTEGER numLobCols;
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
    BOOL isCheckColAttr;
    SQLSMALLINT sqlCType[CONFIG_SQL_COLUMNS_MAX];
    sTestResultData mResult;
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
    char szLobFile[64];
    char szLoadCqd[CONFIG_NUM_CQD_SIZE][50];
    int putDataBatch;
    SQLLEN putDataType;
    BOOL isloadCrc;
}sLoadDataInfo;


typedef struct
{
    sTestTableInfo mTableInfo;
    sLoadDataInfo mLoadDirect;
    sLoadDataInfo mRowsetInfo;
    sLoadDataInfo mLoadAtExec;
    sLoadDataInfo mLobUpdate;
    sSqlStmt mStmtInfo;
    sSqlStmt mSelectInfo;
    sSqlStmt mSqlFileInfo;
    BOOL isAvailableTable;
    BOOL isAvailableStmt;
    BOOL isAvailableSelect;
    BOOL isAvailableSqlFile;
} sODBCTestData;

typedef enum{
    BINDPARAM_EXECUTE_DIRECT = 0,
    BINDPARAM_SQL_PUT_DATA,
    BINDPARAM_ROWSET,
    BINDPARAM_UNKNOW
}LOAD_DATA_TYPE;
typedef enum{
    PUT_DATA_AT_EXEC = 0,
    PUT_LEN_DATA_AT_EXEC,
    PUT_DATA_LOB_UPDATE
}PUT_DATA_TYPE;
typedef struct
{
    //connect info
    TestInfo mTestInfo;
    LOAD_DATA_TYPE loadType;
    /*
    SQLRETURN SQLBindParameter(
                                SQLHSTMT StatementHandle,
                                SQLUSMALLINT ParameterNumber,
                                SQLSMALLINT InputOutputType,
                                SQLSMALLINT ValueType,
                                SQLSMALLINT ParameterType,
                                SQLULEN ColumnSize,
                                SQLSMALLINT DecimalDigits,
                                SQLPOINTER ParameterValuePtr,
                                SQLLEN BufferLength,
                                SQLLEN * StrLen_or_IndPtr);
    */
    SQLSMALLINT inputOutputType;
    SQLLEN lenType[CONFIG_SQL_COLUMNS_MAX];
    sTestTableInfo mTable;
    /*
    SQLRETURN SQLPutData(
                               SQLHSTMT StatementHandle,
                               SQLPOINTER DataPtr,
                               SQLLEN StrLen_or_Ind);
    */
    SQLLEN lenPutBatchSize;//Data length per SQLPutData
    PUT_DATA_TYPE putType;
    //about batch load
    int beginVal;
    int batchSize;
    int totalBatch;
    char szInsertType[256];
    //other
    TCHAR szConnStr[128];
    FILE *fpLog;
}sLoadParamInfo;
typedef struct
{
    /*
    SQLRETURN SQLBindParameter(
                                SQLHSTMT StatementHandle,
                                SQLUSMALLINT ParameterNumber,
                                SQLSMALLINT InputOutputType,
                                SQLSMALLINT ValueType,
                                SQLSMALLINT ParameterType,
                                SQLULEN ColumnSize,
                                SQLSMALLINT DecimalDigits,
                                SQLPOINTER ParameterValuePtr,
                                SQLLEN BufferLength,
                                SQLLEN * StrLen_or_IndPtr);
    */
    SQLSMALLINT inputOutputType;
    SQLSMALLINT sqlType[CONFIG_SQL_COLUMNS_MAX];
    SQLSMALLINT sqlCType[CONFIG_SQL_COLUMNS_MAX];
    SQLULEN columnSize[CONFIG_SQL_COLUMNS_MAX];
    SQLULEN decimalDigits[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN lenType[CONFIG_SQL_COLUMNS_MAX];
    SQLLEN actualDataSize[CONFIG_SQL_COLUMNS_MAX][2];
    SQLINTEGER totalCols;
    BOOL isLobTable;
    BOOL isLobFile;
    char szLobFile[64];
    SQLINTEGER idLobCols[CONFIG_COLUMNS_LOB_MAX];
    SQLINTEGER numLobCols;
}sBindParamAttr;

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
extern unsigned int calcCRC32Ex(unsigned int crc, unsigned char *puchMsg, unsigned int usDataLen);
extern void hexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen);
extern void asciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen);
extern void getRandNumeric(SQLCHAR precision, SQLCHAR scale, SQL_NUMERIC_STRUCT *numeric);
extern void getRandNumericEx(SQLCHAR precision, SQLCHAR scale, SQL_NUMERIC_STRUCT *numeric, double maxValue);
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
extern int calcFileSize(TCHAR *pFileName);

extern BOOL isTypeName(SQLSMALLINT sqlType, TCHAR *pTypeName);
extern TCHAR *sqlTypeToSqltypeName(SQLSMALLINT sqlType, TCHAR *pTypeName);

#ifdef unixcli
#define CONFIG_DEBUG_PRINT
#endif

#define INIT_LOGS(x) openLog(x)
#define RELEASE_LOSG(x) closeLog(x)

#define postLogMsg(option, format,...) \
            do{\
                LogMsgGlobal(option, format, ##__VA_ARGS__);\
                }while(0)\



#define LOG_HEAD(fp)    do{LogMsgEx(fp, NONE, _T("[%s:%d func:%s] "), __FILE__, __LINE__, __FUNCTION__);}while(0)


#ifdef __cplusplus
}
#endif

#endif /* __UTIL_H__ */