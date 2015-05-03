/********************************************************************************
*	�ļ�����:	SingleInstance.h												*
*	����ʱ�䣺	2015/04/09														*
*	��   �� :	xzben															*
*	�ļ�����:	����ģʽ��ģ�����														*
*********************************************************************************/
#ifndef __2015_04_09_SINGLE_INSTANCE_H__
#define __2015_04_09_SINGLE_INSTANCE_H__

template<typename TClass>
class SingleInstance
{
public:
	static TClass* getInstance()
	{
		if (getSingleObj() == nullptr)
		{
			getSingleObj() = new TClass();
			getSingleObj()->initInstance();
		}

		return getSingleObj();
	}
	virtual void initInstance(){}
protected:
	friend class SingleInstanceManager;
	SingleInstance(){};
	virtual ~SingleInstance(){};

	static TClass*& getSingleObj()
	{
		static TClass* s_instance = nullptr;
		return s_instance;
	}
};
#endif//__2015_04_09_SINGLE_INSTANCE_H__