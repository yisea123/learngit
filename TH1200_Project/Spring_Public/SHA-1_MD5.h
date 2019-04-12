/*
 * SHA-1_MD5.h
 *
 *  Created on: 2018年3月26日
 *      Author: yzg
 */

#ifndef SHA_1_MD5_H_
#define SHA_1_MD5_H_

#include	<stddef.h>
///MD5的结果数据长度
#define ZEN_MD5_HASH_SIZE   (16)
///SHA1的结果数据长度
#define ZEN_SHA1_HASH_SIZE  (20)


//计算一个内存数据的MD5值
unsigned char *md5(const unsigned char *buf,
                         size_t size,
                         unsigned char result[ZEN_MD5_HASH_SIZE]);

//计算一个内存数据的SHA1值
unsigned char *sha1(const unsigned char *msg,
                          size_t size,
                          unsigned char result[ZEN_SHA1_HASH_SIZE]);



#endif /* SHA_1_MD5_H_ */
