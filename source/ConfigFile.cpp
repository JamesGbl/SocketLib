#include "ConfigFile.h"
#include <cstdio>

#define MAX_LINE_LEN	4096

static inline bool is_space(char a)
{
	if (a == ' ' || a == '\t' || a == '\n' || a == '\r' || a == '\v' || a == '\f')
		return true;

	return false;
}

CConfigFile::CConfigFile()
{

}

CConfigFile::~CConfigFile()
{

}

bool CConfigFile::init(const char* file)
{
	m_file = file;
	
	FILE *fp = fopen(m_file.c_str(), "r");
	if (NULL == fp)
	{
		printf("can't open config file [%s].\n", m_file.c_str());
		return false;
	}

	char line[MAX_LINE_LEN];
	char *value = NULL;
	m_lineCount = 0;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		m_lineCount++;
		if (checkValue(line) != 0)
		{
			return false;
		}
	}
	return true;
}

int CConfigFile::checkValue(char *line)
{
	char *k_start, *k_end;  // ָʾ key �� line �е���ʼ�ͽ���λ��
	char *v_start, *v_end;  // ֻ�� value �� line �е���ʼ�ͽ���λ��

	int line_len = strlen(line);

	k_start = &line[0];
	v_end = &line[line_len - 1];

	//ȥ����ͷ�Ŀհ׷���
	while (is_space(*k_start) && (k_start <= v_end))
	{
		k_start++;
	}

	//����� # �ſ�ͷ�����ע��
	if (*k_start == '#' || *k_start == '\0')
	{
		return (0); //��������
	}

	v_start = k_end = strchr(line, '=');
	//�Ҳ����Ⱥ�˵�����в���һ����Ч��
	if (k_end == NULL)
	{
		printf("���ñ���� [%s | %d ]  ����һ����Ч�� [%s]\n", m_file.c_str(), m_lineCount, line);
		return(-1);
	}

	k_end--;
	while (is_space(*k_end) && (k_end >= k_start))
	{
		k_end--;
	}
	if (is_space(*k_end))
	{
		printf("���ñ���� [%s | %d ]  ����һ����Ч�� [%s]\n", m_file.c_str(), m_lineCount, line);
		return(-1);
	}

	v_start++;
	while (is_space(*v_start) && (v_start <= v_end))
	{
		v_start++;
	}
	if (is_space(*v_start))
	{
		printf("���ñ���� [%s | %d ]  ����һ����Ч�� [%s]\n", m_file.c_str(), m_lineCount, line);
		return(-1);
	}

	while (is_space(*v_end) && (v_end >= v_start))
	{
		v_end--;
	}
	if (is_space(*v_end))
	{
		printf("���ñ���� [%s | %d ]  ����һ����Ч�� [%s]\n", m_file.c_str(), m_lineCount, line);
		return(-1);
	}

	k_end++;
	v_end++;
	*k_end = *v_end = '\0';
	char* key = k_start;
	char* value = v_start;
	if (!checkKeyValid(key))
		return -1;
	if (!checkValueValid(value))
		return -1;

	m_values.insert(ValueContainer::value_type(key, value));
	return(0);
}

bool CConfigFile::checkKeyValid(char* key)
{
	if (key == NULL || !(isalpha(key[0]) || key[0] == '_') )
	{
		printf("���ñ���� [%s | %d ] ��������������ĸ���»��߿�ͷ [%s]\n", m_file.c_str(), m_lineCount, key == nullptr ? "" : key);
		return false;
	}

	while (*key != '\0' )
	{
		//Ϊ���ַ� ���߲�����ĸ������
		if (is_space(*key) || (!isalpha(*key) && !isdigit(*key)))
		{
			printf("���ñ���� [%s | %d ] ����������һ����Ч�ı��� [%s]\n", m_file.c_str(), m_lineCount, key == nullptr ? "" : key);
			return false;
		}
		key++;
	}

	return true;
}

bool CConfigFile::checkValueValid(char *&value)
{
	if (value == NULL)
	{
		printf("���ñ���� [%s | %d ] ����ֵ��һ����ֵ\n", m_file.c_str(), m_lineCount);
		return false;
	}
	int		index = 0;
	bool	isString = false;
	bool	isNum = false;
	char	Quotat = 0;

	if (value[0] == '\"' || value[0] == '\'')
	{
		Quotat = value[index];
		isString = true;
		index++;
	}

	bool isStringDone = false;
	int dotNum = 0;

	while (value[index] != '\0')
	{
		if (!isString && !isdigit(value[index]))
		{
			if ( (value[index] == '-' || value[index] == '+') && index == 0)
			{
				index++;
				continue;
			}
			
			if (value[index] == '.' && dotNum < 1)
			{
				dotNum++;
				index++;
				continue;
			}

			printf("���ñ���� [%s | %d ] ����ֵֻ�������ֻ����ַ��� [%s]\n", m_file.c_str(), m_lineCount, value);
			return false;
		}
		else if (value[index] == '\'' || value[index] == '\"')
		{
			if (value[index] != Quotat)
			{
				printf("���ñ���� [%s | %d ] ����ֵ����һ����Ч���ַ��� [%s]\n", m_file.c_str(), m_lineCount, value);
				return false;
			}
			else
			{
				value[index] = '\0';
				value = &value[1];
				isStringDone = true;
				continue;
			}
		}
		
		if (value[index] == '#')
		{
			value[index] = '\0';
			break;
		}

		if (isStringDone && !is_space(value[index]))
		{
			printf("���ñ���� [%s | %d ] ����ֵ����һ����Ч���ַ��� [%s]\n", m_file.c_str(), m_lineCount, value);
			return false;
		}

		index++;
	}
	
	return true;
}

void CConfigFile::dumpConfigs()
{
	auto it = m_values.begin();
	while (it != m_values.end())
	{
		printf("%s = %s\n", it->first.c_str(), it->second.c_str());
		it++;
	}
}

int	 CConfigFile::getInt(const char *key, int default)
{
	auto it = m_values.find(key);
	if (it != m_values.end())
	{
		return atoi(it->second.c_str());
	}

	return default;
}

double	CConfigFile::getDouble(const char *key, double default)
{
	auto it = m_values.find(key);
	if (it != m_values.end())
	{
		return atof(it->second.c_str());
	}

	return default;
}
std::string  CConfigFile::getString(const char* key, std::string default)
{
	auto it = m_values.find(key);
	if (it != m_values.end())
	{
		return it->second;
	}

	return default;
}