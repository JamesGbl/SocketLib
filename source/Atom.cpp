#include "Atom.h"
#include "common.h"
#if (CUR_PLATFROM == PLATFORM_WIN32) 
#include <windows.h>
#endif
/*
*	function : ԭ�����Լ� 1
*	parameter: var Ҫ�����ӵ�ԭ������ַ
*	return:
*		WIN32: �����������ֵ
*/
uint32_t AtomSelfAdd(void * var)
{
#if (CUR_PLATFROM == PLATFORM_WIN32) 
	return InterlockedIncrement((long *)(var)); // NOLINT
#else
	return __sync_add_and_fetch((uint32_t *)(var), 1); // NOLINT
#endif
}
/*
*	function : ԭ�����Լ� 1
*	parameter: var Ҫ��������ԭ������ַ
*	return:
*		WIN32: �����Լ����ֵ
*/
uint32_t AtomSelfDec(void * var)
{
#if (CUR_PLATFROM == PLATFORM_WIN32) 
	return InterlockedDecrement((long *)(var)); // NOLINT
#else
	return __sync_add_and_fetch((uint32_t *)(var), -1); // NOLINT
#endif
}
/*
*	function : ԭ��������ָ��ֵ
*	parameter: var Ҫ��������ԭ������ַ�� value Ҫ���ӵ�ֵ
*	return:
*		WIN32: ��������ǰ��ֵ
*/
uint32_t AtomAdd(void * var, const uint32_t value)
{
#if (CUR_PLATFROM == PLATFORM_WIN32) 
	return InterlockedExchangeAdd((long *)(var), value); // NOLINT
#else
	return __sync_fetch_and_add((uint32_t *)(var), value);  // NOLINT
#endif
}
/*
*	function : ԭ������Сָ��ֵ
*	parameter: var Ҫ��������ԭ������ַ�� value Ҫ��С��ֵ
*	return:
*		WIN32: ���ؼ�Сǰ��ֵ
*/
uint32_t AtomDec(void * var, int32_t value)
{
	value = value * -1;
#if (CUR_PLATFROM == PLATFORM_WIN32) 
	return InterlockedExchangeAdd((long *)(var), value); // NOLINT
#else
	return __sync_fetch_and_add((uint32_t *)(var), value);  // NOLINT
#endif
}

/*
*	function : ԭ�������ó�ָ����ֵ
*	parameter: var Ҫ��������ԭ������ַ�� value Ҫ�����õ�ֵ
*	return:
*		WIN32: ��������ǰ��ֵ
*/
uint32_t AtomSet(void * var, const uint32_t value)
{
#if (CUR_PLATFROM == PLATFORM_WIN32) 
	::InterlockedExchange((long *)(var), (long)(value)); // NOLINT
#else
	__sync_lock_test_and_set((uint32_t *)(var), value);  // NOLINT
#endif
	return value;
}

/*
*	function : ��ȡԭ������ֵ
*	parameter: var Ҫ��������ԭ������ַ
*	return:
*		WIN32: ԭ�����ĵ�ǰֵ
*/
uint32_t AtomGet(void * var)
{
#if (CUR_PLATFROM == PLATFORM_WIN32) 
	return InterlockedExchangeAdd((long *)(var), 0); // NOLINT
#else
	return __sync_fetch_and_add((uint32_t *)(var), 0);  // NOLINT
#endif
}
