/********************************************************************************
*	�ļ�����:	DTQueue.h														*
*	����ʱ�䣺	2014/03/02														*
*	��   �� :	xzben															*
*	�ļ�����:	һ���̰߳�ȫ�Ķ�д˫���У����پ������µ�������						*
*********************************************************************************/
#ifndef __2014_03_02_CTASK_H__
#define __2014_03_02_CTASK_H__


#include <mutex>
#include <queue>
#include <cstdint>

template<typename	_TaskType>
class DTQueue
{
public:
	DTQueue() :m_nSize(0){}
	virtual ~DTQueue(){}

	void	addTask2WQueue(const _TaskType& pTask)
	{
		m_wlock.lock();
		m_nSize++;
		m_WriteQueue.push(pTask);
		m_wlock.unlock();
	}

	// ����ȡ��ָ�����������񣬲�����ʣ�������
	int32_t	checkQueue(std::queue<_TaskType>& getQueue, int nNum)
	{
		int32_t bRet = 0;
		m_wlock.lock();
		if (m_nSize > 0)
		{
			int readNum = 0;
			while (!m_WriteQueue.empty() && (nNum > readNum || nNum < 0))
			{
				_TaskType& task = m_WriteQueue.front();
				getQueue.push(task);
				m_WriteQueue.pop();
				readNum++;
				m_nSize--;
				bRet = true;
			}
		}
		bRet = m_nSize;
		
		m_wlock.unlock();

		return bRet;
	}

	uint32_t  getSize()
	{
		uint32_t ret = 0;
		
		m_wlock.lock();
		ret = m_nSize;
		m_wlock.unlock();

		return ret;
	}

private:
	DTQueue& operator =(const DTQueue& r);
	DTQueue(const DTQueue& r);
protected:
	uint32_t						m_nSize;
	std::mutex						m_wlock;
	typedef std::queue<_TaskType>	QueueContainer;
	QueueContainer					m_WriteQueue;
};


#endif//__2014_03_02_CTASK_H__