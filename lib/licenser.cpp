#include "licenser.h"
#include "ini_oper.h"
#include "aes_ex.h"
#include "commdef.h"
#include "http_client.h"
#include "sl_2d_face_common.h"

#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>

#if defined(__GNUC__)
#include <sys/stat.h>
#else
#include <direct.h>
#endif
using namespace std;

namespace tutils {

Licenser::Licenser()
	: m_iFlag(0)
{
	m_strLicensePath = getLicensePath();
	std::thread thread_ = std::thread(&Licenser::cache, this);
	thread_.detach();
}

Licenser::~Licenser()
{}

void Licenser::cache()
{
	while(1)
	{
		this_thread::sleep_for(chrono::seconds(5*60));
		m_iFlag = 0;
	}
}

string Licenser::getLicensePath()
{
	const char* licensePath = getenv("SL_LICENSE_PATH");
	if(!licensePath || string(licensePath).empty())
	{
		return "license";
	}
	return string(licensePath) + "/license";
}

int Licenser::verify(const string& fn)
{
	if(m_iFlag == 1) return 0;
    int ret = 0;
    std::ifstream ifs(m_strLicensePath+"/license");
    if (!ifs.is_open())
    {
        IniOper io(fn);
        string serial = io.get("license", "serial");
        string ip = io.get("license", "addr");
        if(serial.empty() || ip.empty()) return SL_E_LICENSE_CFG_ERR;
        if((ret = createLicense(serial, ip)) != 0) return ret;
        else ifs.open(m_strLicensePath+"/license");
    }
 
 	if(!ifs.is_open()) return SL_E_LICENSE_OS_ERR;

    std::string fc;
    ifs >> fc;
    ifs.close();

    std::string dfc = AESDecrypt(fc);
    std::vector<std::string> vStr = splitStr(dfc);
    if (vStr.size() != 2)
    {   
        return SL_E_LICENSE_CONTENT_ERR;
    }   

    if (getCpuId() != vStr[0])
    {   
        return SL_E_LICENSE_NOT_SUIT;
    }   
    if(vStr[1] != "0")
    {   
        int now = getTime();
        if (now > stoi(vStr[1]))
        {
            return SL_E_LICENSE_EXPIRED;
        }
    }
	m_iFlag = 1;
	return 0;
}

int Licenser::createLicense(const string& serial, const string& ip)
{
    std::string cpuid = getCpuId();
    if (cpuid.empty())
    {   
        return SL_E_LICENSE_OS_ERR;
    }
    
    string resp;
    HttpClient hc(serial, cpuid, ip);
    int ret = hc.doGet(resp); 
    if(ret != 0)
    {   
        return SL_E_LICENSE_NETWORK_ERR;
    }
    string license;
    ret = respParser(resp, license);
    if(ret != 0 && ret != 3)
    {   
        return SL_E_LICENSE_INVAILD_SERIAL;
    }
    if(license.empty())
    {   
        return SL_E_LICENSE_INVAILD_SERIAL;
    }
    
	#if defined(__GNUC__)
    ret = mkdir(m_strLicensePath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    #else
    ret = _mkdir(m_strLicensePath.c_str());
    #endif
	if(ret != 0) return SL_E_LICENSE_OS_ERR;
    
    fstream ofs;
    ofs.open(m_strLicensePath+"/license", ios::out);
    
    ofs << license;
    ofs.close();
    return SL_OK;
}

int Licenser::decrypt(const char* en, char* de, int& len)
{
	string s = tutils::AESDecrypt(en);
	len = s.length() + 1;
	de = (char*)malloc(len);
	if(!de) return SL_E_DECRYPT_OS_ERR;
	memset(de, 0, len);
	memcpy(de, s.c_str(), len);
	unique_lock<mutex> lock(m_mux);
	m_vPtrs.push_back(de);
	return SL_OK;
}

int Licenser::release()
{
	unique_lock<mutex> lock(m_mux);
	for(auto&& p : m_vPtrs) free(p);
	m_vPtrs.clear();
	return SL_OK;
}

}
