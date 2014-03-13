#ifndef __2014_03_02_CMUTEX_H__
#define __2014_03_02_CMUTEX_H__

#include "CObject.h"

/*
*	�߳���Դ������
*/
class Mutex : public NoCopyable
{
public:
	Mutex()
	{
		InitializeCriticalSection( &m_csLock );
	}
	~Mutex()
	{
		DeleteCriticalSection( &m_csLock );
	}
	inline void	lock()
	{
		EnterCriticalSection( &m_csLock );
	}
	inline void	unlock()
	{
		LeaveCriticalSection( &m_csLock );
	}
private:
	CRITICAL_SECTION m_csLock;
};

/*
*	�Զ��������þֲ��������������ڣ��Զ����ý�������
*/
class AutoLock : public NoCopyable
{
public:
	AutoLock(Mutex* pMutex)
		:m_pMutex(pMutex),
		m_bLocked(false)
	{
		this->lock();
	}

	~AutoLock()
	{
		this->unlock();
	}

	inline void lock()
	{
		if(m_pMutex && !m_bLocked)
		{
			m_pMutex->lock();
			m_bLocked  = true;
		}
	}

	inline void unlock()
	{
		if(m_pMutex && m_bLocked)
		{
			m_pMutex->unlock();
			m_bLocked = false;
		}
	}
private:
	Mutex*	m_pMutex;
	bool	m_bLocked;
};
#endif//__2014_03_02_CMUTEX_H__