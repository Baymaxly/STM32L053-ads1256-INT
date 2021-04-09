#ifndef _ADS1256_H
#define _ADS1256_H
/****************************����ͷ�ļ���***********************/
#include "stm32l0xx_hal.h"
#include "gpio.h"
#include "tim.h"
#include "spi.h"
#include "math.h"
#include "stdlib.h"
extern int32_t adcVaule[8];
extern float voltage[8];
extern float filterVoltage[8];
extern float filterVoltage2[8];
extern uint8_t exti_flag;
/****************************����ads1256�Ĵ�����ַ**************/
#define STATUS  0x00                //״̬�Ĵ���
#define MUX     0x01                //ͨ���л��Ĵ���
#define ADCON   0x02                //ADC���ƼĴ���
#define DRATE   0x03                //ADC�������üĴ���
#define IO      0x04                //ͨ��IO�Ĵ���
#define OFC0    0x05
#define OFC1    0x06
#define OFC2    0x07
#define FSC0    0x08
#define FSC1    0x09
#define FSC2    0x0A

/****************************����ads1256����*******************/
#define WAKEUP  0x00
#define RDATA   0x01
#define TDATAC  0x03
#define SDATAC  0x0F
#define RREG    0x10
#define WREG    0x50
#define SELFCAL 0xF0
#define SELFOCAL 0xF1
#define SELFGCAL 0xF2
#define SYSOCAL  0xF3
#define SYSGCAL  0xF4
#define SYNC     0xFC
#define STANDBY  0xFD
#define RESET    0xFE

/*****************************�����������*********************/
#define GAIN_1   0x00
#define GAIN_2   0x01
#define GAIN_4   0x02
#define GAIN_8   0x03
#define GAIN_16  0x04
#define GAIN_32  0x05
#define GAIN_64  0x06

/*****************************��������ads1256�ɼ����ʴ���*******/
#define RATE_30000 0xF0
#define RATE_15000 0xE0
#define RATE_7500  0xD0                       
#define RATE_3750  0xC0
#define RATE_2000  0xB0
#define RATE_1000  0xA1
#define RATE_500   0x92
#define RATE_100   0x82
#define RATE_60    0x72
#define RATE_50    0x63
#define RATE_30    0x53
#define RATE_25    0x43
#define RATE_15    0x33
#define RATE_10    0x23
#define RATE_5     0x13
#define RATE_2_5   0x03

/*****************************����ads1256ͨ��ѡ��**************/
//����������ͨ��
#define MUXP_AIN0  0x00
#define MUXP_AIN1  0x10
#define MUXP_AIN2  0x20
#define MUXP_AIN3  0x30
#define MUXP_AIN4  0x40
#define MUXP_AIN5  0x50
#define MUXP_AIN6  0x60
#define MUXP_AIN7  0x70
#define MUXP_AINCOM 0x80
//���帺����ͨ��
#define MUXN_AIN0   0x00
#define MUXN_AIN1   0x01
#define MUXN_AIN2   0x02
#define MUXN_AIN3   0x03
#define MUXN_AIN4   0x04
#define MUXN_AIN5   0x05
#define MUXN_AIN6   0x06
#define MUXN_AIN7   0x07
#define MUXN_AINCOM 0x08
/****************************����������************************/
void delayXus(uint16_t us);
void spiWriteByte(uint8_t txData);
uint8_t spiReadByte(void);
void spiWriteRegData(uint8_t regAdd, uint8_t regData);
void ads1256Init(void);
int32_t ads1256ReadValue(uint8_t channel);
void setGain(uint8_t gainSelect);
void setRate(uint8_t rate);
void setmode(uint8_t mode);
void setGain(uint8_t gain);
void setRate(uint8_t rate);
void disableInterrupt(void);
void enableInterrupt(void);
float filterlowerpass(float adc);
float kalman_filter(float ADC_Value);
#endif
