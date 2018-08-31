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

SlErrorCode Sl2DFaceLisence(const char* cfg_file)
{
	return tutils::Licenser::getInstance()->verify(cfg_file);
}

SlErrorCode Sl2DFaceDecrypt(const char* en, char* de, int& len)
{
	return tutils::Licenser::getInstance()->decrypt(en, de, len);
}

SlErrorCode Sl2DFaceDecryptRelease()
{
	return tutils::Licenser::getInstance()->release();
}
