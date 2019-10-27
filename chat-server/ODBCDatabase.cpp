#include "LibODBCHeader.h"

ODBCDatabase::ODBCDatabase (DB_TYPE eType)
{
	mType = eType;
}

ODBCDatabase::~ODBCDatabase (void)
{
	mCS.Lock ();
	for (std::list<ODBC*>::iterator iter = mPool.begin (); iter != mPool.end (); ++iter)
	{
		ODBC* pODBC = *iter;
		delete pODBC;
	}
	mPool.clear ();
	mCS.Unlock ();
}

bool ODBCDatabase::Initialize ()
{
	// virtual �Լ��� ���ؼ� �����ϴ� DB ���� ��������
	string strDBType = GetConfigDBType ();
	if (strDBType == "")
	{
		return false;
	}

	string strDBDriver = GetConfigDBDriver ();
	if (strDBDriver == "")
	{
		return false;
	}

	string strDBIP = GetConfigDBIP ();
	if (strDBIP == "")
	{
		return false;
	}

	string strDBName = GetConfigDBName ();
	if (strDBName == "")
	{
		return false;
	}

	string strDBID = GetConfigDBID ();
	if (strDBID == "")
	{
		return false;
	}

	string strDBPWD = GetConfigDBPwd ();
	if (strDBPWD == "")
	{
		return false;
	}

	int nDBPort = GetConfigDBPort ();
	if (nDBPort <= 0)
	{
		return false;
	}

	int nDBCnt = GetConfigDBConnCnt ();
	if (nDBCnt <= 0)
	{
		return false;
	}

	// ������ŭ �������鼭 Connection ����
	for (int i = 0; i < nDBCnt; ++i)
	{
		ODBC* pODBC = new ODBC ();
		int dwRet = pODBC->Connect (strDBType.c_str (), strDBDriver.c_str (), strDBIP.c_str (), nDBPort, "DummyDSN", strDBName.c_str (), strDBID.c_str (), strDBPWD.c_str ());
		if (dwRet != SQL_SUCCESS)
		{
			_Logf (GLog::LL_ERROR, "[ODBC] ODBC Connect return Error(%d).", dwRet);
		}

		mPool.push_back (pODBC);
	}

	return true;
}

ODBC* ODBCDatabase::GetConnection ()
{
	static int nMaxTerm = 0, nMaxTotal = 0;
	static time_t tmLog = 0;
	int nUsing = 0;
	ODBC* pODBCCon = NULL;

	mCS.Lock ();
	for (std::list<ODBC*>::iterator iter = mPool.begin (); iter != mPool.end (); ++iter)
	{
		pODBCCon = *iter;

		// ������� db connection ���� üũ
		if (pODBCCon->IsOnUsing ())
		{
			pODBCCon = NULL;
			++nUsing;
		}
		// ������� �ƴ϶��
		else
		{
			if (nMaxTerm < nUsing + 1)
			{
				nMaxTerm = nUsing + 1;
			}
			if (nMaxTotal < nUsing + 1)
			{
				nMaxTotal = nUsing + 1;
			}
			if (tmLog + LOG_TERM <= time (NULL))
			{
				tmLog = time (NULL);
				// TODO: occurred error!
				nMaxTerm = 0;
			}
			// �����ϱ����� ��������� ����
			pODBCCon->SetOnUsing (true);
			break;
		}
	}
	mCS.Unlock ();

	return pODBCCon;
}

void ODBCDatabase::CheckConnection ()
{
	ODBC *pODBCCon = NULL;

	mCS.Lock ();
	for (list<ODBC*>::iterator iter = mPool.begin (); iter != mPool.end (); ++iter)
	{
		pODBCCon = *iter;

		// ������� db connection ���� üũ
		if (pODBCCon->IsOnUsing ())
		{
			pODBCCon = NULL;
		}
		// ������� �ƴ϶��
		else
		{
			// ���� ����
			pODBCCon->SetOnUsing (true);
			int retSQL = pODBCCon->Query (HEALTH_CHECK_QUERY_STRING.c_str ());
			if (retSQL == SQL_SUCCESS)
			{
				_Logf (GLog::LL_DEBUG, "[ODBC] DBType(%d), ODBCCon(%x) Do Query(%s)... Success", mType, pODBCCon, HEALTH_CHECK_QUERY_STRING.c_str ());
			}
			else
			{
				_Logf (GLog::LL_ERROR, "[ODBC] DBType(%d), ODBCCon(%x) Do Query(%s) retSQL(%d)... Fail", mType, pODBCCon, HEALTH_CHECK_QUERY_STRING.c_str (), retSQL);
			}
			pODBCCon->Commit ();
			pODBCCon->SetOnUsing (false);
		}
	}
	mCS.Unlock ();
}

//-- [ORACLE FADE-OUT]
bool ODBCDatabase::Initialize (bool bAutoCommit)
{
	// virtual �Լ��� ���ؼ� �����ϴ� DB ���� ��������
	string strDBType = GetConfigDBType ();
	if (strDBType == "")
	{
		return false;
	}

	string strDBDriver = GetConfigDBDriver ();
	if (strDBDriver == "")
	{
		return false;
	}

	string strDBIP = GetConfigDBIP ();
	if (strDBIP == "")
	{
		return false;
	}

	string strDBName = GetConfigDBName ();
	if (strDBName == "")
	{
		return false;
	}

	string strDBID = GetConfigDBID ();
	if (strDBID == "")
	{
		return false;
	}

	string strDBPWD = GetConfigDBPwd ();
	if (strDBPWD == "")
	{
		return false;
	}

	int nDBPort = GetConfigDBPort ();
	if (nDBPort <= 0)
	{
		return false;
	}

	int nDBCnt = GetConfigDBConnCnt ();
	if (nDBCnt <= 0)
	{
		return false;
	}

	// ������ŭ �������鼭 Connection ����
	for (int i = 0; i < nDBCnt; ++i)
	{
		ODBC* pODBC = new ODBC ();
		int dwRet = pODBC->Connect (strDBType.c_str (), strDBDriver.c_str (), strDBIP.c_str (), nDBPort, "DummyDSN", strDBName.c_str (), strDBID.c_str (), strDBPWD.c_str (), bAutoCommit);
		if (dwRet != SQL_SUCCESS)
		{
			_Logf (GLog::LL_ERROR, "[ODBC] ODBC Connect return Error(%d).", dwRet);
		}

		mPool.push_back (pODBC);
	}

	return true;
}