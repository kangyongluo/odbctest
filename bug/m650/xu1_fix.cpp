#include <stdio.h>
#include <iostream> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <assert.h>
#include <sql.h>
#include <sqlext.h>
#include <memory>
#include <random>
#include <ctime>
SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;
SQLHWND hWnd;
#define MAX_SQLSTRING_LEN 1000
#define STATE_SIZE 6
#define MAX_CONNECT_STRING 256
#define TRUE 1
#define FALSE 0
using namespace std;
typedef struct {
        SQLDOUBLE meter_id;
        SQL_DATE_STRUCT data_date;
        SQLDOUBLE day_ene_id;
        SQL_DATE_STRUCT metering_time;
        SQLDOUBLE V1;
        SQLDOUBLE V2;
        SQLDOUBLE V3;
        SQLDOUBLE V4;
        SQLDOUBLE V5;
        SQLLEN len_meter_id;
        SQLLEN len_data_date;
        SQLLEN len_day_ene_id;
        SQLLEN len_metering_time;
        SQLLEN len_V1;
        SQLLEN len_V2;
        SQLLEN len_V3;
        SQLLEN len_V4;
        SQLLEN len_V5;
} crm_mnp_npdb_day_energy;

const char *SqlRetText(int rc)
{
  static char buffer[80];
  switch (rc)
  {
    case SQL_SUCCESS:
      return("SQL_SUCCESS");
    case SQL_SUCCESS_WITH_INFO:
      return("SQL_SUCCESS_WITH_INFO");
    case SQL_NO_DATA:
      return("SQL_NO_DATA");
    case SQL_ERROR:
      return("SQL_ERROR");
    case SQL_INVALID_HANDLE:
      return("SQL_INVALID_HANDLE");
    case SQL_STILL_EXECUTING:
      return("SQL_STILL_EXECUTING");
    case SQL_NEED_DATA:
      return("SQL_NEED_DATA");
   }
   sprintf(buffer,"SQL Error %d",rc);
   return(buffer);
}
void CleanUp()
{
  printf("\nXuji ODBC insertion  Test FAILED!!!\n");
  if(hstmt != SQL_NULL_HANDLE)
    SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
  if(hdbc != SQL_NULL_HANDLE)
  {
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
  }
  if(henv != SQL_NULL_HANDLE)
  SQLFreeHandle(SQL_HANDLE_ENV,henv);
  exit(EXIT_FAILURE);
}
void LogDiagnostics(const char *sqlFunction, SQLRETURN rc, bool exitOnError=true)
{
  SQLRETURN diagRC = SQL_SUCCESS;
  SQLSMALLINT recordNumber;
  SQLINTEGER nativeError;
  SQLCHAR messageText[SQL_MAX_MESSAGE_LENGTH];
  SQLCHAR sqlState[6];
  int diagsPrinted = 0;
  bool printedErrorLogHeader = false;
  printf("Function %s returned %s\n", sqlFunction, SqlRetText(rc));
  /* Log any henv Diagnostics */
  recordNumber = 1;
  do
  {
    diagRC = SQLGetDiagRec( SQL_HANDLE_ENV
                          , henv
                          , recordNumber
                          , sqlState
                          , &nativeError
                          , messageText
                          , sizeof(messageText)
                          , NULL
                          );
    if(diagRC==SQL_SUCCESS)
    {
      if(!printedErrorLogHeader)
      {
        printf("Diagnostics associated with environment handle:\n");
        printedErrorLogHeader = true;
      }
      printf("\n\tSQL Diag %d\n\tNative Error: %ld\n\tSQL State: %s\n\tMessage:%s\n",
             recordNumber,nativeError,sqlState,messageText);
    }
    recordNumber++;
  } while (diagRC==SQL_SUCCESS);
  /* Log any hdbc Diagnostics */
  recordNumber = 1;
  printedErrorLogHeader = false;
  do
  {
    diagRC = SQLGetDiagRec( SQL_HANDLE_DBC
                          , hdbc
                          , recordNumber
                          , sqlState
                          , &nativeError
                          , messageText
                          , sizeof(messageText)
                          , NULL
                          );
    if(diagRC==SQL_SUCCESS)
    {
      if(!printedErrorLogHeader)
      {
        printf("Diagnostics associated with connection handle:\n");
        printedErrorLogHeader = true;
      }
      printf("\n\tSQL Diag %d\n\tNative Error: %ld\n\tSQL State: %s\n\tMessage:%s\n",
                    recordNumber,nativeError,sqlState,messageText);
    }
    recordNumber++;
  } while (diagRC==SQL_SUCCESS);
  /* Log any hstmt Diagnostics */
  recordNumber = 1;
  printedErrorLogHeader = false;
  do
  {
    diagRC = SQLGetDiagRec( SQL_HANDLE_STMT
                          , hstmt
                          , recordNumber
                          , sqlState
                          , &nativeError
                          , messageText
                          , sizeof(messageText)
                          , NULL
                          );
   if(diagRC==SQL_SUCCESS)
   {
      if(!printedErrorLogHeader)
      {
        printf("Diagnostics associated with statmement handle:\n");
        printedErrorLogHeader = true;
      }
      printf("\n\tSQL Diag %d\n\tNative Error: %ld\n\tSQL State: %s\n\tMessage:%s\n",
            recordNumber,nativeError,sqlState,messageText);
    }
    recordNumber++;
  } while (diagRC==SQL_SUCCESS);

  if(exitOnError && rc!=SQL_SUCCESS_WITH_INFO)
  CleanUp();
}
void rowsToInsert( crm_mnp_npdb_day_energy *poPointer, SQLINTEGER startVal,  int num){
  char dataDate[10], meteringTime[10];
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distributiony(2014,2016);
  std::uniform_int_distribution<int> distributionm(1,12);
  std::uniform_int_distribution<int> distributiond(1,28);
  std::uniform_real_distribution<double> distributionli(100000000000001,100000020800000 );
  std::uniform_real_distribution<double> distributionnumeric(1,99999 );
  int i;
  for( i =0; i<num; i++ ){
    poPointer[i].meter_id = distributionli(generator);

    poPointer[i].data_date.year = distributiony(generator);
    poPointer[i].data_date.month = distributionm(generator);
    poPointer[i].data_date.day = distributiond(generator);

    poPointer[i].day_ene_id = startVal ;

    poPointer[i].metering_time.year = distributiony(generator);
    poPointer[i].metering_time.month = distributionm(generator);
    poPointer[i].metering_time.day = distributiond(generator);

    poPointer[i].V1 = distributionnumeric(generator);
    poPointer[i].V2 = distributionnumeric(generator);
    poPointer[i].V3 = distributionnumeric(generator);
    poPointer[i].V4 = distributionnumeric(generator);
    poPointer[i].V5 = distributionnumeric(generator);
    startVal++;
  } 
}
// Main Program
 int main (int argc, char *argv[])
{
  SQLCHAR dsnName[20];
  SQLCHAR user[20];
  SQLCHAR password[20];
  SQLCHAR insertType[20];
  SQLCHAR insertTypeUpper[20];
  int rowsetSize;
  SQLINTEGER numOfRowsets;
  SQLDOUBLE beginVal;
 
  SQLRETURN returnCode;
  bool testPassed = true;
  SQLCHAR InConnStr[MAX_CONNECT_STRING];
  SQLCHAR OutConnStr[MAX_CONNECT_STRING];
  SQLSMALLINT ConnStrLength;
  int errflag = 0;
  //optarg = NULL;
  if (argc != 8)
    errflag++;
  if (!errflag )
  {
    strcpy ((char *)dsnName, argv[1]);
    strcpy ((char *)user, argv[2]);
    strcpy ((char *)password, argv[3]);
    char *endptr;
    beginVal = strtol(argv[4], &endptr, 10);
    if (!*argv[4] || *endptr){
      errflag++;
    }
    rowsetSize = strtol(argv[5], &endptr, 10);
    if (!*argv[5] || *endptr){
      errflag++;
    }
    numOfRowsets = strtol(argv[6], &endptr, 10);
    if (!*argv[6] || *endptr){
      errflag++;
    }
    strcpy ((char *)insertType, argv[7]);
    short i =0;
    for(i = 0; i < strlen((char *)insertType); ++i)
      insertTypeUpper[i] = std::toupper(insertType[i]);
    insertTypeUpper[i] = '\0';
    bool found = false;
    int size = strlen((char *)insertTypeUpper);
    if((size == strlen ("INSERT" )) && ( strcmp((char *)insertTypeUpper, "INSERT") ==0) && !found)
      found =true;
    if((size == strlen ("UPSERT" )) && ( strcmp((char *)insertTypeUpper, "UPSERT") ==0) && !found)
      found =true;
    if((size == strlen ("UPSERTUSINGLOAD" )) && ( strcmp((char *)insertTypeUpper, "UPSERTUSINGLOAD") ==0) && !found){
      found =true;
      strcpy((char *)insertTypeUpper, "UPSERT USING LOAD");
    }

    if(found == false)
      errflag++;
  }
  if (errflag)
  {
    printf("Command line error.\n");
    printf("Usage: %s <datasource> <userid> <password> <start val day_ene_id> <rowsetSize> <numOfRowsets> {INSERT | UPSERT | UPSERTUSINGLOAD}\n", argv[0] );
    return FALSE;
  }
  // get start time
  int start_time = clock();

  // Initialize handles to NULL
  henv = SQL_NULL_HANDLE;
  hstmt = SQL_NULL_HANDLE;
  hdbc = SQL_NULL_HANDLE;
  // Allocate Environment Handle
  returnCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
  if(returnCode != SQL_SUCCESS)
    LogDiagnostics("SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv)",returnCode);

  // Set ODBC version to 3.0
  returnCode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
  if(returnCode != SQL_SUCCESS)
    LogDiagnostics("SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0)",returnCode,false);

  // Allocate Connection handles
  returnCode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
  if(returnCode != SQL_SUCCESS)
    LogDiagnostics("SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc)", returnCode);

  //Connect to the database
  sprintf((char*)InConnStr,"DSN=%s;UID=%s;PWD=%s;%c",(char*)dsnName, (char*)user, (char*)password, '\0');
  printf("Using Connect String: %s\n", InConnStr);
  returnCode = SQLDriverConnect(hdbc,hWnd,InConnStr,sizeof(InConnStr),OutConnStr,sizeof(OutConnStr),&ConnStrLength,SQL_DRIVER_NOPROMPT);
  if (!SQL_SUCCEEDED(returnCode))
    LogDiagnostics("SQLDriverConnect",returnCode);
  printf("Successfully connected using SQLDriverConnect.\n");
  //Allocate Statement handles
  returnCode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if (!SQL_SUCCEEDED(returnCode))
    LogDiagnostics("SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt)", returnCode);
    SQLCHAR sqlStmt[1000]; 
    strcpy((char *)sqlStmt , (char *)insertTypeUpper);
    //strcat((char *)sqlStmt,  " INTO TRAFODION.XUJI.E_MP_DAY_ENERGY_P (meter_id, data_date, day_ene_id,metering_time, PAP_E , PAP_E1, PAP_E2, PAP_E3, PAP_E4) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) ") ;
    strcat((char *)sqlStmt,  " INTO E_MP_DAY_ENERGY_P (meter_id, data_date, day_ene_id,metering_time, PAP_E , PAP_E1, PAP_E2, PAP_E3, PAP_E4) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) ") ;
    printf("%s\n", (char *) sqlStmt);
    returnCode = SQLPrepare(hstmt, sqlStmt , SQL_NTS);
    if (!SQL_SUCCEEDED(returnCode))
      LogDiagnostics("SQLPrepare", returnCode);

    crm_mnp_npdb_day_energy * po = new crm_mnp_npdb_day_energy[rowsetSize];
    SQLUSMALLINT * param_status =  new SQLUSMALLINT[rowsetSize];
    SQLINTEGER params_processed = 0;

    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER) sizeof(crm_mnp_npdb_day_energy), 0);
    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)rowsetSize , 0);
    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR, param_status, 0);
    SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR, &params_processed, 0);

    returnCode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
       po[0].len_meter_id, 0, &po[0].meter_id, sizeof(po[0].meter_id), &po[0].len_meter_id);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 1", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_DATE, 
       po[0].len_data_date, 0, &po[0].data_date, sizeof(po[0].data_date), &po[0].len_data_date);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 2", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
         po[0].len_day_ene_id, 0, &po[0].day_ene_id, sizeof(po[0].day_ene_id), &po[0].len_day_ene_id);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 3", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_DATE, 
         po[0].len_metering_time, 0, &po[0].metering_time, sizeof(po[0].metering_time), &po[0].len_metering_time);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 4", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 5, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
         po[0].len_V1, 0, &po[0].V1, sizeof(po[0].V1), &po[0].len_V1);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 5", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 6, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
         po[0].len_V2, 0, &po[0].V2, sizeof(po[0].V2), &po[0].len_V2);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 6", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 7, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
         po[0].len_V3, 0, &po[0].V3, sizeof(po[0].V3), &po[0].len_V3);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 7", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 8, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
         po[0].len_V4, 0, &po[0].V4, sizeof(po[0].V4), &po[0].len_V4);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 8", returnCode);
    }

    returnCode = SQLBindParameter(hstmt, 9, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 
         po[0].len_V5, 0, &po[0].V5, sizeof(po[0].V5), &po[0].len_V5);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLBindParameter 9", returnCode);
    }

  for(int i_numOfRowsets = 0; i_numOfRowsets<numOfRowsets; i_numOfRowsets++){
    rowsToInsert( po, beginVal, rowsetSize);
  
    returnCode = SQLExecute(hstmt);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLExecute(hstmt)", returnCode);
    }
 
    returnCode = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_COMMIT);
    if (!SQL_SUCCEEDED(returnCode)){
      delete[] po ;
      delete[] param_status;
      LogDiagnostics("SQLExecute(hstmt)", returnCode);
    }

    printf(" Done: %d out of %d  times inserted %d rows: Begin Val :  %f .\n", (i_numOfRowsets +1), numOfRowsets, rowsetSize , beginVal);
    beginVal = beginVal + rowsetSize;
  }
  delete[] po ;
  delete[] param_status;
  //Free Statement handle
  returnCode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
  if (!SQL_SUCCEEDED(returnCode))
    LogDiagnostics("SQLFreeHandle(SQL_HANDLE_STMT, hstmt)", returnCode);
  hstmt = SQL_NULL_HANDLE;
  //Disconnect
  returnCode = SQLDisconnect(hdbc);
  if (!SQL_SUCCEEDED(returnCode))
    LogDiagnostics("SQLDisconnect(hdbc)", returnCode);
  //Free Connection handle
  returnCode = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
  if (!SQL_SUCCEEDED(returnCode))
    LogDiagnostics("SQLFreeHandle(SQL_HANDLE_DBC, hdbc)", returnCode);
  hdbc = SQL_NULL_HANDLE;
  //Free Environment handle
  returnCode = SQLFreeHandle(SQL_HANDLE_ENV, henv);
  
  if (!SQL_SUCCEEDED(returnCode))
  LogDiagnostics("SQLFreeHandle(SQL_HANDLE_ENV, henv)", returnCode);
  henv = SQL_NULL_HANDLE;
  int end_time = clock();
  std::cout << "Time taken : " <<  ((end_time-start_time)/double(CLOCKS_PER_SEC)*1000 ) << " milliseconds" << endl;
  exit(EXIT_SUCCESS);
}
