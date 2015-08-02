#include <iostream>
#include "TimerWorker.h"
#include "WorkerPool.h"
#include "SocketDriver.h"
#include "GlobalController.h"
#include "MasterServer.h"
using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("�봫��һ����ʼ���������ļ�\n");
		return 1;
	}
	const char* config_file = argv[1];

	if (!CGlobalController::getInstance()->getConfig()->init(config_file))
	{
		printf("�봫��һ����Ч�ĳ�ʼ���������ļ�\n");
		return 1;
	}

	CGlobalController::getInstance()->init();
	//��ʵ����ʱ��ģ���ʱ����г�ʼ��
	CTimerWorker::getInstance();

	CWorkerPool::getInstance()->start();
	CSocketDriver::getInstance()->start();
	CTimerWorker::getInstance()->start();
	
	CMasterServer::getInstance();
	CGlobalController::getInstance()->waitSystemStop();
	
	return 0;
}