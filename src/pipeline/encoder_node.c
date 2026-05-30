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

#include <stdio.h>

static int encoder_process(
    PipelineNode *node,
    Frame *frame)
{
    (void)node;

    printf("encoder frame size=%zu\n",
           frame->size);

    return 0;
}

PipelineNode *encoder_node_create()
{
    static PipelineNode node;

    node.process = encoder_process;

    node.next = NULL;

    return &node;
}
