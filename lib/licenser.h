#ifndef LICENSER_H__
#define LICENSER_H__
#include "singleton.h"
#include <string>
#include <vector>
#include <mutex>

namespace tutils {

class Licenser : public Singleton<Licenser>
{
public:
	Licenser();
	~Licenser();

	int verify(const std::string&);
	int decrypt(const char*, char*, int&);
	int release();
private:
	std::string getLicensePath();
	int createLicense(const std::string&, const std::string&);
	void cache();
private:
	int m_iFlag;
	std::string m_strLicensePath;
	std::vector<char*> m_vPtrs;
	std::mutex m_mux;
};
}
#endif
