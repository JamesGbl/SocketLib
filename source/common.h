/********************************************************************************
*	�ļ�����:	common.h														*
*	����ʱ�䣺	2015/04/14														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ��ͨ�õĶ���								*
*********************************************************************************/
#ifndef __2015_04_14_COMMON_H__
#define __2015_04_14_COMMON_H__

#include <cstdint>
#include <queue>
#include <string>
#include "config.h"

/*************************************************
**	ϵͳ��ʹ�õ� DELETE ��ȫ�궨��				**
**************************************************/
#define	SAFE_DELETE(__ptr)					if( nullptr != (__ptr) ) { delete __ptr; __ptr = nullptr; }
#define SAFE_DELETE_ARR(__ptr)				if( nullptr != (__ptr) ) { delete[] __ptr; __ptr = nullptr; }

#define socketlib_error( err_msg )					(err_msg);
typedef uint32_t	SERVER_HANDLE; //�����ʾ���
typedef uint32_t	SESSION_ID; //ÿ�������һ��������ʾID

enum : SERVER_HANDLE
{
	SERVER_HANDLE_LOG = 1,
	SERVER_HANDLE_TIMER,
	SERVER_HANDLE_MASTER,
	//////////////////////////////////////////////////////////////////////////
	//must be last
	SYS_SERVER_HANLE_END,
};

enum LOG_LEVEL
{
	LOG_ALL = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
	LOG_OFF,
};

enum TaskType
{
	EMPTY = 0,
	TIME_OUT,
	SYS_LOG,
};

#endif//__2015_04_14_COMMON_H__