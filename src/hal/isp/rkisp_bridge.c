/*******************************************************************************
  * @file        rkisp_bridge.c
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

#include "rkisp_bridge.h"

#include <string.h>

int rkisp_bridge_init(
    RkispBridge *bridge,
    const RkispBridgeConfig *config)
{
    if (!bridge ||
        !config)
        return -1;

    memset(bridge, 0, sizeof(*bridge));

    if (sensor_manager_init(&bridge->sensor,
                            &config->sensor) < 0)
        return -1;

    if (media_graph_init(&bridge->media_graph,
                         &config->media_graph) < 0)
    {
        sensor_manager_deinit(&bridge->sensor);
        return -1;
    }

    if (aiq_controller_init(&bridge->aiq,
                            &config->aiq) < 0)
    {
        media_graph_deinit(&bridge->media_graph);
        sensor_manager_deinit(&bridge->sensor);
        return -1;
    }

    if (hdr_controller_init(&bridge->hdr,
                            config->hdr_mode) < 0)
    {
        aiq_controller_deinit(&bridge->aiq);
        media_graph_deinit(&bridge->media_graph);
        sensor_manager_deinit(&bridge->sensor);
        return -1;
    }

    bridge->initialized = 1;

    return 0;
}

int rkisp_bridge_start(
    RkispBridge *bridge)
{
    if (!bridge ||
        !bridge->initialized)
        return -1;

    if (media_graph_setup_links(&bridge->media_graph) < 0)
        return -1;

    if (hdr_controller_apply(&bridge->hdr) < 0)
        return -1;

    if (aiq_controller_start(&bridge->aiq) < 0)
        return -1;

    if (sensor_manager_start(&bridge->sensor) < 0)
    {
        aiq_controller_stop(&bridge->aiq);
        return -1;
    }

    bridge->running = 1;

    return 0;
}

int rkisp_bridge_stop(
    RkispBridge *bridge)
{
    int ret = 0;

    if (!bridge ||
        !bridge->initialized)
        return -1;

    if (!bridge->running)
        return 0;

    if (sensor_manager_stop(&bridge->sensor) < 0)
        ret = -1;

    if (aiq_controller_stop(&bridge->aiq) < 0)
        ret = -1;

    bridge->running = 0;

    return ret;
}

void rkisp_bridge_deinit(
    RkispBridge *bridge)
{
    if (!bridge ||
        !bridge->initialized)
        return;

    rkisp_bridge_stop(bridge);
    hdr_controller_deinit(&bridge->hdr);
    aiq_controller_deinit(&bridge->aiq);
    media_graph_deinit(&bridge->media_graph);
    sensor_manager_deinit(&bridge->sensor);

    bridge->initialized = 0;
}

int rkisp_setup_pipeline(void)
{
    /*
     * Legacy compatibility wrapper.
     * New code should use RkispBridgeConfig + rkisp_bridge_start().
     */

    return 0;
}
