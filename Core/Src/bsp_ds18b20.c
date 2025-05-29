/**
 ******************************************************************************
 * @file    bsp_ds18b20.c
 * @author  Your Name
 * @version V1.0
 * @date    2023-XX-XX
 * @brief   DS18B20�¶ȴ�����Ӧ�ú����ӿڣ�HAL��汾��
 ******************************************************************************
 */
#include "bsp_ds18b20.h"
#include "cmsis_os2.h"
#include "core_delay_dwt.h"
#include "freeRTOS.h"
#include "projdefs.h"
#include "stm32f4xx_hal.h" // HAL��ͷ�ļ�
#include <stdio.h>

/* ����DS18B20���ӵ�GPIO�˿ں����ţ�����ʵ��Ӳ���޸ģ� */
#define DS18B20_GPIO_PORT GPIOE
#define DS18B20_GPIO_PIN GPIO_PIN_2
#define DS18B20_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE() // ���ݶ˿��޸�

#define DHT11_DELAY_US(us) CPU_TS_Tmr_Delay_US(us) // ��ʱ����

/* ����ߵ͵�ƽ�� */
#define DS18B20_HIGH                                                           \
  HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, GPIO_PIN_SET)
#define DS18B20_LOW                                                            \
  HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, GPIO_PIN_RESET)
#define DS18B20_READ() HAL_GPIO_ReadPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)

/*
 * ��������DS18B20_GPIO_Config
 * ����  ������DS18B20�õ���I/O�ڣ�HAL��汾��
 * ����  ����
 * ���  ����
 */
void DS18B20_GPIO_Config(void) {
  GPIO_InitTypeDef gpio_InitStruct = {0};

  // ʹ��GPIOʱ��
  DS18B20_CLK_ENABLE();

  // ����Ϊ�������ģʽ����ʼ״̬��
  gpio_InitStruct.Pin = DS18B20_GPIO_PIN;
  gpio_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_InitStruct.Pull = GPIO_PULLUP;
  gpio_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &gpio_InitStruct);
}

/*
 * ��������DS18B20_SetInputMode
 * ����  ������Ϊ����ģʽ������/����������Ҫ���ã�
 * ����  ����
 * ���  ����
 */
static void DS18B20_SetInputMode(void) {
  GPIO_InitTypeDef gpio_InitStruct = {0};

  gpio_InitStruct.Pin = DS18B20_GPIO_PIN;
  gpio_InitStruct.Mode = GPIO_MODE_INPUT;
  gpio_InitStruct.Pull = GPIO_PULLUP; // ����Ӳ������ѡ������������
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &gpio_InitStruct);
}

/*
 * ��������DS18B20_SetOutputMode
 * ����  ������Ϊ�������ģʽ
 * ����  ����
 * ���  ����
 */
static void DS18B20_SetOutputMode(void) {
  GPIO_InitTypeDef gpio_InitStruct = {0};

  gpio_InitStruct.Pin = DS18B20_GPIO_PIN;
  gpio_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_InitStruct.Pull = GPIO_PULLUP;
  gpio_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &gpio_InitStruct);
}

/*
 * �������ӻ����͸�λ���壨HAL��汾��
 */
static void DS18B20_Rst(void) {
  DS18B20_SetOutputMode(); // ����Ϊ���ģʽ
  DS18B20_LOW;             // ��������

  // ���ֵ͵�ƽ����480us������750us��
  DHT11_DELAY_US(750);

  DS18B20_HIGH;  // �ͷ�����
  DHT11_DELAY_US(15); // �ȴ��ӻ���Ӧ
}

/*
 * ���ӻ����������صĴ������壨HAL��汾��
 * 0���ɹ�
 * 1��ʧ��
 */
static uint8_t DS18B20_Presence(void) {
  uint16_t timeout = 0;

  DS18B20_SetInputMode(); // ����Ϊ����ģʽ

  // �ȴ��������ͣ��������忪ʼ��
  while (DS18B20_READ() && timeout < 100) {
    timeout++;
    DHT11_DELAY_US(1);
  }
  if (timeout >= 100) {
    printf("DS18B20 Presence Timeout 110\r\n");
    return 1; // ��ʱ
  }

  timeout = 0;
  // �ȴ��������ߣ��������������
  while (!DS18B20_READ() && timeout < 240) {
    timeout++;
    DHT11_DELAY_US(1);
  }
  if (timeout >= 240) {
    printf("ds18b20 Presence Timeout 121\r\n");
    return 1; // ��ʱ
  }

  return 0;
}

/*
 * ��DS18B20��ȡһ��bit��HAL��汾��
 */
static uint8_t DS18B20_Read_Bit(void) {
  uint8_t bit = 0;

  DS18B20_SetOutputMode(); // ����Ϊ���ģʽ
  DS18B20_LOW;             // ������ʱ��϶��ʼ�ź�
  DHT11_DELAY_US(2);            // ��������1us��������15us

  DS18B20_SetInputMode(); // �ͷ����ߣ��л�Ϊ����ģʽ
  DHT11_DELAY_US(12);          // �ȴ������ȶ�

  if (DS18B20_READ())
    bit = 1; // ��ȡ����

  DHT11_DELAY_US(50); // �ȴ���ʱ��϶����
  return bit;
}

/*
 * ��DS18B20��һ���ֽڣ���λ���У�HAL��汾��
 */
uint8_t DS18B20_Read_Byte(void) {
  uint8_t byte = 0, i;
  for (i = 0; i < 8; i++) {
    byte >>= 1;
    if (DS18B20_Read_Bit())
      byte |= 0x80;
  }
  return byte;
}

/*
 * дһ���ֽڵ�DS18B20����λ���У�HAL��汾��
 */
void DS18B20_Write_Byte(uint8_t dat) {
  uint8_t i, temp;
  DS18B20_SetOutputMode(); // ����Ϊ���ģʽ

  for (i = 0; i < 8; i++) {
    temp = dat & 0x01;
    dat >>= 1;

    if (temp) {
      DS18B20_LOW;   // д1����������
      DHT11_DELAY_US(2);  // ����2us
      DS18B20_HIGH;  // �ͷ�����
      DHT11_DELAY_US(60); // ����60us
    } else {
      DS18B20_LOW;   // д0����������
      DHT11_DELAY_US(60); // ����60us
      DS18B20_HIGH;  // �ͷ�����
      DHT11_DELAY_US(2);  // ����2us
    }
  }
}

/*
 * ��ʼ��DS18B20��HAL��汾��
 * ����ֵ��0-�ɹ���1-ʧ��
 */
uint8_t DS18B20_Init(void) {
  DS18B20_GPIO_Config();     // ��ʼ��GPIO
  DS18B20_Rst();             // ���͸�λ����
  return DS18B20_Presence(); // ����������
}

/*
 * ��ȡDS18B20�¶�ֵ��HAL��汾��
 * ����ֵ���¶�ֵ����λ���棩
 */
float DS18B20_Get_Temp(void) {
  uint8_t lowByte, highByte;
  int16_t temp;
  float result;

  // �����¶�ת��
  DS18B20_Rst();
  DS18B20_Presence();
  DS18B20_Write_Byte(0xCC); // ����ROM
  DS18B20_Write_Byte(0x44); // ����ת��

  // ��ȡ�¶�����
  DS18B20_Rst();
  DS18B20_Presence();
  DS18B20_Write_Byte(0xCC); // ����ROM
  DS18B20_Write_Byte(0xBE); // ��ȡ�¶ȼĴ���

  lowByte = DS18B20_Read_Byte();  // ��ȡ���ֽ�
  highByte = DS18B20_Read_Byte(); // ��ȡ���ֽ�

  temp = (highByte << 8) | lowByte; // ��ϳ�16λ�¶�ֵ

  // ת��Ϊʵ���¶�ֵ
  if (temp & 0x8000) // ��������
  {
    temp = ~temp + 1; // ����תԭ��
    result = -(float)temp * 0.0625;
  } else {
    result = (float)temp * 0.0625;
  }

  return result;
}

/**
 * @brief  ��ƥ��ROM����»�ȡDS18B20���к�
 * @param  ds18b20_id�����ڴ�����кŵ������׵�ַ
 * @retval ��
 */
void DS18B20_ReadId(uint8_t *ds18b20_id) {
  uint8_t i;
  DS18B20_Rst();
  DS18B20_Presence();
  DS18B20_Write_Byte(0x33); // ���Ͷ�ȡROM����

  for (i = 0; i < 8; i++) {
    ds18b20_id[i] = DS18B20_Read_Byte();
  }
}
