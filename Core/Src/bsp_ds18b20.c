/**
 ******************************************************************************
 * @file    bsp_ds18b20.c
 * @author  Your Name
 * @version V1.0
 * @date    2023-XX-XX
 * @brief   DS18B20温度传感器应用函数接口（HAL库版本）
 ******************************************************************************
 */
#include "bsp_ds18b20.h"
#include "cmsis_os2.h"
#include "core_delay_dwt.h"
#include "freeRTOS.h"
#include "projdefs.h"
#include "stm32f4xx_hal.h" // HAL库头文件
#include <stdio.h>

/* 定义DS18B20连接的GPIO端口和引脚（根据实际硬件修改） */
#define DS18B20_GPIO_PORT GPIOE
#define DS18B20_GPIO_PIN GPIO_PIN_2
#define DS18B20_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE() // 根据端口修改

#define DHT11_DELAY_US(us) CPU_TS_Tmr_Delay_US(us) // 延时函数

/* 定义高低电平宏 */
#define DS18B20_HIGH                                                           \
  HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, GPIO_PIN_SET)
#define DS18B20_LOW                                                            \
  HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, GPIO_PIN_RESET)
#define DS18B20_READ() HAL_GPIO_ReadPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)

/*
 * 函数名：DS18B20_GPIO_Config
 * 描述  ：配置DS18B20用到的I/O口（HAL库版本）
 * 输入  ：无
 * 输出  ：无
 */
void DS18B20_GPIO_Config(void) {
  GPIO_InitTypeDef gpio_InitStruct = {0};

  // 使能GPIO时钟
  DS18B20_CLK_ENABLE();

  // 配置为推挽输出模式（初始状态）
  gpio_InitStruct.Pin = DS18B20_GPIO_PIN;
  gpio_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_InitStruct.Pull = GPIO_PULLUP;
  gpio_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &gpio_InitStruct);
}

/*
 * 函数名：DS18B20_SetInputMode
 * 描述  ：设置为输入模式（上拉/下拉根据需要配置）
 * 输入  ：无
 * 输出  ：无
 */
static void DS18B20_SetInputMode(void) {
  GPIO_InitTypeDef gpio_InitStruct = {0};

  gpio_InitStruct.Pin = DS18B20_GPIO_PIN;
  gpio_InitStruct.Mode = GPIO_MODE_INPUT;
  gpio_InitStruct.Pull = GPIO_PULLUP; // 根据硬件连接选择上拉或无拉
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &gpio_InitStruct);
}

/*
 * 函数名：DS18B20_SetOutputMode
 * 描述  ：设置为推挽输出模式
 * 输入  ：无
 * 输出  ：无
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
 * 主机给从机发送复位脉冲（HAL库版本）
 */
static void DS18B20_Rst(void) {
  DS18B20_SetOutputMode(); // 设置为输出模式
  DS18B20_LOW;             // 拉低总线

  // 保持低电平至少480us（建议750us）
  DHT11_DELAY_US(750);

  DS18B20_HIGH;  // 释放总线
  DHT11_DELAY_US(15); // 等待从机响应
}

/*
 * 检测从机给主机返回的存在脉冲（HAL库版本）
 * 0：成功
 * 1：失败
 */
static uint8_t DS18B20_Presence(void) {
  uint16_t timeout = 0;

  DS18B20_SetInputMode(); // 设置为输入模式

  // 等待总线拉低（存在脉冲开始）
  while (DS18B20_READ() && timeout < 100) {
    timeout++;
    DHT11_DELAY_US(1);
  }
  if (timeout >= 100) {
    printf("DS18B20 Presence Timeout 110\r\n");
    return 1; // 超时
  }

  timeout = 0;
  // 等待总线拉高（存在脉冲结束）
  while (!DS18B20_READ() && timeout < 240) {
    timeout++;
    DHT11_DELAY_US(1);
  }
  if (timeout >= 240) {
    printf("ds18b20 Presence Timeout 121\r\n");
    return 1; // 超时
  }

  return 0;
}

/*
 * 从DS18B20读取一个bit（HAL库版本）
 */
static uint8_t DS18B20_Read_Bit(void) {
  uint8_t bit = 0;

  DS18B20_SetOutputMode(); // 设置为输出模式
  DS18B20_LOW;             // 产生读时间隙起始信号
  DHT11_DELAY_US(2);            // 保持至少1us但不超过15us

  DS18B20_SetInputMode(); // 释放总线，切换为输入模式
  DHT11_DELAY_US(12);          // 等待数据稳定

  if (DS18B20_READ())
    bit = 1; // 读取数据

  DHT11_DELAY_US(50); // 等待读时间隙结束
  return bit;
}

/*
 * 从DS18B20读一个字节，低位先行（HAL库版本）
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
 * 写一个字节到DS18B20，低位先行（HAL库版本）
 */
void DS18B20_Write_Byte(uint8_t dat) {
  uint8_t i, temp;
  DS18B20_SetOutputMode(); // 设置为输出模式

  for (i = 0; i < 8; i++) {
    temp = dat & 0x01;
    dat >>= 1;

    if (temp) {
      DS18B20_LOW;   // 写1：拉低总线
      DHT11_DELAY_US(2);  // 保持2us
      DS18B20_HIGH;  // 释放总线
      DHT11_DELAY_US(60); // 保持60us
    } else {
      DS18B20_LOW;   // 写0：拉低总线
      DHT11_DELAY_US(60); // 保持60us
      DS18B20_HIGH;  // 释放总线
      DHT11_DELAY_US(2);  // 保持2us
    }
  }
}

/*
 * 初始化DS18B20（HAL库版本）
 * 返回值：0-成功，1-失败
 */
uint8_t DS18B20_Init(void) {
  DS18B20_GPIO_Config();     // 初始化GPIO
  DS18B20_Rst();             // 发送复位脉冲
  return DS18B20_Presence(); // 检测存在脉冲
}

/*
 * 获取DS18B20温度值（HAL库版本）
 * 返回值：温度值（单位：℃）
 */
float DS18B20_Get_Temp(void) {
  uint8_t lowByte, highByte;
  int16_t temp;
  float result;

  // 启动温度转换
  DS18B20_Rst();
  DS18B20_Presence();
  DS18B20_Write_Byte(0xCC); // 跳过ROM
  DS18B20_Write_Byte(0x44); // 启动转换

  // 读取温度数据
  DS18B20_Rst();
  DS18B20_Presence();
  DS18B20_Write_Byte(0xCC); // 跳过ROM
  DS18B20_Write_Byte(0xBE); // 读取温度寄存器

  lowByte = DS18B20_Read_Byte();  // 读取低字节
  highByte = DS18B20_Read_Byte(); // 读取高字节

  temp = (highByte << 8) | lowByte; // 组合成16位温度值

  // 转换为实际温度值
  if (temp & 0x8000) // 负数处理
  {
    temp = ~temp + 1; // 补码转原码
    result = -(float)temp * 0.0625;
  } else {
    result = (float)temp * 0.0625;
  }

  return result;
}

/**
 * @brief  在匹配ROM情况下获取DS18B20序列号
 * @param  ds18b20_id：用于存放序列号的数组首地址
 * @retval 无
 */
void DS18B20_ReadId(uint8_t *ds18b20_id) {
  uint8_t i;
  DS18B20_Rst();
  DS18B20_Presence();
  DS18B20_Write_Byte(0x33); // 发送读取ROM命令

  for (i = 0; i < 8; i++) {
    ds18b20_id[i] = DS18B20_Read_Byte();
  }
}
