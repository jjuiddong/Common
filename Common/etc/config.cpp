
#include "stdafx.h"
#include "config.h"
#include <set>

using namespace common;
using namespace std;


cConfig::cConfig()
{
}

cConfig::~cConfig()
{
}


bool cConfig::Read(const string &fileName)
{
	if (!Parse(fileName))
	{
		InitDefault();
		return false;
	}
	return true;
}


// 환경설정 파일을 읽어서 저장한다.
bool cConfig::Parse(const string &fileName)
{
	std::ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		printf("not exist <%s> config file!!\n", fileName.c_str());
		return false;
	}

	string id, eq, val;
	while (cfgfile >> id)// >> eq >> val)
	{
		val.clear();

		if (id[0] == '#')
		{
			string line;
			getline(cfgfile, line);
			continue;  // skip comments
		}
		else if (id == "plot_command")
		{
			cfgfile >> eq;

			std::string strCmd((std::istreambuf_iterator<char>(cfgfile)), std::istreambuf_iterator<char>());
			common::trim(strCmd);
			m_options[id] = strCmd;
		}
		else
		{
			cfgfile >> eq >> val;
			if (eq != "=") throw std::runtime_error("Parse error");
			m_options[id] = val;
		}
	}

	UpdateParseData();

	return true;
}


// 입력으로 들어온 스트링을 분석한다.
void cConfig::ParseStr(const string &str)
{
	using namespace std;

	stringstream ss(str);

	string id, eq, val;
	while (ss >> id)// >> eq >> val)
	{
		val.clear();

		if (id[0] == '#')
		{
			string line;
			getline(ss, line);
			continue;  // skip comments
		}
		else if (id == "plot_command")
		{
			ss >> eq;

			std::string strCmd((std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());
 			common::trim(strCmd);
			m_options[id] = strCmd; 		}
		else
		{
			ss >> eq;
			if (eq == "=")
			{
				ss >> val;
			}
			else
			{
				//ss >> val;
				const int pos = (int)eq.find('=');
				if (string::npos == pos)
				{
					dbg::ErrLog("cConfig::Parse Error not equal '=' found \n");
					assert(0);
				}
				else
				{
					val = eq.substr(pos+1);
				}
			}
			
			m_options[id] = val;
		}
	}

	UpdateParseData();
}


bool cConfig::GetBool(const string &key, const bool defaultValue)
{
	auto it = m_options.find(key);
	if (m_options.end() == it)
	{
		cout << "not found id = " << key.c_str() << endl;
		return defaultValue;
	}

	return (atoi(m_options[key].c_str()) > 0) ? true : false;
}


float cConfig::GetFloat(const string &key, const float defaultValue)
{
	auto it = m_options.find(key);
	if (m_options.end() == it)
	{
		cout << "not found id = " << key.c_str() << endl;
		return defaultValue;
	}

	return (float)atof(m_options[key].c_str());
}


double cConfig::GetDouble(const string &key, const double defaultValue)
{
	auto it = m_options.find(key);
	if (m_options.end() == it)
	{
		cout << "not found id = " << key.c_str() << endl;
		return defaultValue;
	}

	return atof(m_options[key].c_str());
}


int cConfig::GetInt(const string &key, const int defaultValue)
{
	auto it = m_options.find(key);
	if (m_options.end() == it)
	{
		cout << "not found id = " << key.c_str() << endl;
		return defaultValue;
	}

	return atoi(m_options[key].c_str());
}


string cConfig::GetString(const string &key, const string &defaultValue) //defaultValue=""
{
	auto it = m_options.find(key);
	if (m_options.end() == it)
	{
		cout << "not found id = " << key.c_str() << endl;
		return defaultValue;
	}

	return it->second;
}


// 동일한 이름에 파일이 있다면, 그 파일 형태를 그대로 유지하면서,
// 데이타만 업데이트 되는 형태로 저장하게 한다.
bool cConfig::Write(const string &fileName)
{
 	using namespace std;

	ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		// 동일한 파일이 없다면, 그대로 저장하고 종료한다.
		ofstream ofs(fileName);
		if (!ofs.is_open())
			return false;

		for each (auto &kv in m_options)
		{
			if (kv.second.empty())
				ofs << kv.first << " = " << "0" << endl;
			else
				ofs << kv.first << " = " << kv.second << endl;
		}		
		return true;
	}


	set<string> checkId; //저장 된 정보와 그렇지 않은 정보를 찾기위해 필요함.

	stringstream ss;
	ss << cfgfile.rdbuf();

	stringstream savess;
	char line[128];
	while (ss.getline(line, sizeof(line)))
	{
		stringstream ssline(line);

		string id, eq, val;
		ssline >> id;

		if (id[0] == '#') // 주석
		{
			savess << line << endl;
			continue;
		}
		if (id.empty()) // 개행 문자
		{
			savess << line << endl;
			continue;
		}

		ssline >> eq >> val;
		if (eq != "=") throw std::runtime_error("Parse error");

		if (m_options[id].empty())
			savess << id << " = " << 0 << endl;
		else
			savess << id << " = " << m_options[id] << endl;

		checkId.insert(id);
	}

	// Write되지 않은 Id가 있다면, 검색해서, 저장한다.
	for each (auto &kv in m_options)
	{
		const auto it = checkId.find(kv.first);
		if (checkId.end() == it)
		{
			if (kv.second.empty())
				savess << kv.first << " = " << 0 << endl;
			else
				savess << kv.first << " = " << kv.second << endl;
		}
	}


	ofstream ofs(fileName);
	if (!ofs.is_open())
		return false;
	ofs << savess.rdbuf();

	return true;
}
