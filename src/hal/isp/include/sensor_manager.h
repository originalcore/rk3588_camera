/*******************************************************************************
  * @file        sensor_manager.h
  * @brief       Camera sensor abstraction
*******************************************************************************/

#ifndef __SENSOR_MANAGER_H_
#define __SENSOR_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#define SENSOR_NAME_LEN 32
#define SENSOR_BUS_INFO_LEN 64

typedef struct
{
    char name[SENSOR_NAME_LEN];
    char bus_info[SENSOR_BUS_INFO_LEN];
    int width;
    int height;
    uint32_t mbus_code;
    int fps;
} SensorConfig;

typedef struct
{
    SensorConfig config;
    int streaming;
} SensorManager;

int sensor_manager_init(
    SensorManager *manager,
    const SensorConfig *config);

int sensor_manager_start(
    SensorManager *manager);

int sensor_manager_stop(
    SensorManager *manager);

void sensor_manager_deinit(
    SensorManager *manager);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
