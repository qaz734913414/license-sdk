#ifndef LICENSER_H__
#define LICENSER_H__
#include "singleton.h"
#include <string>

namespace tutils {

class Licenser : public Singleton<Licenser>
{
public:
	Licenser();
	~Licenser();

	int verify();
private:
	int createLicense(const std::string&, const std::string&);
	void cache();
private:
	int m_iFlag;
	std::string m_strCfgFile;
};
}
#endif
