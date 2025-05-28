#ifndef E6EE9D39_6AE5_4157_847D_5C41AF32399D
#define E6EE9D39_6AE5_4157_847D_5C41AF32399D
#include "usart.h"
#include "stdio.h"
#include "string.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

// UART3
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif /* E6EE9D39_6AE5_4157_847D_5C41AF32399D */
