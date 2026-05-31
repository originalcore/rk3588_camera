/*******************************************************************************
  * @file        event_bus.h
  * @brief       h File
  * @author      weihui.jia
  * @attention
  * @brief       Camera event bus
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


#ifndef __EVENT_BUS_H_
#define __EVENT_BUS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"

#include <pthread.h>

#define CAMERA_EVENT_MAX_LISTENERS 16

typedef enum
{
    CAMERA_EVENT_NONE = 0,
    CAMERA_EVENT_FRAME_READY,
    CAMERA_EVENT_LOST,
    CAMERA_EVENT_STREAM_ON,
    CAMERA_EVENT_STREAM_OFF
} CameraEventType;

typedef struct
{
    CameraEventType type;
    Frame frame;
    int code;
    void *data;
} CameraEvent;

typedef void (*CameraEventHandler)(
    const CameraEvent *event,
    void *user_data);

typedef struct
{
    CameraEventHandler handler;
    void *user_data;
} CameraEventListener;

typedef struct
{
    CameraEventListener listener[CAMERA_EVENT_MAX_LISTENERS];
    int listener_count;
    pthread_mutex_t mutex;
} EventBus;

int event_bus_init(
    EventBus *bus);

void event_bus_deinit(
    EventBus *bus);

int event_bus_subscribe(
    EventBus *bus,
    CameraEventHandler handler,
    void *user_data);

int event_bus_publish(
    EventBus *bus,
    const CameraEvent *event);

void camera_event_init(
    CameraEvent *event,
    CameraEventType type);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
