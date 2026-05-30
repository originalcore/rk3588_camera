# Camera Framework Architecture

Date: 2026-05-30

This document uses Mermaid diagrams. Markdown viewers with Mermaid support can render the architecture diagrams directly.

## 1. Layered Architecture

```mermaid
flowchart TB
    APP[src/app<br/>main.c] --> FW[src/framework]

    FW --> CORE[framework/core<br/>Camera<br/>CameraManager<br/>Pipeline<br/>FrameQueue]
    FW --> PN[framework/pipeline<br/>encoder_node<br/>rtsp_node<br/>capture_node]
    FW --> CB[framework/callback<br/>FrameListener]

    CORE --> HAL[src/hal]
    CORE --> THREAD[src/thread<br/>ThreadPool<br/>Worker]
    CORE --> MSG[src/message<br/>EventBus<br/>CameraEvent]

    HAL --> V4L2[hal/v4l2<br/>V4L2Device<br/>MediaController]
    HAL --> DMA[hal/dma<br/>dma_heap<br/>dmabuf_export<br/>dmabuf_import<br/>dmabuf_sync]
    HAL --> ISP[hal/isp<br/>rkisp_bridge<br/>media_graph<br/>sensor_manager<br/>aiq_controller<br/>hdr_controller]

    APP --> UTIL[src/utils/log<br/>zlog wrapper]
    UTIL --> ZLOG[3rdparty/zlog]

    PN --> CODEC[src/codec]
    PN --> NET[src/network]
    PN --> AI[src/ai]
```

## 2. Runtime Data Flow

```mermaid
flowchart LR
    DEV[/Camera Sensor/] --> ISP[ISP HAL<br/>sensor/media/rkisp/aiq]
    ISP --> V4L2[V4L2 Capture]
    V4L2 --> DQ[VIDIOC_DQBUF]
    DQ --> FRAME[Frame<br/>dma_fd<br/>vaddr<br/>size<br/>format]
    FRAME --> QUEUE[FrameQueue]
    QUEUE --> TP[ThreadPool job]
    TP --> GRAPH[Graph Pipeline]
    GRAPH --> ENC[Encoder Node]
    GRAPH --> RTSP[RTSP Node]
    GRAPH --> AI_NODE[AI Node future]
    GRAPH --> REC[Recorder Node future]
    RTSP --> OUT[Network Output]
    TP --> QBUF[VIDIOC_QBUF]
    QBUF --> V4L2
```

## 3. Graph Pipeline

```mermaid
flowchart TD
    CAP[Capture / ISP Output Node] --> ISP_NODE[ISP Node]

    ISP_NODE --> RTSP[RTSP Output]
    ISP_NODE --> AI[AI Inference]
    ISP_NODE --> REC[Recorder]
    ISP_NODE --> ENC[Encoder]

    ENC --> RTSP
    ENC --> REC

    subgraph PipelineNode
        P[process node frame]
        O[outputs 0..7]
    end
```

Pipeline nodes use:

```c
PipelineNode *outputs[PIPELINE_MAX_OUTPUTS];
int output_num;
```

`pipeline_add_output()` connects nodes. Traversal uses a visited set so shared nodes are not started, stopped, processed, or destroyed more than once per traversal.

## 4. Frame And DMA-BUF Model

```mermaid
flowchart LR
    V4L2[V4L2] --> FD[dma_fd]
    RGA[RGA future] --> FD
    MPP[MPP future] --> FD
    RKNN[RKNN future] --> FD

    FD --> FRAME[Frame]
    VA[vaddr optional CPU mapping] --> FRAME

    DMAHEAP[dma_heap] --> FD
    EXPORT[dmabuf_export] --> FD
    IMPORT[dmabuf_import] --> VA
    SYNC[dmabuf_sync] --> FD
```

Current `Frame` contract:

```c
typedef struct {
    int dma_fd;
    void *vaddr;
    size_t size;
    int width;
    int height;
    uint32_t pixfmt;
    uint64_t timestamp;
    int index;
} Frame;
```

The current V4L2 implementation still uses MMAP, so `dma_fd` is `-1` and `vaddr` points to the mapped V4L2 buffer. The HAL is prepared for shared fd pipelines.

## 5. ISP HAL

```mermaid
flowchart TB
    SENSOR_CFG[SensorConfig<br/>OV13855 / IMX415 / IMX462 via config] --> SENSOR[sensor_manager]
    MEDIA_CFG[MediaGraphConfig] --> MG[media_graph]
    AIQ_CFG[AiqConfig] --> AIQ[aiq_controller]
    HDR_CFG[HdrMode] --> HDR[hdr_controller]

    SENSOR --> BRIDGE[rkisp_bridge]
    MG --> BRIDGE
    AIQ --> BRIDGE
    HDR --> BRIDGE

    BRIDGE --> PATH[sensor -> mipi -> cif -> rkisp -> rkaiq]
```

Sensor-specific values belong in `SensorConfig` and `MediaGraphConfig`; framework code should not change when adding a new sensor model.

## 6. Thread And Event Model

```mermaid
sequenceDiagram
    participant Main as main loop
    participant Camera as Camera
    participant Queue as FrameQueue
    participant Pool as ThreadPool
    participant Bus as EventBus
    participant Pipe as Graph Pipeline
    participant V4L2 as V4L2

    Main->>Camera: camera_poll()
    Camera->>V4L2: capture frame
    V4L2-->>Camera: Frame
    Camera->>Queue: push Frame
    Camera->>Pool: submit pipeline job
    Pool->>Queue: pop Frame
    Pool->>Bus: CAMERA_EVENT_FRAME_READY
    Pool->>Pipe: pipeline_push_frame()
    Pipe-->>Pool: processed
    Pool->>V4L2: VIDIOC_QBUF
```

Events currently modeled:

```text
CAMERA_EVENT_FRAME_READY
CAMERA_EVENT_LOST
CAMERA_EVENT_STREAM_ON
CAMERA_EVENT_STREAM_OFF
```

## 7. Build-Time Module Tree

```mermaid
flowchart TB
    TOP[Makefile] --> THIRD[3rdparty]
    TOP --> SRC[src]

    THIRD --> Z[zlog/src]

    SRC --> APP[app]
    SRC --> FRAMEWORK[framework]
    SRC --> HAL[hal]
    SRC --> UTILS[utils]
    SRC --> THREAD[thread]
    SRC --> MESSAGE[message]
    SRC --> CODEC[codec]
    SRC --> NETWORK[network]
    SRC --> AI[ai]

    FRAMEWORK --> CORE[core]
    FRAMEWORK --> CALLBACK[callback]
    FRAMEWORK --> PIPELINE[pipeline]

    HAL --> DMA[dma]
    HAL --> ISP[isp]
    HAL --> V4L2[v4l2]
```
