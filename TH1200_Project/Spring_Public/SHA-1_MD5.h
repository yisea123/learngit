/*
 * SHA-1_MD5.h
 *
 *  Created on: 2018��3��26��
 *      Author: yzg
 */

#ifndef SHA_1_MD5_H_
#define SHA_1_MD5_H_

#include	<stddef.h>
///MD5�Ľ�����ݳ���
#define ZEN_MD5_HASH_SIZE   (16)
///SHA1�Ľ�����ݳ���
#define ZEN_SHA1_HASH_SIZE  (20)


//����һ���ڴ����ݵ�MD5ֵ
unsigned char *md5(const unsigned char *buf,
                         size_t size,
                         unsigned char result[ZEN_MD5_HASH_SIZE]);

//����һ���ڴ����ݵ�SHA1ֵ
unsigned char *sha1(const unsigned char *msg,
                          size_t size,
                          unsigned char result[ZEN_SHA1_HASH_SIZE]);



#endif /* SHA_1_MD5_H_ */
