/*******************************************************************************
  * @file        pipeline.c
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

#include "pipeline.h"

int pipeline_start(
    PipelineNode *node)
{
    while (node)
    {
        if (node->start &&
            node->start(node) < 0)
            return -1;

        node = node->next;
    }

    return 0;
}

int pipeline_stop(
    PipelineNode *node)
{
    int ret = 0;

    while (node)
    {
        if (node->stop &&
            node->stop(node) < 0)
            ret = -1;

        node = node->next;
    }

    return ret;
}

int pipeline_push_frame(
    PipelineNode *node,
    Frame *frame)
{
    while (node)
    {

        if (!node->process ||
            node->process(node, frame) < 0)
            return -1;

        node = node->next;
    }

    return 0;
}

void pipeline_destroy(
    PipelineNode *node)
{
    pipeline_stop(node);

    while (node)
    {
        PipelineNode *next = node->next;

        node->next = NULL;

        if (node->destroy)
            node->destroy(node);

        node = next;
    }
}
