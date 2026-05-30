/*******************************************************************************
  * @file        aiq_controller.h
  * @brief       Rockchip AIQ controller abstraction
*******************************************************************************/

#ifndef __AIQ_CONTROLLER_H_
#define __AIQ_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AIQ_SENSOR_NAME_LEN 32
#define AIQ_IQ_FILE_DIR_LEN 128

typedef struct
{
    char sensor_name[AIQ_SENSOR_NAME_LEN];
    char iq_file_dir[AIQ_IQ_FILE_DIR_LEN];
    int enable_ae;
    int enable_awb;
    int enable_af;
} AiqConfig;

typedef struct
{
    AiqConfig config;
    int running;
} AiqController;

int aiq_controller_init(
    AiqController *controller,
    const AiqConfig *config);

int aiq_controller_start(
    AiqController *controller);

int aiq_controller_stop(
    AiqController *controller);

void aiq_controller_deinit(
    AiqController *controller);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
