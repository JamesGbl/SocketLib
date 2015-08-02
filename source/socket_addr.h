/********************************************************************************
*	�ļ�����:	socket_addr.h													*
*	����ʱ�䣺	2014/03/18														*
*	��   �� :	xzben															*
*	�ļ�����:	���ļ���Ҫʵ��һ����ƽ̨�޹ص�Socket ��ַ��						*
*********************************************************************************/

#include "socket_handle.h"

#ifndef __2014_03_17_SOCKET_ADDRESS_H__
#define __2014_03_17_SOCKET_ADDRESS_H__


/*
*		ƽ̨�޹ص� socket internet address ���࣬Ŀǰʵ��Ϊ ipv4 �汾
*		�Ժ��Դ�Ϊ������չ ipv4 �� ipv6 �汾��Ŀǰϵͳ��ʹ�õ�ַ��ʹ�ô˵�ַ
*/
class InterAddress
{
public:
	static InterAddress addr_any;
public:
	InterAddress();
	InterAddress(int32_t Port, const char* szIp = nullptr);
	InterAddress(sockaddr*	pSockaddr);
	virtual ~InterAddress();

	virtual	void		open(int32_t Port, const char* szIp = nullptr);
	virtual void		open(sockaddr*	pSockaddr);
	virtual int32_t		getAddrLen() const;
	virtual sockaddr*	getAddress() const;
	virtual bool		getAddress(char* szIp, int32_t& port) const;
protected:
	sockaddr_in		m_addr;
};

#endif//__2014_03_17_SOCKET_ADDRESS_H__
