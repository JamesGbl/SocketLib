#include <iostream>
#include "config.h"
#include "TimerWorker.h"
#include "WorkerPool.h"
#include "ServerManager.h"
#include "SocketDriver.h"
#include "GlobalController.h"
#include "LogServer.h"
#include "CoroutineScheduler.h"
#include "MasterServer.h"
#include "ConfigFile.h"
#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("�봫��һ����ʼ���������ļ�\n");
		return 1;
	}
	const char* config_file = argv[1];

	if (!CConfigFile::getInstance()->init(config_file))
	{
		printf("�봫��һ����Ч�ĳ�ʼ���������ļ�\n");
		return 1;
	}

	//ȫ�ֿ����������һ��������ʼ��
	CGlobalController::getInstance();

	//ʵ����һ����־����
	CLogServer::getInstance();
	CMasterServer::getInstance();
	//��ʵ����ʱ��ģ���ʱ����г�ʼ��
	CTimerWorker::getInstance();

	CWorkerPool::getInstance()->start();
	CSocketDriver::getInstance()->start();
	CTimerWorker::getInstance()->start();
	
	CGlobalController::getInstance()->waitSystemStop();
	
	return 0;
}