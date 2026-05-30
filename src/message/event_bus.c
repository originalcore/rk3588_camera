/*******************************************************************************
  * @file        event_bus.c
  * @brief       Camera event bus
*******************************************************************************/

#include "event_bus.h"

#include <string.h>

int event_bus_init(
    EventBus *bus)
{
    if (!bus)
        return -1;

    memset(bus, 0, sizeof(*bus));

    if (pthread_mutex_init(&bus->mutex, NULL) != 0)
        return -1;

    return 0;
}

void event_bus_deinit(
    EventBus *bus)
{
    if (!bus)
        return;

    pthread_mutex_destroy(&bus->mutex);
}

int event_bus_subscribe(
    EventBus *bus,
    CameraEventHandler handler,
    void *user_data)
{
    if (!bus ||
        !handler)
        return -1;

    pthread_mutex_lock(&bus->mutex);

    if (bus->listener_count >= CAMERA_EVENT_MAX_LISTENERS)
    {
        pthread_mutex_unlock(&bus->mutex);
        return -1;
    }

    bus->listener[bus->listener_count].handler = handler;
    bus->listener[bus->listener_count].user_data = user_data;
    bus->listener_count++;

    pthread_mutex_unlock(&bus->mutex);

    return 0;
}

int event_bus_publish(
    EventBus *bus,
    const CameraEvent *event)
{
    CameraEventListener listener[CAMERA_EVENT_MAX_LISTENERS];
    int listener_count;

    if (!bus ||
        !event)
        return -1;

    pthread_mutex_lock(&bus->mutex);
    listener_count = bus->listener_count;
    memcpy(listener,
           bus->listener,
           sizeof(listener[0]) * listener_count);
    pthread_mutex_unlock(&bus->mutex);

    for (int i = 0; i < listener_count; i++)
        listener[i].handler(event, listener[i].user_data);

    return 0;
}

void camera_event_init(
    CameraEvent *event,
    CameraEventType type)
{
    if (!event)
        return;

    memset(event, 0, sizeof(*event));
    event->type = type;
}
