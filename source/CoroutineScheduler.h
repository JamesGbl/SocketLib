/********************************************************************************
*	�ļ�����:	CCoroutineScheduler.h											*
*	����ʱ�䣺	2015/04/11														*
*	��   �� :	xzben															*
*	�ļ�����:	c++Я�̵�������ʵ�֣�ģ��һ��Я��									*
*********************************************************************************/
#ifndef __2015_04_11_COROUTINE_SCHEDULER_H__
#define __2015_04_11_COROUTINE_SCHEDULER_H__

#include <cstdint>

class CoSchedulerImplement;
class CCoroutineScheduler;
typedef void(*coroutine_func)(CCoroutineScheduler* pCoSch, void* context);
typedef int32_t CO_ID;

class CCoroutineScheduler
{
public:
	CCoroutineScheduler();
	virtual ~CCoroutineScheduler();

	CO_ID newCoroutine(coroutine_func func, void* context);
	void	resumeCoroutine(CO_ID id);
	void	yieldCoroutine();
private:

	CoSchedulerImplement	*m_pCore;
};
#endif//__2015_04_11_COROUTINE_SCHEDULER_H__