#include "ads1256.h"
int32_t adcVaule[8] = {0x00};
float voltage[8] = {0.00};
float filterVoltage[8] = {0.00};
float filterVoltage2[8] = {0.00};

uint8_t exti_flag = 0;

void delayXus(uint16_t us) {
    uint16_t diff = 0xffff - 5 - us;
    //���ö�ʱ���ļ���ֵ
    __HAL_TIM_SET_COUNTER(&htim6, diff);
    //������ʱ������
    HAL_TIM_Base_Start(&htim6);
    //�ж���������
    while(diff < 0xffff - 5) {
        diff = __HAL_TIM_GET_COUNTER(&htim6);
    }
    //��ʱ��ɹرն�ʱ������
    HAL_TIM_Base_Stop(&htim6);
}

/*
*   ��  ��:ʵ��SPIЭ�����߷���һ���ֽڵ�������Ϣ
*   ��  ��:�����͵�������Ϣ
*   ����ֵ:��
*/
void spiWriteByte(uint8_t txData) {
    uint8_t tempData = 0x00;
    HAL_SPI_TransmitReceive(&hspi1, &txData, &tempData, 1, 100);
}

/*
*   ��  ��:ʵ��SPIЭ���ܼ����һ���ֽڵ�������Ϣ
*   ��  ��:��
*   ����ֵ:���ܵ���������Ϣ
*/
uint8_t spiReadByte(void) {
    uint8_t tempDataT = 0xff;
    uint8_t tempData = 0x00;
    HAL_SPI_TransmitReceive(&hspi1, &tempDataT, &tempData, 1, 100);
    return tempData;
}

/*
*   ��  ��:��ads1256�Ĵ�����д��һ���ֽڵ�����
*   ��  ��:regAdd�Ĵ�����ַ regData��д���������Ϣ
*   ����ֵ:��
*/
void spiWriteRegData(uint8_t regAdd, uint8_t regData) {
    //����SPIЭ���CS����
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    //�ȴ�RDY�����ű��
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
    //д��Ĵ�ص�ַ
    spiWriteByte(WREG | (regAdd & 0x0F));
    //д�뼴��д�����ݵĸ���
    spiWriteByte(0x00);
    //д��������Ϣ
    spiWriteByte(regData);
    //����SPIЭ���CS����
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

/*
*   ��  ��:��ʼ��ads1256
*   ��  ��:��
*   ����ֵ:��
*/
void ads1256Init(void) {
    disableInterrupt();
	DBG("init\r\n");
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
    //����оƬ����У׼
	DBG("init\r\n");
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    spiWriteByte(SELFCAL);
	DBG("init\r\n");
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
	DBG("init\r\n");
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
    //����ads1256��״̬�Ĵ���
    spiWriteRegData(STATUS, 0x06);      //���ݷ��͸�λ��ǰ �Զ�У׼ ����buf
    spiWriteRegData(MUX, MUXP_AIN0 | MUXN_AIN1); //����ģʽ
    //����ads1256������
    spiWriteRegData(ADCON, GAIN_1);
    //����ads��������
    spiWriteRegData(DRATE, RATE_30000);
    //����IO״̬
    spiWriteRegData(IO, 0x00);
    //�ٴν���У׼
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
    //����оƬ����У׼
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    spiWriteByte(SELFCAL);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	DBG("init\r\n");
    enableInterrupt();
}

/*
*   ��  ��:��ads1256�ж�ȡ�����������Ϣ������
*   ��  ��:��һ��ת��ͨ��
*   ����ֵ:��ȡ����������Ϣ
*/
int32_t ads1256ReadValue(uint8_t channel) {
    int32_t sum[8] = {0.00};
    //�ȴ�׼�����źű��
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
    //�����´�ת����ͨ��
    spiWriteRegData(MUX, channel | MUXN_AINCOM);//����

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    spiWriteByte(SYNC);     //����ͬ������
    delayXus(5);
    spiWriteByte(WAKEUP);   //���ͻ�������
    delayXus(5);            //��ʱһ��
    spiWriteByte(RDATA);    //���Ͷ���������
    delayXus(25);
    sum[channel] |= (spiReadByte() << 16);
    sum[channel] |= (spiReadByte() << 8);
    sum[channel] |= (spiReadByte());
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
    if(sum[channel] > 0x7fffff)
        sum[channel] -= 0x1000000;
    adcVaule[channel] = sum[channel];
    voltage[channel] = (float)(adcVaule[channel] * 5.0 / 8388607); //�����ѹ
    DBG("\r\n\tͨ��%dADC�ɼ�����:%0x,\t��ѹֵ%f\n", channel, adcVaule[channel], voltage[channel]);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
    return sum[channel];
}
/*
*   ��  ��:ʵ��ads����������
*/
void setGain(uint8_t gain) {
    disableInterrupt();
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    switch(gain) {
    case 0:
        spiWriteRegData(ADCON, GAIN_1);
        break;
    case 1:
        spiWriteRegData(ADCON, GAIN_2);
        break;
    default:
        break;
    }
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
    enableInterrupt();
}
/*
*   ��  ��:����ads1256�Ĳɼ�����
*/
void setRate(uint8_t rate) {
    disableInterrupt();
    while(HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin));
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    switch(rate) {
    case 0:
        spiWriteRegData(DRATE, RATE_2_5);
        break;
    case 1:
        spiWriteRegData(DRATE, RATE_10);
        break;
    default:
        break;
    }
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
    enableInterrupt();
}
/*
*   ��  ��:ʵ�����������жϺ���
*/
void disableInterrupt(void) {
    __set_PRIMASK(1);
}

/*
*   ��  ��:����ȫ���ж�
*/
void enableInterrupt(void) {
    __set_PRIMASK(0);
}
/*
*   ��  ��:ʵ���ⲿ�жϻص�����
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//    int32_t sum = 0x00;
    disableInterrupt();
//	DBG("TEST");
    if((GPIO_Pin == DRDY_INT_Pin) && (HAL_GPIO_ReadPin(DRDY_INT_GPIO_Port, DRDY_INT_Pin) == GPIO_PIN_RESET)) {
        //�����´�ת����ͨ��
	    exti_flag = 1;
/*
        spiWriteRegData(MUX, MUXP_AIN0 | MUXN_AINCOM);
        HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
        spiWriteByte(SYNC);     //����ͬ������
        delayXus(5);
        spiWriteByte(WAKEUP);   //���ͻ�������
        delayXus(5);            //��ʱһ��
        spiWriteByte(RDATA);    //���Ͷ���������
        delayXus(25);
        sum |= (spiReadByte() << 16);
        sum |= (spiReadByte() << 8);
        sum |= (spiReadByte());
        HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
        if(sum > 0x7fffff)
            sum -= 0x1000000;
        adcVaule = sum;
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
*/
    }
    enableInterrupt();
}

/*
float filterlowerpass(float adc) {
    //y(n) = qX(n)+(1-q)Y(n-1)
    float filterVaule = 0;
    static float adcold = 0;
    filterVaule = 0.5 * adc + 0.5 * adcold;
    adcold = adc;
    return filterVaule;
}

float kalman_filter(float ADC_Value) {
    float x_k1_k1, x_k_k1;
    static float ADC_OLD_Value;
    float Z_k;
    static float P_k1_k1;

    static float Q = 0.0001;
    static float R = 5;
    static float Kg = 0;
    static float P_k_k1 = 1;

    float kalman_adc;
    static float kalman_adc_old = 0;
    Z_k = ADC_Value;

    if (abs(kalman_adc_old - ADC_Value) >= 10) {
        x_k1_k1 = ADC_Value * 0.382 + kalman_adc_old * 0.618;
    } else {
        x_k1_k1 = kalman_adc_old;
    }
    x_k_k1 = x_k1_k1;
    P_k_k1 = P_k1_k1 + Q;

    Kg = P_k_k1 / (P_k_k1 + R);

    kalman_adc = x_k_k1 + Kg * (Z_k - kalman_adc_old);
    P_k1_k1 = (1 - Kg) * P_k_k1;
    P_k_k1 = P_k1_k1;

    ADC_OLD_Value = ADC_Value;
    kalman_adc_old = kalman_adc;

    return kalman_adc;
}
*/
