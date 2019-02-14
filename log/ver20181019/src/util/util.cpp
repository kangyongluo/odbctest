#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "global.h"


#define CONFIG_TEST_NO_COAST


TCHAR *SQLTypeToChar(SWORD SQLType, TCHAR *buf)
{
   switch (SQLType)
   {
       case SQL_CHAR: _tcscpy(buf, _T("SQL_CHAR")); break;
       case SQL_VARCHAR: _tcscpy(buf, _T("SQL_VARCHAR")); break;
       case SQL_LONGVARCHAR: _tcscpy(buf, _T("SQL_LONGVARCHAR")); break;
       case SQL_WCHAR: _tcscpy(buf, _T("SQL_WCHAR")); break;
       case SQL_WVARCHAR: _tcscpy(buf, _T("SQL_WVARCHAR")); break;
       case SQL_WLONGVARCHAR: _tcscpy(buf, _T("SQL_WLONGVARCHAR")); break;
       case SQL_DECIMAL: _tcscpy(buf, _T("SQL_DECIMAL")); break;
       case SQL_NUMERIC: _tcscpy(buf, _T("SQL_NUMERIC")); break;
       case SQL_SMALLINT: _tcscpy(buf, _T("SQL_SMALLINT")); break;
       case SQL_INTEGER: _tcscpy(buf, _T("SQL_INTEGER")); break;
       case SQL_REAL: _tcscpy(buf, _T("SQL_REAL")); break;
       case SQL_FLOAT: _tcscpy(buf, _T("SQL_FLOAT")); break;
       case SQL_DOUBLE: _tcscpy(buf, _T("SQL_DOUBLE")); break;
       case SQL_BIT: _tcscpy(buf, _T("SQL_BIT")); break;
       case SQL_TINYINT: _tcscpy(buf, _T("SQL_TINYINT")); break;
       case SQL_BIGINT: _tcscpy(buf, _T("SQL_BIGINT")); break;
       case SQL_BINARY: _tcscpy(buf, _T("SQL_BINARY")); break;
       case SQL_VARBINARY: _tcscpy(buf, _T("SQL_VARBINARY")); break;
       case SQL_LONGVARBINARY: _tcscpy(buf, _T("SQL_LONGVARBINARY")); break;
       case SQL_TYPE_DATE: _tcscpy(buf, _T("SQL_TYPE_DATE")); break;
       case SQL_TYPE_TIME: _tcscpy(buf, _T("SQL_TYPE_TIME")); break;
       case SQL_TYPE_TIMESTAMP: _tcscpy(buf, _T("SQL_TYPE_TIMESTAMP")); break;
       //case SQL_TYPE_UTCDATETIME: _tcscpy(buf, _T("SQL_TYPE_UTCDATETIME")); break; // unsupport
       //case SQL_TYPE_UTCTIME: _tcscpy(buf, _T("SQL_TYPE_UTCTIME")); break; // unsupport
       case SQL_INTERVAL_MONTH: _tcscpy(buf, _T("SQL_INTERVAL_MONTH")); break;
       case SQL_INTERVAL_YEAR: _tcscpy(buf, _T("SQL_INTERVAL_YEAR")); break;
       case SQL_INTERVAL_YEAR_TO_MONTH: _tcscpy(buf, _T("SQL_INTERVAL_YEAR_TO_MONTH")); break;
       case SQL_INTERVAL_DAY: _tcscpy(buf, _T("SQL_INTERVAL_DAY")); break;
       case SQL_INTERVAL_HOUR: _tcscpy(buf, _T("SQL_INTERVAL_HOUR")); break;
       case SQL_INTERVAL_MINUTE: _tcscpy(buf, _T("SQL_INTERVAL_MINUTE")); break;
       case SQL_INTERVAL_SECOND: _tcscpy(buf, _T("SQL_INTERVAL_SECOND")); break;
       case SQL_INTERVAL_DAY_TO_HOUR: _tcscpy(buf, _T("SQL_INTERVAL_DAY_TO_HOUR")); break;
       case SQL_INTERVAL_DAY_TO_MINUTE: _tcscpy(buf, _T("SQL_INTERVAL_DAY_TO_MINUTE")); break;
       case SQL_INTERVAL_DAY_TO_SECOND: _tcscpy(buf, _T("SQL_INTERVAL_DAY_TO_SECOND")); break;
       case SQL_INTERVAL_HOUR_TO_MINUTE: _tcscpy(buf, _T("SQL_INTERVAL_HOUR_TO_MINUTE")); break;
       case SQL_INTERVAL_HOUR_TO_SECOND: _tcscpy(buf, _T("SQL_INTERVAL_HOUR_TO_SECOND")); break;
       case SQL_INTERVAL_MINUTE_TO_SECOND: _tcscpy(buf, _T("SQL_INTERVAL_MINUTE_TO_SECOND")); break;
       case SQL_GUID: _tcscpy(buf, _T("SQL_GUID")); break;
       case SQL_ALL_TYPES: _tcscpy(buf, _T("SQL_ALL_TYPES")); break;
       default:
         //_itot(SQLType,buf,10);
         break;
	}
	return buf;
}
TCHAR *SQLCTypeToChar(SWORD CType, TCHAR *buf)
{
   switch (CType) {
      case SQL_C_CHAR:			_stprintf (buf,_T("SQL_C_CHAR"),CType);break;
      case SQL_C_WCHAR:			_stprintf (buf,_T("SQL_C_WCHAR"),CType);break;
	  case SQL_C_LONG:			_stprintf (buf,_T("SQL_C_LONG"),CType);break;
      case SQL_C_SHORT:			_stprintf (buf,_T("SQL_C_SHORT"),CType);break;
      case SQL_C_FLOAT:			_stprintf (buf,_T("SQL_C_FLOAT"),CType);break;
      case SQL_C_DOUBLE:		_stprintf (buf,_T("SQL_C_DOUBLE"),CType);break;
      case SQL_C_NUMERIC:		_stprintf (buf,_T("SQL_C_NUMERIC"),CType);break;
      case SQL_C_DEFAULT:		_stprintf (buf,_T("SQL_C_DEFAULT"),CType);break;
      case SQL_C_DATE:			_stprintf (buf,_T("SQL_C_DATE"),CType);break;
      case SQL_C_TIME:			_stprintf (buf,_T("SQL_C_TIME"),CType);break;
      case SQL_C_TIMESTAMP:			_stprintf (buf,_T("SQL_C_TIMESTAMP"),CType);break;
      case SQL_C_TYPE_DATE:			_stprintf (buf,_T("SQL_C_TYPE_DATE"),CType);break;
      case SQL_C_TYPE_TIME:			_stprintf (buf,_T("SQL_C_TYPE_TIME"),CType);break;
      case SQL_C_TYPE_TIMESTAMP:		_stprintf (buf,_T("SQL_C_TYPE_TIMESTAMP"),CType);break;
      case SQL_C_INTERVAL_YEAR:		_stprintf (buf,_T("SQL_C_INTERVAL_YEAR"),CType);break;
      case SQL_C_INTERVAL_MONTH:		_stprintf (buf,_T("SQL_C_INTERVAL_MONTH"),CType);break;
      case SQL_C_INTERVAL_DAY:		_stprintf (buf,_T("SQL_C_INTERVAL_DAY"),CType);break;
      case SQL_C_INTERVAL_HOUR:		_stprintf (buf,_T("SQL_C_INTERVAL_HOUR"),CType);break;
      case SQL_C_INTERVAL_MINUTE:	_stprintf (buf,_T("SQL_C_INTERVAL_MINUTE"),CType);break;
      case SQL_C_INTERVAL_SECOND:	_stprintf (buf,_T("SQL_C_INTERVAL_SECOND"),CType);break;
      case SQL_C_INTERVAL_YEAR_TO_MONTH:		_stprintf (buf,_T("SQL_C_INTERVAL_YEAR_TO_MONTH"),CType);break;
      case SQL_C_INTERVAL_DAY_TO_HOUR:			_stprintf (buf,_T("SQL_C_INTERVAL_DAY_TO_HOUR"),CType);break;
      case SQL_C_INTERVAL_DAY_TO_MINUTE:		_stprintf (buf,_T("SQL_C_INTERVAL_DAY_TO_MINUTE"),CType);break;
      case SQL_C_INTERVAL_DAY_TO_SECOND:		_stprintf (buf,_T("SQL_C_INTERVAL_DAY_TO_SECOND"),CType);break;
      case SQL_C_INTERVAL_HOUR_TO_MINUTE:		_stprintf (buf,_T("SQL_C_INTERVAL_HOUR_TO_MINUTE"),CType);break;
      case SQL_C_INTERVAL_HOUR_TO_SECOND:		_stprintf (buf,_T("SQL_C_INTERVAL_HOUR_TO_SECOND"),CType);break;
      case SQL_C_INTERVAL_MINUTE_TO_SECOND:	_stprintf (buf,_T("SQL_C_INTERVAL_MINUTE_TO_SECOND"),CType);break;
      case SQL_C_BINARY:		_stprintf (buf,_T("SQL_C_BINARY"),CType);break;
      case SQL_C_BIT:			_stprintf (buf,_T("SQL_C_BIT"),CType);break;
      case SQL_C_SBIGINT:		_stprintf (buf,_T("SQL_C_SBIGINT"),CType);break;
      case SQL_C_UBIGINT:		_stprintf (buf,_T("SQL_C_UBIGINT"),CType);break;
      case SQL_C_TINYINT:		_stprintf (buf,_T("SQL_C_TINYINT"),CType);break;
      case SQL_C_SLONG:			_stprintf (buf,_T("SQL_C_SLONG"),CType);break;
      case SQL_C_SSHORT:		_stprintf (buf,_T("SQL_C_SSHORT"),CType);break;
      case SQL_C_STINYINT:		_stprintf (buf,_T("SQL_C_STINYINT"),CType);break;
      case SQL_C_ULONG:			_stprintf (buf,_T("SQL_C_ULONG"),CType);break;
      case SQL_C_USHORT:		_stprintf (buf,_T("SQL_C_USHORT"),CType);break;
      case SQL_C_UTINYINT:		_stprintf (buf,_T("SQL_C_UTINYINT"),CType);break;
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

void fillData(TCHAR *buf, int size)
{
    TCHAR szData[256] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()");
    int len;
    int offset;
    TCHAR szBuf[64] = {0};
    TCHAR szCrc[10] = {0};
    time_t tm;
    unsigned int crc = 0;

    if(size == 0){
        buf[0] = 0;
        return;
    }
    for(len = 0, offset = 0; len < size; len += offset){
        if((size > 8) && (len == 0)){
            len = 8;
        }
        tm = time( NULL);
        _tcsftime(szBuf, sizeof(szBuf) / sizeof(TCHAR), _T("%Y-%m-%d_%H.%M.%S"), localtime(&tm));
        memcpy(szData, szBuf, _tcslen(szBuf));
        offset = ((size - len) >= (int)_tcslen(szData)) ? (int)_tcslen(szData) : (size - len);
        _tcsncpy(&buf[len], szData, offset);
    }
    buf[len] = 0x00;
    if(size > 8){
        crc = calcCRC32((unsigned char *)&buf[8], size - 8);
        hexToAscii((unsigned char *)&crc, (unsigned char *)szCrc, sizeof(crc));
        buf[0] = szCrc[0];
        buf[1] = szCrc[1];
        buf[2] = szCrc[2];
        buf[3] = szCrc[3];
        buf[4] = szCrc[4];
        buf[5] = szCrc[5];
        buf[6] = szCrc[6];
        buf[7] = szCrc[7];
    }
    
}
void createRandNumericStr(SQLCHAR precision, SQLCHAR scale, char *szNumeric, double maxValue)
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
void setSqlData(TCHAR *pSqlData, 
                  int size, 
                  int beginVal,
                  SQLSMALLINT sqlType,
                  SQLULEN columnSize, 
                  SQLSMALLINT decimalDigits
                  )
{
    TCHAR szBuf[128] = {0};
    double maxValues = beginVal * 1.0;

    switch(sqlType){
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
        fillData(pSqlData, size);
        break;
    case SQL_DECIMAL:
        createRandNumericStr(columnSize, decimalDigits, szBuf, maxValues);
        _stprintf(pSqlData, _T("%s"), szBuf);
        break;
    case SQL_NUMERIC:
        createRandNumericStr(columnSize, decimalDigits, szBuf, maxValues);
        _stprintf(pSqlData, _T("%s"), szBuf);
        break;
    case SQL_TINYINT:
        _stprintf(pSqlData, _T("%d"), (beginVal > 127) ? 127 : beginVal);
        break;
    case SQL_SMALLINT:
        _stprintf(pSqlData, _T("%d"), (beginVal > 32767) ? 32767 : beginVal);
        break;
    case SQL_INTEGER:
        _stprintf(pSqlData, _T("%d"), beginVal);
        break;
    case SQL_BIGINT:
        _stprintf(pSqlData, _T("%d"), beginVal);
        break;
    case SQL_REAL:
        _stprintf(pSqlData, _T("%d"), beginVal);
        break;
    case SQL_DOUBLE:
        _stprintf(pSqlData, _T("%d.%d"), beginVal, 0);
        break;
    case SQL_TYPE_DATE:
        _stprintf(pSqlData, _T("%04d-%02d-%02d"), 2000 + beginVal, 2, 11);
        break;
    case SQL_TYPE_TIME:
        _stprintf(pSqlData, _T("%02d:%02d:%02d"), beginVal & 0x0f, 2, 5);
        break;
    case SQL_TYPE_TIMESTAMP:
        _stprintf(pSqlData, _T("%04d-%02d-%02d %02d:%02d:%02d"), 2000 + beginVal, 2, 11, beginVal & 0x0f, 2, 11);
        break;
    case SQL_INTERVAL_YEAR:
        _stprintf(pSqlData, _T("%u"), beginVal);
        break;
    case SQL_INTERVAL_MONTH:
        _stprintf(pSqlData, _T("%d"), (beginVal > 11) ? 11 : beginVal);
        break;
    case SQL_INTERVAL_DAY:
        _stprintf(pSqlData, _T("%d"), beginVal);
        break;
    case SQL_INTERVAL_HOUR:
        _stprintf(pSqlData, _T("%d"), (beginVal > 11) ? 11 : beginVal);
        break;
    case SQL_INTERVAL_MINUTE:
        _stprintf(pSqlData, _T("%d"), (beginVal > 59) ? 59 : beginVal);
        break;
    case SQL_INTERVAL_SECOND:
        _stprintf(pSqlData, _T("%d"), (beginVal > 59) ? 59 : beginVal);
        break;
    case SQL_INTERVAL_YEAR_TO_MONTH:
        _stprintf(pSqlData, _T("%02d-%02d"), (beginVal > 59) ? 59 : beginVal, 1);
        break;
    case SQL_INTERVAL_DAY_TO_HOUR:
        _stprintf(pSqlData, _T("%u %02d"), beginVal, 1);
        break;
    case SQL_INTERVAL_DAY_TO_MINUTE:
        _stprintf(pSqlData, _T("%02d %02d:%02d"), (beginVal > 59) ? 59 : beginVal, 0, 1);
        break;
    case SQL_INTERVAL_DAY_TO_SECOND:
        _stprintf(pSqlData, _T("%02d %02d:%02d:%02d"), (beginVal > 59) ? 59 : beginVal, 1, 1, 1);
        break;
    case SQL_INTERVAL_HOUR_TO_MINUTE:
        _stprintf(pSqlData, _T("%02d:%02d"), (beginVal > 11) ? 11 : beginVal, 1);
        break;
    case SQL_INTERVAL_HOUR_TO_SECOND:
        _stprintf(pSqlData, _T("%02d:%02d:%02d"), (beginVal > 11) ? 11 : beginVal, 1, 1);
        break;
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        _stprintf(pSqlData, _T("%02d:%02d"), (beginVal > 59) ? 59 : beginVal, 1);
        break;
    default:
        _stprintf(pSqlData, _T("%d"), (beginVal > 127) ? 127 : beginVal);
        break;
    }
}
FILE *openLog(TCHAR *headKey)
{
    FILE *fp = NULL;
    time_t tm;
    TCHAR szBuf[128] = {0};
    TCHAR szLogKey[128] = {0};
    
    if((headKey != NULL) && (_tcslen(headKey) >= 0)){
        _stprintf(szBuf, "%s_", headKey);
        tm = time(NULL);
        strftime(&szBuf[_tcslen(szBuf)], sizeof(szBuf) - _tcslen(szBuf), "%Y-%m-%d_%H.%M.%S.log", localtime(&tm));
        _stprintf(szLogKey, szBuf);
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

void LogMsgEx(FILE *fp, unsigned int option, TCHAR *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    TCHAR szBuf[128] = {0};

    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));
    if(fp == NULL){
        return;
    }
    if(fp != NULL){
        if(option == SHORTTIMESTAMP || option == TIME_STAMP){
            _ftprintf(fp, "%s", timeStr);
        }
        va_start(argsList,format);
        _vftprintf(fp, format, argsList);
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
void LogMsgGlobal(unsigned int option, TCHAR *format, ...)
{
    char timeStr[50] = {0};
    va_list argsList;
    TCHAR szBuf[128] = {0};
    FILE *fp = gTestGlobal.fpLog;

    time_t tm = time( NULL);
    strftime(timeStr, sizeof(timeStr) - 1, "[%Y-%m-%d %H.%M.%S]: ", localtime(&tm));

    if(fp != NULL){
        if(option == SHORTTIMESTAMP || option == TIME_STAMP){
            _ftprintf(fp, "%s", timeStr);
        }
        va_start(argsList,format);
        _vftprintf(fp, format, argsList);
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


