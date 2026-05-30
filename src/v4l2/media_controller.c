/*******************************************************************************
  * @file        media_controller.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-30  V1.0.0  weihui.jia Create
  *
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/

#include "media_controller.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int media_controller_open(
    MediaController *mc,
    const char *dev)
{
    memset(mc, 0, sizeof(*mc));

    mc->media_fd = open(dev,
                        O_RDWR);
    if (mc->media_fd < 0)
        return -1;

    return 0;
}

int media_controller_setup_link(
    MediaController *mc)
{
    (void)mc;

    return 0;
}
