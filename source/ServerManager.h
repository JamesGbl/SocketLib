/********************************************************************************
*	�ļ�����:	ServerManager.h													*
*	����ʱ�䣺	2015/03/30														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ�����з���Ĺ�������											*
*********************************************************************************/
#ifndef __2015_03_30_SERVER_MANAGER_H__
#define __2015_03_30_SERVER_MANAGER_H__

#include "Server.h"
#include "SingleInstance.h"
#include <string>
#include <unordered_map>
#include <mutex>

class CServerManager : public SingleInstance<CServerManager>
{
public:
	CServer*	getServerByHandle(SERVER_HANDLE server_handle);
	CServer*	getServerByName(std::string server_name);
	//ÿ�����񶼱ض�ע�������ľ�������ǲ�һ��ע��������
	bool		registerServer( CServer *pserver);
	bool		registerServerByName(std::string server_name, CServer *pserver);
	void		unregisterServer(CServer *pServer);
	uint32_t	getCurServerNum();
protected:
	friend class SingleInstance<CServerManager>;
	CServerManager();
	~CServerManager();
private:
	void unregisterServerByName(std::string server_name);
	void unregisterServerByHandle(SERVER_HANDLE server_handle);
private:
	typedef std::unordered_map<SERVER_HANDLE, CServer*>  ConHandle2Server;
	typedef std::unordered_map<std::string, CServer*>	 ConName2Server;
	ConHandle2Server	m_handle2Server;
	std::mutex			m_lockHandle;
	ConName2Server		m_name2Server;
	std::mutex			m_lockName;
	uint32_t			m_curServerCount;
};
#endif//__2015_03_30_SERVER_MANAGER_H__