#include "Task.h"
#include "config.h"

CTask* CTask::create(TaskType type, SESSION_ID session_id, SERVER_HANDLE source, CData *data)
{
	CTask *pRet = new CTask;
	pRet->m_type = type;
	pRet->m_session = session_id;
	pRet->m_source = source;
	pRet->m_data = data;
	
	if (data != nullptr)
		data->retain();

	return pRet;
}

void CTask::free(CTask const*& pTask)
{
	SAFE_DELETE(pTask);
}

CTask::CTask(const CTask& rV)
{
	this->m_data = rV.m_data;
	this->m_session = rV.m_session;
	this->m_source = rV.m_source;
	this->m_type = rV.m_type;

	if (rV.m_data != nullptr)
		rV.m_data->retain();
}

CTask& CTask::operator=(const CTask& rV)
{
	this->m_data = rV.m_data;
	this->m_session = rV.m_session;
	this->m_source = rV.m_source;
	this->m_type = rV.m_type;

	if (rV.m_data != nullptr)
		rV.m_data->retain();

	return *this;
}

CTask::CTask()
{
	m_type = EMPTY;
	m_source = 0;
	m_session = 0;
}

CTask::~CTask()
{
	if (m_data != nullptr)
		m_data->release();
}