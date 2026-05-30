/*******************************************************************************
  * @file        pipeline_node_factory.h
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

#ifndef __PIPELINE_NODE_FACTORY_H_
#define __PIPELINE_NODE_FACTORY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "pipeline_node.h"

PipelineNode *encoder_node_create(void);

PipelineNode *rtsp_node_create(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
