/********************************************************************************
*	�ļ�����:	ConfigFile.h													*
*	����ʱ�䣺	2015/04/15														*
*	��   �� :	xzben															*
*	�ļ�����:	ϵͳ�����ļ���ȡʵ��												*
*********************************************************************************/
#ifndef __2015_04_15_CONFIG_FILE_H__
#define __2015_04_15_CONFIG_FILE_H__

#include "SingleInstance.h"
#include <string>
#include <map>

class CConfigFile
{
public:
	static CConfigFile* getInstance()
	{
		static CConfigFile s_instance;
		return &s_instance;
	}

	bool init(const char* file);
	void dumpConfigs();
	int		getInt(const char *key, int default);
	double	getDouble(const char *key, double default);
	std::string  getString(const char* key, std::string default);
protected:
	CConfigFile();
	~CConfigFile();

	
	
	int checkValue(char *line);
	bool checkKeyValid(char* key);
	bool checkValueValid(char	*& value);
private:
	std::string m_file;
	int			m_lineCount;
	typedef std::map<std::string, std::string> ValueContainer;
	ValueContainer m_values;
};
#endif//__2015_04_15_CONFIG_FILE_H__