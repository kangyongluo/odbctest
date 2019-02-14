#include <stdio.h>
#include "util.h"

void LogAllErrors(SQLHANDLE henv,
                  SQLHANDLE hdbc,
                  SQLHANDLE hstmt)
{             
    TCHAR	buf[MAX_STRING_SIZE];
    TCHAR	State[STATE_SIZE];
	SDWORD	NativeError;
    RETCODE returncode;
	BOOL MsgDisplayed;
    int errorCount = 0;

	MsgDisplayed = 0;

   /* Log any henv error messages */
   returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
      _tprintf(_T("   State: %s\n   Native Error: %ld\n   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)henv, (SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* Log any hdbc error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        _tprintf(_T("   State: %s\n   Native Error: %ld\n   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)hdbc, (SQLHANDLE)NULL, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* Log any hstmt error messages */
   returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   while((returncode == SQL_SUCCESS) ||
         (returncode == SQL_SUCCESS_WITH_INFO)){
		State[STATE_SIZE-1]=NULL_STRING;
        if((MsgDisplayed++) > 10){
            MsgDisplayed = 0;
            break;
        }
        _tprintf(_T("   State: %s\n   Native Error: %ld\n   Error: %s\n"),State,NativeError,buf);
      returncode = SQLError((SQLHANDLE)NULL, (SQLHANDLE)NULL, (SQLHANDLE)hstmt, (SQLTCHAR*)State, &NativeError, (SQLTCHAR*)buf, MAX_STRING_SIZE, NULL);
   }

   /* if no error messages were displayed then display a message */
   if(!MsgDisplayed)
		_tprintf(_T("There were no error messages for SQLError() to display\n"));
}  
TrueFalse FullConnectWithOptions(TestInfo *pTestInfo, int Options)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;

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

	returncode = SQLConnect(hdbc,
                           (SQLTCHAR*)pTestInfo->DataSource,(SWORD)_tcslen(pTestInfo->DataSource),
                           (SQLTCHAR*)pTestInfo->UserID,(SWORD)_tcslen(pTestInfo->UserID),
                           (SQLTCHAR*)pTestInfo->Password,(SWORD)_tcslen(pTestInfo->Password)
                           );

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
TrueFalse FullConnectWithOptionsEx(TestInfo *pTestInfo, int Options, TCHAR *connStrEx)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR OutString[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR connectStr[1024] = {0x0};
    
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
    _stprintf(connectStr, _T("DSN=%s;UID=%s;PWD=%s;%s"), pTestInfo->DataSource, pTestInfo->UserID, pTestInfo->Password, connStrEx);
    //_stprintf(connectStr, _T("DSN=%s"), pTestInfo->DataSource);
    LogMsg(NONE, _T("odbc url:%s\n"), connectStr);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)connectStr, SQL_NTS, (SQLTCHAR*)OutString, sizeof(OutString), &olen, SQL_DRIVER_NOPROMPT);
	if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
	{
	    LogAllErrors(henv, hdbc, NULL);
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

void disconnect(SQLHANDLE henv, SQLHANDLE hdbc, SQLHANDLE hstmt)
{
    if(hstmt != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
    if(hdbc != SQL_NULL_HANDLE)
    {
        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
    }
    if(henv != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_ENV,henv);
}
int FullDisconnect(TestInfo *pTestInfo)
{
  RETCODE returncode;                        
  TrueFalse Result;
  
  Result=TRUE;
   
  /* Disconnect from the data source.  If errors, still go on and */
  /* try to free the handles */
  returncode = SQLDisconnect((SQLHANDLE)pTestInfo->hdbc);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
        LogAllErrors(pTestInfo->henv, pTestInfo->hdbc, pTestInfo->hstmt);
	}
   
  /* Free up all handles used by this connection */
  returncode = SQLFreeConnect((SQLHANDLE)pTestInfo->hdbc);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
		LogAllErrors(pTestInfo->henv,pTestInfo->hdbc,pTestInfo->hstmt);
	}

   
  returncode = SQLFreeEnv((SQLHANDLE)pTestInfo->henv);
  if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
		Result=FALSE;	
		LogAllErrors(pTestInfo->henv,pTestInfo->hdbc,pTestInfo->hstmt);
	}
   
  return(Result);
}  /* FullDisconnectEx() */
int addInfoSession(SQLHSTMT hstmt)
{
    SQLRETURN returnCode;
    char buf[256];
    SQLLEN len;
    int i;
    
    returnCode = SQLExecDirect(hstmt, (SQLTCHAR*)"info session", SQL_NTS);
    if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, "Execute \"info session\" fail\n");
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
                returnCode = SQLGetData(hstmt, i, SQL_C_CHAR, (SQLPOINTER)buf, sizeof(buf), &len);
                if (SQL_SUCCEEDED(returnCode)){
                    if(strcmp(InfoSession[sessionId], "END") != 0){
                        if(strcmp(InfoSession[sessionId], "SESSION_ID") == 0 ||
                            strcmp(InfoSession[sessionId], "SERVER_HOST") == 0 ||
                            strcmp(InfoSession[sessionId], "SERVER_PORT") == 0){
                            LogMsg(NONE, "%s:%s\n", InfoSession[sessionId], buf);
                        }
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


