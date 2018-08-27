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

#if defined(__GNUC__)
#include <sys/stat.h>
#else
#include <direct.h>
#endif
using namespace std;

namespace tutils {

Licenser::Licenser()
	: m_iFlag(0)
	, m_strCfgFile("license_cfg.ini")
{
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

int Licenser::verify()
{
	if(m_iFlag == 1) return 0;
    int ret = 0;
    std::ifstream ifs("license/license");
    if (!ifs.is_open())
    {
        IniOper io(m_strCfgFile);
        string serial = io.get("license", "serial");
        string ip = io.get("license", "addr");
        if(serial.empty() || ip.empty()) return SL_E_LICENSE_CFG_ERR;
        if((ret = createLicense(serial, ip)) != 0) return ret;
        else ifs.open("license/license");
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
    ret = mkdir("license", S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    #else
    ret = _mkdir("license");
    #endif
	if(ret != 0) return 104;
    
    fstream ofs;
    ofs.open("license/license", ios::out);
    
    ofs << license;
    ofs.close();
    return SL_OK;
}

}
