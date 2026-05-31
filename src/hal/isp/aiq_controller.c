/*******************************************************************************
  * @file        aiq_controller.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @brief       Rockchip AIQ controller abstraction
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


#include "aiq_controller.h"

#include <string.h>

int aiq_controller_init(
    AiqController *controller,
    const AiqConfig *config)
{
    if (!controller ||
        !config)
        return -1;

    memset(controller, 0, sizeof(*controller));
    controller->config = *config;

    return 0;
}

int aiq_controller_start(
    AiqController *controller)
{
    if (!controller)
        return -1;

    /*
     * Future implementation:
     * rk_aiq_uapi_sysctl_init/start and algorithm module setup.
     */
    controller->running = 1;

    return 0;
}

int aiq_controller_stop(
    AiqController *controller)
{
    if (!controller)
        return -1;

    controller->running = 0;

    return 0;
}

void aiq_controller_deinit(
    AiqController *controller)
{
    if (!controller)
        return;

    controller->running = 0;
}
