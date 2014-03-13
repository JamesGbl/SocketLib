#ifndef __2014_03_02_CSOCKET_H__
#define __2014_03_02_CSOCKET_H__

#include "CObject.h"

/*
*	Socket ���װ��Socket������TCPЭ���һЩ����
*/
class TCPSocket : public NoCopyable
{
public:
	static sockaddr_in	s_getSockAddrIpV4(const char* szIpAddr, const u_short usPort);
	static void			s_getSockAddrIpV4(const sockaddr_in addr, std::string &szIpAddr,  u_short &usPort);
public:
	TCPSocket();
	virtual ~TCPSocket();
	//��ʼ���׽���
	bool	init(const int nAf = AF_INET, const int nProtocl = 0);
	//��ȡ�׽��ְ󶨵ı��ص�ַ
	bool	getLocalAddr(std::string& strIpAddr, u_short& usPort);
	//��ȡ���ӳɹ�����׽��ֵ����ӶԷ��ĵ�ַ
	bool	getPeerAddr(std::string& strIpAddr, u_short& usPort);
	//�˹�ָ���׽��ֵ����ӶԷ��ĵ�ַ
	bool	setPeerAddr(const sockaddr_in& addrPeer);
	bool	attach(SOCKET hSocket);
	SOCKET	dettach();
	bool	bind(const sockaddr_in& addrBind);
	bool	listen(int nBacklog = 10);
	//���׽��ְ󶨵�ָ���˿ڲ�������������
	bool	startServer(int nPort);
	TCPSocket*	accept();
	bool	connect(const sockaddr_in& addrCon);
	int		recv(char* pBuf, int nLen, int nFlag =0);
	int		send(const char *pBuf,const int nLen, int nFlag = 0);
	SOCKET	getSocket();
	bool	initPeerAddress();
	//ʹ��IOCP ʱ��Ҫ��accept��ͨ���˺������������׽��ֵ�����
	bool	updateAcceptContext();
	bool	shutdown(int how = SD_BOTH);
	bool	close();
	bool	disConnect();
protected:
	SOCKET			m_hSocket;	//�׽��־��
	sockaddr_in		m_addrPeer; //�׽������Ӻ���֮���ӵĵ�ַ
	bool			m_bHaveAddr;//�Ƿ�������Ӷ���ĵ�ַ
};

#endif//__2014_03_02_CSOCKET_H__