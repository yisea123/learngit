/*
 * spi_dsp.h
 *
 *  Created on: 2017��3��29��
 *      Author: Administrator
 */

#ifndef SPI_DSP_H_
#define SPI_DSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************
 * ��������		spi_dsp_init
 * ��Ҫ���ܣ�	spi_dsp��ʼ��
 * ������
 * ����ֵ��		0 ���ɹ�		1 ��ʧ��
*********************************************/
int spi_dsp_init(void);

/*********************************************
 * ��������		spi_dsp_read
 * ��Ҫ���ܣ�	��ȡspi_dsp����
 * ������
 * ����ֵ��		>=0��ȡ���ֽ��� 		<0 ��ʧ��
*********************************************/
int spi_dsp_read(u32 addr, u8 *buffer, u32 len);

/*********************************************
 * ��������		spi_dsp_write
 * ��Ҫ���ܣ�	�������ݵ�spi_dsp
 * ������
 * ����ֵ��		>=0���͵��ֽ��� 		<0 ��ʧ��
*********************************************/
int spi_dsp_write(u32 addr, u8 *buffer, u32 len);

#ifdef __cplusplus
}
#endif

#endif /* SPI_DSP_H_ */
