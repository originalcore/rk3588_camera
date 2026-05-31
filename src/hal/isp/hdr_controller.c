/*******************************************************************************
  * @file        hdr_controller.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @brief       HDR mode controller abstraction
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


#include "hdr_controller.h"

#include <string.h>

int hdr_controller_init(
    HdrController *controller,
    HdrMode mode)
{
    if (!controller)
        return -1;

    memset(controller, 0, sizeof(*controller));
    controller->mode = mode;

    return 0;
}

int hdr_controller_apply(
    HdrController *controller)
{
    if (!controller)
        return -1;

    controller->enabled = controller->mode != HDR_MODE_OFF;

    return 0;
}

void hdr_controller_deinit(
    HdrController *controller)
{
    if (!controller)
        return;

    controller->enabled = 0;
}
