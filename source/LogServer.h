/********************************************************************************
*	�ļ�����:	LogServer.h														*
*	����ʱ�䣺	2015/04/10														*
*	��   �� :	xzben															*
*	�ļ�����:	��־�����ʵ��													*
*********************************************************************************/
#ifndef __2015_04_10_LOG_SERVER_H__
#define __2015_04_10_LOG_SERVER_H__

#include "Server.h"
#include "SingleInstance.h"
#include <string>

class CLogServer : public CServer
{
public:
	CLogServer();
	virtual ~CLogServer();

	LOG_LEVEL getLevelByString(std::string level);
	LOG_LEVEL getLevel();
protected:
	virtual void excuteOneTask(TaskType type, SESSION_ID session, SERVER_HANDLE source, CData *pData)override;
private:
	FILE			*m_fp_console;
	FILE			*m_fp_file;
	int				m_day;			//��ǰ����־��������ģ�ÿ��һ����־�ı�
	std::string		m_file;
	LOG_LEVEL		m_level;
};
#endif//__2015_04_10_LOG_SERVER_H__