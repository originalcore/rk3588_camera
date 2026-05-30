/*******************************************************************************
  * @file        camera_log.c
  * @brief       c File
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
