/*******************************************************************************
  * @file        message.c
  * @brief       Core message definitions
*******************************************************************************/

#include "message.h"

#include <string.h>

void camera_message_init(
    CameraMessage *message,
    CameraMessageType type)
{
    if (!message)
        return;

    memset(message, 0, sizeof(*message));
    message->type = type;
}
