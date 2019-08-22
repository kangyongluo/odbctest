#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbcdriver.h"

void LogAllErrors(SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt
                  )
{             
    TCHAR	buf[MAX_STRING_SIZE];
    TCHAR	State[STATE_SIZE];
    char szBuf[MAX_STRING_SIZE];
    char szState[STATE_SIZE];
	SDWORD	NativeError;
    RETCODE returncode;
	BOOL MsgDisplayed = FALSE;
    int errorCount = 0;

   /* Log any henv error messages */
   returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((errorCount++) > 10){
            errorCount = 0;
            break;
        }
        MsgDisplayed = TRUE;
        tcharToChar(State, szState);
        tcharToChar(buf, szBuf);
        printf("   State: %s\n"
			"   Native Error: %ld\n"
                "   Error: %s\n",szState,NativeError,szBuf);
      returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }
    errorCount = 0;
   /* Log any hdbc error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((errorCount++) > 10){
            errorCount = 0;
            break;
        }
        MsgDisplayed = TRUE;
        tcharToChar(State, szState);
        tcharToChar(buf, szBuf);
        printf("   State: %s\n"
			"   Native Error: %ld\n"
                "   Error: %s\n",szState,NativeError,szBuf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }
    errorCount = 0;
   /* Log any hstmt error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        MsgDisplayed = TRUE;
        tcharToChar(State, szState);
        tcharToChar(buf, szBuf);
        printf("   State: %s\n"
			"   Native Error: %ld\n"
                "   Error: %s\n",szState,NativeError,szBuf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		printf("There were no error messages for SQLError() to display\n");
} 

void LogAllErrorsEx(FILE *fpLog,
                  SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt
                  )
{             
    TCHAR	buf[MAX_STRING_SIZE];
    TCHAR	State[STATE_SIZE];
    char szBuf[MAX_STRING_SIZE];
    char szState[STATE_SIZE];
	SDWORD	NativeError;
    RETCODE returncode;
	BOOL MsgDisplayed = FALSE;
    int errorCount = 0;

   /* Log any henv error messages */
   returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        MsgDisplayed = TRUE;
        tcharToChar(State, szState);
        tcharToChar(buf, szBuf);
        LogMsgEx(fpLog, NONE, "   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",szState,NativeError,szBuf);
      returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }
    errorCount = 0;
   /* Log any hdbc error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        MsgDisplayed = TRUE;
        tcharToChar(State, szState);
        tcharToChar(buf, szBuf);
        LogMsgEx(fpLog, NONE, "   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",szState,NativeError,szBuf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }
    errorCount = 0;
   /* Log any hstmt error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        MsgDisplayed = TRUE;
        tcharToChar(State, szState);
        tcharToChar(buf, szBuf);
        LogMsgEx(fpLog, NONE, "   State: %s\n"
							  "   Native Error: %ld\n"
                "   Error: %s\n",szState,NativeError,szBuf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		LogMsgEx(fpLog, NONE, "There were no error messages for SQLError() to display\n");
} 

TrueFalse FullConnectWithOptions(TestInfo *pTestInfo, int Options)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR OutString[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR connectStr[1024] = {0x0};
    char szDdl[1024] = {0};
    //TCHAR dsn[128], user[128], pwd[128];
    
	assert( (Options&CONNECT_ODBC_VERSION_2) || (Options&CONNECT_ODBC_VERSION_3) );

	/* Initialize the basic handles needed by ODBC */
	if (Options&CONNECT_ODBC_VERSION_2)
	{
		returncode = SQLAllocEnv(&henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return (FALSE);
        }
		
		returncode = SQLAllocConnect(henv,&hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeEnv(henv);
			return(FALSE);
		}
	}
	if (Options&CONNECT_ODBC_VERSION_3)
	{
		returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return(FALSE);
        }

		returncode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return (FALSE);
		}

		returncode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return (FALSE);
		}
	}

	// Handle Autocommit_Off Option
	if (Options&CONNECT_AUTOCOMMIT_OFF)
    {/*
		returncode = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			// Cleanup.  No need to check return codes since we are already failing
			SQLFreeEnv(henv);
			return (FALSE);
		}*/
	}
#if 1
    sprintf(szDdl, "DSN=%s;UID=%s;PWD=%s;", pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password);
    charToTChar(szDdl, connectStr);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)connectStr, SQL_NTS, (SQLTCHAR*)OutString, sizeof(OutString), &olen, SQL_DRIVER_NOPROMPT);
#else
    charToTChar(pTestInfo->DataSource, dsn);
    charToTChar(pTestInfo->UserID, user);
    charToTChar(pTestInfo->Password, pwd);
    returncode = SQLConnect(hdbc, (SQLTCHAR *)dsn, SQL_NTS, (SQLTCHAR *)user, SQL_NTS, (SQLTCHAR *)pwd, SQL_NTS);
#endif
	if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
	{
		// Free up handles since we hit a problem.
		if (Options&CONNECT_ODBC_VERSION_2){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return (FALSE);
		}
		if (Options&CONNECT_ODBC_VERSION_3){
			/* Cleanup.  No need to check return codes since we are already failing */		   
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
            return (FALSE);
		}
        return(FALSE);
	}

	pTestInfo->henv = (SQLHANDLE)henv;
	pTestInfo->hdbc = (SQLHANDLE)hdbc;
	
	/* Connection established */
	return (TRUE);
}  /* FullConnectWithOptions() */
TrueFalse FullConnectWithOptionsEx(TestInfo *pTestInfo, int Options, char *connStrEx, FILE *fpLog)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR OutString[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR connectStr[1024] = {0x0};
    char szDdl[1024] = {0};
    //TCHAR dsn[128], user[128], pwd[128];
    
	assert( (Options&CONNECT_ODBC_VERSION_2) || (Options&CONNECT_ODBC_VERSION_3) );

	/* Initialize the basic handles needed by ODBC */
	if (Options&CONNECT_ODBC_VERSION_2)
	{
		returncode = SQLAllocEnv(&henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return (FALSE);
        }
		
		returncode = SQLAllocConnect(henv,&hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeEnv(henv);
			return(FALSE);
		}
	}
	if (Options&CONNECT_ODBC_VERSION_3)
	{
		returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return(FALSE);
        }

		returncode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return (FALSE);
		}

		returncode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return (FALSE);
		}
	}

	// Handle Autocommit_Off Option
	if (Options&CONNECT_AUTOCOMMIT_OFF)
    {/*
		returncode = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			// Cleanup.  No need to check return codes since we are already failing
			SQLFreeEnv(henv);
			return (FALSE);
		}*/
	}  
#if 1
    sprintf(szDdl, "DSN=%s;UID=%s;PWD=%s;%s", pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password, connStrEx);
    charToTChar(szDdl, connectStr);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)connectStr, SQL_NTS, (SQLTCHAR*)OutString, sizeof(OutString), &olen, SQL_DRIVER_NOPROMPT);
#else
    charToTChar(pTestInfo->DataSource, dsn);
    charToTChar(pTestInfo->UserID, user);
    charToTChar(pTestInfo->Password, pwd);
    returncode = SQLConnect(hdbc, (SQLTCHAR *)dsn, SQL_NTS, (SQLTCHAR *)user, SQL_NTS, (SQLTCHAR *)pwd, SQL_NTS);
#endif
    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
	{
	    LogMsgEx(fpLog, NONE, "[%s:%d]Call SQLConnect fail.\n", __FILE__, __LINE__);
	    LogAllErrorsEx(fpLog, henv, hdbc, SQL_NULL_HANDLE);
		// Free up handles since we hit a problem.
		if (Options&CONNECT_ODBC_VERSION_2){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return (FALSE);
		}
		if (Options&CONNECT_ODBC_VERSION_3){
			/* Cleanup.  No need to check return codes since we are already failing */		   
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
            return (FALSE);
		}
        return(FALSE);
	}

	pTestInfo->henv = (SQLHANDLE)henv;
	pTestInfo->hdbc = (SQLHANDLE)hdbc;
	
	/* Connection established */
	return (TRUE);
}  /* FullConnectWithOptions() */

int FullDisconnectEx(FILE *fpLog, TestInfo *pTestInfo)
{
  RETCODE returncode;                        
  TrueFalse Result;
  
  Result=TRUE;
   
  /* Disconnect from the data source.  If errors, still go on and */
  /* try to free the handles */
  returncode = SQLDisconnect((SQLHANDLE)pTestInfo->hdbc);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
        LogAllErrorsEx(fpLog, pTestInfo->henv, pTestInfo->hdbc, pTestInfo->hstmt);
	}
   
  /* Free up all handles used by this connection */
  returncode = SQLFreeConnect((SQLHANDLE)pTestInfo->hdbc);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
		LogAllErrorsEx(fpLog, pTestInfo->henv,pTestInfo->hdbc,pTestInfo->hstmt);
	}

   
  returncode = SQLFreeEnv((SQLHANDLE)pTestInfo->henv);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
		LogAllErrorsEx(fpLog, pTestInfo->henv,pTestInfo->hdbc,pTestInfo->hstmt);
	}
   
  return(Result);
}  /* FullDisconnectEx() */
int addInfoSessionEx(SQLHSTMT hstmt, FILE *fpLog)
{
    SQLRETURN returnCode;
    char szBuf[256];
    TCHAR szQuery[256] = {0};
    SQLLEN len;
    int i;

    charToTChar("info session", szQuery);
    returnCode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
    if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO)){
        LogMsgEx(NULL, NONE, "Execute \"info session\" fail\n");
        return -1;
    }
    else{
        SQLSMALLINT num;
        const char *InfoSession[] = {
            "SESSION_ID",
            "SERVER_PROCESS_NAME",
            "SERVER_PROCESS_ID",
            "SERVER_HOST",
            "SERVER_PORT",
            "MAPPED_SLA",
            "MAPPED_CONNECT_PROFILE",
            "MAPPED_DISCONNECT_PROFILE",
            "CONNECTED_INTERVAL_SEC",
            "CONNECT_TIME",
            "DATABASE_USER_NAME",
            "USER_NAME",
            "ROLE_NAME",
            "APP_NAME",
            "TENANT_NAME",
            "END"
        };
        int sessionId;
        
        returnCode = SQLFetch(hstmt);
        returnCode = SQLNumResultCols(hstmt, &num);
        if(num > 15){
            num = 15;
        }
        sessionId = 0;
        while (returnCode != SQL_NO_DATA)
        {
            for (i = 1; i <= num; ++i)
            {
                returnCode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)szQuery, sizeof(szQuery), &len);
                if (SQL_SUCCEEDED(returnCode)){
                    if(strcmp(InfoSession[sessionId], "END") != 0){
#ifdef UNICODE
                        ucToAscii(szQuery, szBuf);
#else
                        sprintf(szBuf, szQuery);
#endif
                        LogMsgEx(fpLog, NONE, "%s:%s\n", InfoSession[sessionId], szBuf);
                        sessionId++;
                    }
                    else{
                        break;
                    }
                }
                else{
                    break;
                }
            }
            returnCode = SQLFetch(hstmt);
        }
    }
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);
        
    return 0;
}
int cleanupTableData(TestInfo *pTestInfo, char *szTableName, FILE *fpLog)
{
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    SQLHANDLE hstmt = SQL_NULL_HANDLE;
    char szBuf[128] = {0};
    TCHAR szQuery[128] = {0};
    RETCODE retcode;
    TestInfo sTestInfo;
    
    memcpy(&sTestInfo, pTestInfo, sizeof(TestInfo));
    if (!FullConnectWithOptions(&sTestInfo, CONNECT_ODBC_VERSION_3))
    {
        LogMsgEx(fpLog, NONE, "Unable to connect\n");
        return -1;
    }
    henv = sTestInfo.henv;
    hdbc = sTestInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)){
        LogMsgEx(fpLog, NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n", __LINE__);
        LogMsgEx(fpLog, NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsgEx(fpLog, NONE,"SQLAllocStmt hstmt fail, line %d\n", __LINE__);
            FullDisconnectEx(fpLog, &sTestInfo);
            return -1;
        }
    }
    sTestInfo.hstmt = hstmt;
    
    sprintf(szBuf, "purgedata %s", szTableName);
    LogMsgEx(fpLog, NONE, "%s\n", szBuf);
    charToTChar(szBuf, szQuery);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
    if(retcode != SQL_SUCCESS){
        LogAllErrorsEx(fpLog, henv, hdbc, hstmt);
        FullDisconnectEx(fpLog, &sTestInfo);
        return -1;
    }
    FullDisconnectEx(fpLog, &sTestInfo);
    
    return 0;
}

