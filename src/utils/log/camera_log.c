/*******************************************************************************
  * @file        camera_log.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/

#include "camera_log.h"

int camera_log_init(
    const char *config_path)
{
    return dzlog_init(config_path,
                      "camera");
}

void camera_log_fini(void)
{
    zlog_fini();
}
