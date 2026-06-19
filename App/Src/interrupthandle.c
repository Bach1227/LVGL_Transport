#include "interrupthandle.h"
#include "lv_port_disp.h"
#include "gpio.h"
#include "Transportation.h"

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
    {
        lv_display_t * disp = lv_port_disp_get_display();
        if (disp != NULL)
        {
            lv_display_flush_ready(disp);
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_15)
    {
        static uint8_t isfree = 1;
        Transport_Free(isfree);
    }
}