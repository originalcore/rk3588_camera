/*******************************************************************************
  * @file        rtsp_node.c
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

#include "pipeline_node.h"

#include <stdio.h>
#include <stdlib.h>

static int rtsp_start(
    PipelineNode *node)
{
    (void)node;

    return 0;
}

static int rtsp_stop(
    PipelineNode *node)
{
    (void)node;

    return 0;
}

static int rtsp_process(
    PipelineNode *node,
    Frame *frame)
{
    (void)node;

    printf("rtsp frame size=%zu\n", frame->size);

    return 0;
}

static void rtsp_destroy(
    PipelineNode *node)
{
    free(node);
}

PipelineNode *rtsp_node_create(void)
{
    PipelineNode *node;

    node = calloc(1, sizeof(*node));
    if (!node)
        return NULL;

    node->start = rtsp_start;
    node->stop = rtsp_stop;
    node->process = rtsp_process;
    node->destroy = rtsp_destroy;

    return node;
}
