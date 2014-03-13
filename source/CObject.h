#ifndef __2014_03_02_COBJECT_H__
#define __2014_03_02_COBJECT_H__

#include <assert.h>
#include "config.h"
#include "COS.h"


/*
*	˽�л����ƹ��캯���͸��ƺ���
*	ʹ��������޷�����
*/
class NoCopyable
{
protected:
	NoCopyable(){};
	~NoCopyable(){};
private:
	NoCopyable(const NoCopyable& );
	NoCopyable& operator=(const NoCopyable&);
};

class CObject
{
public:
	CObject();
	virtual ~CObject();
	
private:
	int		m_nObjectId;
	int		m_nRefence;
};

#endif//__2014_03_02_COBJECT_H__