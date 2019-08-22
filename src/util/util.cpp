#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "global.h"


#define CONFIG_TEST_NO_COAST


char *SQLTypeToChar(SWORD SQLType, char *buf)
{
   switch (SQLType)
   {
       case SQL_CHAR: strcpy(buf, "SQL_CHAR"); break;
       case SQL_VARCHAR: strcpy(buf, "SQL_VARCHAR"); break;
       case SQL_LONGVARCHAR: strcpy(buf, "SQL_LONGVARCHAR"); break;
       case SQL_WCHAR: strcpy(buf, "SQL_WCHAR"); break;
       case SQL_WVARCHAR: strcpy(buf, "SQL_WVARCHAR"); break;
       case SQL_WLONGVARCHAR: strcpy(buf, "SQL_WLONGVARCHAR"); break;
       case SQL_DECIMAL: strcpy(buf, "SQL_DECIMAL"); break;
       case SQL_NUMERIC: strcpy(buf, "SQL_NUMERIC"); break;
       case SQL_SMALLINT: strcpy(buf, "SQL_SMALLINT"); break;
       case SQL_INTEGER: strcpy(buf, "SQL_INTEGER"); break;
       case SQL_REAL: strcpy(buf, "SQL_REAL"); break;
       case SQL_FLOAT: strcpy(buf, "SQL_FLOAT"); break;
       case SQL_DOUBLE: strcpy(buf, "SQL_DOUBLE"); break;
       case SQL_BIT: strcpy(buf, "SQL_BIT"); break;
       case SQL_TINYINT: strcpy(buf, "SQL_TINYINT"); break;
       case SQL_BIGINT: strcpy(buf, "SQL_BIGINT"); break;
       case SQL_BINARY: strcpy(buf, "SQL_BINARY"); break;
       case SQL_VARBINARY: strcpy(buf, "SQL_VARBINARY"); break;
       case SQL_LONGVARBINARY: strcpy(buf, "SQL_LONGVARBINARY"); break;
       case SQL_TYPE_DATE: strcpy(buf, "SQL_TYPE_DATE"); break;
       case SQL_TYPE_TIME: strcpy(buf, "SQL_TYPE_TIME"); break;
       case SQL_TYPE_TIMESTAMP: strcpy(buf, "SQL_TYPE_TIMESTAMP"); break;
       //case SQL_TYPE_UTCDATETIME: strcpy(buf, "SQL_TYPE_UTCDATETIME"); break; // unsupport
       //case SQL_TYPE_UTCTIME: strcpy(buf, "SQL_TYPE_UTCTIME"); break; // unsupport
       case SQL_INTERVAL_MONTH: strcpy(buf, "SQL_INTERVAL_MONTH"); break;
       case SQL_INTERVAL_YEAR: strcpy(buf, "SQL_INTERVAL_YEAR"); break;
       case SQL_INTERVAL_YEAR_TO_MONTH: strcpy(buf, "SQL_INTERVAL_YEAR_TO_MONTH"); break;
       case SQL_INTERVAL_DAY: strcpy(buf, "SQL_INTERVAL_DAY"); break;
       case SQL_INTERVAL_HOUR: strcpy(buf, "SQL_INTERVAL_HOUR"); break;
       case SQL_INTERVAL_MINUTE: strcpy(buf, "SQL_INTERVAL_MINUTE"); break;
       case SQL_INTERVAL_SECOND: strcpy(buf, "SQL_INTERVAL_SECOND"); break;
       case SQL_INTERVAL_DAY_TO_HOUR: strcpy(buf, "SQL_INTERVAL_DAY_TO_HOUR"); break;
       case SQL_INTERVAL_DAY_TO_MINUTE: strcpy(buf, "SQL_INTERVAL_DAY_TO_MINUTE"); break;
       case SQL_INTERVAL_DAY_TO_SECOND: strcpy(buf, "SQL_INTERVAL_DAY_TO_SECOND"); break;
       case SQL_INTERVAL_HOUR_TO_MINUTE: strcpy(buf, "SQL_INTERVAL_HOUR_TO_MINUTE"); break;
       case SQL_INTERVAL_HOUR_TO_SECOND: strcpy(buf, "SQL_INTERVAL_HOUR_TO_SECOND"); break;
       case SQL_INTERVAL_MINUTE_TO_SECOND: strcpy(buf, "SQL_INTERVAL_MINUTE_TO_SECOND"); break;
       case SQL_GUID: strcpy(buf, "SQL_GUID"); break;
       case SQL_ALL_TYPES: strcpy(buf, "SQL_ALL_TYPES"); break;
       default:
         //_itot(SQLType,buf,10);
         break;
	}
	return buf;
}
char *SQLCTypeToChar(SWORD CType, char *buf)
{
   switch (CType) {
      case SQL_C_CHAR:			sprintf (buf,"SQL_C_CHAR",  CType);break;
      case SQL_C_WCHAR:			sprintf (buf,"SQL_C_WCHAR", CType);break;
	  case SQL_C_LONG:			sprintf (buf,"SQL_C_LONG", CType);break;
      case SQL_C_SHORT:			sprintf (buf,"SQL_C_SHORT", CType);break;
      case SQL_C_FLOAT:			sprintf (buf,"SQL_C_FLOAT", CType);break;
      case SQL_C_DOUBLE:		sprintf (buf,"SQL_C_DOUBLE", CType);break;
      case SQL_C_NUMERIC:		sprintf (buf,"SQL_C_NUMERIC", CType);break;
      case SQL_C_DEFAULT:		sprintf (buf,"SQL_C_DEFAULT", CType);break;
      case SQL_C_DATE:			sprintf (buf,"SQL_C_DATE", CType);break;
      case SQL_C_TIME:			sprintf (buf,"SQL_C_TIME", CType);break;
      case SQL_C_TIMESTAMP:			sprintf (buf,"SQL_C_TIMESTAMP", CType);break;
      case SQL_C_TYPE_DATE:			sprintf (buf,"SQL_C_TYPE_DATE", CType);break;
      case SQL_C_TYPE_TIME:			sprintf (buf,"SQL_C_TYPE_TIME", CType);break;
      case SQL_C_TYPE_TIMESTAMP:		sprintf (buf,"SQL_C_TYPE_TIMESTAMP", CType);break;
      case SQL_C_INTERVAL_YEAR:		sprintf (buf,"SQL_C_INTERVAL_YEAR", CType);break;
      case SQL_C_INTERVAL_MONTH:		sprintf (buf,"SQL_C_INTERVAL_MONTH", CType);break;
      case SQL_C_INTERVAL_DAY:		sprintf (buf,"SQL_C_INTERVAL_DAY", CType);break;
      case SQL_C_INTERVAL_HOUR:		sprintf (buf,"SQL_C_INTERVAL_HOUR", CType);break;
      case SQL_C_INTERVAL_MINUTE:	sprintf (buf,"SQL_C_INTERVAL_MINUTE", CType);break;
      case SQL_C_INTERVAL_SECOND:	sprintf (buf,"SQL_C_INTERVAL_SECOND", CType);break;
      case SQL_C_INTERVAL_YEAR_TO_MONTH:		sprintf (buf,"SQL_C_INTERVAL_YEAR_TO_MONTH", CType);break;
      case SQL_C_INTERVAL_DAY_TO_HOUR:			sprintf (buf,"SQL_C_INTERVAL_DAY_TO_HOUR", CType);break;
      case SQL_C_INTERVAL_DAY_TO_MINUTE:		sprintf (buf,"SQL_C_INTERVAL_DAY_TO_MINUTE", CType);break;
      case SQL_C_INTERVAL_DAY_TO_SECOND:		sprintf (buf,"SQL_C_INTERVAL_DAY_TO_SECOND", CType);break;
      case SQL_C_INTERVAL_HOUR_TO_MINUTE:		sprintf (buf,"SQL_C_INTERVAL_HOUR_TO_MINUTE", CType);break;
      case SQL_C_INTERVAL_HOUR_TO_SECOND:		sprintf (buf,"SQL_C_INTERVAL_HOUR_TO_SECOND", CType);break;
      case SQL_C_INTERVAL_MINUTE_TO_SECOND:	sprintf (buf,"SQL_C_INTERVAL_MINUTE_TO_SECOND", CType);break;
      case SQL_C_BINARY:		sprintf (buf,"SQL_C_BINARY", CType);break;
      case SQL_C_BIT:			sprintf (buf,"SQL_C_BIT", CType);break;
      case SQL_C_SBIGINT:		sprintf (buf,"SQL_C_SBIGINT", CType);break;
      case SQL_C_UBIGINT:		sprintf (buf,"SQL_C_UBIGINT", CType);break;
      case SQL_C_TINYINT:		sprintf (buf,"SQL_C_TINYINT", CType);break;
      case SQL_C_SLONG:			sprintf (buf,"SQL_C_SLONG", CType);break;
      case SQL_C_SSHORT:		sprintf (buf,"SQL_C_SSHORT", CType);break;
      case SQL_C_STINYINT:		sprintf (buf,"SQL_C_STINYINT", CType);break;
      case SQL_C_ULONG:			sprintf (buf,"SQL_C_ULONG", CType);break;
      case SQL_C_USHORT:		sprintf (buf,"SQL_C_USHORT", CType);break;
      case SQL_C_UTINYINT:		sprintf (buf,"SQL_C_UTINYINT", CType);break;
      default:
         //_itot(CType,buf,10);
         break;
	}
	return buf;
}

#ifndef unixcli
int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif
void invertUint32(unsigned int *dBuf,unsigned int *srcBuf)
{
    int i;
    unsigned int tmp[4];
 
    tmp[0] = 0;
    
    for(i=0;i< 32;i++)
    {
      if(srcBuf[0]& (1 << i))
        tmp[0]|=1<<(15 - i);
    }
    dBuf[0] = tmp[0];
}

void invertUint8(unsigned char *dBuf,unsigned char *srcBuf)
{
    int i;
    unsigned char tmp[4];
    
    tmp[0] = 0;
    for(i=0;i< 8;i++)
    {
      if(srcBuf[0]& (1 << i))
        tmp[0]|=1<<(7-i);
    }
    dBuf[0] = tmp[0];   
}
unsigned int calcCRC32(unsigned char *puchMsg, unsigned int usDataLen)
{
  int i;
  unsigned int wCRCin = 0xFFFFFFFF;
  unsigned int wCPoly = 0x04C11DB7;
  unsigned int wChar = 0;
  while (usDataLen--) 	
  {
        wChar = *(puchMsg++);
        invertUint8((unsigned char *)&wChar,(unsigned char *)&wChar);
        wCRCin ^= (wChar << 24);
        for(i = 0;i < 8;i++)
        {
          if(wCRCin & 0x80000000)
            wCRCin = (wCRCin << 1) ^ wCPoly;
          else
            wCRCin = wCRCin << 1;
        }
  }
  invertUint32(&wCRCin,&wCRCin);
  
  return (wCRCin ^ 0xFFFFFFFF) ;
}
unsigned int calcCRC32Ex(unsigned int crc, unsigned char *puchMsg, unsigned int usDataLen)
{
  int i;
  unsigned int wCRCin = 0xFFFFFFFF;
  //unsigned int wCPoly = 0x04C11DB7;
  unsigned int wCPoly = crc;
  unsigned int wChar = 0;
  while (usDataLen--) 	
  {
        wChar = *(puchMsg++);
        invertUint8((unsigned char *)&wChar,(unsigned char *)&wChar);
        wCRCin ^= (wChar << 24);
        for(i = 0;i < 8;i++)
        {
          if(wCRCin & 0x80000000)
            wCRCin = (wCRCin << 1) ^ wCPoly;
          else
            wCRCin = wCRCin << 1;
        }
  }
  invertUint32(&wCRCin,&wCRCin);
  
  return (wCRCin ^ 0xFFFFFFFF) ;
}

void hexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen)
{
    unsigned char Nibble[2];
    int i,j;

    for (i = 0; i < nLen; i++)
    {
        Nibble[0] = (pHex[i] & 0xF0) >> 4;
        Nibble[1] = pHex[i] & 0x0F;
        for (j = 0; j < 2; j++)
        {
            if (Nibble[j] < 10)
                Nibble[j] += 0x30;
            else
            {
                if (Nibble[j] < 16)
                    Nibble[j] = Nibble[j] - 10 + 'A';
            }
            *pAscii++ = Nibble[j];
        }
    }  
}
void asciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen)
{
    if (nLen % 2)
        return;
    int nHexLen = nLen / 2;
    int i, j;

    for (i = 0; i < nHexLen; i++)
    {
        unsigned char Nibble[2];
        Nibble[0] = *pAscii++;
        Nibble[1] = *pAscii++;
        for (j = 0; j < 2; j++)
        {
            if (Nibble[j] <= 'F' && Nibble[j] >= 'A')
                Nibble[j] = Nibble[j] - 'A' + 10;
            else if (Nibble[j] <= 'f' && Nibble[j] >= 'a')
                Nibble[j] = Nibble[j] - 'a' + 10;
            else if (Nibble[j] >= '0' && Nibble[j] <= '9')
                Nibble[j] = Nibble[j] - '0';
            else
                return;
        }
        pHex[i] = Nibble[0] << 4;
        pHex[i] |= Nibble[1];
    }
}
void ucToAscii(SQLWCHAR *uc, char *ascii)
{
    int i;

    for (i = 0; uc[i]; i++)
    {
        ascii[i] = uc[i] & 0x00ff;
    }

    ascii[i] = 0;
}
void asciiToUnicode(char *ascii, SQLWCHAR *szUc)
{
    int i;

    for (i = 0; ascii[i]; i++)
    {
        szUc[i] = ascii[i];
    }
    szUc[i] = 0;
}
void charToTChar(char *szChar, TCHAR *szTChar)
{
#ifdef UNICODE
    int i;

    for (i = 0; szChar[i]; i++)
    {
        szTChar[i] = szChar[i];
    }
    szTChar[i] = 0;
#else
    sprintf(szTChar, szChar);
#endif
}
void tcharToChar(TCHAR *szTChar, char *szChar)
{
#ifdef UNICODE
    int i;

    for (i = 0; szTChar[i]; i++)
    {
        szChar[i] = szTChar[i] & 0x00ff;
    }

    szChar[i] = 0;
#else
    sprintf(szChar, szTChar);
#endif
}

void fillData(TCHAR *buf, int size)
{
    char szData[256] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()";
    int len;
    int offset;
    char szBuf[64] = {0};
    char szCrc[10] = {0};
    time_t tm;
    unsigned int crc = 0;
    static unsigned int index = 0;

    if(size <= 0){
        buf[0] = 0;
        return;
    }
    for(len = 0, offset = 0; len < size; len += offset){
        tm = time( NULL);
        strftime(szBuf, sizeof(szBuf), "%Y-%m-%d_%H.%M.%S",  localtime(&tm));
        sprintf(szCrc, "%d", index);
        memcpy(szData, szCrc, strlen(szCrc));
        memcpy(&szData[strlen(szCrc)], szBuf, strlen(szBuf));
        offset = ((size - len) >= (int)strlen(szData)) ? (int)strlen(szData) : (size - len);
#ifdef UNICODE
        int pos;

        for(pos = 0; pos < offset; pos++) buf[len + pos] = szData[pos];
        buf[len + pos] = 0x0000; 
#else
        strncpy(&buf[len], szData, offset);
#endif
    }
    if(size > 8){
        crc = calcCRC32((unsigned char *)buf, len - 8);
        hexToAscii((unsigned char *)&crc, (unsigned char *)szCrc, sizeof(crc));
        buf[len - 8] = szCrc[0];
        buf[len - 7] = szCrc[1];
        buf[len - 6] = szCrc[2];
        buf[len - 5] = szCrc[3];
        buf[len - 4] = szCrc[4];
        buf[len - 3] = szCrc[5];
        buf[len - 2] = szCrc[6];
        buf[len - 1] = szCrc[7];
        buf[len] = 0x00;
    }
    else{
        buf[len] = 0x00;
    }
    index++;
}
static char *myrTrim(char *string)                     
{                                             
   char *strPtr;                              
                                              
   for (strPtr = string + strlen(string) - 1; 
        strPtr >= string && (*strPtr == ' ' || *strPtr == '\t') ;  
        *(strPtr--) = '\0');                  
   return(string);                            
} 
static int myConvertCharToCNumeric( SQL_NUMERIC_STRUCT *numericTmp, char* cTmpBuf)
{
    unsigned char localBuf[101];
    char* tempPtr = (char*)localBuf,*tempPtr1;
    int i,j,a,b,current,calc,length;

    SQLCHAR tempPrecision;
    SQLCHAR tempScale;
    SQLCHAR tempSign;
    SQLCHAR tmpVal[101];

    if(strlen(myrTrim(cTmpBuf)) > sizeof(tmpVal))
        return -1;

    memset( tmpVal, 0, sizeof(tmpVal));
    memset( localBuf, 0, sizeof(localBuf));

    length = strlen(strncpy( tempPtr, cTmpBuf, sizeof(localBuf)-1 ));
    if( tempPtr[ length - 1 ] == '.' ) tempPtr[ length - 1 ] = '\0';

    tempSign = (*tempPtr == '-')? 2: 1;

    if( *tempPtr == '+' || *tempPtr == '-' ) tempPtr++;

    if((tempPtr1 = strchr( tempPtr, '.' )) == NULL )
    {
        tempPrecision = strlen(tempPtr);
        tempScale = 0;
    }
    else
    {
        tempPrecision = strlen(tempPtr) - 1;
        tempScale = strlen(tempPtr1) - 1;
    }
    if( tempPrecision > CONFIG_ENDIAN_PRECISION_MAX )
        return -1;

    for( length = 0, tempPtr1 = (char*)localBuf ;*tempPtr != 0; tempPtr++ )
    {
        if(*tempPtr == '.') continue;
        *tempPtr1++ = *tempPtr - '0';
        length++;
    }
    memset( tempPtr1, 0, sizeof(localBuf) - length );

    for( j=0; j < 2 * sizeof(tmpVal); j++)
    {
        a=b=calc=0;

        for( i=0; i < length ; i++)
        {
            current = localBuf[i];
            calc = calc * 10 + current;
            a = calc % 16;
            b = calc / 16;

            localBuf[i] =  b;
            calc = a;
        }
        switch( j % 2 )
        {
        case 0:
            tmpVal[j / 2 ] = a;
            break;
        case 1:
            tmpVal[j / 2 ] |= a<<4;
            break;
        }
    }

    for( i= sizeof(tmpVal) - 1; i > SQL_MAX_NUMERIC_LEN - 1; i--)
        if(tmpVal[i] != 0)
            return -1;

    numericTmp->sign = tempSign;
    numericTmp->precision = tempPrecision;
    numericTmp->scale = tempScale;
    memcpy(numericTmp->val, tmpVal, SQL_MAX_NUMERIC_LEN);

    return 0;
}

void createRandNumericStr(SQLCHAR precision, SQLCHAR scale, char *szNumeric)
{
    SQLCHAR i, pos;

    pos = 0;
    if(precision == scale){
        szNumeric[pos++] =  '0';
    }
    for(i = 0; i < (SQLCHAR)(precision - scale); i++){
        szNumeric[pos++] = 1 + (unsigned char)(8.0 * rand()/(RAND_MAX + 1.0)) + 0x30;
    }
    szNumeric[pos++] = '.';
    if(scale == 0){
        szNumeric[pos++] = '0';
    }
    for(i = 0; i < (SQLCHAR)scale; i++){
        szNumeric[pos++] = 1 + (unsigned char)(8.0 * rand()/(RAND_MAX + 1.0)) + 0x30;
    }
    szNumeric[pos] = 0;
}
void createRandNumericStrEx(SQLCHAR precision, SQLCHAR scale, char *szNumeric, double maxValue)
{
    SQLCHAR i, pos;

    pos = 0;
    if(precision == scale){
        szNumeric[pos++] =  '0';
    }
    for(i = 0; i < (SQLCHAR)(precision - scale); i++){
        szNumeric[pos++] = 1 + (unsigned char)(8.0 * rand()/(RAND_MAX + 1.0)) + 0x30;
        if(atof(szNumeric) > maxValue) break;
    }
    szNumeric[pos++] = '.';
    if(scale == 0){
        szNumeric[pos++] = '0';
    }
    for(i = 0; i < (SQLCHAR)scale; i++){
        szNumeric[pos++] = 1 + (unsigned char)(8.0 * rand()/(RAND_MAX + 1.0)) + 0x30;
    }
    szNumeric[pos] = 0;
}

void getRandNumeric(SQLCHAR precision, SQLCHAR scale, SQL_NUMERIC_STRUCT *numeric)
{
    char szBuf[130] = {0};
    
    memset(numeric->val, 0, sizeof(numeric->val));
    if(precision == 0){ 
        numeric->sign = 1;
        return;
    }
    numeric->precision = (precision > (CONFIG_ENDIAN_PRECISION_MAX - 1)) ? (CONFIG_ENDIAN_PRECISION_MAX - 1) : precision;
    numeric->scale = (scale > numeric->precision) ? (numeric->precision) : scale;
    
    createRandNumericStr(numeric->precision, numeric->scale, szBuf);
    if(myConvertCharToCNumeric(numeric, szBuf) != 0){
        printf("convert char to numeric fail. line = %d\n", __LINE__);
    }
}
void getRandNumericEx(SQLCHAR precision, SQLCHAR scale, SQL_NUMERIC_STRUCT *numeric, double maxValue)
{
    char szBuf[130] = {0};
    
    memset(numeric->val, 0, sizeof(numeric->val));
    if(precision == 0){ 
        numeric->sign = 1;
        return;
    }
    numeric->precision = (precision > (CONFIG_ENDIAN_PRECISION_MAX - 1)) ? (CONFIG_ENDIAN_PRECISION_MAX - 1) : precision;
    numeric->scale = (scale > numeric->precision) ? (numeric->precision) : scale;
    
    createRandNumericStrEx(numeric->precision, numeric->scale, szBuf, maxValue);
    if(myConvertCharToCNumeric(numeric, szBuf) != 0){
        printf("convert char to numeric fail. line = %d\n", __LINE__);
    }
}

void setSqlData(TCHAR *pSqlData, 
                  int size, 
                  int beginVal,
                  SQLSMALLINT sqlType,
                  SQLULEN columnSize, 
                  SQLSMALLINT decimalDigits
                  )
{
    char szBuf[1024] = {0};
    char szStr[1024] = {0};
    double maxValues = beginVal * 1.0;

    switch(sqlType){
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
        fillData(pSqlData, (size < columnSize) ? size : columnSize);
        return;
    case SQL_DECIMAL:
        createRandNumericStrEx(columnSize, decimalDigits, szBuf, maxValues);
        sprintf(szStr, "%s", szBuf);
        break;
    case SQL_NUMERIC:
        createRandNumericStrEx(columnSize, decimalDigits, szBuf, maxValues);
        sprintf(szStr, "%s", szBuf);
        break;
    case SQL_TINYINT:
        sprintf(szStr, "%d", (beginVal > 127) ? 127 : beginVal);
        break;
    case SQL_SMALLINT:
        sprintf(szStr, "%d", (beginVal > 32767) ? 32767 : beginVal);
        break;
    case SQL_INTEGER:
        sprintf(szStr, "%d", beginVal);
        break;
    case SQL_BIGINT:
        sprintf(szStr, "%d", beginVal);
        break;
    case SQL_REAL:
        sprintf(szStr, "%d", beginVal);
        break;
    case SQL_DOUBLE:
        sprintf(szStr, "%d.%d", beginVal, 0);
        break;
    case SQL_TYPE_DATE:
        sprintf(szStr, "%04d-%02d-%02d", 2000 + beginVal, 2, 11);
        break;
    case SQL_TYPE_TIME:
        sprintf(szStr, "%02d:%02d:%02d", beginVal & 0x0f, 2, 5);
        break;
    case SQL_TYPE_TIMESTAMP:
        sprintf(szStr, "%04d-%02d-%02d %02d:%02d:%02d", 2000 + beginVal, 2, 11, beginVal & 0x0f, 2, 11);
        break;
    case SQL_INTERVAL_YEAR:
        sprintf(szStr, "%u", beginVal);
        break;
    case SQL_INTERVAL_MONTH:
        sprintf(szStr, "%d", (beginVal > 11) ? 11 : beginVal);
        break;
    case SQL_INTERVAL_DAY:
        sprintf(szStr, "%d", beginVal);
        break;
    case SQL_INTERVAL_HOUR:
        sprintf(szStr, "%d", (beginVal > 11) ? 11 : beginVal);
        break;
    case SQL_INTERVAL_MINUTE:
        sprintf(szStr, "%d", (beginVal > 59) ? 59 : beginVal);
        break;
    case SQL_INTERVAL_SECOND:
        sprintf(szStr, "%d", (beginVal > 59) ? 59 : beginVal);
        break;
    case SQL_INTERVAL_YEAR_TO_MONTH:
        sprintf(szStr, "%02d-%02d", (beginVal > 59) ? 59 : beginVal, 1);
        break;
    case SQL_INTERVAL_DAY_TO_HOUR:
        sprintf(szStr, "%u %02d", beginVal, 1);
        break;
    case SQL_INTERVAL_DAY_TO_MINUTE:
        sprintf(szStr, "%02d %02d:%02d", (beginVal > 59) ? 59 : beginVal, 0, 1);
        break;
    case SQL_INTERVAL_DAY_TO_SECOND:
        sprintf(szStr, "%02d %02d:%02d:%02d", (beginVal > 59) ? 59 : beginVal, 1, 1, 1);
        break;
    case SQL_INTERVAL_HOUR_TO_MINUTE:
        sprintf(szStr, "%02d:%02d", (beginVal > 11) ? 11 : beginVal, 1);
        break;
    case SQL_INTERVAL_HOUR_TO_SECOND:
        sprintf(szStr, "%02d:%02d:%02d", (beginVal > 11) ? 11 : beginVal, 1, 1);
        break;
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        sprintf(szStr, "%02d:%02d", (beginVal > 59) ? 59 : beginVal, 1);
        break;
    default:
        sprintf(szStr, "%d", (beginVal > 127) ? 127 : beginVal);
        break;
    }
#ifdef UNICODE
    charToTChar(szStr, pSqlData);
#else
    sprintf(pSqlData, szStr);
#endif
}
FILE *openLog(char *headKey)
{
    FILE *fp = NULL;
    time_t tm;
    char szBuf[128] = {0};
    char szLogKey[128] = {0};

    tm = time(NULL);
    if((headKey != NULL) && (strlen(headKey) >= 0)){
        strftime(szBuf, sizeof(szBuf), "%Y-%m-%d_%H.%M.%S.log", localtime(&tm));
        sprintf(szLogKey, "%s_%s", headKey, szBuf);
    }
    else{
        strftime(szLogKey, sizeof(szLogKey), "%Y-%m-%d_%H.%M.%S.log", localtime(&tm));
    }
    fp = fopen(szLogKey, "a+");
        
    return fp;
}
int closeLog(FILE *fp)
{
    if(fp){
        fclose(fp);
        fp= NULL;
        return 0;
    }
    return -1;
}

void LogMsgEx(FILE *fp, unsigned int option, char *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    char szBuf[128] = {0};

    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    if(fp == NULL){
        return;
    }
    if(fp != NULL){
        if(option == SHORTTIMESTAMP || option == TIME_STAMP){
            fprintf(fp, "%s", timeStr);
        }
        va_start(argsList,format);
        vfprintf(fp, format, argsList);
        va_end(argsList);
        fflush(fp);
    } 
    
#ifdef CONFIG_DEBUG_PRINT
    if((option == SHORTTIMESTAMP || option == TIME_STAMP) && (!(option & PRINT_OFF))){
        printf("%s", timeStr);
    }
    va_start(argsList,format);
#ifdef unixcli
    if(!(option & PRINT_OFF))
        vprintf(format, argsList);
#else
    if(!(option & PRINT_OFF))
        vprintf_s(format, argsList);
#endif
    va_end(argsList);
#endif /* CONFIG_DEBUG_PRINT */
}
void LogMsgGlobal(unsigned int option, char *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    char szBuf[128] = {0};
    FILE *fp = gTestGlobal.fpLog;

    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));

    if(fp != NULL){
        if(option == SHORTTIMESTAMP || option == TIME_STAMP){
            fprintf(fp, "%s", timeStr);
        }
        va_start(argsList,format);
        vfprintf(fp, format, argsList);
        va_end(argsList);
        fflush(fp);
    } 
#ifdef CONFIG_DEBUG_PRINT
    if(option == SHORTTIMESTAMP || option == TIME_STAMP){
        printf("%s", timeStr);
    }
    va_start(argsList,format);
#ifdef unixcli
    vprintf(format, argsList);
#else
    vprintf_s(format, argsList);
#endif
    va_end(argsList);
#endif /* CONFIG_DEBUG_PRINT */

}
int calcFileSize(char *pFileName)
{
    FILE *fp = NULL;
    int fileSize = 0;
    
    fp = fopen(pFileName, "rb+");
    if(fp){
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        fclose(fp);
    }

    return fileSize;
}
int getFileEncoding(char *pFileName, char *encoding)
{
    FILE *fp = NULL;
    unsigned char szBuf[32] = {0};
    int ret = 0;

    if(pFileName == NULL || encoding == NULL) return -1;
    if(strlen(pFileName)== 0) return -1;
    
    fp = fopen(pFileName, "rb+");
    if(fp){
        fread(szBuf, 1, sizeof(szBuf) - 1, fp);
        fclose(fp);
    }
    printf("Identify the encoding type of %s:0x%x 0x%x 0x%x 0x%x\n", pFileName, szBuf[0], szBuf[1], szBuf[2], szBuf[3]);
    if((szBuf[0] == 0xfe) && (szBuf[1] == 0xff)){
        sprintf(encoding, ENCODING_UCS2_BIG_ENDIAN);
    }
    else if((szBuf[0] == 0xff) && (szBuf[1] == 0xfe)){
        sprintf(encoding, ENCODING_UCS2_LITTLE_ENDIAN);
    }
    else if((szBuf[0] == 0xef) && (szBuf[1] == 0xbb) && (szBuf[2] == 0xbf)){
        sprintf(encoding, ENCODING_UTF8);
    }
    else if((szBuf[0] == 0xE4) && (szBuf[1] == 0xB8) && (szBuf[2] == 0xAD)){
        sprintf(encoding, ENCODING_UTF_8);
    }
    else if((szBuf[0] == 0xD6) && (szBuf[1] == 0xD0)){
        sprintf(encoding, ENCODING_GBK);
    }
    else{
        sprintf(encoding, ENCODING_UNKNOW);
        ret = -1;
    }
    printf("Encoding type of %s is \"%s\"\n", pFileName, encoding);
    
    return ret;
}

BOOL isTypeName(SQLSMALLINT sqlType, char *pTypeName)
{
    BOOL ret = FALSE;
    
    switch (sqlType)
   {
       case SQL_CHAR: 
            if(strcmp(pTypeName, "CHAR") == 0) ret = TRUE; 
            break;
       case SQL_VARCHAR: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_LONGVARCHAR: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_WCHAR: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_WVARCHAR: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_WLONGVARCHAR: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_DECIMAL: 
            if(strcmp(pTypeName, "DECIMAL") == 0) ret = TRUE;
            if(strcmp(pTypeName, "DECIMAL UNSIGNED") == 0) ret = TRUE;
            break;
       case SQL_NUMERIC: 
            if(strcmp(pTypeName, "DECIMAL") == 0) ret = TRUE;
            if(strcmp(pTypeName, "DECIMAL UNSIGNED") == 0) ret = TRUE; 
            break;
       case SQL_SMALLINT: 
            if(strcmp(pTypeName, "SMALLINT") == 0) ret = TRUE;
            if(strcmp(pTypeName, "SMALLINT UNSIGNED") == 0) ret = TRUE;
            break;
       case SQL_INTEGER: 
            if(strcmp(pTypeName, "INTEGER UNSIGNED") == 0) ret = TRUE;
            if(strcmp(pTypeName, "INTEGER") == 0) ret = TRUE; 
            break;
       case SQL_REAL: 
            if(strcmp(pTypeName, "REAL") == 0) ret = TRUE; 
            break;
       case SQL_FLOAT: 
            if(strcmp(pTypeName, "DOUBLE PRECISION") == 0) ret = TRUE; 
            break;
       case SQL_DOUBLE: 
            if(strcmp(pTypeName, "DOUBLE PRECISION") == 0) ret = TRUE; 
            break;
       case SQL_BIT: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_TINYINT: 
            if(strcmp(pTypeName, "TINYINT") == 0) ret = TRUE;
            if(strcmp(pTypeName, "TINYINT UNSIGNED") == 0) ret = TRUE; 
            break;
       case SQL_BIGINT: 
            if(strcmp(pTypeName, "BIGINT") == 0) ret = TRUE;
            if(strcmp(pTypeName, "BIGINT UNSIGNED") == 0) ret = TRUE;
            break;
       case SQL_BINARY: 
            if(strcmp(pTypeName, "BINARY") == 0) ret = TRUE; 
            break;
       case SQL_VARBINARY: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_LONGVARBINARY: 
            if(strcmp(pTypeName, "VARCHAR") == 0) ret = TRUE; 
            break;
       case SQL_TYPE_DATE: 
            if(strcmp(pTypeName, "DATE") == 0) ret = TRUE; 
            break;
       case SQL_TYPE_TIME: 
            if(strcmp(pTypeName, "TIME") == 0) ret = TRUE; 
            break;
       case SQL_TYPE_TIMESTAMP: 
            if(strcmp(pTypeName, "TIMESTAMP") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_MONTH: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_YEAR: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_YEAR_TO_MONTH: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_HOUR: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_MINUTE: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_SECOND: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY_TO_HOUR: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY_TO_MINUTE: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_DAY_TO_SECOND: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_HOUR_TO_MINUTE: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_HOUR_TO_SECOND: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       case SQL_INTERVAL_MINUTE_TO_SECOND: 
            if(strcmp(pTypeName, "INTERVAL") == 0) ret = TRUE; 
            break;
       default:
            break;
	}

    return ret;
}

char *sqlTypeToSqltypeName(SQLSMALLINT sqlType, char *pTypeName)
{
    switch (sqlType)
   {
       case SQL_CHAR: 
            sprintf(pTypeName, "CHAR"); 
            break;
       case SQL_VARCHAR: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_LONGVARCHAR: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_WCHAR: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_WVARCHAR: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_WLONGVARCHAR: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_DECIMAL: 
            sprintf(pTypeName, "DECIMAL");
            break;
       case SQL_NUMERIC: 
            sprintf(pTypeName, "DECIMAL");
            break;
       case SQL_SMALLINT: 
            sprintf(pTypeName, "SMALLINT");
            break;
       case SQL_INTEGER: 
            sprintf(pTypeName, "INTEGER UNSIGNED");
            break;
       case SQL_REAL: 
            sprintf(pTypeName, "REAL"); 
            break;
       case SQL_FLOAT: 
            sprintf(pTypeName, "DOUBLE PRECISION"); 
            break;
       case SQL_DOUBLE: 
            sprintf(pTypeName, "DOUBLE PRECISION"); 
            break;
       case SQL_BIT: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_TINYINT: 
            sprintf(pTypeName, "TINYINT");
            break;
       case SQL_BIGINT: 
            sprintf(pTypeName, "BIGINT");
            break;
       case SQL_BINARY: 
            sprintf(pTypeName, "BINARY"); 
            break;
       case SQL_VARBINARY: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_LONGVARBINARY: 
            sprintf(pTypeName, "VARCHAR"); 
            break;
       case SQL_TYPE_DATE: 
            sprintf(pTypeName, "DATE"); 
            break;
       case SQL_TYPE_TIME: 
            sprintf(pTypeName, "TIME"); 
            break;
       case SQL_TYPE_TIMESTAMP: 
            sprintf(pTypeName, "TIMESTAMP"); 
            break;
       case SQL_INTERVAL_MONTH: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_YEAR: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_YEAR_TO_MONTH: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_DAY: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_HOUR: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_MINUTE: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_SECOND: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_DAY_TO_HOUR: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_DAY_TO_MINUTE: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_DAY_TO_SECOND: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_HOUR_TO_MINUTE: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_HOUR_TO_SECOND: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       case SQL_INTERVAL_MINUTE_TO_SECOND: 
            sprintf(pTypeName, "INTERVAL"); 
            break;
       default:
            break;
	}

    return pTypeName;
}
int getSQLFromFile(FILE *fp, char *sql, unsigned int nsize)
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


