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
	, m_iExpireTs(-1)
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
	if(m_iFlag == 1 || m_iExpireTs == 0) return 0;
	if(m_iExpireTs > 0)
	{
		int now = getTime();
		if(now > m_iExpireTs)
		{
			return SL_E_LICENSE_EXPIRED;
		}
		return 0;
	}
    int ret = 0;
    std::ifstream ifs(fn+"/license/license");
    if (!ifs.is_open())
    {
        IniOper io(fn+"/license_cfg.ini");
        string serial = io.get("license", "serial");
        string ip = io.get("license", "addr");
        if(serial.empty() || ip.empty()) return SL_E_LICENSE_CFG_ERR;
        if((ret = createLicense(fn, serial, ip)) != 0) return ret;
        else ifs.open(fn+"/license/license");
    }
 
 	if(!ifs.is_open()) return SL_E_LICENSE_OS_ERR;

    std::string fc;
    ifs >> fc;
    ifs.close();

	std::string dfc = deconfusion(fc);
	cout << fc <<endl;
	cout << dfc << endl;
    std::string ddfc = AESDecrypt(dfc);
    std::vector<std::string> vStr = splitStr(ddfc);
    if (vStr.size() != 3)
    {   
        return SL_E_LICENSE_CONTENT_ERR;
    }   

    if (getCpuId() != vStr[0])
    {   
        return SL_E_LICENSE_NOT_SUIT;
    }
	if (!vStr[1].empty())
	{
		vector<string> vMac;
		int r = getMac(vMac);
		if(r != 0) return SL_E_LICENSE_OS_ERR;
		int flag = 0;
		for(auto && mac : vMac)
		{
			if(mac == vStr[1])
			{
				flag = 1;
				break;
			}
		}
		if(flag != 1) return SL_E_LICENSE_NOT_SUIT;
	}
    if(vStr[2] != "0")
    {   
        int now = getTime();
        if (now > stoi(vStr[2]))
        {
            return SL_E_LICENSE_EXPIRED;
        }
    }
	m_iFlag = 1;
	m_iExpireTs = stoi(vStr[2]);
	return 0;
}

int Licenser::createLicense(const string& path, const string& serial, const string& ip)
{
    std::string cpuid = getCpuId();
	std::vector<std::string> vMac;
	int ret = getMac(vMac);
	if(ret != 0) return SL_E_LICENSE_OS_ERR;
    if (cpuid.empty())
    {   
        return SL_E_LICENSE_OS_ERR;
    }
	string hardwareid = cpuid + "|";
	if(!vMac.empty()) hardwareid += vMac[0];
    
    string resp;
    HttpClient hc(serial, hardwareid, ip);
    ret = hc.doGet(resp); 
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
    
	string pf = path + "/license";
	#if defined(__GNUC__)
    ret = mkdir(pf.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    #else
    ret = _mkdir(pf.c_str());
    #endif
	if(ret != 0) return SL_E_LICENSE_OS_ERR;
    
    fstream ofs;
    ofs.open(path+"/license/license", ios::out);
    
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
