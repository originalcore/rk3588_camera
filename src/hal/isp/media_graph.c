/*******************************************************************************
  * @file        media_graph.c
  * @brief       Media controller graph abstraction
*******************************************************************************/

#include "media_graph.h"

#include <string.h>

int media_graph_init(
    MediaGraph *graph,
    const MediaGraphConfig *config)
{
    if (!graph ||
        !config)
        return -1;

    memset(graph, 0, sizeof(*graph));
    graph->config = *config;

    return 0;
}

int media_graph_setup_links(
    MediaGraph *graph)
{
    if (!graph)
        return -1;

    /*
     * Future implementation:
     * media_device -> enumerate entities -> MEDIA_IOC_SETUP_LINK
     * sensor -> mipi -> cif -> rkisp
     */
    graph->configured = 1;

    return 0;
}

void media_graph_deinit(
    MediaGraph *graph)
{
    if (!graph)
        return;

    graph->configured = 0;
}
