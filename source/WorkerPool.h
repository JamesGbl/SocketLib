/********************************************************************************
*	�ļ�����:	WorkerPool.h													*
*	����ʱ�䣺	2015/03/29														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ�Ĺ��������еķ����������ִ�ж�������������					*
*********************************************************************************/
#ifndef __2015_03_29_WORKER_POOL_H__
#define __2015_03_29_WORKER_POOL_H__

#include <cstdint>
#include "Task.h"
#include "Server.h"
#include "SingleInstance.h"

class WorkThreadPool;

class CWorkerPool : public SingleInstance<CWorkerPool>
{
public:
	bool  addTask(SERVER_HANDLE server_handle, CTask* pTask);
	void  addServer(CServer* pServer);
	void  start();
protected:
	friend class SingleInstance<CWorkerPool>;
	CWorkerPool();
	~CWorkerPool();
private:
	WorkThreadPool* m_pCore;
};
#endif//__2015_03_29_WORKER_POOL_H__