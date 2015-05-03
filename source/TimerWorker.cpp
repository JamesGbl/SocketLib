#include "TimerWorker.h"
#include "GlobalController.h"
#include "WorkerPool.h"
#include "Task.h"
#include "MasterServer.h"
#include <ctime>
#include <iostream>
#include <mutex>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

//////////////////////////////////////////////////////////////////////////
// �����ָ��� // ����Ϊ�ڲ����ò���, ������
#define TIMER_NEAR_SHIFT				8
#define TIMER_LEVEL_SHIFT				6
#define TIMER_NEAR						(1<<TIMER_NEAR_SHIFT)
#define TIMER_LEVEL						(1<<TIMER_LEVEL_SHIFT)
#define TIMER_NEAR_MASK					(TIMER_NEAR-1)
#define TIMER_LEVEL_MASK				(TIMER_LEVEL-1)
//////////////////////////////////////////////////////////////////////////
// �����ָ��� // һ��Ϊ�ڲ�ʵ�ֵ�ϸ�ڲ���
#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = static_cast<long>(clock);
	tp->tv_usec = wtm.wMilliseconds * 1000;

	return (0);
}
#endif

struct TimerNode
{
	struct TimerNode *next;
	uint64_t expire_time;

	SERVER_HANDLE server_handle;
	SESSION_ID	  session_id;
	//�ڵ㸽��������
	CData		 *pData;
};

struct TimerList
{
	TimerNode	head;
	TimerNode*	tail;

	TimerList()
	{
		head.next = nullptr;
		tail = &head;
	}

	void link_node(TimerNode* node)
	{
		this->tail->next = node;
		this->tail = node;
		node->next = nullptr;
	}

	TimerNode* link_clear()
	{
		TimerNode* ret = this->head.next;

		this->head.next = nullptr;
		this->tail = &this->head;

		return ret;
	}
};

/*
*	ʱ�䵥λΪ 10 ����
*/
class TimerCore
{
public:
	TimerCore();
	~TimerCore();

	uint64_t			get_systime(); //���ص�ǰϵͳʱ��(10���뵥λ)
	uint64_t			get_time();
	uint32_t			get_time_msec();
	uint32_t			get_time_sec();
	uint64_t			get_start_time();

	int32_t				init();
	void				dispatch_list(struct TimerNode *current);
	void				timer_execute();
	void				timer_shift();
	void				update_time(uint64_t cp);
	int32_t				update();
	int32_t				add_node(TimerNode* node);
	int32_t				add_timeout(uint32_t delay_msec, SERVER_HANDLE handle, SESSION_ID session_id, CData *pData = nullptr);
private:
	TimerList		m_near[TIMER_NEAR];
	TimerList		m_wait[4][TIMER_LEVEL];
	std::mutex		m_lock;					// �ں���
	uint32_t		m_time;					// �ӷ�������ʼ������ʱ���
	uint32_t		m_unCurrentSec;			// ��ǰʱ�����������
	uint32_t		m_unCurrentMSec;		// ��ǰʱ��ĺ��벿��
	uint64_t		m_unCurTimePoint;		// ��ǰʱ���	
	uint64_t		m_unStartTimePoint;		// ��������ʼʱ��ʱ���
};

TimerCore::TimerCore()
{
	m_time = 0;
	m_unCurrentSec = 0;
	m_unCurrentMSec = 0;
	m_unCurTimePoint = 0;
	m_unStartTimePoint = 0;
}

TimerCore::~TimerCore()
{

}

//���ص�ǰϵͳʱ��(10���뵥λ)
uint64_t	TimerCore::get_systime()
{
	uint64_t t;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = (uint64_t)tv.tv_sec * 100;
	t += tv.tv_usec / 10000;

	return t;
}

uint32_t	TimerCore::get_time_msec()
{
	return m_unCurrentMSec;
}
uint32_t	TimerCore::get_time_sec()
{
	return m_unCurrentSec;
}

uint64_t	TimerCore::get_time()
{
	return m_unCurTimePoint;
}

uint64_t	TimerCore::get_start_time()
{
	return m_unStartTimePoint;
}

int32_t TimerCore::init()
{
	uint64_t cp = get_systime();

	m_time = 0;
	m_unCurrentSec = static_cast<uint32_t>(cp / 100);
	m_unCurrentMSec = static_cast<uint32_t>(10 * (cp - m_unCurrentSec));
	m_unCurTimePoint = cp;
	m_unStartTimePoint = cp;

	return 0;
}

void TimerCore::dispatch_list(struct TimerNode *current)
{
	while (current)
	{
		SERVER_HANDLE hanlde = current->server_handle;
		SESSION_ID  session_id = current->session_id;

		uint64_t expire = current->expire_time;
		////////////////////////////////////////
		//�˴�ִ��Ҫ������߼�, ��Ҫ�ٶ�ʱ��ģ����ִ�о�����߼�
		//Ӧ�ý������ִ�з��͸������߳�ִ�У�����ᵼ�¶�ʱ���������
		//LOG_DEBUG("timeout server handle[ %d ] session[ %d ]", hanlde, session_id);
		CWorkerPool::getInstance()->addTask(hanlde, CTask::create(TaskType::TIME_OUT, session_id, hanlde,  current->pData));
		///////////////////////////////////////
		struct TimerNode *tmp = current;
		current = current->next;
		free(tmp);
	}
}

void	TimerCore::timer_execute()
{
	m_lock.lock();

	int idx = m_time & TIMER_NEAR_MASK;
	while (m_near[idx].head.next) {
		struct TimerNode *current = m_near[idx].link_clear();
		m_lock.unlock();

		// dispatch_list don't need lock T
		dispatch_list(current);

		m_lock.lock();
	}

	m_lock.unlock();
}

void	TimerCore::timer_shift()
{
	m_lock.lock();

	int mask = TIMER_NEAR;
	int time = (++m_time) >> TIMER_NEAR_SHIFT;
	int i = 0;

	while ((m_time & (mask - 1)) == 0) {
		int idx = time & TIMER_LEVEL_MASK;
		if (idx != 0) {
			--idx;
			struct TimerNode *current = m_wait[i][idx].link_clear();
			while (current) {
				struct TimerNode *temp = current->next;
				add_node(current);
				current = temp;
			}
			break;
		}
		mask <<= TIMER_LEVEL_SHIFT;
		time >>= TIMER_LEVEL_SHIFT;
		++i;
	}
	m_lock.unlock();
}

void	TimerCore::update_time(uint64_t new_cp)
{
	m_lock.lock();

	//std::cout<<"current time point:"<<new_cp<<std::endl;

	uint64_t old_cp = m_unCurTimePoint;
	uint32_t diff = (uint32_t)(new_cp - old_cp);

	uint32_t msec = m_unCurrentMSec;
	uint32_t new_sec = m_unCurrentSec;
	uint32_t new_msec = msec + diff;

	if (new_msec < msec)
	{
		// when cs > 0xffffffff(about 497 days), time rewind
		new_sec += 0xffffffff / 100;
	}

	m_unCurTimePoint = new_cp;
	m_unCurrentSec = new_sec;
	m_unCurrentMSec = new_msec;

	m_lock.unlock();
}

int32_t TimerCore::update()
{
	timer_execute();

	timer_shift();

	timer_execute();
	return 0;
}

int32_t		TimerCore::add_node(TimerNode* node)
{
	if (nullptr == node) return 1; //error

	uint64_t expire = node->expire_time;
	uint64_t curtime = m_time;

	//����ڵ���ֹʱ�����ٽ�ִ�е�ʱ�䷶Χ��
	if ((expire | TIMER_NEAR_MASK) == (curtime | TIMER_NEAR_MASK))
	{
		m_near[expire&TIMER_NEAR_MASK].link_node(node);
	}
	else//��������ٽ�ִ�е�LEVEL �ڼ���������level���ڵ�������б���
	{
		uint32_t mask = TIMER_NEAR << TIMER_NEAR_SHIFT;
		int level = 0;
		for (level = 0; level < 3; level++)
		{
			if ((expire | (mask - 1)) == (curtime | (mask - 1)))
				break;

			mask = mask << TIMER_NEAR_SHIFT;
		}
		uint32_t idx = ((expire >> (TIMER_NEAR_SHIFT + level*TIMER_LEVEL_SHIFT)) & TIMER_LEVEL_MASK) - 1;
		m_wait[level][idx].link_node(node);
	}

	return 0;
}

int32_t		TimerCore::add_timeout(uint32_t delay_msec, SERVER_HANDLE handle, SESSION_ID session_id, CData *pData /* = nullptr */)
{
	// ����һ��ʱ��ڵ㣬�����丽����������������
	TimerNode* newNode = (TimerNode*)malloc(sizeof(TimerNode));
	if (nullptr == newNode) // error 
	{
		return 1;
	}
	newNode->server_handle = handle;
	newNode->session_id = session_id;

	newNode->pData = pData;

	m_lock.lock();

	uint64_t expire_time = m_time + delay_msec;
	newNode->expire_time = expire_time;
	newNode->next = nullptr;
	add_node(newNode);
	m_lock.unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// �����ָ��ߣ�һ��Ϊ�����׳��ӿڵ�ʵ�ֲ���
CTimerWorker::CTimerWorker()
{
	m_pCore = new TimerCore;
	m_pCore->init();
}

CTimerWorker::~CTimerWorker()
{

}


bool CTimerWorker::start()
{
	return CThread::start();
}
void CTimerWorker::run()
{
	ThreadGuard guard;
	while ( !CGlobalController::getInstance()->isSystemStop() )
	{
		this->updateTimer();
	}
}
//��õ�ǰϵͳʱ��
int32_t CTimerWorker::getTimeOfDay(struct timeval *tp, void *tzp)
{
	return ::gettimeofday(tp, tzp);
}

// ��ʱ���ڲ����£��˽ӿ�Ӧ����һ������һ���߳��в��ϵ�ȥ����ִ��
int32_t	CTimerWorker::updateTimer()
{
	uint64_t systime = m_pCore->get_systime();
	uint64_t cp = m_pCore->get_time();

	if (systime < cp)// ����.....
	{
		LOG_WARN("error timer assert(systime < cp) is failed!!");
	}
	else if (systime != cp)
	{
		uint32_t diff = static_cast<uint32_t>(systime - cp);
		m_pCore->update_time(systime);

		for (uint32_t i = 0; i < diff; i++) {
			m_pCore->update();
		}
	}

	return 0;
}

int32_t CTimerWorker::addTimeout(uint32_t delay_msec, SERVER_HANDLE handle, uint32_t session_id, CData *pData /* = nullptr */)
{
	return m_pCore->add_timeout(delay_msec, handle, session_id, pData);
}

//��ȡ��ǰʱ���
uint64_t CTimerWorker::getTime()
{
	return m_pCore->get_time();
}

std::string CTimerWorker::getFormatTime(std::string fmt /*= "%Y-%m-%d %H:%M:%S"*/)
{
	struct tm *p;
	time_t t = getTime() / 100;
	p = localtime(&t);
	char s[100];
	strftime(s, sizeof(s), fmt.c_str(), p);
	return std::string(s);
}

struct tm	CTimerWorker::getCurTime()
{
	struct tm *p;
	time_t t = getTime() / 100;
	p = localtime(&t);
	return *p;
}

//��ȡ��ʼ����ʱ���
uint64_t CTimerWorker::getStartTime()
{
	return m_pCore->get_start_time();
}

struct tm CTimerWorker::getSystemLocalTime()
{
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	return *timeinfo;
}