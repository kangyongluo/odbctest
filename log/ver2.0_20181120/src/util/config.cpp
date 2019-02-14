#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef unixcli
#include <ctype.h>
#else
#include <windows.h>
#endif
#include "config.h"

#ifdef unixcli
static char * FirstVisibleBYTE( char* pStr)
{
    while( pStr )
    {
        switch( *pStr )
        {
        case '\t':break;
        case ' ':break;
        default:return pStr;
        }
        ++pStr;
    }
    return NULL;
}
static char * ReadLn(char *ret, int maxlen, FILE *fp)
{
    char * sz;
    
    fgets( ret, maxlen, fp );
    sz = ret;
    while( *sz )
    {
        if( ( *sz == '\r' ) || ( *sz == '\n' ) )
            *sz = '\0';
        ++sz;
    }
    return ret;
}
int GetPrivateProfileString(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            char *pReturnedString,  // destination buffer
                            unsigned int nSize,              // size of destination buffer
                            const char *pFileName        // initialization file name
                )
{
    char    pLine[MAX_LEN_BYTES_OF_LINE+1], pOneSectionName[MAX_LEN_BYTES_OF_LINE+1];
    FILE    *fp = NULL;
    int        i,j,nIsMatch;
    char    *pLineHead = NULL, *pValueHead = NULL, 
            *pSectionNameHead = NULL, *pSectionTail = NULL, *pEquelMark = NULL;
    char    QuotationMark;    
    int    nRetIndex, ret = 0;

    memset( pLine, 0, MAX_LEN_BYTES_OF_LINE+1 );
    memset( pOneSectionName, 0, MAX_LEN_BYTES_OF_LINE+1 );
    memset( pReturnedString, 0, nSize );
    
    fp = fopen( pFileName, "r" );
    if( fp == NULL)
    {
        goto end;
    }

    if( ( pSectionName ) && ( *pSectionName ) )    /*Want find section*/
    {
        while( !feof( fp ) )
        {
            ReadLn( pLine, MAX_LEN_BYTES_OF_LINE, fp );
            pLineHead = FirstVisibleBYTE( pLine );
            if( !pLineHead ) continue;
            if( *pLineHead == '#' ) continue; /*Comment line*/
            if( *pLineHead != '[' )
                continue;
            pSectionNameHead = FirstVisibleBYTE( pLineHead + 1 );    /*Parse one section name*/
            if( !pSectionNameHead )
                continue;
            i = 0;
            nIsMatch = 1;
            while( pSectionNameHead[i] && ( pSectionNameHead[i] != ']' )
                && pSectionName[i])
            {
                if( toupper( pSectionNameHead[i] ) != toupper( pSectionName[i] ) )
                {
                    nIsMatch = 0;
                    break;
                }
                
                i++;
            }
            if( !nIsMatch )
                continue;
            if( pSectionName[i] != '\0' )    /*Not full match*/
                continue;
            pSectionTail = FirstVisibleBYTE( &pSectionNameHead[i] );
            if( ! pSectionTail )    
                continue;
            if( *pSectionTail != ']' )
                continue;
            else
                break;
        }
    }

    while( !feof( fp ) )    /*Want find key*/
    {
        ReadLn( pLine, MAX_LEN_BYTES_OF_LINE, fp );
        pLineHead = FirstVisibleBYTE( pLine );
        if( !pLineHead ) continue;
        if( *pLineHead == '#' ) continue; /* Comment line */
        if( ( pSectionName ) && ( *pSectionName ) )
            if( *pLineHead == '[' ) break; /* Next section */

        i=0;
        nIsMatch = 1;
        while( pLineHead[i] && pKeyName[i] )
        {
#ifdef GET_CFG_CASE_SENSITIVE
            if( pLineHead[i] != pKeyName[i] )
#else
            if( toupper( pLineHead[i] ) != toupper( pKeyName[i] ) )
#endif    /*GET_CFG_CASE_SENSITIVE*/
            {
                nIsMatch = 0;
                break;
            }
            i++;
        }
        if( !nIsMatch )
            continue;
        pEquelMark = FirstVisibleBYTE( &pLineHead[i] );
        if( !pEquelMark )
            continue;
        if( (*pEquelMark) != '=' )            /*Not full match key name*/        
            continue;
        
        /*Key name found and founded char '='*/
        pValueHead = FirstVisibleBYTE( pEquelMark+1 );    /*Get value addr*/
        nRetIndex = 0;
        if( ! pValueHead )        /*Empty value*/
        {
            *pReturnedString = '\0';
            goto end;
        }
        
        if( pValueHead[0] == '\'' || pValueHead[0] == '\"' )    /*Format "****" or '****'*/
        {
            QuotationMark = pValueHead[0];
            j = 1;
            while( pValueHead[j] && ( nRetIndex < nSize) 
                && ( pValueHead[j] != QuotationMark ) )
            {
                if( pValueHead[j] == '\\' )
                {
                    switch( pValueHead[j+1] )
                    {
                    case '\'':pReturnedString[nRetIndex] = '\'';break;
                    case 't':pReturnedString[nRetIndex] = '\t';break;
                    case 'n':pReturnedString[nRetIndex] = '\n';break;
                    case 'r':pReturnedString[nRetIndex] = '\r';break;
                    case '\"':pReturnedString[nRetIndex] = '\"';break;
                    case '\\':pReturnedString[nRetIndex] = '\\';break;
                    case '\0':pReturnedString[nRetIndex] = '\0';break;
                    default:pReturnedString[nRetIndex] = pValueHead[j+1];break;
                    }
                    j+=2;nRetIndex++;
                }
                else
                {
                    pReturnedString[nRetIndex] = pValueHead[j];
                    j++; nRetIndex++;
                }
            }
            goto end;
        }
        else
        {
            strncpy( pReturnedString, pValueHead, nSize );
            ret = strlen(pReturnedString);
            while(pReturnedString[ret - 1]){
                if(pReturnedString[ret - 1] == ' ' || 
                    pReturnedString[ret - 1] == 0x0d ||
                    pReturnedString[ret - 1] == 0x0a){
                    pReturnedString[ret - 1] = 0x00;
                    ret--;
                }
                else{
                    break;
                }
                if(ret == 0){
                    break;
                }
            }
            goto end;
        }
    }

end:
    if( ret <= 0 )
    {  
        if(pDefault){
            strncpy( pReturnedString, pDefault, nSize );
            ret = strlen(pReturnedString);
        }
    }
    if( fp )
        fclose( fp );
    return ret;
}
#endif

int getConfigString(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            char *pReturnedString,  // destination buffer
                            unsigned int nSize,              // size of destination buffer
                            const char *pFileName        // initialization file name
                            )
{
    return GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    pReturnedString, 
                                    nSize, 
                                    pFileName);
}
int getConfigBool(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            BOOL *pReturnedBool,  // destination buffer
                            const char *pFileName        // initialization file name
                            )
{
    char szBuf[5] = {0};

    GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    szBuf, 
                                    sizeof(szBuf), 
                                    pFileName);
    if(strcasecmp(szBuf, "TRUE") == 0){
        *pReturnedBool = TRUE;
    }
    else{
        *pReturnedBool = FALSE;
    }
    return 1;
}
int getConfigInt(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            int *pReturnedInt,  // destination buffer
                            const char *pFileName        // initialization file name
                            )
{
    char szBuf[20] = {0};

    GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    szBuf, 
                                    sizeof(szBuf), 
                                    pFileName);

    *pReturnedInt = atoi(szBuf);

    return sizeof(int);
}
CConfig::CConfig(void)
{
    char szBuf[128] = {0};

    pODBCTestData = new sODBCTestData;
    if(pODBCTestData == NULL){
        printf("new object for config fail !!!\n");
        return;
    }
    memset(pODBCTestData, 0, sizeof(sODBCTestData));
    mFpSql = NULL;

    mFd = 0;
    mFdSqlData = 0;
    mSqlSet = 0;
    isTableDefault = FALSE;

    if(GetPrivateProfileString("table", "tableName", "", szBuf, sizeof(szBuf), ODBC_TEST_CFG_FILE) == 0){
        if(GetPrivateProfileString("table", "sql_file", "", szBuf, sizeof(szBuf), ODBC_TEST_CFG_FILE) == 0){
            isTableDefault = TRUE;
        }
    }
}
CConfig::~CConfig()
{
    if(mFpSql){
        fclose(mFpSql);
        mFpSql = NULL;
    }
    if(pODBCTestData != NULL){
        delete pODBCTestData;
    }
}
int CConfig::init(char *pFile)
{
    if(mFpSql != NULL){
        fclose(mFpSql);
        mFpSql = NULL;
    }
    mFpSql = fopen(pFile, "r" );
    if(mFpSql == NULL) return -1;

    return 0;
}
void CConfig::reset(void)
{
    mFd = 0;
    if(mFpSql != NULL){
        fclose(mFpSql);
        mFpSql = NULL;
    }
}
void CConfig::setDefaultTable(void)
{
    SQLINTEGER i;
    char *szTable = "CREATE TABLE IF NOT EXISTS TB_ODBC_TEST(\n"
                    "C00 BLOB,\n"
                    "C01 CHAR(100) CHARACTER SET ISO88591 ,\n"
                    "C02 CHAR(100) CHARACTER SET UCS2 ,\n"
                    "C03 CHAR(100) CHARACTER SET UTF8 ,\n"
                    "C04 CHAR VARYING(100) CHARACTER SET ISO88591 ,\n"
                    "C05 CHAR VARYING(100) CHARACTER SET UCS2 ,\n"
                    "C06 CHAR VARYING(100) CHARACTER SET UTF8 ,\n"
                    "C07 VARCHAR(100) CHARACTER SET ISO88591 ,\n"
                    "C08 VARCHAR(100) CHARACTER SET UCS2 ,\n"
                    "C09 VARCHAR(100) CHARACTER SET UTF8 ,\n"
                    "C10 VARCHAR2(100) CHARACTER SET ISO88591 ,\n"
                    "C11 VARCHAR2(100) CHARACTER SET UCS2 ,\n"
                    "C12 VARCHAR2(100) CHARACTER SET UTF8 ,\n"
                    "C13 NCHAR(100) ,\n"
                    "C14 NCHAR VARYING(100) ,\n"
                    "C15 NATIONAL CHAR(100) ,\n"
                    "C16 NATIONAL CHAR VARYING(100) ,\n"
                    "C17 DECIMAL(12, 6) ,\n"
                    "C18 DECIMAL(9, 6) UNSIGNED ,\n"
                    "C19 NUMERIC(18, 6) ,\n"
                    "C20 NUMERIC(9, 6) UNSIGNED ,\n"
                    "C21 TINYINT ,\n"
                    "C22 TINYINT UNSIGNED ,\n"
                    "C23 SMALLINT ,\n"
                    "C24 SMALLINT UNSIGNED ,\n"
                    "C25 INT NOT NULL,\n"
                    "C26 INT UNSIGNED ,\n"
                    "C27 LARGEINT NOT NULL,\n"
                    "C28 LARGEINT UNSIGNED,\n"
                    "C29 BIGINT ,\n"
                    "C30 REAL ,\n"
                    "C31 FLOAT ,\n"
                    "C32 FLOAT(4) ,\n"
                    "C33 DOUBLE PRECISION ,\n"
                    "C34 DATE ,\n"
                    "C35 TIME,\n"
                    "C36 TIME(4),\n"
                    "C37 TIMESTAMP,\n"
                    "C38 TIMESTAMP(6),\n"
                    "C39 INTERVAL YEAR(2) ,\n"
                    "C40 INTERVAL MONTH(2) ,\n"
                    "C41 INTERVAL DAY(2) ,\n"
                    "C42 INTERVAL HOUR(2) ,\n"
                    "C43 INTERVAL MINUTE(2) ,\n"
                    "C44 INTERVAL SECOND(2,6) ,\n"
                    "C45 INTERVAL YEAR(2) TO MONTH ,\n"
                    "C46 INTERVAL DAY(2) TO HOUR ,\n"
                    "C47 INTERVAL DAY(2) TO MINUTE ,\n"
                    "C48 INTERVAL DAY(2) TO SECOND(6) ,\n"
                    "C49 INTERVAL HOUR(2) TO MINUTE ,\n"
                    "C50 INTERVAL HOUR(2) TO SECOND(6) ,\n"
                    "C51 INTERVAL MINUTE(2) TO SECOND(6),\n"
                    "PRIMARY KEY (C25)\n"
                    ")\n"
                    "SALT USING 10 PARTITIONS ON (C25)\n";
    sprintf(pODBCTestData->mTableInfo.szTableName, "%s", "TB_ODBC_TEST");
    
    pODBCTestData->mTableInfo.columNum = 52;
    for(i = 0; i < pODBCTestData->mTableInfo.columNum; i++)
        sprintf(pODBCTestData->mTableInfo.szColName[i], "C%02d", i);
    pODBCTestData->mTableInfo.isLobTable = TRUE;
    pODBCTestData->mTableInfo.numLobCols = 1;
    pODBCTestData->mTableInfo.idLobCols[0] = 0;
    
    pODBCTestData->mTableInfo.sqlCType[0] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[1] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[2] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[3] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[4] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[5] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[6] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[7] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[8] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[9] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[10] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[11] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[12] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[13] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[14] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[15] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[16] = SQL_C_CHAR;
    pODBCTestData->mTableInfo.sqlCType[17] = SQL_C_NUMERIC;
    pODBCTestData->mTableInfo.sqlCType[18] = SQL_C_NUMERIC;
    pODBCTestData->mTableInfo.sqlCType[19] = SQL_C_NUMERIC;
    pODBCTestData->mTableInfo.sqlCType[20] = SQL_C_NUMERIC;
    pODBCTestData->mTableInfo.sqlCType[21] = SQL_C_TINYINT;
    pODBCTestData->mTableInfo.sqlCType[22] = SQL_C_TINYINT;
    pODBCTestData->mTableInfo.sqlCType[23] = SQL_C_SSHORT;
    pODBCTestData->mTableInfo.sqlCType[24] = SQL_C_USHORT;
    pODBCTestData->mTableInfo.sqlCType[25] = SQL_C_ULONG;
    pODBCTestData->mTableInfo.sqlCType[26] = SQL_C_SLONG;
    pODBCTestData->mTableInfo.sqlCType[27] = SQL_C_SBIGINT;
    pODBCTestData->mTableInfo.sqlCType[28] = SQL_C_UBIGINT;
    pODBCTestData->mTableInfo.sqlCType[29] = SQL_C_SBIGINT;
    pODBCTestData->mTableInfo.sqlCType[30] = SQL_C_FLOAT;
    pODBCTestData->mTableInfo.sqlCType[31] = SQL_C_FLOAT;
    pODBCTestData->mTableInfo.sqlCType[32] = SQL_C_FLOAT;
    pODBCTestData->mTableInfo.sqlCType[33] = SQL_C_DOUBLE;
    pODBCTestData->mTableInfo.sqlCType[34] = SQL_C_TYPE_DATE;
    pODBCTestData->mTableInfo.sqlCType[35] = SQL_C_TYPE_TIME;
    pODBCTestData->mTableInfo.sqlCType[36] = SQL_C_TYPE_TIME;
    pODBCTestData->mTableInfo.sqlCType[37] = SQL_C_TYPE_TIMESTAMP;
    pODBCTestData->mTableInfo.sqlCType[38] = SQL_C_TYPE_TIMESTAMP;
    pODBCTestData->mTableInfo.sqlCType[39] = SQL_C_INTERVAL_YEAR;
    pODBCTestData->mTableInfo.sqlCType[40] = SQL_C_INTERVAL_MONTH;
    pODBCTestData->mTableInfo.sqlCType[41] = SQL_C_INTERVAL_DAY;
    pODBCTestData->mTableInfo.sqlCType[42] = SQL_C_INTERVAL_HOUR;
    pODBCTestData->mTableInfo.sqlCType[43] = SQL_C_INTERVAL_MINUTE;
    pODBCTestData->mTableInfo.sqlCType[44] = SQL_C_INTERVAL_SECOND;
    pODBCTestData->mTableInfo.sqlCType[45] = SQL_C_INTERVAL_YEAR_TO_MONTH;
    pODBCTestData->mTableInfo.sqlCType[46] = SQL_C_INTERVAL_DAY_TO_HOUR;
    pODBCTestData->mTableInfo.sqlCType[47] = SQL_C_INTERVAL_DAY_TO_MINUTE;
    pODBCTestData->mTableInfo.sqlCType[48] = SQL_C_INTERVAL_DAY_TO_SECOND;
    pODBCTestData->mTableInfo.sqlCType[49] = SQL_C_INTERVAL_HOUR_TO_MINUTE;
    pODBCTestData->mTableInfo.sqlCType[50] = SQL_C_INTERVAL_HOUR_TO_SECOND;
    pODBCTestData->mTableInfo.sqlCType[51] = SQL_C_INTERVAL_MINUTE_TO_SECOND;

    pODBCTestData->mTableInfo.sqlType[0] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[1] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[2] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[3] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[4] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[5] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[6] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[7] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[8] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[9] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[10] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[11] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[12] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[13] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[14] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[15] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[16] = SQL_CHAR;
    pODBCTestData->mTableInfo.sqlType[17] = SQL_DECIMAL;
    pODBCTestData->mTableInfo.sqlType[18] = SQL_DECIMAL;
    pODBCTestData->mTableInfo.sqlType[19] = SQL_DECIMAL;
    pODBCTestData->mTableInfo.sqlType[20] = SQL_DECIMAL;
    pODBCTestData->mTableInfo.sqlType[21] = SQL_TINYINT;
    pODBCTestData->mTableInfo.sqlType[22] = SQL_TINYINT;
    pODBCTestData->mTableInfo.sqlType[23] = SQL_SMALLINT;
    pODBCTestData->mTableInfo.sqlType[24] = SQL_SMALLINT;
    pODBCTestData->mTableInfo.sqlType[25] = SQL_INTEGER;
    pODBCTestData->mTableInfo.sqlType[26] = SQL_INTEGER;
    pODBCTestData->mTableInfo.sqlType[27] = SQL_BIGINT;
    pODBCTestData->mTableInfo.sqlType[28] = SQL_BIGINT;
    pODBCTestData->mTableInfo.sqlType[29] = SQL_BIGINT;
    pODBCTestData->mTableInfo.sqlType[30] = SQL_REAL;
    pODBCTestData->mTableInfo.sqlType[31] = SQL_VARCHAR;
    pODBCTestData->mTableInfo.sqlType[32] = SQL_FLOAT;
    pODBCTestData->mTableInfo.sqlType[33] = SQL_DOUBLE;
    pODBCTestData->mTableInfo.sqlType[34] = SQL_TYPE_DATE;
    pODBCTestData->mTableInfo.sqlType[35] = SQL_TYPE_TIME;
    pODBCTestData->mTableInfo.sqlType[36] = SQL_TYPE_TIME;
    pODBCTestData->mTableInfo.sqlType[37] = SQL_TYPE_TIMESTAMP;
    pODBCTestData->mTableInfo.sqlType[38] = SQL_TYPE_TIMESTAMP;
    pODBCTestData->mTableInfo.sqlType[39] = SQL_INTERVAL_YEAR;
    pODBCTestData->mTableInfo.sqlType[40] = SQL_INTERVAL_MONTH;
    pODBCTestData->mTableInfo.sqlType[41] = SQL_INTERVAL_DAY;
    pODBCTestData->mTableInfo.sqlType[42] = SQL_INTERVAL_HOUR;
    pODBCTestData->mTableInfo.sqlType[43] = SQL_INTERVAL_MINUTE;
    pODBCTestData->mTableInfo.sqlType[44] = SQL_INTERVAL_SECOND;
    pODBCTestData->mTableInfo.sqlType[45] = SQL_INTERVAL_YEAR_TO_MONTH;
    pODBCTestData->mTableInfo.sqlType[46] = SQL_INTERVAL_DAY_TO_HOUR;
    pODBCTestData->mTableInfo.sqlType[47] = SQL_INTERVAL_DAY_TO_MINUTE;
    pODBCTestData->mTableInfo.sqlType[48] = SQL_INTERVAL_DAY_TO_SECOND;
    pODBCTestData->mTableInfo.sqlType[49] = SQL_INTERVAL_HOUR_TO_MINUTE;
    pODBCTestData->mTableInfo.sqlType[50] = SQL_INTERVAL_HOUR_TO_SECOND;
    pODBCTestData->mTableInfo.sqlType[51] = SQL_INTERVAL_MINUTE_TO_SECOND;

    pODBCTestData->mTableInfo.columnSize[0] = 16777216;
    pODBCTestData->mTableInfo.columnSize[1] = 100;
    pODBCTestData->mTableInfo.columnSize[2] = 100;
    pODBCTestData->mTableInfo.columnSize[3] = 100;
    pODBCTestData->mTableInfo.columnSize[4] = 100;
    pODBCTestData->mTableInfo.columnSize[5] = 100;
    pODBCTestData->mTableInfo.columnSize[6] = 100;
    pODBCTestData->mTableInfo.columnSize[7] = 100;
    pODBCTestData->mTableInfo.columnSize[8] = 100;
    pODBCTestData->mTableInfo.columnSize[9] = 100;
    pODBCTestData->mTableInfo.columnSize[10] = 100;
    pODBCTestData->mTableInfo.columnSize[11] = 100;
    pODBCTestData->mTableInfo.columnSize[12] = 100;
    pODBCTestData->mTableInfo.columnSize[13] = 100;
    pODBCTestData->mTableInfo.columnSize[14] = 100;
    pODBCTestData->mTableInfo.columnSize[15] = 100;
    pODBCTestData->mTableInfo.columnSize[16] = 100;
    pODBCTestData->mTableInfo.columnSize[17] = 12;
    pODBCTestData->mTableInfo.columnSize[18] = 9;
    pODBCTestData->mTableInfo.columnSize[19] = 18;
    pODBCTestData->mTableInfo.columnSize[20] = 9;
    pODBCTestData->mTableInfo.columnSize[21] = 0;
    pODBCTestData->mTableInfo.columnSize[22] = 0;
    pODBCTestData->mTableInfo.columnSize[23] = 0;
    pODBCTestData->mTableInfo.columnSize[24] = 0;
    pODBCTestData->mTableInfo.columnSize[25] = 0;
    pODBCTestData->mTableInfo.columnSize[26] = 0;
    pODBCTestData->mTableInfo.columnSize[27] = 0;
    pODBCTestData->mTableInfo.columnSize[28] = 0;
    pODBCTestData->mTableInfo.columnSize[29] = 0;
    pODBCTestData->mTableInfo.columnSize[30] = 0;
    pODBCTestData->mTableInfo.columnSize[31] = 0;
    pODBCTestData->mTableInfo.columnSize[32] = 0;
    pODBCTestData->mTableInfo.columnSize[33] = 0;
    pODBCTestData->mTableInfo.columnSize[34] = 0;
    pODBCTestData->mTableInfo.columnSize[35] = 0;
    pODBCTestData->mTableInfo.columnSize[36] = 0;
    pODBCTestData->mTableInfo.columnSize[37] = 0;
    pODBCTestData->mTableInfo.columnSize[38] = 0;
    pODBCTestData->mTableInfo.columnSize[39] = 0;
    pODBCTestData->mTableInfo.columnSize[40] = 0;
    pODBCTestData->mTableInfo.columnSize[41] = 0;
    pODBCTestData->mTableInfo.columnSize[42] = 0;
    pODBCTestData->mTableInfo.columnSize[43] = 0;
    pODBCTestData->mTableInfo.columnSize[44] = 0;
    pODBCTestData->mTableInfo.columnSize[45] = 0;
    pODBCTestData->mTableInfo.columnSize[46] = 0;
    pODBCTestData->mTableInfo.columnSize[47] = 0;
    pODBCTestData->mTableInfo.columnSize[48] = 0;
    pODBCTestData->mTableInfo.columnSize[49] = 0;
    pODBCTestData->mTableInfo.columnSize[50] = 0;
    pODBCTestData->mTableInfo.columnSize[51] = 0;

    pODBCTestData->mTableInfo.decimalDigits[0] = 0;
    pODBCTestData->mTableInfo.decimalDigits[1] = 0;
    pODBCTestData->mTableInfo.decimalDigits[2] = 0;
    pODBCTestData->mTableInfo.decimalDigits[3] = 0;
    pODBCTestData->mTableInfo.decimalDigits[4] = 0;
    pODBCTestData->mTableInfo.decimalDigits[5] = 0;
    pODBCTestData->mTableInfo.decimalDigits[6] = 0;
    pODBCTestData->mTableInfo.decimalDigits[7] = 0;
    pODBCTestData->mTableInfo.decimalDigits[8] = 0;
    pODBCTestData->mTableInfo.decimalDigits[9] = 0;
    pODBCTestData->mTableInfo.decimalDigits[10] = 0;
    pODBCTestData->mTableInfo.decimalDigits[11] = 0;
    pODBCTestData->mTableInfo.decimalDigits[12] = 0;
    pODBCTestData->mTableInfo.decimalDigits[13] = 0;
    pODBCTestData->mTableInfo.decimalDigits[14] = 0;
    pODBCTestData->mTableInfo.decimalDigits[15] = 0;
    pODBCTestData->mTableInfo.decimalDigits[16] = 0;
    pODBCTestData->mTableInfo.decimalDigits[17] = 6;
    pODBCTestData->mTableInfo.decimalDigits[18] = 6;
    pODBCTestData->mTableInfo.decimalDigits[19] = 6;
    pODBCTestData->mTableInfo.decimalDigits[20] = 6;
    pODBCTestData->mTableInfo.decimalDigits[21] = 0;
    pODBCTestData->mTableInfo.decimalDigits[22] = 0;
    pODBCTestData->mTableInfo.decimalDigits[23] = 0;
    pODBCTestData->mTableInfo.decimalDigits[24] = 0;
    pODBCTestData->mTableInfo.decimalDigits[25] = 0;
    pODBCTestData->mTableInfo.decimalDigits[26] = 0;
    pODBCTestData->mTableInfo.decimalDigits[27] = 0;
    pODBCTestData->mTableInfo.decimalDigits[28] = 0;
    pODBCTestData->mTableInfo.decimalDigits[29] = 0;
    pODBCTestData->mTableInfo.decimalDigits[30] = 0;
    pODBCTestData->mTableInfo.decimalDigits[31] = 0;
    pODBCTestData->mTableInfo.decimalDigits[32] = 0;
    pODBCTestData->mTableInfo.decimalDigits[33] = 0;
    pODBCTestData->mTableInfo.decimalDigits[34] = 0;
    pODBCTestData->mTableInfo.decimalDigits[35] = 0;
    pODBCTestData->mTableInfo.decimalDigits[36] = 0;
    pODBCTestData->mTableInfo.decimalDigits[37] = 0;
    pODBCTestData->mTableInfo.decimalDigits[38] = 0;
    pODBCTestData->mTableInfo.decimalDigits[39] = 0;
    pODBCTestData->mTableInfo.decimalDigits[40] = 0;
    pODBCTestData->mTableInfo.decimalDigits[41] = 0;
    pODBCTestData->mTableInfo.decimalDigits[42] = 0;
    pODBCTestData->mTableInfo.decimalDigits[43] = 0;
    pODBCTestData->mTableInfo.decimalDigits[44] = 0;
    pODBCTestData->mTableInfo.decimalDigits[45] = 0;
    pODBCTestData->mTableInfo.decimalDigits[46] = 0;
    pODBCTestData->mTableInfo.decimalDigits[47] = 0;
    pODBCTestData->mTableInfo.decimalDigits[48] = 0;
    pODBCTestData->mTableInfo.decimalDigits[49] = 0;
    pODBCTestData->mTableInfo.decimalDigits[50] = 0;
    pODBCTestData->mTableInfo.decimalDigits[51] = 0;

}
int CConfig::readTable(char *pSection, sTestTableInfo *psTestTableInfo)
{
    char szBuf[512] = {0};
    int ret;
    char *result = NULL;
    struct sSqlCTypeInfo_
    {
        char szSqlCType[40];
        SQLSMALLINT sqlCType;
    } sSqlCTypeInfo[]= {
        {"SQL_C_CHAR",SQL_C_CHAR},
        {"SQL_C_TCHAR",SQL_C_TCHAR},
        {"SQL_C_WCHAR", SQL_C_WCHAR},
        {"SQL_C_BIT", SQL_C_BIT},
        {"SQL_C_NUMERIC", SQL_C_NUMERIC},
        {"SQL_C_STINYINT", SQL_C_STINYINT},
        {"SQL_C_UTINYINT", SQL_C_UTINYINT},
        {"SQL_C_TINYINT", SQL_C_TINYINT},
        {"SQL_C_SBIGINT", SQL_C_SBIGINT},
        {"SQL_C_UBIGINT", SQL_C_UBIGINT},
        {"SQL_C_SSHORT", SQL_C_SSHORT},
        {"SQL_C_USHORT",SQL_C_USHORT},
        {"SQL_C_SHORT", SQL_C_SHORT},
        {"SQL_C_SLONG", SQL_C_SLONG},
        {"SQL_C_ULONG", SQL_C_ULONG},
        {"SQL_C_LONG", SQL_C_LONG},
        {"SQL_C_FLOAT", SQL_C_FLOAT},
        {"SQL_C_DOUBLE", SQL_C_DOUBLE},
        {"SQL_C_BINARY", SQL_C_BINARY},
        {"SQL_C_DATE", SQL_C_TYPE_DATE},
        {"SQL_C_TIME", SQL_C_TYPE_TIME},
        {"SQL_C_TIMESTAMP", SQL_C_TYPE_TIMESTAMP},
        {"SQL_C_TYPE_DATE", SQL_C_TYPE_DATE},
        {"SQL_C_TYPE_TIME", SQL_C_TYPE_TIME},
        {"SQL_C_TYPE_TIMESTAMP", SQL_C_TYPE_TIMESTAMP},
        {"SQL_C_INTERVAL_YEAR", SQL_C_INTERVAL_YEAR},
        {"SQL_C_INTERVAL_MONTH", SQL_C_INTERVAL_MONTH},
        {"SQL_C_INTERVAL_DAY", SQL_C_INTERVAL_DAY},
        {"SQL_C_INTERVAL_HOUR", SQL_C_INTERVAL_HOUR},
        {"SQL_C_INTERVAL_MINUTE", SQL_C_INTERVAL_MINUTE},
        {"SQL_C_INTERVAL_SECOND", SQL_C_INTERVAL_SECOND},
        {"SQL_C_INTERVAL_YEAR_TO_MONTH", SQL_C_INTERVAL_YEAR_TO_MONTH},
        {"SQL_C_INTERVAL_DAY_TO_HOUR", SQL_C_INTERVAL_DAY_TO_HOUR},
        {"SQL_C_INTERVAL_DAY_TO_MINUTE", SQL_C_INTERVAL_DAY_TO_MINUTE},
        {"SQL_C_INTERVAL_DAY_TO_SECOND", SQL_C_INTERVAL_DAY_TO_SECOND},
        {"SQL_C_INTERVAL_HOUR_TO_MINUTE", SQL_C_INTERVAL_HOUR_TO_MINUTE},
        {"SQL_C_INTERVAL_HOUR_TO_SECOND", SQL_C_INTERVAL_HOUR_TO_SECOND},
        {"SQL_C_INTERVAL_MINUTE_TO_SECOND", SQL_C_INTERVAL_MINUTE_TO_SECOND}
    };
    struct sSqlTypeInfo_
    {
        char szSqlType[40];
        SQLSMALLINT sqlType;
    } sSqlTypeInfo[]= {
        {"SQL_CHAR",SQL_CHAR},
        {"SQL_VARCHAR", SQL_VARCHAR},
        {"SQL_LONGVARCHAR", SQL_LONGVARCHAR},
        {"SQL_WCHAR", SQL_WCHAR},
        {"SQL_WVARCHAR", SQL_WVARCHAR},
        {"SQL_WLONGVARCHAR", SQL_WLONGVARCHAR},
        {"SQL_NUMERIC", SQL_NUMERIC},
        {"SQL_BIT", SQL_BIT},
        {"SQL_TINYINT", SQL_TINYINT},
        {"SQL_SMALLINT", SQL_SMALLINT},
        {"SQL_INTEGER", SQL_INTEGER},
        {"SQL_BIGINT",SQL_BIGINT},
        {"SQL_REAL", SQL_REAL},
        {"SQL_FLOAT", SQL_FLOAT},
        {"SQL_DOUBLE", SQL_DOUBLE},
        {"SQL_BINARY", SQL_BINARY},
        {"SQL_VARBINARY", SQL_VARBINARY},
        {"SQL_LONGVARBINARY", SQL_LONGVARBINARY},
        {"SQL_TYPE_DATE", SQL_TYPE_DATE},
        {"SQL_TYPE_TIME", SQL_TYPE_TIME},
        {"SQL_TYPE_TIMESTAMP", SQL_TYPE_TIMESTAMP},
        {"SQL_INTERVAL_YEAR", SQL_INTERVAL_YEAR},
        {"SQL_INTERVAL_MONTH", SQL_INTERVAL_MONTH},
        {"SQL_INTERVAL_DAY", SQL_INTERVAL_DAY},
        {"SQL_INTERVAL_HOUR", SQL_INTERVAL_HOUR},
        {"SQL_INTERVAL_MINUTE", SQL_INTERVAL_MINUTE},
        {"SQL_INTERVAL_SECOND", SQL_INTERVAL_SECOND},
        {"SQL_INTERVAL_YEAR_TO_MONTH", SQL_INTERVAL_YEAR_TO_MONTH},
        {"SQL_INTERVAL_DAY_TO_HOUR", SQL_INTERVAL_DAY_TO_HOUR},
        {"SQL_INTERVAL_DAY_TO_MINUTE", SQL_INTERVAL_DAY_TO_MINUTE},
        {"SQL_INTERVAL_DAY_TO_SECOND", SQL_INTERVAL_DAY_TO_SECOND},
        {"SQL_INTERVAL_HOUR_TO_MINUTE", SQL_INTERVAL_HOUR_TO_MINUTE},
        {"SQL_INTERVAL_HOUR_TO_SECOND", SQL_INTERVAL_HOUR_TO_SECOND},
        {"SQL_INTERVAL_MINUTE_TO_SECOND", SQL_INTERVAL_MINUTE_TO_SECOND}
    };
    int i, j;
    
    ret = getConfigString(
                    pSection, 
                    "table_name", 
                    "", 
                    psTestTableInfo->szTableName, 
                    sizeof(psTestTableInfo->szTableName), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -1;
    }
    
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "sql_type", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -2;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        for(i = 0; i < sizeof(sSqlTypeInfo) / sizeof(sSqlTypeInfo[0]); i++){
            if(strcasecmp(sSqlTypeInfo[i].szSqlType, result) == 0){
                psTestTableInfo->sqlType[j++] = sSqlTypeInfo[i].sqlType;
                break; 
            }
        }
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    psTestTableInfo->columNum = j;
    
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "sql_c_type", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -2;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        for(i = 0; i < sizeof(sSqlCTypeInfo) / sizeof(sSqlCTypeInfo[0]); i++){
           if(strcasecmp(sSqlCTypeInfo[i].szSqlCType, result) == 0){
                psTestTableInfo->sqlCType[j++] = sSqlCTypeInfo[i].sqlCType;
                break; 
            }
        }
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "column_size", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -2;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        psTestTableInfo->columnSize[j++] = atoi(result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "data_size", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        psTestTableInfo->actualDataSize[j++][0] = atoi(result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "data_digits", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -2;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        psTestTableInfo->actualDataSize[j++][1] = atoi(result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "decimal_digits", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -2;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        psTestTableInfo->decimalDigits[j++] = atoi(result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }

    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "column_name", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psTestTableInfo->szColName[j++], result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psTestTableInfo->szCqd[j++], result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_NUM_CQD_SIZE) break;
    }
    for(i = 0; i < psTestTableInfo->columNum; i++){
        if(psTestTableInfo->actualDataSize[i][0] > psTestTableInfo->columnSize[i]){
            psTestTableInfo->actualDataSize[i][0] = psTestTableInfo->columnSize[i];
        }
        if(psTestTableInfo->actualDataSize[i][1] > psTestTableInfo->decimalDigits[i]){
            psTestTableInfo->actualDataSize[i][1] = psTestTableInfo->decimalDigits[i];
        }
    }
    getConfigBool(pSection, 
                    "lob_table", 
                    "FLASE", 
                    &psTestTableInfo->isLobTable, 
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "lob_columns", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    psTestTableInfo->numLobCols = 0;
    memset(psTestTableInfo->idLobCols, 0, sizeof(psTestTableInfo->idLobCols));
    if(strlen(szBuf) > 0){
        result = strtok(szBuf, ",");
        while( result != NULL ) {
            psTestTableInfo->idLobCols[psTestTableInfo->numLobCols++] = atoi(result);
            result = strtok(NULL, ",");
            if(psTestTableInfo->numLobCols >= (sizeof(psTestTableInfo->idLobCols) / sizeof(psTestTableInfo->idLobCols[0]))) break;
        }
    }
    
    return 0;
}

int CConfig::scanTestTable(void)
{
    char szSection[64] = {0};
    int ret;

    if(pODBCTestData == NULL) return -1;
    
    pODBCTestData->isAvailableSqlFile = FALSE;
    pODBCTestData->isAvailableStmt = FALSE;
    pODBCTestData->isAvailableSelect = FALSE;
    pODBCTestData->isAvailableTable = FALSE;
    if(isTableDefault){
        if(mFd != 0){
            return -1;
        }
        mFd = -1;
        pODBCTestData->isAvailableTable = TRUE;
        return 0;
    }
    memset(&pODBCTestData->mTableInfo, 0, sizeof(pODBCTestData->mTableInfo));
    if(mFd == 0) _stprintf(szSection, _T("%s"), _T("table"));
    else _stprintf(szSection, _T("%s%s%d"), _T("table"), "_", mFd);
    ret = readTable(szSection, &pODBCTestData->mTableInfo);
    if(ret == 0){
        if(readStmt(szSection, &pODBCTestData->mStmtInfo) == 0)
            pODBCTestData->isAvailableStmt = TRUE;
        if(readSelect(szSection, &pODBCTestData->mSelectInfo) == 0)
            pODBCTestData->isAvailableSelect = TRUE;
        readLoadDirectInfo(szSection, &pODBCTestData->mLoadDirect);
        readRowsetInfo(szSection, &pODBCTestData->mRowsetInfo);
        readAtExecInfo(szSection, &pODBCTestData->mLoadAtExec);
        readLobUpdateInfo(szSection, &pODBCTestData->mLobUpdate);
        mFd++;
        pODBCTestData->isAvailableTable = TRUE;
        return 0;
    }
    else if(ret == -2){
        mFd++;
        return 0;
    }
    return -1;
}
int CConfig::getTableName(char *ddl, char *name, int size)
{
    char *p;
    int pos = 0;

    p = strstr(ddl, "TABLE");
    if(p == NULL){
        p = strstr(ddl, "table");
    }
    if(p != NULL){
        p += strlen("table");
        while(*p){
            if(isalnum(*p) || (*p == '_')){
                break;
            }
            p++;
        }
        while(*p){
            if(isalnum(*p) || (*p == '_')){
                name[pos++] = *p;
            }
            else{
                *p = 0x00;
                break;
            }
            if(pos >= size) break;
            p++;
        }
    }

    return 0;
}
int CConfig::findSQL(FILE *fp, char *sql, unsigned int nsize)
{
    int ret = -1;
    char szBuf[1024] = {0};
    char szDDL[2048] = {0};
    int len, i, offset;

    if(fp == NULL) return -1;
    
    offset = 0;
    while (fgets(szBuf , sizeof(szBuf) , fp) != NULL) {
        if((offset == 0) && (szBuf[0] == '#')){ 
            offset = 0;
            memset(szBuf, 0, sizeof(szBuf));
            continue;
        }
        if((offset == 0) && (szBuf[0] == '-') && (szBuf[1] == '-')){ 
            offset = 0;
            memset(szBuf, 0, sizeof(szBuf));
            continue;
        }
        if((offset == 0) && (szBuf[0] == '/') && (szBuf[1] == '/')){ 
            offset = 0;
            memset(szBuf, 0, sizeof(szBuf));
            continue;
        }
        len = strlen(szBuf);
        i = len - 1;
        while(szBuf[i] == ' '){
            szBuf[i] = 0x00;
            i--;
            if(i == 0) break;
        }
        
        len = strlen(szBuf);
        i = len - 1;
        while(szBuf[i] == 0x0a || szBuf[i] == 0x0d){
            szBuf[i] = 0x00;
            i--;
            if(i == 0) break;
        }
        len = strlen(szBuf);
        if(len == 0) continue;
        if(offset == 0){
           offset = len;
           strncpy(szDDL, szBuf, offset);
        }
        else if((len > 0) && ((offset + len) < sizeof(szDDL))){
            strncpy(&szDDL[offset], szBuf, len);
            offset += len;            
        }
        else{
            offset = 0;
        }
        
        if((offset > 0) && (szDDL[offset - 1] == ';')){
            if(offset <= nsize){
                strncpy(sql, szDDL, offset);
                return 0;
            }
            offset = 0;
        }
        else{
            szDDL[offset] = '\n';
            offset++;
        }
        memset(szBuf, 0, sizeof(szBuf));
    }
    return ret;
}
int CConfig::readLoadDirectInfo(char *pSection, sLoadDataInfo *psLoadDataInfo)
{
    char szBuf[256] = {0};
    char *result;
    int i;
    
    psLoadDataInfo->batchSize = 1;
    getConfigInt(pSection, 
            "load_direct_rows", 
            "1", 
            &psLoadDataInfo->totalBatch,
            ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
            "load_direct_begin_value", 
            "0", 
            &psLoadDataInfo->beginVal,
            ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "load_direct_insert", 
                    "INSERT", 
                    psLoadDataInfo->szInsertType, 
                    sizeof(psLoadDataInfo->szInsertType), 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
            "load_direct_threads", 
            "1", 
            &psLoadDataInfo->threads,
            ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "load_direct_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    psLoadDataInfo->putDataType = SQL_NTS;
    getConfigString(pSection, 
                    "load_direct_file", 
                    "", 
                    psLoadDataInfo->szLobFile, 
                    sizeof(psLoadDataInfo->szLobFile), 
                    ODBC_TEST_CFG_FILE);
    if(_tcslen(psLoadDataInfo->szLobFile) > 0){
        psLoadDataInfo->putDataType = SQL_DATA_AT_EXEC;
    }
    getConfigBool(pSection, 
                    "load_direct_file_crc", 
                    "FLASE", 
                    &psLoadDataInfo->isloadCrc, 
                    ODBC_TEST_CFG_FILE);
    
    return 0;
}
int CConfig::readRowsetInfo(char *pSection, sLoadDataInfo *psLoadDataInfo)
{
    char szBuf[256] = {0};
    char *result;
    int i;
    
    getConfigInt(pSection, 
                "rowset_batch_size", 
                "2", 
                &psLoadDataInfo->batchSize,
                ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                "rowset_batch_num", 
                "1", 
                &psLoadDataInfo->totalBatch,
                ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                "rowset_begin_value", 
                "0", 
                &psLoadDataInfo->beginVal,
                ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "rowset_type", 
                    "INSERT", 
                    psLoadDataInfo->szInsertType, 
                    sizeof(psLoadDataInfo->szInsertType), 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                "rowset_threads", 
                "1", 
                &psLoadDataInfo->threads,
                ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "rowset_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigString(pSection, 
                    "rowset_file", 
                    "", 
                    psLoadDataInfo->szLobFile, 
                    sizeof(psLoadDataInfo->szLobFile), 
                    ODBC_TEST_CFG_FILE);
    
    return 0;
}
int CConfig::readAtExecInfo(char *pSection, sLoadDataInfo *psLoadDataInfo)
{
    int i;
    char szBuf[256] = {0};
    char *result;
    int ret;
    
    ret = getConfigString(pSection, 
                    "load_at_exec_file", 
                    "", 
                    psLoadDataInfo->szLobFile, 
                    sizeof(psLoadDataInfo->szLobFile), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        //return -1;
    }
    psLoadDataInfo->batchSize = 1;
    getConfigInt(pSection, 
                "load_at_exec_rows", 
                "1", 
                &psLoadDataInfo->totalBatch,
                ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                "load_at_exec_begin_value", 
                "0", 
                &psLoadDataInfo->beginVal,
                ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "load_at_exec_insert", 
                    "INSERT", 
                    psLoadDataInfo->szInsertType, 
                    sizeof(psLoadDataInfo->szInsertType), 
                    ODBC_TEST_CFG_FILE);
    
    getConfigInt(pSection, 
                "load_at_exec_threads", 
                "1", 
                &psLoadDataInfo->threads,
                ODBC_TEST_CFG_FILE);

    
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "load_at_exec_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigInt(pSection, 
                "load_at_exec_put_batch", 
                "4096", 
                &psLoadDataInfo->putDataBatch,
                ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "load_at_exec_crc", 
                    "FLASE", 
                    &psLoadDataInfo->isloadCrc, 
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "load_at_exec_type", 
                    "SQL_LEN_DATA_AT_EXEC", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(strcasecmp(szBuf, "SQL_LEN_DATA_AT_EXEC") == 0){
        psLoadDataInfo->putDataType = !SQL_DATA_AT_EXEC;
    }
    else{
        psLoadDataInfo->putDataType = SQL_DATA_AT_EXEC;
    }
    
    return 0;
}
int CConfig::readLobUpdateInfo(char *pSection, sLoadDataInfo *psLoadDataInfo)
{
    int i;
    char szBuf[256] = {0};
    char *result;
    int ret;
    
    ret = getConfigString(pSection, 
                    "lobupdate_file", 
                    "", 
                    psLoadDataInfo->szLobFile, 
                    sizeof(psLoadDataInfo->szLobFile), 
                    ODBC_TEST_CFG_FILE);
    psLoadDataInfo->batchSize = 1;
    getConfigInt(pSection, 
                "lobupdate_rows", 
                "1", 
                &psLoadDataInfo->totalBatch,
                ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                "lobupdate_begin_value", 
                "0", 
                &psLoadDataInfo->beginVal,
                ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "lobupdate_insert", 
                    "INSERT", 
                    psLoadDataInfo->szInsertType, 
                    sizeof(psLoadDataInfo->szInsertType), 
                    ODBC_TEST_CFG_FILE);
    
    getConfigInt(pSection, 
                "lobupdate_threads", 
                "1", 
                &psLoadDataInfo->threads,
                ODBC_TEST_CFG_FILE);

    
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "lobupdate_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigInt(pSection, 
                "lobupdate_put_batch", 
                "4096", 
                &psLoadDataInfo->putDataBatch,
                ODBC_TEST_CFG_FILE);
    psLoadDataInfo->isloadCrc = FALSE;
    psLoadDataInfo->putDataType = SQL_DATA_AT_EXEC;
    
    return 0;
}

int CConfig::readStmt(char *pSection, sSqlStmt *psStmtInfo)
{
    int ret;
    int i, j;
    char szBuf[512] = {0};
    char *result;
    struct sSqlCTypeInfo_
    {
        char szSqlCType[40];
        SQLSMALLINT sqlCType;
    } sSqlCTypeInfo[]= {
        {"SQL_C_CHAR",SQL_C_CHAR},
        {"SQL_C_TCHAR",SQL_C_TCHAR},
        {"SQL_C_WCHAR", SQL_C_WCHAR},
        {"SQL_C_BIT", SQL_C_BIT},
        {"SQL_C_NUMERIC", SQL_C_NUMERIC},
        {"SQL_C_STINYINT", SQL_C_STINYINT},
        {"SQL_C_UTINYINT", SQL_C_UTINYINT},
        {"SQL_C_TINYINT", SQL_C_TINYINT},
        {"SQL_C_SBIGINT", SQL_C_SBIGINT},
        {"SQL_C_UBIGINT", SQL_C_UBIGINT},
        {"SQL_C_SSHORT", SQL_C_SSHORT},
        {"SQL_C_USHORT",SQL_C_USHORT},
        {"SQL_C_SHORT", SQL_C_SHORT},
        {"SQL_C_SLONG", SQL_C_SLONG},
        {"SQL_C_ULONG", SQL_C_ULONG},
        {"SQL_C_LONG", SQL_C_LONG},
        {"SQL_C_FLOAT", SQL_C_FLOAT},
        {"SQL_C_DOUBLE", SQL_C_DOUBLE},
        {"SQL_C_BINARY", SQL_C_BINARY},
        {"SQL_C_DATE", SQL_C_TYPE_DATE},
        {"SQL_C_TIME", SQL_C_TYPE_TIME},
        {"SQL_C_TIMESTAMP", SQL_C_TYPE_TIMESTAMP},
        {"SQL_C_TYPE_DATE", SQL_C_TYPE_DATE},
        {"SQL_C_TYPE_TIME", SQL_C_TYPE_TIME},
        {"SQL_C_TYPE_TIMESTAMP", SQL_C_TYPE_TIMESTAMP},
        {"SQL_C_INTERVAL_YEAR", SQL_C_INTERVAL_YEAR},
        {"SQL_C_INTERVAL_MONTH", SQL_C_INTERVAL_MONTH},
        {"SQL_C_INTERVAL_DAY", SQL_C_INTERVAL_DAY},
        {"SQL_C_INTERVAL_HOUR", SQL_C_INTERVAL_HOUR},
        {"SQL_C_INTERVAL_MINUTE", SQL_C_INTERVAL_MINUTE},
        {"SQL_C_INTERVAL_SECOND", SQL_C_INTERVAL_SECOND},
        {"SQL_C_INTERVAL_YEAR_TO_MONTH", SQL_C_INTERVAL_YEAR_TO_MONTH},
        {"SQL_C_INTERVAL_DAY_TO_HOUR", SQL_C_INTERVAL_DAY_TO_HOUR},
        {"SQL_C_INTERVAL_DAY_TO_MINUTE", SQL_C_INTERVAL_DAY_TO_MINUTE},
        {"SQL_C_INTERVAL_DAY_TO_SECOND", SQL_C_INTERVAL_DAY_TO_SECOND},
        {"SQL_C_INTERVAL_HOUR_TO_MINUTE", SQL_C_INTERVAL_HOUR_TO_MINUTE},
        {"SQL_C_INTERVAL_HOUR_TO_SECOND", SQL_C_INTERVAL_HOUR_TO_SECOND},
        {"SQL_C_INTERVAL_MINUTE_TO_SECOND", SQL_C_INTERVAL_MINUTE_TO_SECOND}
    };
   
    ret = getConfigString(pSection, 
                        "stmt", 
                        "", 
                        psStmtInfo->szSql, 
                        sizeof(psStmtInfo->szSql), 
                        ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -1;
    }
    getConfigBool(pSection, 
                    "stmt_ignore_fail", 
                    "FALSE", 
                    &psStmtInfo->isIgnoreFail, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "stmt_checkout", 
                    "FALSE", 
                    &psStmtInfo->isCheckResult, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "stmt_result_null", 
                    "FALSE", 
                    &psStmtInfo->isResultNull, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "stmt_times", 
                    "1", 
                    &psStmtInfo->testTimes,
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "stmt_interval", 
                    "1", 
                    &psStmtInfo->interval,
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "save_file", 
                    "test.jpg", 
                    psStmtInfo->szSaveFile, 
                    sizeof(psStmtInfo->szSaveFile), 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "save_result", 
                    "FALSE", 
                    &psStmtInfo->isSaveFile, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "save_column_id", 
                    "0", 
                    &psStmtInfo->saveColId,
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "stmt_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psStmtInfo->szStmtCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigBool(pSection, 
                    "stmt_calc_crc", 
                    "TRUE", 
                    &psStmtInfo->isCalcCrc, 
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "select_c_type", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -2;
    }
    for(i = 0; i < CONFIG_SQL_COLUMNS_MAX; i++){
       psStmtInfo->sqlCType[i] = SQL_C_TCHAR;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        for(i = 0; i < sizeof(sSqlCTypeInfo) / sizeof(sSqlCTypeInfo[0]); i++){
           if(strcasecmp(sSqlCTypeInfo[i].szSqlCType, result) == 0){
                psStmtInfo->sqlCType[j++] = sSqlCTypeInfo[i].sqlCType;
                break; 
            }
        }
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    return 0;
}
int CConfig::readSelect(char *pSection, sSqlStmt *psStmtInfo)
{
    int ret;
    int i, j;
    char szBuf[512] = {0};
    char *result;
    struct sSqlCTypeInfo_
    {
        char szSqlCType[40];
        SQLSMALLINT sqlCType;
    } sSqlCTypeInfo[]= {
        {"SQL_C_CHAR",SQL_C_CHAR},
        {"SQL_C_TCHAR",SQL_C_TCHAR},
        {"SQL_C_WCHAR", SQL_C_WCHAR},
        {"SQL_C_BIT", SQL_C_BIT},
        {"SQL_C_NUMERIC", SQL_C_NUMERIC},
        {"SQL_C_STINYINT", SQL_C_STINYINT},
        {"SQL_C_UTINYINT", SQL_C_UTINYINT},
        {"SQL_C_TINYINT", SQL_C_TINYINT},
        {"SQL_C_SBIGINT", SQL_C_SBIGINT},
        {"SQL_C_UBIGINT", SQL_C_UBIGINT},
        {"SQL_C_SSHORT", SQL_C_SSHORT},
        {"SQL_C_USHORT",SQL_C_USHORT},
        {"SQL_C_SHORT", SQL_C_SHORT},
        {"SQL_C_SLONG", SQL_C_SLONG},
        {"SQL_C_ULONG", SQL_C_ULONG},
        {"SQL_C_LONG", SQL_C_LONG},
        {"SQL_C_FLOAT", SQL_C_FLOAT},
        {"SQL_C_DOUBLE", SQL_C_DOUBLE},
        {"SQL_C_BINARY", SQL_C_BINARY},
        {"SQL_C_DATE", SQL_C_TYPE_DATE},
        {"SQL_C_TIME", SQL_C_TYPE_TIME},
        {"SQL_C_TIMESTAMP", SQL_C_TYPE_TIMESTAMP},
        {"SQL_C_TYPE_DATE", SQL_C_TYPE_DATE},
        {"SQL_C_TYPE_TIME", SQL_C_TYPE_TIME},
        {"SQL_C_TYPE_TIMESTAMP", SQL_C_TYPE_TIMESTAMP},
        {"SQL_C_INTERVAL_YEAR", SQL_C_INTERVAL_YEAR},
        {"SQL_C_INTERVAL_MONTH", SQL_C_INTERVAL_MONTH},
        {"SQL_C_INTERVAL_DAY", SQL_C_INTERVAL_DAY},
        {"SQL_C_INTERVAL_HOUR", SQL_C_INTERVAL_HOUR},
        {"SQL_C_INTERVAL_MINUTE", SQL_C_INTERVAL_MINUTE},
        {"SQL_C_INTERVAL_SECOND", SQL_C_INTERVAL_SECOND},
        {"SQL_C_INTERVAL_YEAR_TO_MONTH", SQL_C_INTERVAL_YEAR_TO_MONTH},
        {"SQL_C_INTERVAL_DAY_TO_HOUR", SQL_C_INTERVAL_DAY_TO_HOUR},
        {"SQL_C_INTERVAL_DAY_TO_MINUTE", SQL_C_INTERVAL_DAY_TO_MINUTE},
        {"SQL_C_INTERVAL_DAY_TO_SECOND", SQL_C_INTERVAL_DAY_TO_SECOND},
        {"SQL_C_INTERVAL_HOUR_TO_MINUTE", SQL_C_INTERVAL_HOUR_TO_MINUTE},
        {"SQL_C_INTERVAL_HOUR_TO_SECOND", SQL_C_INTERVAL_HOUR_TO_SECOND},
        {"SQL_C_INTERVAL_MINUTE_TO_SECOND", SQL_C_INTERVAL_MINUTE_TO_SECOND}
    };
    
    ret = getConfigString(pSection, 
                        "select_stmt", 
                        "", 
                        psStmtInfo->szSql, 
                        sizeof(psStmtInfo->szSql), 
                        ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -1;
    }
    getConfigBool(pSection, 
                    "select_ignore_fail", 
                    "FALSE", 
                    &psStmtInfo->isIgnoreFail, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "select_checkout", 
                    "FALSE", 
                    &psStmtInfo->isCheckResult, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "select_result_null", 
                    "FALSE", 
                    &psStmtInfo->isResultNull, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "select_times", 
                    "1", 
                    &psStmtInfo->testTimes,
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "select_interval", 
                    "1", 
                    &psStmtInfo->interval,
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "select_save_file", 
                    "test.jpg", 
                    psStmtInfo->szSaveFile, 
                    sizeof(psStmtInfo->szSaveFile), 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "select_save_result", 
                    "FALSE", 
                    &psStmtInfo->isSaveFile, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "select_save_column_id", 
                    "0", 
                    &psStmtInfo->saveColId,
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "select_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psStmtInfo->szStmtCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigBool(pSection, 
                    "select_calc_crc", 
                    "TRUE", 
                    &psStmtInfo->isCalcCrc, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "select_col_attr", 
                    "FALSE", 
                    &psStmtInfo->isCheckColAttr,
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "select_c_type", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    for(i = 0; i < CONFIG_SQL_COLUMNS_MAX; i++){
       psStmtInfo->sqlCType[i] = SQL_C_TCHAR;
    }
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        for(i = 0; i < sizeof(sSqlCTypeInfo) / sizeof(sSqlCTypeInfo[0]); i++){
           if(strcasecmp(sSqlCTypeInfo[i].szSqlCType, result) == 0){
                psStmtInfo->sqlCType[j++] = sSqlCTypeInfo[i].sqlCType;
                break; 
            }
        }
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    return 0;
}

int CConfig::readSqlFile(char *pSection, sSqlStmt *psSqlFileInfo)
{
    char szSection[64] = {0};
    char szKey[64] = {0};
    char szBuf[512] = {0};
    int ret;
    char *result = NULL;
    char szDataFile[64] = {0};
    int i, j;
    struct sSqlReturn_
    {
        char szRetcode[40];
        RETCODE retcode;
    } sSqlReturn[]= {
        {"SQL_SUCCESS",SQL_SUCCESS},
        {"SQL_SUCCESS_WITH_INFO", SQL_SUCCESS_WITH_INFO},
        {"SQL_ERROR",SQL_ERROR},
        {"SQL_NO_DATA", SQL_NO_DATA},
    };

    ret = getConfigString(pSection, 
                        "sql_file", 
                        "", 
                        psSqlFileInfo->szSqlFile, 
                        sizeof(psSqlFileInfo->szSqlFile), 
                        ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -1;
    }
    if(mFpSql == NULL){
        iSqlStat = 0;
        mFpSql = fopen(psSqlFileInfo->szSqlFile, "r");
        if(mFpSql == NULL) return -1;
    }
    ret = findSQL(mFpSql, psSqlFileInfo->szSql, sizeof(psSqlFileInfo->szSql) - 1);
    if(ret != 0){
        fclose(mFpSql);
        mFpSql = NULL;
        if(iSqlStat == 1){
            mSqlSet++;
        }
        return -1;
    }
    if(iSqlStat == 0) iSqlStat = 1;
    ret = getConfigString(pSection, 
                        "data_file", 
                        "", 
                        szDataFile, 
                        sizeof(szDataFile), 
                        ODBC_TEST_CFG_FILE);
    if(ret == 0){
        psSqlFileInfo->isCheckResult = FALSE;
        psSqlFileInfo->retcode = SQL_SUCCESS;
        psSqlFileInfo->isResultNull = FALSE;
    }
    else{
        if(mSqlSet == 0) _stprintf(szSection, _T("%s"), _T("sql_data"));
        else _stprintf(szSection, _T("%s%d"), _T("sql_data_"), mSqlSet);
        for(j = 0; j < CONFIG_TEST_RESULT_ROWS; j++){
            if(j == 0){
                _stprintf(szKey, _T("%s"), _T("result"));
            }
            else{
                _stprintf(szKey, _T("%s_%d"), _T("result"), j);
            }
            getConfigString(pSection, 
                            szKey, 
                            "", 
                            szBuf, 
                            sizeof(szBuf), 
                            szDataFile);
            psSqlFileInfo->mResult[j].numOfResult = 0;
            result = strtok(szBuf, ",");
            while( result != NULL ) {
                sprintf(psSqlFileInfo->mResult[j].szResult[psSqlFileInfo->mResult[j].numOfResult], "%s", result);
                psSqlFileInfo->mResult[j].numOfResult++;
                result = strtok(NULL, ",");
                if(psSqlFileInfo->mResult[j].numOfResult >= CONFIG_TEST_NUM_RESULT) break;
            }
        }
        memset(szBuf, 0, sizeof(szBuf));
        getConfigString(pSection, 
                        "retcode", 
                        "SQL_SUCCESS", 
                        szBuf, 
                        sizeof(szBuf), 
                        szDataFile);
        for(i = 0; i < sizeof(sSqlReturn) / sizeof(sSqlReturn[0]); i++){
           if(strcasecmp(szBuf, sSqlReturn[i].szRetcode) == 0){
                psSqlFileInfo->retcode = sSqlReturn[i].retcode;
                break; 
            }
        }
        getConfigBool(pSection, 
                    "ignore_fail", 
                    "FALSE", 
                    &psSqlFileInfo->isIgnoreFail, 
                    szDataFile);
        getConfigBool(pSection, 
                    "checkout", 
                    "FALSE", 
                    &psSqlFileInfo->isCheckResult, 
                    szDataFile);
        getConfigBool(pSection, 
                    "result_null", 
                    "FALSE", 
                    &psSqlFileInfo->isResultNull, 
                    szDataFile);
        
    }
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "sqlfile_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        _stprintf(psSqlFileInfo->szStmtCqd[i++], result);
        result = strtok(NULL, ",");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
        
    return 0;
    
}
int CConfig::scanTestSqlFile(void)
{
    char szSection[64] = {0};
    int i;
    int ret = -1;

    if(pODBCTestData == NULL) return -1;
    
    pODBCTestData->isAvailableSqlFile = FALSE;
    pODBCTestData->isAvailableStmt = FALSE;
    pODBCTestData->isAvailableSelect = FALSE;
    pODBCTestData->isAvailableTable = FALSE;
    if(isTableDefault){
        return -1;
    }
    memset(pODBCTestData, 0, sizeof(sODBCTestData));
    
    for(i = 0; i < 2; i++){
        if(mSqlSet == 0) _stprintf(szSection, _T("%s"), _T("table"));
        else _stprintf(szSection, _T("%s%d"), _T("table_"), mSqlSet);
        ret = readSqlFile(szSection, &pODBCTestData->mSqlFileInfo);
        if(ret == 0){
            pODBCTestData->isAvailableSqlFile = TRUE;
            if(readTable(szSection, &pODBCTestData->mTableInfo) == 0)
                pODBCTestData->isAvailableTable = TRUE;
            if(readStmt(szSection, &pODBCTestData->mStmtInfo) == 0)
                pODBCTestData->isAvailableStmt = TRUE;
            if(readSelect(szSection, &pODBCTestData->mSelectInfo) == 0)
                pODBCTestData->isAvailableSelect = TRUE;
            readLoadDirectInfo(szSection, &pODBCTestData->mLoadDirect);
            readRowsetInfo(szSection, &pODBCTestData->mRowsetInfo);
            readAtExecInfo(szSection, &pODBCTestData->mLoadAtExec);
            readLobUpdateInfo(szSection, &pODBCTestData->mLobUpdate);
            break;
        }
    }
    if(ret != 0) return -1;
    
    return 0;
}
sODBCTestData *CConfig::getTestData(void)
{
    if(pODBCTestData) return pODBCTestData;
    return NULL;
}
