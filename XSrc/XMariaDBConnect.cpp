#include "XMariaDBConnect.h"


XMariaDBConnect::XMariaDBConnect()
	:
	_pConnect(nullptr)
{
	Init();
}

XMariaDBConnect::~XMariaDBConnect()
{
	Done();
}

void XMariaDBConnect::Ping()
{
	mysql_ping(_pConnect); //启用断线重连。
}

int XMariaDBConnect::SearchStudentByUserName(const char* pUserName)
{
	char query[1024] = {};
	sprintf(query, "select Name from Students where UserName = '%s'", pUserName);
	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	MYSQL_RES* ret = mysql_store_result(_pConnect);
	if (ret == NULL)
	{
		show_error(_pConnect);
		return -2;
	}

	my_ulonglong num = mysql_num_rows(ret);

	mysql_free_result(ret);

	return (int)num;
}

int XMariaDBConnect::SearchStudentBySchoolAndStudentID(const char* pSchool, const char* pStudentID)
{
	char query[1024] = {};
	sprintf(query, "select Name from Students where School = '%s' and StudentID = '%s'", pSchool, pStudentID);
	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	MYSQL_RES* ret = mysql_store_result(_pConnect);
	if (ret == NULL)
	{
		show_error(_pConnect);
		return -2;
	}

	my_ulonglong num = mysql_num_rows(ret);

	mysql_free_result(ret);

	return (int)num;
}

int XMariaDBConnect::UpdateStudentBySchoolAndStudentID(
	const char* pDevicel, const char* pSchool,
	const char* pMajor, const char* pStudentID,
	const char* pName, const char* pPhoneNumber,
	const char* pUserName, const char* pPassword,
	const char* pDeviceName, const char* pDeviceType)
{
	char query[1024] = {};
	sprintf(query, "update Students \
					set Device = '%s', Major = '%s', Name = '%s', PhoneNumber = '%s', UserName = '%s', Password = '%s', DeviceName = '%s', DeviceType = '%s' \
					where School = '%s' and StudentID = '%s'",
		pDevicel, pMajor, pName, pPhoneNumber, pUserName, pPassword, pDeviceName, pDeviceType, pSchool, pStudentID);

	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	my_ulonglong num = mysql_affected_rows(_pConnect);

	return (int)num;
}

int XMariaDBConnect::SearchStudentByUserNameAndPassword(const char* pUserName, const char* pPassword)
{
	char query[1024] = {};
	sprintf(query, "select Name from Students where UserName = '%s' and Password = '%s'", pUserName, pPassword);
	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	MYSQL_RES* ret = mysql_store_result(_pConnect);
	if (ret == NULL)
	{
		show_error(_pConnect);
		return -2;
	}

	my_ulonglong num = mysql_num_rows(ret);

	mysql_free_result(ret);

	return (int)num;
}

int XMariaDBConnect::SearchMsgIDByUserName(const char* pUserName, int& msgID)
{
	char query[1024] = {};
	sprintf(query, "select MsgID from Students where UserName = '%s'", pUserName);
	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	MYSQL_RES* ret = mysql_store_result(_pConnect);
	if (ret == NULL)
	{
		show_error(_pConnect);
		return -2;
	}

	my_ulonglong num = mysql_num_rows(ret);
	if (num != 1)
	{
		return (int)num;
	}

	MYSQL_ROW row = mysql_fetch_row(ret);
	msgID = atoi(row[0]);

	mysql_free_result(ret);

	return (int)num;
}

int XMariaDBConnect::SearchMsg(int& id, char* pMsg)
{
	char query[1024] = {};
	sprintf(query, "select ID, Msg from Msgs order by ID desc limit 1");
	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	MYSQL_RES* ret = mysql_store_result(_pConnect);
	if (ret == NULL)
	{
		show_error(_pConnect);
		return -2;
	}

	my_ulonglong num = mysql_num_rows(ret);
	if (num != 1)
	{
		return (int)num;
	}

	MYSQL_ROW row = mysql_fetch_row(ret);
	id = atoi(row[0]);
	strcpy(pMsg, row[1]);

	mysql_free_result(ret);

	return (int)num;
}

int XMariaDBConnect::UpdateMsgIDByUserName(const char* pUserName, int msgID)
{
	char query[1024] = {};
	sprintf(query, "update Students set MsgID = %d	where UserName = '%s'", msgID, pUserName);

	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	my_ulonglong num = mysql_affected_rows(_pConnect);

	return (int)num;
}

int XMariaDBConnect::SearchInfoByUserName(const char* pUserName, char* pSchool, char* pMajor, char* pStudentID, char* pName, char* pPhoneNumber)
{
	char query[1024] = {};
	sprintf(query, "select School, Major, StudentID, Name, PhoneNumber from Students where UserName = '%s'", pUserName);
	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	MYSQL_RES* ret = mysql_store_result(_pConnect);
	if (ret == NULL)
	{
		show_error(_pConnect);
		return -2;
	}

	my_ulonglong num = mysql_num_rows(ret);
	if (num != 1)
	{
		return (int)num;
	}

	MYSQL_ROW row = mysql_fetch_row(ret);
	strcpy(pSchool, row[0]);
	strcpy(pMajor, row[1]);
	strcpy(pStudentID, row[2]);
	strcpy(pName, row[3]);
	strcpy(pPhoneNumber, row[4]);

	mysql_free_result(ret);

	return (int)num;
}

int XMariaDBConnect::InsertFeedbackByUserName(const char* pUserName, const char* pContent)
{
	char query[1024] = {};
	sprintf(query, "insert into Feedbacks values(null, '%s', '%s')", pUserName, pContent);

	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	my_ulonglong num = mysql_affected_rows(_pConnect);

	return (int)num;
}

int XMariaDBConnect::UpdatePassword(const char* pUserName, const char* pPassword, const char* pNewPassword)
{
	char query[1024] = {};
	sprintf(query, "update Students set Password = '%s'	where UserName = '%s' and Password = '%s'", pNewPassword, pUserName, pPassword);

	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	my_ulonglong num = mysql_affected_rows(_pConnect);

	return (int)num;
}

int XMariaDBConnect::UpdatePhoneNumber(const char* pUserName, const char* pPassword, const char* pNewPhoneNumber)
{
	char query[1024] = {};
	sprintf(query, "update Students set PhoneNUmber = '%s'	where UserName = '%s' and Password = '%s'", pNewPhoneNumber, pUserName, pPassword);

	if (mysql_real_query(_pConnect, query, (unsigned long)strlen(query)))
	{
		show_error(_pConnect);
		return -1;
	}

	my_ulonglong num = mysql_affected_rows(_pConnect);

	return (int)num;
}

void XMariaDBConnect::show_error(MYSQL *mysql)
{
	XError("(%d) [%s] \"%s\"",
		mysql_errno(mysql),
		mysql_sqlstate(mysql),
		mysql_error(mysql));
	//mysql_close(mysql);
	//exit(-1);
}

void XMariaDBConnect::Init()
{
	_pConnect = mysql_init(nullptr);
	char value = 1;
	mysql_options(_pConnect, MYSQL_OPT_RECONNECT, &value);		//启用断线重连。
	if (!mysql_real_connect(_pConnect, "127.0.0.1", "aoyi", "yang", "aoyi", 0, "/var/run/mysqld/mysqld.sock", 0))
		show_error(_pConnect);
}

void XMariaDBConnect::Done()
{
	mysql_close(_pConnect);
	_pConnect = nullptr;
}