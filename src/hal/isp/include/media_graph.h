/*******************************************************************************
  * @file        media_graph.h
  * @brief       Media controller graph abstraction
*******************************************************************************/

#ifndef __MEDIA_GRAPH_H_
#define __MEDIA_GRAPH_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MEDIA_GRAPH_NAME_LEN 64

typedef struct
{
    char media_device[MEDIA_GRAPH_NAME_LEN];
    char sensor_entity[MEDIA_GRAPH_NAME_LEN];
    char mipi_entity[MEDIA_GRAPH_NAME_LEN];
    char cif_entity[MEDIA_GRAPH_NAME_LEN];
    char isp_entity[MEDIA_GRAPH_NAME_LEN];
} MediaGraphConfig;

typedef struct
{
    MediaGraphConfig config;
    int configured;
} MediaGraph;

int media_graph_init(
    MediaGraph *graph,
    const MediaGraphConfig *config);

int media_graph_setup_links(
    MediaGraph *graph);

void media_graph_deinit(
    MediaGraph *graph);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
