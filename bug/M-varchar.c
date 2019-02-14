//#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sqlext.h>
//#include <tchar.h>

void daig(SQLSMALLINT handleType, SQLHANDLE handle)
{
    SQLRETURN retcode;
    SQLCHAR   sqlState[10];
    SQLINTEGER nativeError;
    SQLCHAR    messageBuf[1024];
    SQLSMALLINT availableLen;

    SQLSMALLINT i = 1;
    retcode = SQLGetDiagRec(handleType, handle, i, sqlState, &nativeError, messageBuf, sizeof(messageBuf), &availableLen);
    while (SQL_SUCCEEDED(retcode))
    {
        printf("[%s][%d][%s]\n", sqlState, nativeError, messageBuf);
        //++i;
		i++;
		printf("line %d\n", __LINE__);
        retcode = SQLGetDiagRec(handleType, handle, i, sqlState, &nativeError, messageBuf, sizeof(messageBuf), &availableLen);
		printf("line %d\n", __LINE__);
    }
}

int check(SQLRETURN ret, char *func, SQLSMALLINT handleType, SQLHANDLE handle)
{
    if (SQL_SUCCEEDED(ret))
    {
        printf("%s SUCESS\n", func);
        return 0;
    }
    else
    {
        printf("%s FAILED\n", func);
        daig(handleType, handle);
        return -1;
    }
}

void testCharToDate(SQLHANDLE hstmt)
{
    SQLRETURN retcode;
    char buf[1024] = { '\0' };
    SQLLEN len = SQL_NTS;

    DATE_STRUCT date;

    char *stmts[] = {
        //"drop table SQLToDate",
        //"create table SQLToDate(c2 varchar(32767))",
		"create table tb",
        "insert into SQLToDate values('1997-10-11')",
        "select * from SQLToDate",
        "end"
    };
	size_t i;
    for (i = 0; strcmp(stmts[i], "end"); i++)
    {
        retcode = SQLExecDirect(hstmt, (SQLCHAR*)stmts[i], SQL_NTS);
        check(retcode, "SQLExecDirect", SQL_HANDLE_STMT, hstmt);
    }

    retcode = SQLBindCol(hstmt, 1, SQL_C_DATE, &date, 0, &len);
    check(retcode, "SQLBindCol", SQL_HANDLE_STMT, hstmt);
   

    retcode = SQLFetch(hstmt);
    check(retcode, "SQLFetch", SQL_HANDLE_STMT, hstmt);
}  // crashed here

int main(int argc, char *argv[])
{
    SQLHANDLE henv, hdbc, hstmt;
    SQLRETURN retcode;
    //char Instr[1024] = "DSN=ORADB;UID=system;PWD=123456";
    //char Instr[1024] = "DSN=mariaDB;UID=root;PWD=123456";
    //char Instr[1024] = "DSN=gyvm-15;UID=trafodion;PWD=traf123";
    //char Instr[1024] = "DSN=SQLServer;UID=sa;PWD=123456";
    char Instr[1024] = "DSN=traf;UID=trafodion;PWD=traf123";
    char Outstr[1024];
    SQLSMALLINT len;

    retcode = SQLAllocEnv(&henv);
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_NTS);
    retcode = SQLAllocConnect(henv, &hdbc);
    retcode = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)Instr, SQL_NTS, (SQLCHAR*)Outstr, 1024, &len, SQL_DRIVER_NOPROMPT);
    check(retcode, "SQLDriverConnect", SQL_HANDLE_DBC, hdbc);
    retcode = SQLAllocStmt(hdbc, &hstmt);

    testCharToDate(hstmt);

    if (hstmt != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    if (hdbc != SQL_NULL_HANDLE)
    {
        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    }
    if (henv != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    printf("test complete...\n");
    getchar();

    return 0;
}
