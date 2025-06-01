#include "cds_gpio_exit.h"
#include "gpio.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include <stdint.h>

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_0) // Assuming GPIO_PIN_0 is connected to KEY_1
  {
    key1_toggle_red_led_pin();
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART3) {
    HAL_UART_Transmit_IT(&huart1, uart3_rx_buffer,
                         sizeof(uart3_rx_buffer)); // Echo back received data
    // Check the received command and toggle the corresponding LED
    if (uart3_rx_buffer[0] == 'R') {
      HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin); // Toggle red LED
    } else if (uart3_rx_buffer[0] == 'G') {
      HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin); // Toggle green LED
    } else if (uart3_rx_buffer[0] == 'B') {
      HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin); // Toggle blue LED
    } else {
      uint8_t response[] = "Invalid Command";
      HAL_UART_Transmit_IT(huart, response, sizeof(response));
    }
    HAL_UARTEx_ReceiveToIdle_IT(
        &huart1, uart3_rx_buffer,
        sizeof(uart3_rx_buffer)); // Re-enable the interrupt for next reception
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART1) {
    HAL_UART_Transmit_DMA(&huart1, uart3_rx_buffer,
                          Size); // Echo back received data
    // Check the received command and toggle the corresponding LED
    if (uart3_rx_buffer[0] == 'R') {
      HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin); // Toggle red LED
      HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_B_Pin | LED_G_Pin,
                        GPIO_PIN_RESET); // Ensure other LEDs are off
    } else if (uart3_rx_buffer[0] == 'G') {
      HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin); // Toggle green LED
    } else if (uart3_rx_buffer[0] == 'B') {
      HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin); // Toggle blue LED
    } else {
      uint8_t response[] = "Invalid Command";
      HAL_UART_Transmit_DMA(&huart1, response, sizeof(response));
    }
    HAL_UARTEx_ReceiveToIdle_DMA(
        &huart1, uart3_rx_buffer,
        sizeof(uart3_rx_buffer)); // Re-enable the interrupt for next reception
    // 关闭DMA传输过半中断（HAL库默认开启，但我们只需要接收完成中断）
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  }
}

/* 重定向printf到UART */

/* 禁用半主机模式 */
#pragma import(__use_no_semihosting)

/* 标准库所需的最低实现 */
struct __FILE {
  int handle;
};
FILE __stdout;

/* 避免使用半主机模式的 _sys_exit */
void _sys_exit(int x) { x = x; }

/* 重定向 fputc 到 UART */
int fputc(int ch, FILE *f) {
  uint8_t c = (uint8_t)ch;

  /* 阻塞方式发送一个字符 */
  HAL_UART_Transmit(&huart1, &c, 1, HAL_MAX_DELAY);
  return ch;
}

/// 重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f) {
  uint8_t c;
  /* 阻塞方式接收一个字符 */
  HAL_UART_Receive(&huart1, &c, 1, HAL_MAX_DELAY);
  return c; // 返回接收到的字符
}

int __io_putchar(int ch) {
  uint8_t c[1];
  c[0] = ch & 0x00FF;
  HAL_UART_Transmit(&huart1, &*c, 1, 10);
  return ch;
}

int _write(int file, char *ptr, int len) {
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++) {
    __io_putchar(*ptr++);
  }
  return len;
}
