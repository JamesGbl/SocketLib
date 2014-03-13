#include "Thread.h"
#include "Logger.h"


//////////////////////////////////////////////////////////////////////////
//Thread
DWORD __stdcall Thread::ThreadFunc(void *arg)
{
	static volatile int s_RunningThreadNumberCounter = 0;
	s_RunningThreadNumberCounter ++;

	Thread *pThead = (Thread*)arg;
	//������Ϣ
	Logger::getInstace()->debug("Thread %s is start.\r\nCurrent Thread number[%d]", pThead->m_strThreadName.c_str(), s_RunningThreadNumberCounter);
	//�߳̿�ʼ�����������б�־
	pThead->m_ThreadMutex.lock();
	pThead->m_bActive = true;
	pThead->m_bComplete = false;
	pThead->m_ThreadMutex.unlock(); 

	//�߳����ص�
	pThead->run();

	//�߳̽����������б�־
	pThead->m_ThreadMutex.lock();
	pThead->m_bActive = false;
	pThead->m_bComplete = true;
	pThead->m_ThreadMutex.unlock();

	s_RunningThreadNumberCounter --;
	Logger::getInstace()->debug("Thread[%s] is End.\r\nCurrent Thread Number[%d]", pThead->m_strThreadName.c_str(), s_RunningThreadNumberCounter);

	//��� ����ʱ���� Join �������߳̽���ʱ�Զ�������ɾ��
	if( !pThead->isJoinable() )
	{
		SAFE_DELETE(pThead);
	}
	else //�������Ϊ�� Join ���� �߳̽���ʱֻ�ǹر��߳̾�����̶߳����� join ���߳�ȥɾ��
	{
		::CloseHandle(pThead->m_hThreadHandle);
		pThead->m_hThreadHandle = NULL;
	}

	return 0;
}

void Thread::join()
{
	WaitForSingleObject(m_hThreadHandle, INFINITE);
}

bool Thread::start()
{
	if(m_bActive) //����Ѿ������ˣ�ֱ�ӷ���
		return false;

	DWORD dwThreadID;
	m_hThreadHandle = CreateThread(NULL,
		0, 
		(LPTHREAD_START_ROUTINE)ThreadFunc,
		(void*) this, 
		0, 
		&dwThreadID);

	if(NULL == m_hThreadHandle)
	{
		Logger::getInstace()->error("[%s] is Start failded", m_strThreadName.c_str());
		return false;
	}

	return true;
}