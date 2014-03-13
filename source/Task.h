#ifndef __2014_03_02_CTASK_H__
#define __2014_03_02_CTASK_H__


#include "CObject.h"

/**
*	�������ģ�壬ʹ�õ���˫���е�˼�룬��Ҫ��Ϊ�˼��ٻ�������ʹ��
*	��ģ�����Ѿ����ڶ���һ�ˣ��˶��������������ʵ�ֶ��̰߳�ȫ�������Ĵ洢��
*	���ж������ڴ洢����ʹ��ʱ��洢��
*		Ҫ�ɾ���ʹ��������е�����ʵ�֣���ʵ�� _addTask() �ӿڣ����洢���������ʹ�ö�����
**/
template<typename	_TaskType>
class TaskQueue : public NoCopyable
{
public:
	TaskQueue():m_nSize(0){}
	virtual ~TaskQueue(){}

	void	addTask(const _TaskType& pTask)
	{
		AutoLock lock(&m_TaskQueuemutex);
		m_taskQueue.push(pTask);
		m_nSize ++;
	}

	bool	checkQueue()
	{
		AutoLock lock(&m_TaskQueuemutex);
		while(!m_taskQueue.empty())
		{
			_TaskType& task = m_taskQueue.front();
			_addTask(task);
			m_taskQueue.pop();
		}
		m_nSize = 0;
		return true;
	}
protected:
	virtual void	_addTask(_TaskType pTask) = 0;
	int				m_nSize;
protected:
	Mutex				m_TaskQueuemutex;
	typedef std::queue<_TaskType>	QueueContainer;
	QueueContainer		m_taskQueue;
};


#endif//__2014_03_02_CTASK_H__