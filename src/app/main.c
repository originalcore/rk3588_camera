/*******************************************************************************
  * @file        main.c
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

#include "camera.h"
#include "camera_manager.h"
#include "pipeline_node_factory.h"

#include <stdio.h>


int main(int argc, char *argv[])
{
    CameraManager mgr = {0};

    Camera front_cam = {0};

    PipelineNode *encoder;

    PipelineNode *rtsp;

    (void)argc;
    (void)argv;

    if (camera_open(&front_cam,
                    "/dev/video0") < 0)
    {
        fprintf(stderr,
                "failed to open camera device\n");
        return 1;
    }

    if (camera_start(&front_cam) < 0)
    {
        fprintf(stderr,
                "failed to start camera device\n");
        camera_close(&front_cam);
        return 1;
    }

    encoder = encoder_node_create();
    rtsp = rtsp_node_create();

    encoder->next = rtsp;

    camera_set_pipeline(&front_cam,
                        encoder);

    if (camera_manager_add(&mgr,
                           &front_cam) < 0)
    {
        fprintf(stderr,
                "failed to add camera\n");
        camera_close(&front_cam);
        return 1;
    }

    while (1)
    {

        if (camera_poll(&front_cam) < 0)
        {
            fprintf(stderr,
                    "failed to poll camera\n");
            break;
        }
    }

    camera_close(&front_cam);

    return 0;
}
