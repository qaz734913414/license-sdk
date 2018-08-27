#ifndef COMMDEF_H__
#define COMMDEF_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <ctime>

#include "jparser.h"

namespace tutils {

inline int getTime()
{
	time_t now_time;
	now_time = time(NULL);
	return now_time;
}

inline std::vector<std::string> splitStr(const std::string& s)
{
	std::string temp = s;
	std::vector<std::string> vStr;
	size_t pos = s.find("|");
	while (pos != std::string::npos)
	{
		std::string t = temp.substr(0, pos);
		temp = temp.substr(pos + 1);
		vStr.push_back(t);
		pos = temp.find("|");
	}
	vStr.push_back(temp);
	return vStr;
}


// intrinsics
#if defined(__GNUC__)    // GCC
#include <cpuid.h>
#elif defined(_MSC_VER)    // MSVC
#if _MSC_VER >=1400    // VC2005
#include <intrin.h>
#endif    // #if _MSC_VER >=1400
#else
#error Only supports MSVC or GCC.
#endif    // #if defined(__GNUC__)

inline void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)    // GCC
	__cpuid_count(InfoType, ECXValue, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
#if defined(_WIN64) || _MSC_VER>=1600    // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
	__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else
	if (NULL == CPUInfo)    return;
	_asm {
		// load. 读取参数到寄存器.
		mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID
		cpuid;
		// save. 将寄存器保存到CPUInfo
		mov[edi], eax;
		mov[edi + 4], ebx;
		mov[edi + 8], ecx;
		mov[edi + 12], edx;
	}
#endif
#endif    // #if defined(__GNUC__)
}

inline std::string getCpuId()
{
	int32_t deBuf[4];
	getcpuidex((unsigned int*)deBuf, 01,0);
	//__cpuidex(deBuf, 01, 0);
	char buff[64] = { 0 };
	sprintf(buff, "%.8x%.8x", deBuf[3], deBuf[0]);
	return buff;
}

inline int respParser(const std::string& resp, std::string& r)
{
    JValue val;
    JParserRet ret = JParseDecode(resp, &val);
    if(ret != JPARSER_OK)
    {   
        return -1; 
    }
	int status = -1;
    if(val.getType() == JValue::JVT_OBJECT)
    {
        map<string, JValue*> ji = val.getObject();
        for(map<string, JValue*>::const_iterator it=ji.begin(); it!=ji.end(); ++it)
        {
            if(it->first == "status")
            {
                status = it->second->getNum();
            }
			else if(it->first == "license")
			{
				r = it->second->getStr();
			}
        }
    }  
    return status; 
}
}
#endif
