/********************************************************************************
*	�ļ�����:	Atom.h														*
*	����ʱ�䣺	2014/10/21														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳԭ�Ӳ�������									*
*********************************************************************************/

#ifndef __2014_10_21_ATOM_H__
#define __2014_10_21_ATOM_H__

#include <cstdint>

/*
*	function : ԭ�����Լ� 1
*	parameter: var Ҫ�����ӵ�ԭ������ַ
*	return:
*		WIN32: �����������ֵ
*/
extern inline uint32_t AtomSelfAdd(void * var);

/*
*	function : ԭ�����Լ� 1
*	parameter: var Ҫ��������ԭ������ַ
*	return:
*		WIN32: �����Լ����ֵ
*/
extern inline uint32_t AtomSelfDec(void * var);

/*
*	function : ԭ��������ָ��ֵ
*	parameter: var Ҫ��������ԭ������ַ�� value Ҫ���ӵ�ֵ
*	return:
*		WIN32: ��������ǰ��ֵ
*/
extern inline uint32_t AtomAdd(void * var, const uint32_t value);

/*
*	function : ԭ������Сָ��ֵ
*	parameter: var Ҫ��������ԭ������ַ�� value Ҫ��С��ֵ
*	return:
*		WIN32: ���ؼ�Сǰ��ֵ
*/
extern inline uint32_t AtomDec(void * var, int32_t value);

/*
*	function : ԭ�������ó�ָ����ֵ
*	parameter: var Ҫ��������ԭ������ַ�� value Ҫ�����õ�ֵ
*	return:
*		WIN32: ��������ǰ��ֵ
*/
extern inline uint32_t AtomSet(void * var, const uint32_t value);

/*
*	function : ��ȡԭ������ֵ
*	parameter: var Ҫ��������ԭ������ַ
*	return:
*		WIN32: ԭ�����ĵ�ǰֵ
*/
extern inline uint32_t AtomGet(void * var);

#endif//__2014_10_21_ATOM_H__