#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "cds_gpio_exit.h"
#include "gpio.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0) // Assuming GPIO_PIN_0 is connected to KEY_1
    {
        key1_toggle_red_led_pin();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        HAL_UART_Transmit_IT(&huart3, uart3_rx_buffer, 2); // Echo back received data
        // Check the received command and toggle the corresponding LED
        if(uart3_rx_buffer[0] == 'R') {
            HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin); // Toggle red LED
        } else if(uart3_rx_buffer[0] == 'G') {
            HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin); // Toggle green LED
        } else if(uart3_rx_buffer[0] == 'B') {
            HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin); // Toggle blue LED
        } else {
            uint8_t response[] = "Invalid Command";
            HAL_UART_Transmit_IT(huart, response, sizeof(response));
        }
        HAL_UART_Receive_IT(huart, uart3_rx_buffer, 2); // Re-enable the interrupt for next reception
    }
}
