#include "interrupthandle.h"
#include "lv_port_disp.h"

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