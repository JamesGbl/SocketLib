/********************************************************************************
*	�ļ�����:	WorkerPool.h													*
*	����ʱ�䣺	2015/03/29														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ�Ĺ��������еķ����������ִ�ж�������������					*
*********************************************************************************/
#ifndef __2015_03_29_WORKER_POOL_H__
#define __2015_03_29_WORKER_POOL_H__

#include <cstdint>
#include "common.h"
#include "SingleInstance.h"

class WorkThreadPool;
class CTask;

class CWorkerPool : public SingleInstance<CWorkerPool>
{
public:
	bool  addTask(SERVER_HANDLE server_handle, CTask* pTask);
	void  start();
protected:
	friend class SingleInstance<CWorkerPool>;
	friend class CServer;
	void  addServer(CServer* pServer);
	CWorkerPool();
	~CWorkerPool();
private:
	WorkThreadPool* m_pCore;
};
#endif//__2015_03_29_WORKER_POOL_H__
