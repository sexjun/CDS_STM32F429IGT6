#include "stm32f4xx_hal.h"
#include <stdint.h>

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    key1_toggle_red_led_pin();
}