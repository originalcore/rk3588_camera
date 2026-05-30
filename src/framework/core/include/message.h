/*******************************************************************************
  * @file        message.h
  * @brief       Core message definitions
*******************************************************************************/

#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"

typedef enum
{
    CAMERA_MSG_NONE = 0,
    CAMERA_MSG_START,
    CAMERA_MSG_STOP,
    CAMERA_MSG_FRAME,
    CAMERA_MSG_ERROR,
    CAMERA_MSG_EXIT
} CameraMessageType;

typedef struct
{
    CameraMessageType type;
    Frame frame;
    int code;
    void *data;
} CameraMessage;

void camera_message_init(
    CameraMessage *message,
    CameraMessageType type);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
