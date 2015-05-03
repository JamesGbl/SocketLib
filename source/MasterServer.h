/********************************************************************************
*	�ļ�����:	MasterServer.h													*
*	����ʱ�䣺	2015/04/11														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ�����ķ���													*
*********************************************************************************/
#ifndef __2015_04_11_MASTER_SERVER_H__
#define __2015_04_11_MASTER_SERVER_H__

#include "Server.h"
#include "SingleInstance.h"

// ȫ����־��
#define LOG_DEBUG(pattern, ...)	CMasterServer::getInstance()->debug((pattern), ##__VA_ARGS__)
#define LOG_INFO(pattern, ...)  CMasterServer::getInstance()->info((pattern),  ##__VA_ARGS__)
#define LOG_WARN(pattern, ...)	CMasterServer::getInstance()->warn((pattern),  ##__VA_ARGS__)
#define LOG_ERROR(pattern, ...) CMasterServer::getInstance()->error((pattern), ##__VA_ARGS__)
#define LOG_FATAL(pattern, ...) CMasterServer::getInstance()->fatal((pattern), ##__VA_ARGS__)

class CMasterServer : public CServer, public SingleInstance<CMasterServer>
{
public:

protected:
	friend class SingleInstance<CMasterServer>;
	CMasterServer();
	virtual ~CMasterServer();
	virtual void initInstance() override;
	virtual void excuteOneTask(TaskType type, SESSION_ID session, SERVER_HANDLE source, CData *pData)override;
protected:
	void CALLBACK tick(CData *pData);
private:
	int		m_count;
};
#endif//__2015_04_11_MASTER_SERVER_H__