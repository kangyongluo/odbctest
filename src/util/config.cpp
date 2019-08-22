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

#else
int GetPrivateProfileStringEx(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            char *pReturnedString,  // destination buffer
                            unsigned int nSize,              // size of destination buffer
                            const char *pFileName        // initialization file name
                            )
{
#if (!defined(unixcli)) && defined(UNICODE)
    wchar_t szSection[128] = {0};
    wchar_t szKey[128] = {0};
    wchar_t szDefault[128] = {0};
    wchar_t szReturn[128] = {0};
    wchar_t szFile[128] = {0};
    int ret;

    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pSectionName, -1, szSection, sizeof(szSection) - 1);
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pKeyName, -1, szKey, sizeof(szKey) - 1);
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pDefault, -1, szDefault, sizeof(szDefault) - 1);
    ret = GetPrivateProfileString(szSection, 
                                    szKey, 
                                    szDefault, 
                                    szReturn, 
                                    nSize, 
                                    szFile);
    WideCharToMultiByte(CP_UTF8, 0, szReturn, -1, pReturnedString, nSize, NULL, NULL);
    return ret;
#else
    return GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    pReturnedString, 
                                    nSize, 
                                    pFileName);
#endif
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
#if (!defined(unixcli)) && defined(UNICODE)
    return GetPrivateProfileStringEx(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    pReturnedString, 
                                    nSize, 
                                    pFileName);
#else
    return GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    pReturnedString, 
                                    nSize, 
                                    pFileName);
#endif
}
int getConfigBool(const char *pSectionName,// section name
                            const char *pKeyName,        // key name
                            const char *pDefault,        // default string
                            BOOL *pReturnedBool,  // destination buffer
                            const char *pFileName        // initialization file name
                            )
{
    char szBuf[6] = {0};
    
#if (!defined(unixcli)) && defined(UNICODE)
    GetPrivateProfileStringEx(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    szBuf, 
                                    sizeof(szBuf), 
                                    pFileName);
#else
    GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    szBuf, 
                                    sizeof(szBuf), 
                                    pFileName);
#endif
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
    
#if (!defined(unixcli)) && defined(UNICODE)
    GetPrivateProfileStringEx(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    szBuf, 
                                    sizeof(szBuf), 
                                    pFileName);
#else
    GetPrivateProfileString(pSectionName, 
                                    pKeyName, 
                                    pDefault, 
                                    szBuf, 
                                    sizeof(szBuf), 
                                    pFileName);
#endif
    *pReturnedInt = atoi(szBuf);

    return sizeof(int);
}
CConfig::CConfig(void)
{
    
}
CConfig::~CConfig()
{

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
        {"SQL_C_INTERVAL_MINUTE_TO_SECOND", SQL_C_INTERVAL_MINUTE_TO_SECOND},
        {"SQL_C_DEFAULT", SQL_C_DEFAULT}
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
        {"SQL_INTERVAL_MINUTE_TO_SECOND", SQL_INTERVAL_MINUTE_TO_SECOND},
        {"SQL_DEFAULT", SQL_DEFAULT}
    };
    int i, j;
    SQLINTEGER len;
    char flg = 0;
    
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
        flg = 1;
        for(i = 0; i < sizeof(sSqlTypeInfo) / sizeof(sSqlTypeInfo[0]); i++){
            if(strcasecmp(sSqlTypeInfo[i].szSqlType, result) == 0){
                psTestTableInfo->sqlType[j++] = sSqlTypeInfo[i].sqlType;
                flg = 0;
                break; 
            }
        }
        if(flg) printf("Unknow SQL Type:%s !!!!!!!!!!\n", result);
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
        flg = 0;
        for(i = 0; i < sizeof(sSqlCTypeInfo) / sizeof(sSqlCTypeInfo[0]); i++){
           if(strcasecmp(sSqlCTypeInfo[i].szSqlCType, result) == 0){
                psTestTableInfo->sqlCType[j++] = sSqlCTypeInfo[i].sqlCType;
                flg = 0;
                break; 
            }
        }
        if(flg) printf("Unknow SQL C Type:%s !!!!!!!!!!\n", result);
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
        sprintf(psTestTableInfo->szColName[j++], result);
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
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psTestTableInfo->szCqd[j++], result);
        result = strtok(NULL, ";");
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
    for(i = 0; i < CONFIG_SQL_COLUMNS_MAX; i++)
        psTestTableInfo->paraValueLen[i] =  SQL_DEFAULT;
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "parameter_len", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        if(strcasecmp("SQL_NTS", result) == 0){
            psTestTableInfo->paraValueLen[j++] = SQL_NTS;
        }
        else if(strcasecmp("SQL_NULL_DATA", result) == 0){
            psTestTableInfo->paraValueLen[j++] = SQL_NULL_DATA;
        }
        else{
            len = (SQLLEN)atoi(result);
            if(len > 0){
                len = (psTestTableInfo->actualDataSize[j][0] > 0) ? psTestTableInfo->actualDataSize[j][0] : len;
                psTestTableInfo->paraValueLen[j] = (SQLLEN)(len);
            }
            else{
                psTestTableInfo->paraValueLen[j] = SQL_DEFAULT;
            }
            j++;
        }
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    
    return 0;
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
    getConfigString(pSection, 
                    "load_direct_where", 
                    "", 
                    psLoadDataInfo->szInsertWhere, 
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
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ";");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    psLoadDataInfo->putDataType = SQL_NTS;
    getConfigString(pSection, 
                    "load_direct_file", 
                    "", 
                    psLoadDataInfo->szLobFile, 
                    sizeof(psLoadDataInfo->szLobFile), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "load_direct_file_charset", 
                    "utf-8", 
                    psLoadDataInfo->szCharsetOfFile, 
                    sizeof(psLoadDataInfo->szCharsetOfFile), 
                    ODBC_TEST_CFG_FILE);
    if(strlen(psLoadDataInfo->szLobFile) > 0){
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
                    "rowset_insert", 
                    "INSERT", 
                    psLoadDataInfo->szInsertType, 
                    sizeof(psLoadDataInfo->szInsertType), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "rowset_where", 
                    "", 
                    psLoadDataInfo->szInsertWhere, 
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
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ";");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigString(pSection, 
                    "rowset_file", 
                    "", 
                    psLoadDataInfo->szLobFile, 
                    sizeof(psLoadDataInfo->szLobFile), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "rowset_file_charset", 
                    "utf-8", 
                    psLoadDataInfo->szCharsetOfFile, 
                    sizeof(psLoadDataInfo->szCharsetOfFile), 
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
    getConfigString(pSection, 
                    "load_at_exec_file_charset", 
                    "utf-8", 
                    psLoadDataInfo->szCharsetOfFile, 
                    sizeof(psLoadDataInfo->szCharsetOfFile), 
                    ODBC_TEST_CFG_FILE);
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
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ";");
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
    getConfigString(pSection, 
                    "lobupdate_file_charset", 
                    "utf-8", 
                    psLoadDataInfo->szCharsetOfFile, 
                    sizeof(psLoadDataInfo->szCharsetOfFile), 
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
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psLoadDataInfo->szLoadCqd[i++], result);
        result = strtok(NULL, ";");
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

int CConfig::readSQL(char *pSection, sSqlStmt *psStmtInfo)
{
    int ret;
    int i, j;
    char szBuf[512] = {0};
    char *result;
    char szKey[64] = {0};
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
                        "sql", 
                        "", 
                        psStmtInfo->szSql, 
                        sizeof(psStmtInfo->szSql), 
                        ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -1;
    }
    getConfigBool(pSection, 
                    "sql_ignore_fail", 
                    "FALSE", 
                    &psStmtInfo->isIgnoreFail, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "sql_checkout", 
                    "FALSE", 
                    &psStmtInfo->isCheckResult, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "sql_result_null", 
                    "FALSE", 
                    &psStmtInfo->isResultNull, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "sql_times", 
                    "1", 
                    &psStmtInfo->testTimes,
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "sql_interval", 
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
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "save_column_id", 
                    "1", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        psStmtInfo->saveColId[j++] = atoi(result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "sql_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psStmtInfo->szStmtCqd[i++], result);
        result = strtok(NULL, ";");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigBool(pSection, 
                    "sql_calc_crc", 
                    "TRUE", 
                    &psStmtInfo->isCalcCrc, 
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

    psStmtInfo->mResult.totalCols = 0;
    psStmtInfo->mResult.totalRows = 0;
    for(psStmtInfo->mResult.totalRows = 0; psStmtInfo->mResult.totalRows < CONFIG_ROWS_RESULT_MAX; psStmtInfo->mResult.totalRows++){
        if(psStmtInfo->mResult.totalRows == 0){
            sprintf(szKey, "%s", "sql_result");
        }
        else{
            sprintf(szKey, "%s%d", "sql_result_", psStmtInfo->mResult.totalRows);
        }
        memset(szBuf, 0, sizeof(szBuf));
        ret = getConfigString(
                        pSection, 
                        szKey, 
                        "", 
                        szBuf, 
                        sizeof(szBuf), 
                        ODBC_TEST_CFG_FILE);
        if(ret <= 0) break;
        i = 0;
        result = strtok(szBuf, ";");
        while( result != NULL ) {
            psStmtInfo->mResult.szResult[psStmtInfo->mResult.totalRows][i++] = strdup(result);
            result = strtok(NULL, ";");
            if(i >= CONFIG_COLUMNS_RESULT_MAX) break;
        }
        psStmtInfo->mResult.totalRows++;
        if(psStmtInfo->mResult.totalCols == 0)
            psStmtInfo->mResult.totalCols = i;
    }   
    getConfigString(
                    pSection, 
                    "sql_digit", 
                    "0", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    psStmtInfo->range[0] = 0;
    psStmtInfo->range[1] = 2147483647;
#ifdef unixcli
    if(strncasecmp(szBuf, "range(", 6) == 0){
        sscanf(&szBuf[6], "%d,%d", &psStmtInfo->range[0], &psStmtInfo->range[1]);
    }
#else
    if (strnicmp(szBuf, "range(", 6) == 0){
        sscanf(&szBuf[6], "%d,%d", &psStmtInfo->range[0], &psStmtInfo->range[1]);
    }
#endif
    else if((szBuf[0] >= '0') && (szBuf[0] <= '9')){
        psStmtInfo->range[0] = atoi(&szBuf[0]);
        psStmtInfo->range[1] = 2147483647;
    }
    return 0;
}
int CConfig::readSQLFileInfo(char *pSection, sSqlStmt *psStmtInfo)
{
    int i;
    char szBuf[512] = {0};
    char *result;
    
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "sqlfile_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psStmtInfo->szStmtCqd[i++], result);
        result = strtok(NULL, ";");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }

    return 0;
}
int CConfig::readSQLBigObject(char *pSection, sSqlStmt *psStmtInfo)
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
                        "query", 
                        "", 
                        psStmtInfo->szSql, 
                        sizeof(psStmtInfo->szSql), 
                        ODBC_TEST_CFG_FILE);
    if(ret == 0){
        return -1;
    }
    getConfigBool(pSection, 
                    "query_ignore_fail", 
                    "FALSE", 
                    &psStmtInfo->isIgnoreFail, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "query_checkout", 
                    "FALSE", 
                    &psStmtInfo->isCheckResult, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "query_result_null", 
                    "FALSE", 
                    &psStmtInfo->isResultNull, 
                    ODBC_TEST_CFG_FILE);
    getConfigInt(pSection, 
                    "query_times", 
                    "1", 
                    &psStmtInfo->testTimes,
                    ODBC_TEST_CFG_FILE);
    if(psStmtInfo->testTimes <= 0) psStmtInfo->testTimes = 1;
    getConfigInt(pSection, 
                    "query_interval", 
                    "1", 
                    &psStmtInfo->interval,
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "query_save_file", 
                    "test.jpg", 
                    psStmtInfo->szSaveFile, 
                    sizeof(psStmtInfo->szSaveFile), 
                    ODBC_TEST_CFG_FILE);
    getConfigString(pSection, 
                    "query_save_charset", 
                    ENCODING_UNKNOW, 
                    psStmtInfo->szCharset, 
                    sizeof(psStmtInfo->szCharset), 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "query_save_result", 
                    "FALSE", 
                    &psStmtInfo->isSaveFile, 
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    ret = getConfigString(
                    pSection, 
                    "query_save_column", 
                    "1", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    j = 0;
    result = strtok(szBuf, ",");
    while( result != NULL ) {
        psStmtInfo->saveColId[j++] = atoi(result);
        result = strtok(NULL, ",");
        if(j >= CONFIG_SQL_COLUMNS_MAX) break;
    }
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "query_cqd", 
                    "", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    i = 0;
    result = strtok(szBuf, ";");
    while( result != NULL ) {
        sprintf(psStmtInfo->szStmtCqd[i++], result);
        result = strtok(NULL, ";");
        if(i >= CONFIG_NUM_CQD_SIZE) break;
    }
    getConfigBool(pSection, 
                    "query_calc_crc", 
                    "TRUE", 
                    &psStmtInfo->isCalcCrc, 
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "query_col_attr", 
                    "FALSE", 
                    &psStmtInfo->isCheckColAttr,
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "query_prepare", 
                    "FALSE", 
                    &psStmtInfo->isPrepare,
                    ODBC_TEST_CFG_FILE);
    getConfigBool(pSection, 
                    "query_mult_prepare", 
                    "TRUE", 
                    &psStmtInfo->isMultPrepare,
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "query_c_type", 
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
    getConfigInt(pSection, 
                    "query_buf_length", 
                    "16777216", 
                    (int *)&psStmtInfo->lengRecv,
                    ODBC_TEST_CFG_FILE);
    memset(szBuf, 0, sizeof(szBuf));
    getConfigString(
                    pSection, 
                    "query_digit", 
                    "0", 
                    szBuf, 
                    sizeof(szBuf), 
                    ODBC_TEST_CFG_FILE);
    psStmtInfo->range[0] = 0;
    psStmtInfo->range[1] = 2147483647;
#ifdef unixcli
    if(strncasecmp(szBuf, "range(", 6) == 0){
        sscanf(&szBuf[6], "%d,%d", &psStmtInfo->range[0], &psStmtInfo->range[1]);
    }
#else
    if (strnicmp(szBuf, "range(", 6) == 0){
        sscanf(&szBuf[6], "%d,%d", &psStmtInfo->range[0], &psStmtInfo->range[1]);
    }
#endif
    else if((szBuf[0] >= '0') && (szBuf[0] <= '9')){
        psStmtInfo->range[0] = atoi(&szBuf[0]);
        psStmtInfo->range[1] = 2147483647;
    }
    
    return 0;
}
