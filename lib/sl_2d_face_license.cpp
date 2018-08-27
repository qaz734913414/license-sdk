#include "sl_2d_face_license.h"
#include "licenser.h"
#include "ini_oper.h"
#include "aes_ex.h"
#include "commdef.h"
#include "http_client.h"

#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

#if defined(__GNUC__)
#include <sys/stat.h>
#else
#include <direct.h>
#endif

SlErrorCode Sl2DFaceLisence()
{
	return tutils::Licenser::getInstance()->verify();
}

SlErrorCode Sl2DFaceDecrypt(const char* en, char* de, int& len)
{
	string s = tutils::AESDecrypt(en);
	len = s.length() + 1;
	de = (char*)malloc(len);
	if(!de) return SL_E_DECRYPT_OS_ERR;
	memset(de, 0, len);
	memcpy(de, s.c_str(), len);
	return SL_OK;
}
