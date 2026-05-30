/*******************************************************************************
  * @file        pipeline_node.h
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

#ifndef __PIPELINE_NODE_H_
#define __PIPELINE_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "frame.h"

typedef struct PipelineNode PipelineNode;

struct PipelineNode
{

    int (*process)(PipelineNode *node, Frame *frame);

    void (*destroy)( PipelineNode *node);

    PipelineNode *next;

    void *private_data;
};



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
