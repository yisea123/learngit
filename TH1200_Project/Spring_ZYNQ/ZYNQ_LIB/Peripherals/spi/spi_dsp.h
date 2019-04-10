/*
 * spi_dsp.h
 *
 *  Created on: 2017年3月29日
 *      Author: Administrator
 */

#ifndef SPI_DSP_H_
#define SPI_DSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************
 * 函数名：		spi_dsp_init
 * 主要功能：	spi_dsp初始化
 * 参数：
 * 返回值：		0 ：成功		1 ：失败
*********************************************/
int spi_dsp_init(void);

/*********************************************
 * 函数名：		spi_dsp_read
 * 主要功能：	获取spi_dsp数据
 * 参数：
 * 返回值：		>=0获取的字节数 		<0 ：失败
*********************************************/
int spi_dsp_read(u32 addr, u8 *buffer, u32 len);

/*********************************************
 * 函数名：		spi_dsp_write
 * 主要功能：	发送数据到spi_dsp
 * 参数：
 * 返回值：		>=0发送的字节数 		<0 ：失败
*********************************************/
int spi_dsp_write(u32 addr, u8 *buffer, u32 len);

#ifdef __cplusplus
}
#endif

#endif /* SPI_DSP_H_ */
