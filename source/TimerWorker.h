/********************************************************************************
*	�ļ�����:	TimerWorker.h													*
*	����ʱ�䣺	2015/03/29														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳʱ�������������ͨ�������ٻ�õ�ǰʱ�����Ӷ�ʱ��				*
*********************************************************************************/
#ifndef __2015_03_29_TIMER_WORKDER_H__
#define __2015_03_29_TIMER_WORKDER_H__

#include <cstdint>
#include "common.h"
#include "Thread.h"
#include "SingleInstance.h"

class TimerCore;
class CTimerWorker : public CThread, public SingleInstance<CTimerWorker>
{
public:
	virtual bool start() override;
	//��ʱ����Ӷ�ʱ����
	// delay_msec ��λΪ10 ����
	int32_t	addTimeout(uint32_t delay_msec, SERVER_HANDLE hanle, uint32_t session_id, CData *pData = nullptr);
	//��ȡ��ǰʱ���
	uint64_t	getTime();

	//��ȡ��ǰ����ʱ�������ַ���
	std::string getFormatTime(std::string fmt = "%Y-%m-%d %H:%M:%S");
	struct tm	getCurTime();
	//��ȡ��ʼ����ʱ���
	uint64_t getStartTime();

	//��ȡ��ǰϵͳ�ı���ʱ��
	int32_t getTimeOfDay(struct timeval *tp, void *tzp);
	//��ȡ��ǰϵͳ�ı���ʱ��
	struct tm getSystemLocalTime();
protected:
	virtual void run() override;
	

	// ��ʱ���ڲ����£��˽ӿ�Ӧ����һ������һ���߳��в��ϵ�ȥ����ִ��
	int32_t	updateTimer();

	friend class SingleInstance<CTimerWorker>;
	CTimerWorker();
	virtual ~CTimerWorker();
private:

	TimerCore *m_pCore;
};
#endif//__2015_03_29_TIMER_WORKDER_H__