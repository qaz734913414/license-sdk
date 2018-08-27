#ifndef INIOPER_H
#define INIOPER_H

#include <string>
#include <map>
#include <vector>

namespace tutils {
class IniOper
{
public:
	IniOper(const std::string&);
    void init();
    void load();

    std::string get(const std::string&, const std::string&);
private:
	std::string trim(const std::string&);
private:
    std::string m_strFileName;
    std::vector<std::string> m_vLines;
    std::map<std::string, std::map<std::string, std::string> > m_mKv;
};
}
#endif // INIOPER_H
