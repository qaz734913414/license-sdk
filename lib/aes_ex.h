#ifndef AEX_EX_H__
#define AEX_EX_H__

#include <string>
#include <iostream>

#include "aes.h"
#include "base64.h"

namespace tutils {

inline std::string AESDecryptRaw(const std::string& s, const std::string& key = "0l0w5B4JQXVJDLt7JGU0Le3nbV0hH4te")
{
	AES aes(256);

	unsigned char iv[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	std::string temp = s;
	std::string ret;
	unsigned int len;

	while (!temp.empty())
	{
		std::string t = temp.substr(0, 16);
		if (temp.length() >= 16) temp = temp.substr(16);
		else temp = "";

		unsigned char *out = aes.DecryptCBC((unsigned char*)t.c_str(), 16, (unsigned char*)key.c_str(), iv, len);
		ret += std::string((char*)out, len);
		delete[] out;
	}

	while (ret[ret.length() - 1] == '\0')
	{
		ret = ret.substr(0, ret.length() - 1);
	}
	return (ret);
}

inline std::string AESDecrypt(const std::string& s, const std::string& key = "0l0w5B4JQXVJDLt7JGU0Le3nbV0hH4te")
{
	if(s.empty()) return "";

	AES aes(256);

	unsigned char iv[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	std::string temp;
	Base64::Decode(s, &temp);
	std::string ret;
	unsigned int len;

	while (!temp.empty())
	{
		std::string t = temp.substr(0, 16);
		if (temp.length() >= 16) temp = temp.substr(16);
		else temp = "";
		
		unsigned char *out = aes.DecryptCBC((unsigned char*)t.c_str(), 16, (unsigned char*)key.c_str(), iv, len);
		ret += std::string((char*)out, len);
		delete[] out;
	}

	while (ret[ret.length() - 1] == '\0')
	{
		ret = ret.substr(0, ret.length() - 1);
	}
	return (ret);
}


inline std::string AESEncryptRaw(const std::string& s, const std::string& key = "0l0w5B4JQXVJDLt7JGU0Le3nbV0hH4te")
{
	AES aes(256);

	unsigned char iv[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	std::string temp = s;
	std::string ret;
	unsigned int len;
	while (!temp.empty())
	{
		std::string t = temp.substr(0, 16);
		if (temp.length() >= 16) temp = temp.substr(16);
		else temp = "";
		while (t.length() != 16)
		{
			t += '\0';
		}
		unsigned char *out = aes.EncryptCBC((unsigned char*)t.c_str(), 16, (unsigned char*)key.c_str(), iv, len);
		ret += std::string((char*)out, len);
		delete[] out;
	}
	return ret;
}


inline std::string AESEncrypt(const std::string& s, const std::string& key = "0l0w5B4JQXVJDLt7JGU0Le3nbV0hH4te")
{
	std::string raw = AESEncryptRaw(s, key);
	std::string ret;
	Base64::Encode(raw, &ret);
	return ret;
}

}
#endif
