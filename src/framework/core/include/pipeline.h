/*******************************************************************************
  * @file        pipeline.h
  * @brief       h File
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


#ifndef __PIPELINE_H_
#define __PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "pipeline_node.h"

int pipeline_add_output(
    PipelineNode *node,
    PipelineNode *output);

int pipeline_start(
    PipelineNode *node);

int pipeline_stop(
    PipelineNode *node);

int pipeline_push_frame(
    PipelineNode *node,
    Frame *frame);

void pipeline_destroy(
    PipelineNode *node);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
