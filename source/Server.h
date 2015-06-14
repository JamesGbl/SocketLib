/********************************************************************************
*	�ļ�����:	Server.h														*
*	����ʱ�䣺	2015/03/29														*
*	��   �� :	xzben															*
*	�ļ�����:	����Ļ���														*
*********************************************************************************/
#ifndef __2015_03_29_SERVER_H__
#define __2015_03_29_SERVER_H__

#include "DTQueue.h"
#include "Session.h"
#include "common.h"
#include "Task.h"
#include "Ref.h"

class CTask;

class CServer : public CSession, public CRef, public DTQueue<CTask const*>
{
public:
	CServer();
	virtual ~CServer();

	SERVER_HANDLE	getServerHandle() const { return m_server_handle; }
	std::string		getServerName();
	bool			setServerName(std::string name);
	

	virtual void	setFlagInGlobalQueue();
	virtual void	clearFlagInGlobalQueue();
	virtual bool	isInGlobalQueue();
	bool			addTask(CTask const *pTask);

	void			timeout(uint32_t delay_msec, SERVER_TIMEOUT_CALLBACK callback, CData *pData = nullptr);
	void			timeout(uint32_t delay_msec, NORMAL_TIMEOUT_CALLBACK callback, CData *pData = nullptr);
protected:
	//���������ִ��һ������
	virtual void	excuteOneTask(TaskType type, SESSION_ID session, SERVER_HANDLE source, CData *pData) = 0;
	//�����ռ���������ֹͣʱ�յ�������Ĭ�ϴ�����ֱ���ͷ�����
	virtual void	collectStopTask(CTask const *pTask);
	friend class	WorkThreadPool;
	int32_t			dispatchTasks(int num);
protected:
	uint32_t				m_bIsInGlobalQueue;
	SERVER_HANDLE			m_server_handle;
	std::string				m_serverName;
	std::mutex				m_lock;
private:
	static SERVER_HANDLE	s_server_handle;
};
#endif//__2015_03_29_SERVER_H__