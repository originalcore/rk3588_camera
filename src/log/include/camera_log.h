/*******************************************************************************
  * @file        camera_log.h
  * @brief       h File
*******************************************************************************/
/*@{*/

#ifndef __CAMERA_LOG_H_
#define __CAMERA_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int camera_log_init(
    const char *config_path);

void camera_log_fini(void);

#define CAMERA_LOG_DEBUG(...) dzlog_debug(__VA_ARGS__)
#define CAMERA_LOG_INFO(...) dzlog_info(__VA_ARGS__)
#define CAMERA_LOG_WARN(...) dzlog_warn(__VA_ARGS__)
#define CAMERA_LOG_ERROR(...) dzlog_error(__VA_ARGS__)
#define CAMERA_LOG_FATAL(...) dzlog_fatal(__VA_ARGS__)

#include "zlog.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
