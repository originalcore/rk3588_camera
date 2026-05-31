/*******************************************************************************
  * @file        sensor_manager.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @brief       Camera sensor abstraction
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


#include "sensor_manager.h"

#include <string.h>

int sensor_manager_init(
    SensorManager *manager,
    const SensorConfig *config)
{
    if (!manager ||
        !config)
        return -1;

    memset(manager, 0, sizeof(*manager));
    manager->config = *config;

    return 0;
}

int sensor_manager_start(
    SensorManager *manager)
{
    if (!manager)
        return -1;

    manager->streaming = 1;

    return 0;
}

int sensor_manager_stop(
    SensorManager *manager)
{
    if (!manager)
        return -1;

    manager->streaming = 0;

    return 0;
}

void sensor_manager_deinit(
    SensorManager *manager)
{
    if (!manager)
        return;

    manager->streaming = 0;
}
