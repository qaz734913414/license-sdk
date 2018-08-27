#include "ini_oper.h"
#include <fstream>
#include <iostream>
using namespace std;

namespace tutils {

IniOper::IniOper(const string& fn)
	: m_strFileName(fn)
{
	init();
}

void IniOper::init()
{
    load();
}

void IniOper::load()
{
    m_mKv.clear();

    ifstream ifs(m_strFileName);
    if(!ifs.is_open())
    {
        return;
    }

    string line;
	string section;
    while(getline(ifs, line))
    {
        m_vLines.push_back(line);
        line = trim(line);
        if(line.empty() || line[0] == '#')
        {
            continue;
        }
        if(line.find('[') == 0 && line.find(']') == line.length()-1)
        {
			section = line.substr(1, line.length()-2);
			continue;
        }
        size_t pos = line.find('=');
        if(pos != string::npos)
        {
            string key = trim(line.substr(0, pos));
            string val = trim(line.substr(pos+1));
            m_mKv[section][key] = val;
        }
    }
}

string IniOper::get(const string& section, const string& key)
{
	if(m_mKv.find(section) == m_mKv.end() || m_mKv[section].find(key) == m_mKv[section].end())
		return "";
    return m_mKv[section][key];
}

std::string IniOper::trim(const std::string& in) 
{   
	std::string out;
	for(size_t i=0; i<in.size(); ++i)
	{   
		if(in[i] == ' ' || in[i] == '\t' || in[i] == '\r' || in[i] == '\n')
		{
			continue;
		}
		out.append(1, in[i]);
	}
	return out;
}
}
