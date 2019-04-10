/*
 * SHA-1_MD5.c
 *
 *  Created on: 2018��3��26��
 *      Author: yzg
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include "adt_datatype.h"
#include "SHA-1_MD5.h"

//�ֽ����Сͷ�ʹ�ͷ������
#define ZEN_LITTLE_ENDIAN  0x0123
#define ZEN_BIG_ENDIAN     0x3210

//Ŀǰ���еĴ��붼��Ϊ��Сͷ�������
#ifndef ZEN_BYTES_ORDER
#define ZEN_BYTES_ORDER    ZEN_LITTLE_ENDIAN
#endif

#ifndef ZEN_SWAP_UINT16
#define ZEN_SWAP_UINT16(x)  ((((x) & 0xff00) >>  8) | (((x) & 0x00ff) <<  8))
#endif
#ifndef ZEN_SWAP_UINT32
#define ZEN_SWAP_UINT32(x)  ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
							 (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#endif
#ifndef ZEN_SWAP_UINT64
#define ZEN_SWAP_UINT64(x)  ((((x) & 0xff00000000000000) >> 56) | (((x) & 0x00ff000000000000) >>  40) | \
							 (((x) & 0x0000ff0000000000) >> 24) | (((x) & 0x000000ff00000000) >>  8) | \
							 (((x) & 0x00000000ff000000) << 8 ) | (((x) & 0x0000000000ff0000) <<  24) | \
							 (((x) & 0x000000000000ff00) << 40 ) | (((x) & 0x00000000000000ff) <<  56))
#endif

//��һ�����ַ��������飬����������һ��uint32_t���飬ͬʱÿ��uint32_t���ֽ���
void *swap_uint32_memcpy(void *to, const void *from, size_t length)
{
 memcpy(to, from, length);
 size_t remain_len =  (4 - (length & 3)) & 3;
 size_t i = 0;
 //���ݲ���4�ֽڵı���,����0
 if (remain_len)
 {
     for (i = 0; i < remain_len; ++i)
     {
         *((char *)(to) + length + i) = 0;
     }
     //������4�ı���
     length += remain_len;
 }

 //���е����ݷ�ת
 for (i = 0; i < length / 4; ++i)
 {
     ((uint32_t *)to)[i] = ZEN_SWAP_UINT32(((uint32_t *)to)[i]);
 }

 return to;
}

///MD5�Ľ�����ݳ���
//static const size_t ZEN_MD5_HASH_SIZE   = 16;
///SHA1�Ľ�����ݳ���
//static const size_t ZEN_SHA1_HASH_SIZE  = 20;


/*!
@brief      ��ĳ���ڴ���MD5��
@return     unsigned char* ���صĵĽ����
@param[in]  buf    ��MD5���ڴ�BUFFERָ��
@param[in]  size   BUFFER����
@param[out] result ���
*/
unsigned char *md5(const unsigned char *buf,
                size_t size,
                unsigned char result[ZEN_MD5_HASH_SIZE]);


/*!
@brief      ���ڴ��BUFFER��SHA1ֵ
@return     unsigned char* ���صĵĽ��
@param[in]  buf    ��SHA1���ڴ�BUFFERָ��
@param[in]  size   BUFFER����
@param[out] result ���
*/
unsigned char *sha1(const unsigned char *buf,
                 size_t size,
                 unsigned char result[ZEN_SHA1_HASH_SIZE]);


//================================================================================================
//MD5���㷨

//ÿ�δ����BLOCK�Ĵ�С
static const size_t ZEN_MD5_BLOCK_SIZE = 64;

//md5�㷨�������ģ�����һЩ״̬���м����ݣ����
typedef struct md5_ctx
{
 //��������ݵĳ���
 uint64_t length_;
 //��û�д�������ݳ���
 uint64_t unprocessed_;
 //ȡ�õ�HASH������м����ݣ�
 uint32_t  hash_[4];
} md5_ctx;


#define ROTL32(dword, n) ((dword) << (n) ^ ((dword) >> (32 - (n))))
#define ROTR32(dword, n) ((dword) >> (n) ^ ((dword) << (32 - (n))))
#define ROTL64(qword, n) ((qword) << (n) ^ ((qword) >> (64 - (n))))
#define ROTR64(qword, n) ((qword) >> (n) ^ ((qword) << (64 - (n))))


/*!
@brief      �ڲ���������ʼ��MD5��context������
@param      ctx
*/
static void zen_md5_init(md5_ctx *ctx)
{
 ctx->length_ = 0;
 ctx->unprocessed_ = 0;

 /* initialize state */
 ctx->hash_[0] = 0x67452301;
 ctx->hash_[1] = 0xefcdab89;
 ctx->hash_[2] = 0x98badcfe;
 ctx->hash_[3] = 0x10325476;
}

/* First, define four auxiliary functions that each take as input
* three 32-bit words and returns a 32-bit word.*/

/* F(x,y,z) = ((y XOR z) AND x) XOR z - is faster then original version */
#define MD5_F(x, y, z) ((((y) ^ (z)) & (x)) ^ (z))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

/* transformations for rounds 1, 2, 3, and 4. */
#define MD5_ROUND1(a, b, c, d, x, s, ac) { \
     (a) += MD5_F((b), (c), (d)) + (x) + (ac); \
     (a) = ROTL32((a), (s)); \
     (a) += (b); \
 }
#define MD5_ROUND2(a, b, c, d, x, s, ac) { \
     (a) += MD5_G((b), (c), (d)) + (x) + (ac); \
     (a) = ROTL32((a), (s)); \
     (a) += (b); \
 }
#define MD5_ROUND3(a, b, c, d, x, s, ac) { \
     (a) += MD5_H((b), (c), (d)) + (x) + (ac); \
     (a) = ROTL32((a), (s)); \
     (a) += (b); \
 }
#define MD5_ROUND4(a, b, c, d, x, s, ac) { \
     (a) += MD5_I((b), (c), (d)) + (x) + (ac); \
     (a) = ROTL32((a), (s)); \
     (a) += (b); \
 }


/*!
@brief      �ڲ���������64���ֽڣ�16��uint32_t���������ժҪ���Ӵգ���������������Լ�����Сͷ����
@param      state ��Ŵ����hash���ݽ��
@param      block Ҫ�����block��64���ֽڣ�16��uint32_t������
*/
static void zen_md5_process_block(uint32_t state[4], const uint32_t block[ZEN_MD5_BLOCK_SIZE / 4])
{
 register unsigned a, b, c, d;
 a = state[0];
 b = state[1];
 c = state[2];
 d = state[3];

 const uint32_t *x = NULL;

 //MD5�����������ݶ���Сͷ����.��ͷ��������Ҫ����
#if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
 x = block;
#else
 uint32_t swap_block[ZEN_MD5_BLOCK_SIZE / 4];
 swap_uint32_memcpy(swap_block, block, 64);
 x = swap_block;
#endif


 MD5_ROUND1(a, b, c, d, x[ 0],  7, 0xd76aa478);
 MD5_ROUND1(d, a, b, c, x[ 1], 12, 0xe8c7b756);
 MD5_ROUND1(c, d, a, b, x[ 2], 17, 0x242070db);
 MD5_ROUND1(b, c, d, a, x[ 3], 22, 0xc1bdceee);
 MD5_ROUND1(a, b, c, d, x[ 4],  7, 0xf57c0faf);
 MD5_ROUND1(d, a, b, c, x[ 5], 12, 0x4787c62a);
 MD5_ROUND1(c, d, a, b, x[ 6], 17, 0xa8304613);
 MD5_ROUND1(b, c, d, a, x[ 7], 22, 0xfd469501);
 MD5_ROUND1(a, b, c, d, x[ 8],  7, 0x698098d8);
 MD5_ROUND1(d, a, b, c, x[ 9], 12, 0x8b44f7af);
 MD5_ROUND1(c, d, a, b, x[10], 17, 0xffff5bb1);
 MD5_ROUND1(b, c, d, a, x[11], 22, 0x895cd7be);
 MD5_ROUND1(a, b, c, d, x[12],  7, 0x6b901122);
 MD5_ROUND1(d, a, b, c, x[13], 12, 0xfd987193);
 MD5_ROUND1(c, d, a, b, x[14], 17, 0xa679438e);
 MD5_ROUND1(b, c, d, a, x[15], 22, 0x49b40821);

 MD5_ROUND2(a, b, c, d, x[ 1],  5, 0xf61e2562);
 MD5_ROUND2(d, a, b, c, x[ 6],  9, 0xc040b340);
 MD5_ROUND2(c, d, a, b, x[11], 14, 0x265e5a51);
 MD5_ROUND2(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
 MD5_ROUND2(a, b, c, d, x[ 5],  5, 0xd62f105d);
 MD5_ROUND2(d, a, b, c, x[10],  9,  0x2441453);
 MD5_ROUND2(c, d, a, b, x[15], 14, 0xd8a1e681);
 MD5_ROUND2(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
 MD5_ROUND2(a, b, c, d, x[ 9],  5, 0x21e1cde6);
 MD5_ROUND2(d, a, b, c, x[14],  9, 0xc33707d6);
 MD5_ROUND2(c, d, a, b, x[ 3], 14, 0xf4d50d87);
 MD5_ROUND2(b, c, d, a, x[ 8], 20, 0x455a14ed);
 MD5_ROUND2(a, b, c, d, x[13],  5, 0xa9e3e905);
 MD5_ROUND2(d, a, b, c, x[ 2],  9, 0xfcefa3f8);
 MD5_ROUND2(c, d, a, b, x[ 7], 14, 0x676f02d9);
 MD5_ROUND2(b, c, d, a, x[12], 20, 0x8d2a4c8a);

 MD5_ROUND3(a, b, c, d, x[ 5],  4, 0xfffa3942);
 MD5_ROUND3(d, a, b, c, x[ 8], 11, 0x8771f681);
 MD5_ROUND3(c, d, a, b, x[11], 16, 0x6d9d6122);
 MD5_ROUND3(b, c, d, a, x[14], 23, 0xfde5380c);
 MD5_ROUND3(a, b, c, d, x[ 1],  4, 0xa4beea44);
 MD5_ROUND3(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
 MD5_ROUND3(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
 MD5_ROUND3(b, c, d, a, x[10], 23, 0xbebfbc70);
 MD5_ROUND3(a, b, c, d, x[13],  4, 0x289b7ec6);
 MD5_ROUND3(d, a, b, c, x[ 0], 11, 0xeaa127fa);
 MD5_ROUND3(c, d, a, b, x[ 3], 16, 0xd4ef3085);
 MD5_ROUND3(b, c, d, a, x[ 6], 23,  0x4881d05);
 MD5_ROUND3(a, b, c, d, x[ 9],  4, 0xd9d4d039);
 MD5_ROUND3(d, a, b, c, x[12], 11, 0xe6db99e5);
 MD5_ROUND3(c, d, a, b, x[15], 16, 0x1fa27cf8);
 MD5_ROUND3(b, c, d, a, x[ 2], 23, 0xc4ac5665);

 MD5_ROUND4(a, b, c, d, x[ 0],  6, 0xf4292244);
 MD5_ROUND4(d, a, b, c, x[ 7], 10, 0x432aff97);
 MD5_ROUND4(c, d, a, b, x[14], 15, 0xab9423a7);
 MD5_ROUND4(b, c, d, a, x[ 5], 21, 0xfc93a039);
 MD5_ROUND4(a, b, c, d, x[12],  6, 0x655b59c3);
 MD5_ROUND4(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
 MD5_ROUND4(c, d, a, b, x[10], 15, 0xffeff47d);
 MD5_ROUND4(b, c, d, a, x[ 1], 21, 0x85845dd1);
 MD5_ROUND4(a, b, c, d, x[ 8],  6, 0x6fa87e4f);
 MD5_ROUND4(d, a, b, c, x[15], 10, 0xfe2ce6e0);
 MD5_ROUND4(c, d, a, b, x[ 6], 15, 0xa3014314);
 MD5_ROUND4(b, c, d, a, x[13], 21, 0x4e0811a1);
 MD5_ROUND4(a, b, c, d, x[ 4],  6, 0xf7537e82);
 MD5_ROUND4(d, a, b, c, x[11], 10, 0xbd3af235);
 MD5_ROUND4(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
 MD5_ROUND4(b, c, d, a, x[ 9], 21, 0xeb86d391);

 state[0] += a;
 state[1] += b;
 state[2] += c;
 state[3] += d;
}


/*!
@brief      �ڲ��������������ݵ�ǰ�沿��(>64�ֽڵĲ���)��ÿ�����һ��64�ֽڵ�block�ͽ����Ӵմ���
@param[out] ctx  �㷨��context�����ڼ�¼һЩ����������ĺͽ��
@param[in]  buf  ��������ݣ�
@param[in]  size ��������ݳ���
*/
static void zen_md5_update(md5_ctx *ctx, const unsigned char *buf, size_t size)
{
 //Ϊʲô����=����Ϊ��ĳЩ�����£����Զ�ε���zen_md5_update����������������뱣֤ǰ��ĵ��ã�ÿ�ζ�û��unprocessed_
 ctx->length_ += size;

 //ÿ������Ŀ鶼��64�ֽ�
 while (size >= ZEN_MD5_BLOCK_SIZE)
 {
     zen_md5_process_block(ctx->hash_, (const uint32_t *)(buf));
     buf  += ZEN_MD5_BLOCK_SIZE;
     size -= ZEN_MD5_BLOCK_SIZE;
 }

 ctx->unprocessed_ = size;
}


/*!
@brief      �ڲ��������������ݵ�ĩβ���֣�����Ҫƴ�����1��������������Ҫ�����BLOCK������0x80�����ϳ��Ƚ��д���
@param[in]  ctx    �㷨��context�����ڼ�¼һЩ����������ĺͽ��
@param[in]  buf    ���������
@param[in]  size   ����buffer�ĳ���
@param[out] result ���صĽ����
*/
static void zen_md5_final(md5_ctx *ctx, const unsigned char *buf, size_t size, unsigned char *result)
{
 uint32_t message[ZEN_MD5_BLOCK_SIZE / 4];

 //����ʣ������ݣ�����Ҫƴ�����1��������������Ҫ����Ŀ飬ǰ����㷨��֤�ˣ����һ����϶�С��64���ֽ�
 if (ctx->unprocessed_)
 {
     memcpy(message, buf + size - ctx->unprocessed_, (size_t)( ctx->unprocessed_));
 }

 //�õ�0x80Ҫ����ڵ�λ�ã���uint32_t �����У���
 uint32_t index = ((uint32_t)ctx->length_ & 63) >> 2;
 uint32_t shift = ((uint32_t)ctx->length_ & 3) * 8;

 //���0x80��ȥ�����Ұ����µĿռ䲹��0
 message[index]   &= ~(0xFFFFFFFF << shift);
 message[index++] ^= 0x80 << shift;

 //������block���޷����������ĳ����޷����ɳ���64bit����ô�ȴ������block
 if (index > 14)
 {
     while (index < 16)
     {
         message[index++] = 0;
     }

     zen_md5_process_block(ctx->hash_, message);
     index = 0;
 }

 //��0
 while (index < 14)
 {
     message[index++] = 0;
 }

 //���泤�ȣ�ע����bitλ�ĳ���,����������ҿ��������˰��죬
 uint64_t data_len = (ctx->length_) << 3;

 //ע��MD5�㷨Ҫ���64bit�ĳ�����СͷLITTLE-ENDIAN���룬ע������ıȽ���!=
#if ZEN_BYTES_ORDER != ZEN_LITTLE_ENDIAN
 data_len = ZEN_SWAP_UINT64(data_len);
#endif

 message[14] = (uint32_t) (data_len & 0x00000000FFFFFFFF);
 message[15] = (uint32_t) ((data_len & 0xFFFFFFFF00000000ULL) >> 32);

 zen_md5_process_block(ctx->hash_, message);

 //ע������Сͷ���ģ��ڴ�ͷ������Ҫ����ת��
#if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
 memcpy(result, &ctx->hash_, ZEN_MD5_HASH_SIZE);
#else
 swap_uint32_memcpy(result, &ctx->hash_, ZEN_MD5_HASH_SIZE);
#endif

}


//����һ���ڴ����ݵ�MD5ֵ
unsigned char *md5(const unsigned char *buf,
                         size_t size,
                         unsigned char result[ZEN_MD5_HASH_SIZE])
{
	if(buf == NULL || result == NULL)
	{
		return NULL;
	}

	md5_ctx ctx;
	zen_md5_init(&ctx);
	zen_md5_update(&ctx, buf, size);
	zen_md5_final(&ctx, buf, size, result);

	return result;
}




//================================================================================================
//SHA1���㷨

//ÿ�δ����BLOCK�Ĵ�С
static const size_t ZEN_SHA1_BLOCK_SIZE = 64;

//SHA1�㷨�������ģ�����һЩ״̬���м����ݣ����
typedef struct sha1_ctx
{

 //��������ݵĳ���
 uint64_t length_;
 //��û�д�������ݳ���
 uint64_t unprocessed_;
 /* 160-bit algorithm internal hashing state */
 uint32_t hash_[5];
} sha1_ctx;

//�ڲ�������SHA1�㷨�������ĵĳ�ʼ��
static void zen_sha1_init(sha1_ctx *ctx)
{
 ctx->length_ = 0;
 ctx->unprocessed_ = 0;
 // ��ʼ���㷨�ļ���������ħ����
 ctx->hash_[0] = 0x67452301;
 ctx->hash_[1] = 0xefcdab89;
 ctx->hash_[2] = 0x98badcfe;
 ctx->hash_[3] = 0x10325476;
 ctx->hash_[4] = 0xc3d2e1f0;
}


/*!
@brief      �ڲ���������һ��64bit�ڴ�����ժҪ(�Ӵ�)����
@param      hash  ��ż���hash����ĵ�����
@param      block Ҫ����Ĵ�����ڴ��
*/
static void zen_sha1_process_block(uint32_t hash[5],
                                const uint32_t block[ZEN_SHA1_BLOCK_SIZE / 4])
{
 size_t        t;
 uint32_t      wblock[80];
 register uint32_t      a, b, c, d, e, temp;

 //SHA1�㷨������ڲ�����Ҫ���Ǵ�ͷ���ģ���Сͷ�Ļ���ת��
#if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
 swap_uint32_memcpy(wblock, block, ZEN_SHA1_BLOCK_SIZE);
#else
 ::memcpy(wblock, block, ZEN_SHA1_BLOCK_SIZE);
#endif

 //����
 for (t = 16; t < 80; t++)
 {
     wblock[t] = ROTL32(wblock[t - 3] ^ wblock[t - 8] ^ wblock[t - 14] ^ wblock[t - 16], 1);
 }

 a = hash[0];
 b = hash[1];
 c = hash[2];
 d = hash[3];
 e = hash[4];

 for (t = 0; t < 20; t++)
 {
     /* the following is faster than ((B & C) | ((~B) & D)) */
     temp =  ROTL32(a, 5) + (((c ^ d) & b) ^ d)
             + e + wblock[t] + 0x5A827999;
     e = d;
     d = c;
     c = ROTL32(b, 30);
     b = a;
     a = temp;
 }

 for (t = 20; t < 40; t++)
 {
     temp = ROTL32(a, 5) + (b ^ c ^ d) + e + wblock[t] + 0x6ED9EBA1;
     e = d;
     d = c;
     c = ROTL32(b, 30);
     b = a;
     a = temp;
 }

 for (t = 40; t < 60; t++)
 {
     temp = ROTL32(a, 5) + ((b & c) | (b & d) | (c & d))
            + e + wblock[t] + 0x8F1BBCDC;
     e = d;
     d = c;
     c = ROTL32(b, 30);
     b = a;
     a = temp;
 }

 for (t = 60; t < 80; t++)
 {
     temp = ROTL32(a, 5) + (b ^ c ^ d) + e + wblock[t] + 0xCA62C1D6;
     e = d;
     d = c;
     c = ROTL32(b, 30);
     b = a;
     a = temp;
 }

 hash[0] += a;
 hash[1] += b;
 hash[2] += c;
 hash[3] += d;
 hash[4] += e;
}


/*!
@brief      �ڲ��������������ݵ�ǰ�沿��(>64�ֽڵĲ���)��ÿ�����һ��64�ֽڵ�block�ͽ����Ӵմ���
@param      ctx  �㷨�������ģ���¼�м����ݣ������
@param      msg  Ҫ���м��������buffer
@param      size ����
*/
static void zen_sha1_update(sha1_ctx *ctx,
                         const unsigned char *buf,
                         size_t size)
{
 //Ϊ����zen_sha1_update���Զ�ν��룬���ȿ����ۼ�
 ctx->length_ += size;

 //ÿ������Ŀ鶼��64�ֽ�
 while (size >= ZEN_SHA1_BLOCK_SIZE)
 {
     zen_sha1_process_block(ctx->hash_, (const uint32_t *)(buf));
     buf  += ZEN_SHA1_BLOCK_SIZE;
     size -= ZEN_SHA1_BLOCK_SIZE;
 }

 ctx->unprocessed_ = size;
}


/*!
@brief      �ڲ��������������ݵ���󲿷֣����0x80,��0�����ӳ�����Ϣ
@param      ctx    �㷨�������ģ���¼�м����ݣ������
@param      msg    Ҫ���м��������buffer
@param      result ���صĽ��
*/
static void zen_sha1_final(sha1_ctx *ctx,
                        const unsigned char *msg,
                        size_t size,
                        unsigned char *result)
{

 uint32_t message[ZEN_SHA1_BLOCK_SIZE / 4];

 //����ʣ������ݣ�����Ҫƴ�����1��������������Ҫ����Ŀ飬ǰ����㷨��֤�ˣ����һ����϶�С��64���ֽ�
 if (ctx->unprocessed_)
 {
     memcpy(message, msg + size - ctx->unprocessed_, (size_t)( ctx->unprocessed_));
 }

 //�õ�0x80Ҫ����ڵ�λ�ã���uint32_t �����У���
 uint32_t index = ((uint32_t)ctx->length_ & 63) >> 2;
 uint32_t shift = ((uint32_t)ctx->length_ & 3) * 8;

 //���0x80��ȥ�����Ұ����µĿռ䲹��0
 message[index]   &= ~(0xFFFFFFFF << shift);
 message[index++] ^= 0x80 << shift;

 //������block���޷����������ĳ����޷����ɳ���64bit����ô�ȴ������block
 if (index > 14)
 {
     while (index < 16)
     {
         message[index++] = 0;
     }

     zen_sha1_process_block(ctx->hash_, message);
     index = 0;
 }

 //��0
 while (index < 14)
 {
     message[index++] = 0;
 }

 //���泤�ȣ�ע����bitλ�ĳ���,����������ҿ��������˰��죬
 uint64_t data_len = (ctx->length_) << 3;

 //ע��SHA1�㷨Ҫ���64bit�ĳ����Ǵ�ͷBIG-ENDIAN����Сͷ������Ҫ����ת��
#if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
 data_len = ZEN_SWAP_UINT64(data_len);
#endif

 message[14] = (uint32_t) (data_len & 0x00000000FFFFFFFF);
 message[15] = (uint32_t) ((data_len & 0xFFFFFFFF00000000ULL) >> 32);

 zen_sha1_process_block(ctx->hash_, message);

 //ע�����Ǵ�ͷ���ģ���Сͷ������Ҫ����ת��
#if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
 swap_uint32_memcpy(result, &ctx->hash_, ZEN_SHA1_HASH_SIZE);
#else
 memcpy(result, &ctx->hash_, ZEN_SHA1_HASH_SIZE);
#endif
}



//����һ���ڴ����ݵ�SHA1ֵ
unsigned char *sha1(const unsigned char *msg,
                          size_t size,
                          unsigned char result[ZEN_SHA1_HASH_SIZE])
{
	if(msg == NULL || result == NULL)
	{
		return NULL;
	}

	sha1_ctx ctx;
	zen_sha1_init(&ctx);
	zen_sha1_update(&ctx, msg, size);
	zen_sha1_final(&ctx, msg, size, result);

	return result;
}

int sha1_md5_test(void)
{
	size_t i=0;
	int ret = 0;
	static unsigned char test_buf[7][81] =
	{
		{ "" },
		{ "a" },
		{ "abc" },
		{ "message digest" },
		{ "abcdefghijklmnopqrstuvwxyz" },
		{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" },
		{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890" }
	};

	static const size_t test_buflen[7] =
	{
		0, 1, 3, 14, 26, 62, 80
	};

	static const unsigned char md5_test_sum[7][16] =
	{
		{ 0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,  0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E },
		{ 0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8,  0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61 },
		{ 0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,  0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72 },
		{ 0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D,  0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0 },
		{ 0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00,  0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B },
		{ 0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5,  0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F },
		{ 0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,  0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A }
	};
	unsigned char result[32] ={0};

	for(i=0;i<7;++i)
	{
		md5(test_buf[i],test_buflen[i],result);
		ret = memcmp(result,md5_test_sum[i],16);
		if (ret != 0)
		{
			//ps_debugout("md5�������\r\n");
		}
		else
		{
			//ps_debugout("md5������ȷ\r\n");
		}
	}

	static const unsigned char sha1_test_sum[7][20] =
	{
		{ 0xda,0x39,0xa3,0xee,0x5e,0x6b,0x4b,0x0d,0x32,0x55,0xbf,0xef,0x95,0x60,0x18,0x90,0xaf,0xd8,0x07,0x09 },
		{ 0x86,0xf7,0xe4,0x37,0xfa,0xa5,0xa7,0xfc,0xe1,0x5d,0x1d,0xdc,0xb9,0xea,0xea,0xea,0x37,0x76,0x67,0xb8 },
		{ 0xa9,0x99,0x3e,0x36,0x47,0x06,0x81,0x6a,0xba,0x3e,0x25,0x71,0x78,0x50,0xc2,0x6c,0x9c,0xd0,0xd8,0x9d },
		{ 0xc1,0x22,0x52,0xce,0xda,0x8b,0xe8,0x99,0x4d,0x5f,0xa0,0x29,0x0a,0x47,0x23,0x1c,0x1d,0x16,0xaa,0xe3 },
		{ 0x32,0xd1,0x0c,0x7b,0x8c,0xf9,0x65,0x70,0xca,0x04,0xce,0x37,0xf2,0xa1,0x9d,0x84,0x24,0x0d,0x3a,0x89 },
		{ 0x76,0x1c,0x45,0x7b,0xf7,0x3b,0x14,0xd2,0x7e,0x9e,0x92,0x65,0xc4,0x6f,0x4b,0x4d,0xda,0x11,0xf9,0x40 },
		{ 0x50,0xab,0xf5,0x70,0x6a,0x15,0x09,0x90,0xa0,0x8b,0x2c,0x5e,0xa4,0x0f,0xa0,0xe5,0x85,0x55,0x47,0x32 },
	};

	for(i=0;i<7;++i)
	{
		sha1(test_buf[i],test_buflen[i],result);
		ret = memcmp(result,sha1_test_sum[i],20);
		if (ret != 0)
		{
			//ps_debugout("sha1�������\r\n");
		}
		else
		{
			//ps_debugout("sha1������ȷ\r\n");
		}
	}
	return 0;
}
