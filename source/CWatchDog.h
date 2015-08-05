/********************************************************************************
*	�ļ�����:	CWatchDog.h													*
*	����ʱ�䣺	2015/07/26														*
*	��   �� :	xzben															*
*	�ļ�����:	���Ź�����													*
*********************************************************************************/
#ifndef __2015_07_26_WATCHDOG_H__
#define __2015_07_26_WATCHDOG_H__

#include "Server.h"
#include "SingleInstance.h"
#include "socket.h"
#include "package.h"


class CWatchDog : public CServer
{
public:
	CWatchDog(short prot, const char* ip = nullptr);
	virtual ~CWatchDog();
	virtual void excuteOneTask(TaskType type, SESSION_ID session, SERVER_HANDLE source, CData *pData)override;

	virtual void handleAccept(SOCKET_HANDLE sock, std::string &RemoteIp);
	virtual void handleMsg(SOCKET_HANDLE sock, Package* package);
	virtual void handleClose(SOCKET_HANDLE sock);

private:
	SOCKET_HANDLE m_listenSock;
};
#endif//__2015_07_26_WATCHDOG_H__