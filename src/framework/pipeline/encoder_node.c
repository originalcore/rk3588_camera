/*******************************************************************************
  * @file        encoder_node.c
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
#include "camera_log.h"

#include <stdlib.h>

static int encoder_start(
    PipelineNode *node)
{
    (void)node;

    return 0;
}

static int encoder_stop(
    PipelineNode *node)
{
    (void)node;

    return 0;
}

static int encoder_process(
    PipelineNode *node,
    Frame *frame)
{
    (void)node;

    CAMERA_LOG_DEBUG("encoder frame size=%zu", frame->size);

    return 0;
}

static void encoder_destroy(
    PipelineNode *node)
{
    free(node);
}

PipelineNode *encoder_node_create(void)
{
    PipelineNode *node;

    node = calloc(1, sizeof(*node));
    if (!node)
        return NULL;

    node->start = encoder_start;
    node->stop = encoder_stop;
    node->process = encoder_process;
    node->destroy = encoder_destroy;

    return node;
}
