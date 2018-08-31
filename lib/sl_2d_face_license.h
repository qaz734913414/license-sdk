#ifndef SL_2D_FACE_LICENSE_H__
#define SL_2D_FACE_LICENSE_H__
#include "sl_2d_face_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * license 认证接口
 * 参数：serial： 序列号，用来首次认证时申请license
 *       addr： license服务的地址，首次认证时使用。形式为域名或ip:port，暂时传入"114.116.39.66:8898"
 * 返回值：认证成功返回0，其他为失败
 */
SlErrorCode Sl2DFaceLisence(const char* cfg_file);

/*
 * 解密接口
 * 参数：en：加密且base64编码的字符串，由AES加密工具生成，工具见sdk包
 * 		 de: 解密后的字符串
 * 		 len：输出参数，存储解密后的字符串长度
 * 返回值：0成功，其他失败
 */
SlErrorCode Sl2DFaceDecrypt(const char* en, char* de,int& len);

/*
 * 资源释放接口
 */
SlErrorCode Sl2DFaceDecryptRelease();

#ifdef __cplusplus
}
#endif

#endif
