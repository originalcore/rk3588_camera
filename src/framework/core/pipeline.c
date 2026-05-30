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

#define PIPELINE_MAX_VISITED_NODES 64

typedef struct
{
    PipelineNode *node[PIPELINE_MAX_VISITED_NODES];
    int count;
} PipelineVisitContext;

static int pipeline_visit_mark(
    PipelineVisitContext *ctx,
    PipelineNode *node)
{
    if (!node)
        return 0;

    for (int i = 0; i < ctx->count; i++)
    {
        if (ctx->node[i] == node)
            return 0;
    }

    if (ctx->count >= PIPELINE_MAX_VISITED_NODES)
        return -1;

    ctx->node[ctx->count++] = node;

    return 1;
}

int pipeline_add_output(
    PipelineNode *node,
    PipelineNode *output)
{
    if (!node ||
        !output ||
        node->output_num >= PIPELINE_MAX_OUTPUTS)
        return -1;

    node->outputs[node->output_num++] = output;

    return 0;
}

static int pipeline_start_node(
    PipelineNode *node,
    PipelineVisitContext *ctx)
{
    int visit;

    visit = pipeline_visit_mark(ctx, node);
    if (visit <= 0)
        return visit;

    if (node->start &&
        node->start(node) < 0)
        return -1;

    for (int i = 0; i < node->output_num; i++)
    {
        if (pipeline_start_node(node->outputs[i], ctx) < 0)
            return -1;
    }

    return 0;
}

int pipeline_start(
    PipelineNode *node)
{
    PipelineVisitContext ctx = {0};

    if (!node)
        return 0;

    return pipeline_start_node(node, &ctx);
}

static int pipeline_stop_node(
    PipelineNode *node,
    PipelineVisitContext *ctx)
{
    int ret = 0;
    int visit;

    visit = pipeline_visit_mark(ctx, node);
    if (visit < 0)
        return -1;
    if (visit == 0)
        return 0;

    for (int i = 0; i < node->output_num; i++)
    {
        if (pipeline_stop_node(node->outputs[i], ctx) < 0)
            ret = -1;
    }

    if (node->stop &&
        node->stop(node) < 0)
        ret = -1;

    return ret;
}

int pipeline_stop(
    PipelineNode *node)
{
    PipelineVisitContext ctx = {0};

    if (!node)
        return 0;

    return pipeline_stop_node(node, &ctx);
}

static int pipeline_push_frame_node(
    PipelineNode *node,
    Frame *frame,
    PipelineVisitContext *ctx)
{
    int ret = 0;
    int visit;

    visit = pipeline_visit_mark(ctx, node);
    if (visit < 0)
        return -1;
    if (visit == 0)
        return 0;

    if (!node->process ||
        node->process(node, frame) < 0)
        return -1;

    for (int i = 0; i < node->output_num; i++)
    {
        if (pipeline_push_frame_node(node->outputs[i], frame, ctx) < 0)
            ret = -1;
    }

    return ret;
}

int pipeline_push_frame(
    PipelineNode *node,
    Frame *frame)
{
    PipelineVisitContext ctx = {0};

    if (!node ||
        !frame)
        return -1;

    return pipeline_push_frame_node(node, frame, &ctx);
}

static void pipeline_destroy_node(
    PipelineNode *node,
    PipelineVisitContext *ctx)
{
    int visit;

    visit = pipeline_visit_mark(ctx, node);
    if (visit <= 0)
        return;

    for (int i = 0; i < node->output_num; i++)
        pipeline_destroy_node(node->outputs[i], ctx);

    node->output_num = 0;

    if (node->destroy)
        node->destroy(node);
}

void pipeline_destroy(
    PipelineNode *node)
{
    PipelineVisitContext ctx = {0};

    pipeline_stop(node);
    pipeline_destroy_node(node, &ctx);
}
