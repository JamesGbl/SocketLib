/********************************************************************************
*	�ļ�����:	SocketDriver.h													*
*	����ʱ�䣺	2014/03/29														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ������ʵ��													*
*********************************************************************************/
#ifndef __2015_03_29_SOCKET_DRIVER_H__
#define __2015_03_29_SOCKET_DRIVER_H__

#include "SingleInstance.h"
#include "Thread.h"

class CSocketDriver : public CThread, public SingleInstance<CSocketDriver>
{
public:
	
protected:
	friend class SingleInstance<CSocketDriver>;
	CSocketDriver();
	virtual ~CSocketDriver();
	virtual void initInstance() override;
	virtual void run()override;
};
#endif//__2015_03_29_SOCKET_DRIVER_H__