/*******************************************************************************
  * @file        hdr_controller.h
  * @brief       HDR mode controller abstraction
*******************************************************************************/

#ifndef __HDR_CONTROLLER_H_
#define __HDR_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    HDR_MODE_OFF = 0,
    HDR_MODE_2_FRAME,
    HDR_MODE_3_FRAME
} HdrMode;

typedef struct
{
    HdrMode mode;
    int enabled;
} HdrController;

int hdr_controller_init(
    HdrController *controller,
    HdrMode mode);

int hdr_controller_apply(
    HdrController *controller);

void hdr_controller_deinit(
    HdrController *controller);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
