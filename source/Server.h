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

class CTask;

class CServer : public CSession, public DTQueue<CTask const*>
{
public:
	CServer(SERVER_HANDLE handle = SYS_SERVER_HANLE_END);
	virtual ~CServer();

	SERVER_HANDLE	getServerHandle() const { return m_server_handle; }
	std::string		getServerName();
	bool			setServerName(std::string name);

	void			timeout(uint32_t delay_msec, SERVER_TIMEOUT_CALLBACK callback, CData *pData = nullptr);
	void			timeout(uint32_t delay_msec, NORMAL_TIMEOUT_CALLBACK callback, CData *pData = nullptr);
	void			log(const LOG_LEVEL level, const char* pattern, ...);

	void			debug(const char * pattern, ...);
	void			info(const char * pattern, ...);
	void			warn(const char * pattern, ...);
	void			error(const char * pattern, ...);
	void			fatal(const char * pattern, ...);

	virtual void	setFlagInGlobalQueue();
	virtual void	clearFlagInGlobalQueue();
	virtual bool	isInGlobalQueue();
protected:
	void			logva(const LOG_LEVEL level, const char* pattern, va_list vp);
	//���������ִ��һ������
	virtual void	excuteOneTask(TaskType type, SESSION_ID session, SERVER_HANDLE source, CData *pData) = 0;
	//�����ռ���������ֹͣʱ�յ�������Ĭ�ϴ�����ֱ���ͷ�����
	virtual void	collectStopTask(CTask const *pTask);
private:
	friend class	WorkThreadPool;
	bool			addTask(CTask const *pTask);
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