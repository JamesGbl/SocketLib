/********************************************************************************
*	�ļ�����:	Thread.h														*
*	����ʱ�䣺	2015/03/29														*
*	��   �� :	xzben															*
*	�ļ�����:	�̹߳���ʵ�ֵĻ��࣬�̳�CThread���඼��ӵ��һ���߳�					*
*********************************************************************************/
#ifndef __2015_03_29_THREAD_H__
#define __2015_03_29_THREAD_H__

#include "config.h"
#include <thread>

class CThread
{
public:
	CThread();
	~CThread();
	static void sleep(uint32_t milsecond);

	virtual bool start();
	virtual void run() = 0;
	bool isFinish(){ return m_bIsFinish; }
	bool isStart(){ return m_bIsStart; }
private:
	friend void		ThreadFunc(void* param);
	//std::thread		*m_pCore;
	bool			m_bIsFinish;
	bool			m_bIsStart;

};
#endif//__2015_03_29_THREAD_H__
