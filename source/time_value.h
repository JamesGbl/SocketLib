/********************************************************************
*	�ļ�����:	time_value.h										*
*	����ʱ�䣺	2014/03/17											*
*	��   �� :	xzben												*
*	�ļ�����:	ƽ̨�޹�ʱ��											*
*********************************************************************/

#ifndef __2014_03_23_TIME_VALUE_H__
#define __2014_03_23_TIME_VALUE_H__


#include <cstdint>
#include <time.h>
#include "common.h"
#if (CUR_PLATFROM == PLATFORM_WIN32)
	#include <winsock2.h>
#elif (CUR_PLATFROM == PLATFROM_LINUX)
	#include<sys/time.h>
#endif//_WIN32

const int32_t ONE_SECOND_TO_MSECOND = 1000;
const int32_t ONE_SECOND_TO_USECOND = 1000 * 1000;
const int32_t ONE_MSECOND_TO_USECOND = 1000;

class TimeValue
{
public:
	TimeValue();
	explicit TimeValue(int32_t sencond, int32_t msec = 0, int32_t usec = 0);
	explicit TimeValue(const struct timeval& tmval);
	virtual ~TimeValue();

	//��
	int32_t sec();
	//����
	int32_t msec();
	//΢��
	int32_t usec();


	void set(int32_t sec, int32_t msec = 0, int32_t usec = 0);

#if	 (CUR_PLATFROM == PLATFORM_WIN32)
	struct timeval const* getTimeval() const { return &m_tmVal; }
#elif (CUR_PLATFROM == PLATFROM_LINUX)
	struct timeval* getTimeval() const;
#endif
// 	operator struct timeval const  () const;
// 	operator struct timeval const *() const;

	TimeValue& operator+(const TimeValue& rTime);
	TimeValue& operator-(const TimeValue& rTime);

private:
	void normalize();

private:
	struct timeval	m_tmVal;
};

#endif//__2014_03_23_TIME_VALUE_H__
