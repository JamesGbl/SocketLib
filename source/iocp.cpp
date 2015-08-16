#include "iocp.h"
#include "GlobalController.h"
#include "socket.h"
#include "SocketDriver.h"

#if (CUR_PLATFROM == PLATFORM_WIN32)

class   OverlappedData
{
public:
	OVERLAPPED Overlapped;
	SocketIOType OperationType;

	static void free(OverlappedData*& lp)
	{
		SAFE_DELETE(lp);
	}

	OverlappedData(SocketIOType type)
	{
		memset(&Overlapped, 0, sizeof(OVERLAPPED));
		OperationType = type;
	}
};

class AcceptOverlappedData : public OverlappedData
{
public:
	static void free(AcceptOverlappedData*& lp)
	{
		SAFE_DELETE(lp);
	}
	static AcceptOverlappedData* create()
	{
		return new AcceptOverlappedData;
	}

	AcceptOverlappedData()
	:OverlappedData(IO_Accept)
	{
		m_dwLocalAddressLength = sizeof(SOCKADDR_IN)+16;//�ͻ��˾�����IP
		m_dwRemoteAddressLength = sizeof(SOCKADDR_IN)+16;//�ͻ�������IP
		memset(m_outPutBuf, 0, sizeof(SOCKADDR_IN)* 2 + 32);
	}
	/**
	* ָ��������ӽ����Ŀͻ��˾�������������ַ���ڴ�
	* ����ʹ�ö�̬������ڴ��
	* ���ݸ�AcceptEx()�ĵ�3������
	*
	*/
	char m_outPutBuf[sizeof(SOCKADDR_IN)* 2 + 32];
	/**
	* �ͻ��˾�����IP��Ϣ����
	* ���ݸ�AcceptEx()�ĵ�5������
	*/
	unsigned long m_dwLocalAddressLength;
	/**
	* �ͻ�������IP��Ϣ����
	* ���ݸ�AcceptEx()�ĵ�6������
	*/
	unsigned long m_dwRemoteAddressLength;
	//�����Ӷ�Ӧ���׽���
	SOCKET		m_hAccept;
};

class RWOverlappedData : public OverlappedData
{
public:
	static void free(RWOverlappedData*& lp)
	{
		SAFE_DELETE(lp);
	}
	static RWOverlappedData* create(SocketIOType	type)
	{
		return new RWOverlappedData(type);
	}

	RWOverlappedData(SocketIOType	type)
		:OverlappedData(type)
	{
		_wsaBuffer.buf = NULL;
		_wsaBuffer.len = 0;
		package = nullptr;
	}

	WSABUF _wsaBuffer;	//WSARecv���ջ�������,���ݸ�WSARecv()�ĵ�2������
	Package* package;
};

class ConnectOverlappedData : public OverlappedData
{
public:
	static void free(ConnectOverlappedData*& lp)
	{
		SAFE_DELETE(lp);
	}
	static ConnectOverlappedData* create(SERVER_HANDLE handle)
	{
		return new ConnectOverlappedData(handle);
	}

	ConnectOverlappedData(SERVER_HANDLE handle)
		: OverlappedData(IO_Connect)
		, requestServer(handle)
	{

	}
	
	SERVER_HANDLE  requestServer;
};

typedef OverlappedData*			LPOverlappedData;
typedef AcceptOverlappedData*	LPAcceptOverlappedData;
typedef ConnectOverlappedData*	LPConnectOverlappedData;
typedef RWOverlappedData*		LPRWOverlappedData;
//////////////////////////////////////////////////////////////////////////
IOCPDriver::IOCPDriver()
{
	//������ɶ˿ڣ����ƶ�������߳���Ϊcpu�ں���
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (NULL == m_hCompletionPort)
	{
		LOG_FATAL("IOCPDriver ������ɶ˿�ʧ��");
	}
}

IOCPDriver::~IOCPDriver()
{
	if (m_hCompletionPort != NULL)
		CloseHandle(m_hCompletionPort);
}

int32_t IOCPDriver::poll_event_process(IOEvent *events, int32_t max, int waittime /*=-1*/)
{
	if (max <= 0) 
		return 0;

	DWORD   dwNumRead = 0; // ��¼��ǰ IO �¼���ͨ������ size
	LPOverlappedData lpOverlapped = NULL; // ��ȡ�¼���Ӧ�� Overlapped 

	dwNumRead = 0;
	SOCKET_HANDLE sock = INVALID_SOCKET;

	IOEvent event;
	do 
	{
		if (!GetQueuedCompletionStatus(m_hCompletionPort, &dwNumRead, (LPDWORD)&sock, (LPOVERLAPPED*)&lpOverlapped, waittime))
		{
			DWORD dwErrorCode = ::GetLastError();
			//////////////////////////////////////////////////////////////////////////
			//�Ϸ��Ĵ���
			if (WAIT_TIMEOUT == dwErrorCode ||  // ��ʱ
				ERROR_INVALID_HANDLE == dwErrorCode) //����ʱ��δ֪ԭ��Ƿ�����������ǲ���ǰһ�γ���رպ�δ���������¼�
			{
				return 0;
			}
			//////////////////////////////////////////////////////////////////////////
			//��������error
			LOG_DEBUG("IOCPDriver >> GetQueuedCompletionStatus failed >> error [%d] socket[%d] ", dwErrorCode, sock);
			if (ERROR_OPERATION_ABORTED == dwErrorCode) //Accept���ڽ��������ӵ�socket�ر�
			{
				LOG_FATAL("IOCPDriver >> GetQueuedCompletionStatus >> Accept ���ڽ��������ӵ�socket�ر�");
				OverlappedData::free(lpOverlapped);
				accept(sock);

				return 0;
			}
			if (IO_Accept == lpOverlapped->OperationType) //Accept�ϵ�socket�رգ�����Ͷ�ݼ���
			{
				LOG_ERROR("IOCPDriver >> GetQueuedCompletionStatus ʧ�� >> Accept�ϵ�socket�رգ�����Ͷ�ݼ���");
				OverlappedData::free(lpOverlapped);
				accept(sock);
			}
			else if(IO_Connect == lpOverlapped->OperationType)
			{
				LOG_DEBUG("IOCPDriver >> GetQueuedCompletionStatus ����ʧ��");
				OverlappedData::free(lpOverlapped);
				event.evt_type = IO_Error;
				Socket connectSock(sock);
				connectSock.close();
			}
			else//�ͻ����쳣�Ͽ��������ߣ��ϵ磬��ֹ����
			{
				LOG_DEBUG("IOCPDriver >> GetQueuedCompletionStatus failed ������ ClientSocket [%d] ����ر�", sock);
				OverlappedData::free(lpOverlapped);
				event.evt_type = IO_Close;
			}
		}
		else if (sock == INVALID_SOCKET)	//"Iocp���ؿ�ָ�룡"
		{
			LOG_FATAL("IOCPDriver �¼����г�����ʱ���ֿ�ָ��");
			return 0;
		}
		else if (0 == dwNumRead && IO_Read == lpOverlapped->OperationType) //���ӶϿ�
		{
			OverlappedData::free(lpOverlapped);
			event.evt_type = IO_Close;
		}
		else
		{
			event.evt_type = lpOverlapped->OperationType;
			if (IO_Accept == lpOverlapped->OperationType)
			{
				accept(sock);
				LPAcceptOverlappedData acpOvelapped = (LPAcceptOverlappedData)(lpOverlapped);
				Socket sock;
				sock.attach(acpOvelapped->m_hAccept);
				sock.updateAcceptContext();
				event.acpt_sock = sock.dettach();
			}
			else if (IO_Read == lpOverlapped->OperationType)
			{
				LPRWOverlappedData rwOverladed = LPRWOverlappedData(lpOverlapped);
				Package* package = rwOverladed->package;
				PackageSize readSize = (PackageSize)dwNumRead;
				package->FillData(readSize);
				if (!package->isFillComplete())
				{
					event.evt_type = IO_ReadPart;
					recv(sock, package);
				}
				else
				{
					event.package = package;
					Package* newPackage = CSocketDriver::getInstance()->resetSocketPackage(sock);
					ASSERT(newPackage != package);
					recv(sock, newPackage);
				}
			}
			else if (IO_Write == lpOverlapped->OperationType)
			{
				LPRWOverlappedData rwOverladed = LPRWOverlappedData(lpOverlapped);
				Package* package = rwOverladed->package;
				PackageSize sendSize = (PackageSize)dwNumRead;
				package->offsetData(sendSize);
				send(sock, package);
			}
			else if (IO_Connect == lpOverlapped->OperationType)
			{
				LPConnectOverlappedData conOvelapped = (LPConnectOverlappedData)(lpOverlapped);
				event.evt_type = IO_Connect;
				event.connect_server = conOvelapped->requestServer;
				Socket conSock(sock);
				conSock.updateConnectContext();
				conSock.dettach();
			}
			OverlappedData::free(lpOverlapped);
		}
	} while (0);

	event.evt_sock = sock;
	events[0] = event;

	return 1;
}

int32_t IOCPDriver::poll_add(SOCKET_HANDLE sock)
{
	if (INVALID_SOCKET == sock)
	{
		LOG_ERROR("IOCPDriver >> poll_add ���԰���Ч�׽���");
		return -1;
	}
	if (NULL == ::CreateIoCompletionPort((HANDLE)sock, m_hCompletionPort, (ULONG_PTR)sock, 0))
	{
		LOG_ERROR("socket [%d] �󶨵���ɶ˿�ʧ��", sock);
		return -1;
	}

	return 0;
}

int32_t IOCPDriver::poll_del(SOCKET_HANDLE sock)
{
	return 0;
}

int32_t IOCPDriver::poll_listen(SOCKET_HANDLE sock)
{
	if (poll_add(sock) != 0)
		return -1;

	for (int i = 0; i < 10; i++)
	{
		accept(sock);
	}

	return 0;
}

int32_t IOCPDriver::poll_connect(SERVER_HANDLE handle, const short port, const char* ip)
{
	Socket socket(Socket::SOCK_TCP);
	socket.setBlocked(false);
	socket.bind(InterAddress(0));

	//����overlapped
	LPConnectOverlappedData overlapConnect = ConnectOverlappedData::create(handle);
	if (overlapConnect == nullptr) return -1;
	
	InterAddress addr(port, ip);

	DWORD sendCnt;
	SOCKET_HANDLE sock = socket.getHandle();
	poll_add(sock);
	
	
	LPFN_CONNECTEX pConnectEx;

	DWORD dwBytes = 0;
	GUID funcGuide = WSAID_CONNECTEX;
	if (0 != ::WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&funcGuide, sizeof(funcGuide), &pConnectEx,
		sizeof(pConnectEx), &dwBytes, NULL, NULL))
	{
		return -1;
	}

	socket.dettach();
	//Ͷ�����ݽ��ղ���
	if (!pConnectEx(sock,
		addr.getAddress(), addr.getAddrLen(), NULL, 0,
		&sendCnt, &overlapConnect->Overlapped))
	{
		int nErrCode = WSAGetLastError();
		if (ERROR_IO_PENDING != nErrCode)
		{
			ConnectOverlappedData::free(overlapConnect);
			LOG_ERROR("Connected [ %d | %s ] failed !", port, ip);
			return -1;
		}
	}

	return 0;
}

int32_t IOCPDriver::poll_send(SOCKET_HANDLE sock, void* buf, int32_t sz)
{
	Package *package = Package::create(sz, buf);
	if (package->isFillComplete())
		send(sock, package);
	else
	{
		Package::free(package);
		return -1;
	}

	return 0;
}

int32_t IOCPDriver::poll_recv(SOCKET_HANDLE sock)
{
	return recv(sock);
}

//////////////////////////////////////////////////////////////////////////
int32_t		IOCPDriver::recv(SOCKET_HANDLE sock, Package* pack /*= nullptr*/)
{
	//����overlapped
	LPRWOverlappedData overlapRead = RWOverlappedData::create(IO_Read);
	Package *package = pack == nullptr ? CSocketDriver::getInstance()->getSocketPackage(sock) : pack;
	if (package == nullptr)
		return -1;

	void *pDataBuffer = nullptr;
	PackageSize nDataSize = 0;
	overlapRead->package = package;
	package->getFillData(nDataSize, pDataBuffer);
	overlapRead->_wsaBuffer.buf = (char*)pDataBuffer;
	overlapRead->_wsaBuffer.len = nDataSize;

	DWORD dwRecv = 0;
	DWORD dwFlags = 0;
	//Ͷ�����ݽ��ղ���
	if (SOCKET_ERROR == ::WSARecv(sock,
		&overlapRead->_wsaBuffer,
		1, &dwRecv, &dwFlags,
		&overlapRead->Overlapped, NULL))
	{
		int nErrCode = WSAGetLastError();
		if (ERROR_IO_PENDING != nErrCode)
		{
			RWOverlappedData::free(overlapRead);
			LOG_ERROR("ClientSocket[%d], addRecv ʧ��", sock);
			return -1;
		}
	}
	
	return 0;
}

int32_t		IOCPDriver::send(SOCKET_HANDLE sock, Package* package)
{
	//�˰��Ѿ���������
	if (package->isSendComplete())
	{
		Package::free(package);
		return 0;
	}

	//Ͷ���µķ����¼�
	LPRWOverlappedData overlapWrite = RWOverlappedData::create(IO_Read);

	void *pDataBuffer = nullptr;
	PackageSize nDataSize = 0;
	package->getSendData(nDataSize, pDataBuffer);

	overlapWrite->package = package;
	overlapWrite->_wsaBuffer.buf = (char*)pDataBuffer;
	overlapWrite->_wsaBuffer.len = nDataSize;

	DWORD dwSend = 0;
	DWORD dwFlags = 0;

	//Ͷ�����ݽ��ղ���
	if (SOCKET_ERROR == ::WSASend(sock,
		&overlapWrite->_wsaBuffer,
		1, &dwSend, dwFlags,
		&overlapWrite->Overlapped, NULL))
	{
		int nErrCode = WSAGetLastError();
		if (ERROR_IO_PENDING != nErrCode)
		{
			LOG_ERROR("ClientSocket[%d], addSend ʧ��", sock);
			RWOverlappedData::free(overlapWrite);
			return -1;
		}
	}
	return 0;
}

int32_t		IOCPDriver::accept(SOCKET_HANDLE sock)
{
	if (sock == INVALID_SOCKET)
	{
		LOG_WARN("AcceptSocket >> addAcept >> ����INVALID_SOCKET");
		return -1;
	}
	LPAcceptOverlappedData overlappedData = AcceptOverlappedData::create();

	//����һ���׽�������׼����������
	Socket socket;
	socket.open(Socket::SOCK_TCP);
	socket.setBlocked(false);
	SOCKET_HANDLE hSockClient = socket.dettach();
	overlappedData->m_hAccept = hSockClient;

	//Ͷ�ݽ������Ӳ���
	if (!::AcceptEx(sock,
		hSockClient,
		overlappedData->m_outPutBuf, 0,
		overlappedData->m_dwLocalAddressLength,
		overlappedData->m_dwRemoteAddressLength,
		NULL, &overlappedData->Overlapped))
	{
		int nErrCode = WSAGetLastError();
		if (ERROR_IO_PENDING != nErrCode)
		{
			LOG_ERROR("AcceptSocket[ %d ] Ͷ��AcceptEx ʧ��", sock);
			AcceptOverlappedData::free(overlappedData);
			return -1;
		}
	}

	return 0;
}

#endif // (CUR_PLATFROM == PLATFORM_WIN32)