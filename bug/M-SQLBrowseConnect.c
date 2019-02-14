#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*******************************************************************************************************************/
int M_SQLBrowseConnect(TestInfo *pTestInfo)
{
    SQLLEN len;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    TCHAR szConnStrIn[512] = {0};
    TCHAR szConnStrOut[512] = {0};
    SWORD		cbConnStrOut;
    int ret = 0;

    retcode = SQLAllocEnv(&henv);                 /* Environment handle */
    if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("call SQLAllocEnv fail.\n"));
        return -1;
    }
	retcode = SQLAllocConnect(henv, &hdbc);    /* Connection handle  */
	if(retcode != SQL_SUCCESS){
        LogMsg(NONE, _T("call SQLAllocConnect fail.\n"));
        LogAllErrors(henv,hdbc,hstmt);
        return -1;
    }
    _stprintf(szConnStrIn, _T("DSN=%s;UID=%s;PWD=%s;"), pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password);
    _tprintf(_T("in url:%s\n"),szConnStrIn);
	retcode = SQLBrowseConnect(hdbc,szConnStrIn,SQL_NTS, szConnStrOut,sizeof(szConnStrOut),&cbConnStrOut);
	if(retcode != SQL_SUCCESS){
		LogAllErrors(henv,hdbc,hstmt);
        ret = -1;
	}
    else{
        
		_tprintf(_T("Out url:%s\n"),szConnStrOut);
    }
TEST_FAIL:
    disconnect(henv, hdbc, hstmt);
    LogMsg(TIME_STAMP, _T("complete test:%s\n"), (ret == 0) ? _T("sucess") : _T("fail")); 
    
    return ret;
}
/*******************************************************************************************************************/
void help(void)
{
    printf("\t-h:print this help\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema name\t");
    printf("sample:\t./debug -d TRAF_ANSI -u trafodion -p traf123 -s ODBCTEST\n");
}
//g++  xxx.c -o debug -I /usr/lib64 -L /usr/lib64 -ltrafodbc64 -lpthread -lz -licuuc -licudata -Dunixcli
//g++ xxx.c -o -Wall -Dunixcli -g -w  -I.  -lodbc -lpthread -DUNIXODBC -o debug
//./debug -d TRAF_ANSI -u trafodion -p traf123
int main(int argc, char *argv[])
{
    TestInfo oriInfo;
    char buf[256] = {0};
    int num;
 
    memset(&oriInfo, 0, sizeof(oriInfo));
   
    if(argc <= 1){
        help();
        return -1;
    }
    for(num = 1; num < argc; num++){
        if(_tcscmp(argv[num], _T("-h")) == 0){
            help();
            return 0;
        }
        else if((_tcscmp(argv[num], _T("-d")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.DataSource, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-u")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.UserID, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-p")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Password, argv[num]);
        }
        else if((_tcscmp(argv[num], _T("-s")) == 0) && (argc > (num + 1))){
            num++;
            _tcscpy (oriInfo.Schema, argv[num]);
        }
    }
    if(_tcslen(oriInfo.DataSource) == 0 ||
        _tcslen(oriInfo.UserID) == 0 ||
        _tcslen(oriInfo.Password) == 0/* ||
        _tcslen(oriInfo.Schema) == 0*/
        ){
        help();
#ifndef unixcli
        printf("Please enter any key exit\n");
        getchar();
#endif
        return -1;
    }
    M_SQLBrowseConnect(&oriInfo);
#ifndef unixcli
    printf("Please enter any key exit\n");
    getchar();
#endif
    return 0;
}


