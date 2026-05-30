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
#include "version.h"
#include "camera_manager.h"
#include "pipeline.h"
#include "pipeline_node_factory.h"

#include <linux/videodev2.h>
#include <stdio.h>

void print_version(void)
{
    printf("\n\n================================================================================\n");
    printf("********************************************************************************\n");
    printf ("%s\n", version_string);
    printf("********************************************************************************\n");
    printf("================================================================================\n\n");
}

int main(int argc, char *argv[])
{
    CameraManager *mgr;

    Camera *front_cam;

    PipelineNode *encoder;

    PipelineNode *rtsp;

    CameraConfig config = {
        .device = "/dev/video0",
        .width = 1920,
        .height = 1080,
        .pixfmt = V4L2_PIX_FMT_NV12,
        .buffer_count = 4,
    };

    (void)argc;
    (void)argv;

    print_version();

    mgr = camera_manager_create();
    if (!mgr)
    {
        fprintf(stderr, "failed to create camera manager\n");
        return 1;
    }

    front_cam = camera_create(&config);
    if (!front_cam)
    {
        fprintf(stderr, "failed to open camera device\n");
        camera_manager_destroy(mgr);
        return 1;
    }

    if (camera_start(front_cam) < 0)
    {
        fprintf(stderr, "failed to start camera device\n");
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        return 1;
    }

    encoder = encoder_node_create();
    rtsp = rtsp_node_create();
    if (!encoder || !rtsp)
    {
        fprintf(stderr, "failed to create pipeline nodes\n");
        pipeline_destroy(encoder);
        pipeline_destroy(rtsp);
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        return 1;
    }

    encoder->next = rtsp;

    camera_set_pipeline(front_cam, encoder);

    if (camera_manager_add(mgr, front_cam) < 0)
    {
        fprintf(stderr, "failed to add camera\n");
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        return 1;
    }

    front_cam = NULL;

    while (1)
    {

        Camera *cam = camera_manager_get(mgr, 0);

        if (!cam || camera_poll(cam) < 0)
        {
            fprintf(stderr, "failed to poll camera\n");
            break;
        }
    }

    camera_manager_destroy(mgr);

    return 0;
}
