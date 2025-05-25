#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "gpio.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0) // Assuming GPIO_PIN_0 is connected to KEY_1
    {
        key1_toggle_red_led_pin();
    }
}
