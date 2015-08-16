#ifndef __2014_01_04_IODRIVER_H__
#define __2014_01_04_IODRIVER_H__

#include "socket.h"
#include "package.h"

typedef enum enum_SocketIOType : uint16_t
{
	IO_UNKNOW	= 0,	  //δ֪
	IO_Write	= 1 << 0,      // д
	IO_Read		= 1 << 1 ,	  // ��
	IO_ReadPart = 1 << 2,	  // ��ȡ�˲�������
	IO_Accept	= 1 << 3,	  // ��������
	IO_Connect	= 1 << 4,	  // ���Ӳ���
	IO_Close	= 1 << 5,	  // �׽��ֹر�

	IO_Error	= 1 << 15,	  // io ����
}SocketIOType;



#if (CUR_PLATFROM == PLATFORM_WIN32)
	#define  MAX_EVENT	1
#elif (CUR_PLATFROM == PLATFORM_UNKNOW)
	#define  MAX_EVENT	64
#endif


typedef struct st_IOEvent
{
	st_IOEvent()
	{
		evt_sock = INVALID_SOCKET;
		acpt_sock = INVALID_SOCKET;
		evt_type = IO_UNKNOW;
		package = nullptr;
	}

	SOCKET_HANDLE evt_sock;
	SocketIOType  evt_type;

	union
	{
		SOCKET_HANDLE acpt_sock;		 //Accept ��������չ���� �������ӵ�socket
		SERVER_HANDLE connect_server;    //connect ��������չ���� �������ӵķ���handle
		Package*	  package;			 //���ݴ��� ����չ�������� package
	};
}IOEvent;

class IODriver
{
public:
	IODriver(){};
	virtual ~IODriver(){};

	virtual int32_t poll_event_process(IOEvent *events, int32_t max, int waittime = -1) = 0;
	virtual int32_t poll_add(SOCKET_HANDLE sock) = 0;
	virtual int32_t poll_del(SOCKET_HANDLE sock) = 0;
	virtual int32_t poll_listen(SOCKET_HANDLE sock) = 0;
	virtual int32_t poll_connect(SERVER_HANDLE handle, const short port, const char* ip) = 0;
	virtual int32_t poll_send(SOCKET_HANDLE sock, void* buf, int32_t sz) = 0;
	virtual int32_t poll_recv(SOCKET_HANDLE sock) = 0;

};

#endif//__2014_01_04_IODRIVER_H__