/**
  @@@ START COPYRIGHT @@@

  (C) Copyright 2015 Hewlett-Packard Development Company, L.P.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  @@@ END COPYRIGHT @@@
*/
#ifndef _U_TCHAR_H
#define _U_TCHAR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNICODE

#ifdef __cplusplus
} /* ... extern "C" ... */
#endif

#include <wchar.h>
//#include "wcharsplmt.h"
#include <stddef.h>
#include <inttypes.h>
//#include <unicode/utypes.h>
//#include <unicode/ustring.h>
//#include <unicode/uiter.h>
//#include <unicode/ustdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef unixODBC
#ifndef TCHAR
#define TCHAR	UChar	
#endif
#endif


#define _TEOF		WEOF

//#define __T(x)			(UChar*)u##x
#define __T(x)		(UChar*)(L##x)
//#define __T(x)		(UChar*)((UChar*)(L##x)+1)
//#define __TCHR(x)	(L##x)

/** Program **/
#define _tmain      main

/* i/o Functions */
#define _tprintf		u_printf_conv
#define _ftprintf       u_fprintf_conv
#define _stprintf       u_sprintf_conv
#define _vftprintf		u_vfprintf_conv
#define _tfopen			u_fopen_u
#define _tflush		u_fflush
#define _putts          u_printf_conv
#define _fputts			u_fputs
#define _fputtc         u_fputc
#define _tperror		u_perror
#define _fgetts         u_fgets

/* Set Locale funciton */
#define _tsetlocale		_wsetlocale

/* Time Function */
//#define _tcsftime		wcsftime
#define _tcsftime		u_strftime
#define _tctime			u_ctime

/* String conversion Functions */
#define _tstof			ucnv_tof
#define _itot			ucnv_fromi
#define _ltot			ucnv_froml
#define _tstoi			ucnv_toi	
#define mbstowcs		u_mbstoucs

/* Redundant "logical-character" mappings */
#define _tcsupr			u_strupr

/* String Functions */
#define _tcscat         u_strcat
#define _tcsncat        u_strncat
#define _tcschr         u_strchr
#define _tcscpy(x, y)      u_strcpy((UChar*)x, (const UChar*)y)
#define _tcslen(x)         u_strlen((UChar*)x)
#define _tcsncpy(x,y,n)    u_strncpy((UChar*)x,(const UChar*)y,n)
#define _tcsstr         u_strstr
#define _tcstok         u_strtok
#define _tcsdup			u_strdup 
#define _tcscmp(x,y)         u_strcmp((const UChar*)x,(const UChar*)y)
#define _tcsicmp(x,y)        u_stricmp((const UChar*)x,(const UChar*)y)
#define _tcsncmp(x,y,n)        u_strncmp((const UChar*)x,(const UChar*)y,n)
#define _tcsnicmp       u_strnicmp
#define _tmemset       u_memset

/* Reserved functions for possible future usage*/
#define _tcscspn        u_strcspn 
#define _totupper       (TCHAR)toupper
#define _totlower       (TCHAR)tolower
#else  /* ndef _UNICODE */

#ifdef __cplusplus
}		/* ...extern "C"... */
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TCHAR
#define TCHAR	char
#endif

#define __T(x)		x

/*** Program ***/
#define _tmain			main
#define _tsetlocale		setlocale
#define _tperror		perror

/*** Time Function ***/
#define _tcsftime		strftime
#define _tctime			ctime

/*** File and unformated I/O funcions ***/
#define _tfopen			fopen
#define _tflush		fflush
#define _putts			puts
#define _fgetts         fgets
#define _fputts			fputs
#define _fputtc			fputc
#define _tcschr         strchr

/*** formated i/o ***/
#define _tprintf		printf
#define _ftprintf		fprintf
#define _vftprintf		vfprintf

/*** String operation functions ***/
#define _tcslen			strlen
#define _tcscspn        strcspn
#define _tcstok         strtok
#define _tcsstr         strstr
#define _tcsncpy        strncpy
#define _stprintf		sprintf
#define _tcscpy			strcpy
#define _tcscat         strcat
#define _tcsncat        strncat
#define _tcsnicmp		strncasecmp
#define _tcsicmp		strcasecmp
#define _tcscmp			strcmp
#define _tcsncmp		strncmp
#define _tmemset		memset
#ifdef unixcli
#define _tcsdup			strdup
#define _totupper       toupper
#define _totlower		tolower
#endif /*unixcli*/
/* String conversion Functions */
#define _tstof			atof
#define _itot			_itoa
#define _ltot			_ltoa
#define _tstoi			atoi

/* Redundant "logical-character" mappings */
#define _tcsupr			_strupr

#endif /* _UNICODE */


#ifndef unixcli
#if _MSC_VER
#define snprintf    _snprintf
#define strcasecmp  _stricmp
#endif /*_MSC_VER*/
#endif

#define _T(x)		__T(x)
#define _TEXT(x)	__T(x)

#ifdef __cplusplus
}
#endif


#endif /* ... _TCHAR_L_H */
