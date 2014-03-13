#ifndef __2014_03_02_CThread_H__
#define __2014_03_02_CThread_H__

#include "CObject.h"
#include "Mutex.h"
/*
*	�̷߳�װ
*/
class Thread : public NoCopyable
{
public:
	Thread(std::string strName, bool bJoinable = true)
		:m_strThreadName(strName)
		,m_bComplete(false)
		,m_bActive(false)
		,m_hThreadHandle(NULL)
		,m_bJoinable(bJoinable)
	{
		
	}
	virtual ~Thread()
	{
		if(NULL != m_hThreadHandle)
		{
			::CloseHandle(m_hThreadHandle);
		}
	}
	//�߳�˯�� ��λΪ��
	void	sleep(const long sec)	const	{ ::Sleep( sec*1000 );	}
	//�߳�˯�� ��λΪ����
	void	msleep(const long msec)	const	{ ::Sleep( msec );		}
	//�߳�˯�ߣ���λΪ΢��
	void	usleep(const long usec) const	{ ::Sleep( usec/1000 ); }
	//�ж��߳��Ƿ��Ѿ�����
	bool	isActive()				const	{ return m_bActive;		}
	//�ж��Ƿ�Ҫ��������߳�
	bool	isFinal()				const	{ return m_bComplete;	}
	//��������߳�
	void	final()							{ m_bComplete = true;	}
	//�߳��Ƿ��Join, join able ���߳��߳̽���ʱ������ɾ���̶߳��󣬷������߳̽���ʱ���Զ�delete�̶߳��� 
	bool	isJoinable()			const	{ return m_bJoinable;	}
	//�߳�����
	bool	start();
	//�ȼ��߳̽�����������̺߳ϲ�
	void	join();
	//�߳�ִ�����ݵĻص�����
	virtual bool run() = 0;
	//��ȡ�̵߳�����
	const std::string&	getThreadName()const{ return m_strThreadName; }
protected:
	static DWORD __stdcall ThreadFunc(void *arg);
protected:
	std::string			m_strThreadName; // �߳���
	Mutex				m_ThreadMutex;	 // �̻߳�����
	volatile bool		m_bComplete;	 // �߳̽�������	
	volatile bool		m_bActive;		 // �߳����б�־
	HANDLE				m_hThreadHandle; // �߳̾��
	// �߳��Ƿ���Ҫ�ϲ���joinable ���߳����߳̽�����ʱ��Ὣ����delete������������߳̽�����֮��û��delete����Ȼ���Լ���ʹ��.
	bool				m_bJoinable;
};

#endif//__2014_03_02_CThread_H__
