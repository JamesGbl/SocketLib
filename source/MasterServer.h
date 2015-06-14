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