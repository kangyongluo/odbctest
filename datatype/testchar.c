/*
*����Ŀ��:���Զ�̬�󶨲���
*				�󶨵�C��������ΪSQL_C_WCHAR��ODBC��������ΪSQL_CHAR
*
*���Խ��:
*			esgynDB:
*					��SQL_C_WCHAR��������Ϊ0x31 ,0x00, 0x32, 0x00, 0x30, 0x00, 0x38, 0x00,ֻ�ܳɹ������һ���ֽ�
*			MySQL:�ɹ�ִ��;
*/
int SQLBindParameterSQLCWcharToChar(SQLHANDLE hstmt)
{
	SQLLEN len;
	RETCODE retcode;
	unsigned char tmpw[10] = {0x31 ,0x00, 0x32, 0x00, 0x30, 0x00, 0x38, 0x00};
	
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"drop table datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n");
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table datatype(c CHAR(30))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		return -1;
	}
	len = SQL_NTS;
	retcode = SQLBindParameter(hstmt,
					(SWORD)1,
					SQL_PARAM_INPUT,
					SQL_C_WCHAR,
					SQL_CHAR,
					8,
					0,
					tmpw,
					4,
					&len);
	
	
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter  fail:\n\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"insert into datatype values(?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n\n");
		return -1;
	}
	return 0;
}
/*
*����Ŀ��:���԰󶨲�����ȡSQL
*				�󶨵�C��������ΪSQL_C_WCHAR��ODBC��������ΪSQL_CHAR
*
*���Խ��:
*			��SQL����Ϊ:
*						C
*						------------------------------
*						123

*			esgynDB:��ȡ����Ϊchar����,0x31 0x32 0x33
*			MySQL:��ȡ����Ϊwchar_t����,0x31 0x00 0x32 0x00 0x33 0x00
*/

int SQLBindColSQLWcharToChar(SQLHANDLE hstmt)
{
	SQLLEN len;
	RETCODE retcode;
	unsigned char buf[100] = {0};


	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"drop table datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n");
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table datatype(c CHAR(20))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"insert into datatype values('123')", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"select * from datatype",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select  fail:\n\n");
		return -1;
	}
	len  = 0;
	retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, buf, sizeof(buf), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("bind col fail:\n\n");
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if (retcode == SQL_NO_DATA){
		printf("sqlfetch  fail:\n\n");
		return -1;
	}
	int i;
	for(i = 0; i < len; i++){
		printf("0x%x ", buf[i]);
	}
	printf("\n");

	return 0;
}
/*
*����Ŀ��:���԰󶨲�����ȡSQL
*				�󶨵�C��������ΪSQL_C_UBIGINT��ODBC��������ΪSQL_CHAR
*
*���Խ��:

*			esgynDB:�󶨺�ִ��SQLExecDirect(hstmt, "insert into table values(?)", SQL_NTS)ʧ��
*			MySQL:�ɹ�ִ��;
*/

int SQLBindParameterSQLCUbigintToSQLChar(SQLHANDLE hstmt)
{
	SQLLEN len;
	RETCODE retcode;
	unsigned long long data = 85;
	
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"drop table datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n");
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table datatype(c CHAR(30))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		return -1;
	}
	len = SQL_NTS;
	retcode = SQLBindParameter(hstmt,
					(SWORD)1,
					SQL_PARAM_INPUT,
					SQL_C_UBIGINT,
					SQL_CHAR,
					30,
					0,
					&data,
					sizeof(data),
					&len);
	
	
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter  fail:\n\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"insert into datatype values(?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n\n");
		return -1;
	}
	return 0;
}
/*
*����Ŀ��:���԰󶨲�����ȡSQL
*				�󶨵�C��������ΪSQL_C_UBIGINT��ODBC��������ΪSQL_CHAR
*
*���Խ��:
*			��SQL����Ϊ:
*						C
*						------------------------------
*						123

*			esgynDB:��������һֱ��0
*			MySQL:������������
*/

int SQLBindColSQLUbigintToChar(SQLHANDLE hstmt)
{
	SQLLEN len;
	RETCODE retcode;
	unsigned long long data = 0;


	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"drop table datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n");
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table datatype(c CHAR(20))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"insert into datatype values('123')", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"select * from datatype",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select  fail:\n\n");
		return -1;
	}
	len  = 0;
	retcode = SQLBindCol(hstmt, 1, SQL_C_UBIGINT, &data, sizeof(data), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("bind col fail:\n\n");
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if (retcode == SQL_NO_DATA){
		printf("sqlfetch  fail:\n\n");
		return -1;
	}
	printf("%d \n", data);

	return 0;
}
/*
*����Ŀ��:���Զ�̬�󶨲���
*				�󶨵�C��������ΪSQL_C_NUMERIC��ODBC��������ΪSQL_CHAR
*
*���Խ��:
*			esgynDB:SQL_NUMERIC_STRUCT�ṹ����ִ�С������:��
*			SQL_NUMERIC_STRUCT numeric;
*			//��������ʱ��ֵ:
*			memset(&numeric, 0, sizeof(numeric));
*			numeric.precision = 3;
*			numeric.scale = 0;
*			numeric.sign = 1;
*			numeric.val[0] = 0xa2;
*			//��ȡ���ݺ�:
*			numeric.precision = 3;
*			numeric.scale = 0;
*			numeric.sign = 1;
*			numeric.val[15] = 0xa2;
*
*			
*			MySQL:�޴�����;
*/

int SQLBindParameterSQLNumericToSQLChar(SQLHANDLE hstmt)
{
	SQLLEN len;
	RETCODE retcode;
	SQL_NUMERIC_STRUCT numeric, tmpnumeric;
	int i;
	
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"drop table datatype", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("drop table fail:\n");
	}
	retcode = SQLExecDirect(hstmt, (SQLCHAR*)"create table datatype(c CHAR(30))", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("create table fail:\n");
		return -1;
	}
	len = SQL_NTS;
	memset(&numeric, 0, sizeof(numeric));
	numeric.precision = 3;
	numeric.scale = 0;
	numeric.sign = 1;
	numeric.val[0] = 0xa2;
	printf("precision = %d scale = %d sign = %d\n", numeric.precision, numeric.scale, numeric.sign);
	for(i = 0; i < 16; i++){
		printf("0x%x ", numeric.val[i]);
	}
	printf("\n");
	retcode = SQLBindParameter(hstmt,
					(SWORD)1,
					SQL_PARAM_INPUT,
					SQL_C_NUMERIC,
					SQL_CHAR,
					2,
					0,
					&numeric,
					sizeof(numeric),
					&len);
	
	
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("SQLBindParameter  fail:\n\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt, (SQLTCHAR*)"insert into datatype values(?)", SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("insert table fail:\n\n");
		return -1;
	}
	retcode = SQLExecDirect(hstmt,(SQLCHAR*)"select * from datatype",SQL_NTS);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("select  fail:\n\n");
		return -1;
	}
	memset(&tmpnumeric, 0, sizeof(tmpnumeric));
	len  = 0;
	retcode = SQLBindCol(hstmt, 1, SQL_C_NUMERIC, &tmpnumeric, sizeof(tmpnumeric), &len);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		printf("bind col fail:\n\n");
		return -1;
	}
	retcode = SQLFetch(hstmt);
	if (retcode == SQL_NO_DATA){
		printf("sqlfetch  fail:\n\n");
		return -1;
	}
	printf("precision = %d scale = %d sign = %d\n", tmpnumeric.precision, tmpnumeric.scale, tmpnumeric.sign);
	for(i = 0; i < 16; i++){
		printf("0x%x ", tmpnumeric.val[i]);
	}
	printf("\n");
	return 0;
}

