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
#include "camera_log.h"
#include "pipeline.h"
#include "pipeline_node_factory.h"

#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    const char *log_conf;
    CameraConfig camera;
} AppConfig;

static void print_version(void)
{
    CAMERA_LOG_INFO("******************************************************************");
    CAMERA_LOG_INFO("%s", version_string);
    CAMERA_LOG_INFO("******************************************************************\n");
}

static void print_usage(
    const char *program)
{
    printf("Usage: %s [--log-conf PATH] [--device PATH]\n",
           program);
    printf("  --log-conf PATH  zlog config file path, default: conf/zlog.conf\n");
    printf("  --device PATH    V4L2 device node, default: /dev/video0\n");
    printf("  --help           show this help\n");
}

static int parse_args(
    int argc,
    char *argv[],
    AppConfig *app_config,
    const char **error)
{
    app_config->log_conf = "conf/zlog.conf";
    app_config->camera.device = "/dev/video0";
    app_config->camera.width = 1920;
    app_config->camera.height = 1080;
    app_config->camera.pixfmt = V4L2_PIX_FMT_NV12;
    app_config->camera.buffer_count = 4;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--log-conf") == 0)
        {
            if (++i >= argc ||
                argv[i][0] == '\0' ||
                argv[i][0] == '-')
            {
                *error = "--log-conf requires a config file path";
                return -1;
            }

            app_config->log_conf = argv[i];
        }
        else if (strcmp(argv[i], "--device") == 0)
        {
            if (++i >= argc ||
                argv[i][0] == '\0' ||
                argv[i][0] == '-')
            {
                *error = "--device requires a V4L2 device path";
                return -1;
            }

            app_config->camera.device = argv[i];
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            print_usage(argv[0]);
            return 1;
        }
        else
        {
            *error = "unknown argument";
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    CameraManager *mgr;

    Camera *front_cam;

    PipelineNode *encoder;

    PipelineNode *rtsp;

    AppConfig app_config;

    const char *arg_error = NULL;

    int ret;

    ret = parse_args(argc,
                     argv,
                     &app_config,
                     &arg_error);
    if (ret > 0)
        return 0;
    if (ret < 0)
    {
        if (arg_error)
            fprintf(stderr, "Error: %s\n", arg_error);

        print_usage(argv[0]);
        return 1;
    }

    if (camera_log_init(app_config.log_conf) < 0)
        return 1;

    mgr = camera_manager_create();
    if (!mgr)
    {
        CAMERA_LOG_ERROR("failed to create camera manager");
        camera_log_fini();
        return 1;
    }

    print_version();

    front_cam = camera_create(&app_config.camera);
    if (!front_cam)
    {
        CAMERA_LOG_ERROR("failed to open camera device");
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    encoder = encoder_node_create();
    rtsp = rtsp_node_create();
    if (!encoder || !rtsp)
    {
        CAMERA_LOG_ERROR("failed to create pipeline nodes");
        pipeline_destroy(encoder);
        pipeline_destroy(rtsp);
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    if (pipeline_add_output(encoder, rtsp) < 0)
    {
        CAMERA_LOG_ERROR("failed to link pipeline nodes");
        pipeline_destroy(encoder);
        pipeline_destroy(rtsp);
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    if (camera_set_pipeline(front_cam, encoder) < 0)
    {
        CAMERA_LOG_ERROR("failed to set camera pipeline");
        pipeline_destroy(encoder);
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    if (camera_start(front_cam) < 0)
    {
        CAMERA_LOG_ERROR("failed to start camera device");
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    if (camera_manager_add(mgr, front_cam) < 0)
    {
        CAMERA_LOG_ERROR("failed to add camera");
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    front_cam = NULL;

    while (1)
    {

        Camera *cam = camera_manager_get(mgr, 0);

        if (!cam || camera_poll(cam) < 0)
        {
            CAMERA_LOG_ERROR("failed to poll camera");
            break;
        }
    }

    camera_manager_destroy(mgr);
    camera_log_fini();

    return 0;
}
