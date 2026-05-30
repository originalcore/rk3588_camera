/*******************************************************************************
  * @file        rkisp_bridge.h
  * @brief       Rockchip ISP bridge abstraction
*******************************************************************************/

#ifndef __RKISP_BRIDGE_H_
#define __RKISP_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "aiq_controller.h"
#include "hdr_controller.h"
#include "media_graph.h"
#include "sensor_manager.h"

typedef struct
{
    SensorConfig sensor;
    MediaGraphConfig media_graph;
    AiqConfig aiq;
    HdrMode hdr_mode;
} RkispBridgeConfig;

typedef struct
{
    SensorManager sensor;
    MediaGraph media_graph;
    AiqController aiq;
    HdrController hdr;
    int initialized;
    int running;
} RkispBridge;

int rkisp_bridge_init(
    RkispBridge *bridge,
    const RkispBridgeConfig *config);

int rkisp_bridge_start(
    RkispBridge *bridge);

int rkisp_bridge_stop(
    RkispBridge *bridge);

void rkisp_bridge_deinit(
    RkispBridge *bridge);

int rkisp_setup_pipeline(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
