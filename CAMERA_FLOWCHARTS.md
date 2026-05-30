# Camera Framework Flowcharts

Date: 2026-05-30

This document uses Mermaid diagrams. Markdown viewers that support Mermaid can render these flowcharts directly.

## 1. Application Startup Flow

```mermaid
flowchart TD
    A[main] --> B[camera_manager_create]
    B -->|success| C[camera_create /dev/video0]
    B -->|failed| Z1[return 1]

    C -->|success| D[camera_start]
    C -->|failed| C1[camera_manager_destroy]
    C1 --> Z1

    D -->|success| E[encoder_node_create]
    D -->|failed| D1[camera_destroy]
    D1 --> D2[camera_manager_destroy]
    D2 --> Z1

    E --> F[rtsp_node_create]
    F -->|both success| G[encoder.next = rtsp]
    E -->|failed| E1[cleanup created nodes]
    F -->|failed| E1
    E1 --> E2[camera_destroy]
    E2 --> E3[camera_manager_destroy]
    E3 --> Z1

    G --> H[camera_set_pipeline]
    H --> I[camera_manager_add]
    I -->|success| J[poll loop]
    I -->|failed| I1[camera_destroy]
    I1 --> I2[camera_manager_destroy]
    I2 --> Z1

    J --> K[camera_manager_get index 0]
    K --> L[camera_poll]
    L -->|success| J
    L -->|failed| M[break loop]
    M --> N[camera_manager_destroy]
    N --> O[return 0]
```

## 2. Ownership And Lifecycle

```mermaid
flowchart TD
    M[CameraManager] -->|owns| C[Camera]
    C -->|owns| P1[PipelineNode: Encoder]
    P1 -->|next| P2[PipelineNode: RTSP]
    C -->|contains| V[V4L2Device]

    MD[camera_manager_destroy] --> CD[camera_destroy for each Camera]
    CD --> CC[camera_close]
    CC --> PD[pipeline_destroy]
    PD --> D1[encoder destroy callback]
    D1 --> D2[free encoder node]
    PD --> D3[rtsp destroy callback]
    D3 --> D4[free rtsp node]
    CC --> VD[v4l2_device_close]
    VD --> VB[munmap buffers]
    VD --> VF[close fd]
    CD --> CF[free Camera]
    MD --> MF[free CameraManager]
```

## 3. Camera Creation Flow

```mermaid
flowchart TD
    A[camera_create dev] --> B[calloc Camera]
    B -->|failed| Z[return NULL]
    B -->|success| C[camera_open]

    C --> D[v4l2_device_open]
    D -->|failed| F1[free Camera]
    F1 --> Z

    D -->|success| E[v4l2_device_set_format 1920x1080 NV12]
    E -->|failed| E1[v4l2_device_close]
    E1 --> F1

    E -->|success| G[v4l2_device_request_buffer 4]
    G -->|failed| G1[v4l2_device_close]
    G1 --> F1

    G -->|success| H[return Camera]
```

## 4. V4L2 Buffer Setup Flow

```mermaid
flowchart TD
    A[v4l2_device_request_buffer] --> B[validate count]
    B -->|invalid| Z[return -1]
    B -->|valid| C[VIDIOC_REQBUFS]
    C -->|failed| Z
    C -->|success| D[validate req.count]
    D -->|invalid| Z

    D -->|valid| E[for each buffer]
    E --> F[VIDIOC_QUERYBUF]
    F -->|failed| X[release mapped buffers]
    X --> Z

    F -->|success| G[mmap buffer]
    G -->|failed| X
    G -->|success| H[VIDIOC_QBUF]
    H -->|failed| X
    H -->|success| I{more buffers?}
    I -->|yes| E
    I -->|no| J[return 0]
```

## 5. Frame Poll And Pipeline Flow

```mermaid
flowchart TD
    A[camera_poll] --> B[v4l2_device_capture]
    B --> C[VIDIOC_DQBUF]
    C -->|failed| Z[return -1]
    C -->|success| D[validate buffer index]
    D -->|invalid| Z
    D -->|valid| E[fill Frame data]

    E --> F[camera_dispatch_frame]
    F --> G{pipeline exists?}
    G -->|no| K[v4l2_device_queue]
    G -->|yes| H[pipeline_push_frame]

    H --> I[encoder process]
    I -->|failed| Z
    I --> J[rtsp process]
    J -->|failed| Z
    J --> K

    K --> L[VIDIOC_QBUF]
    L -->|failed| Z
    L -->|success| M[return 0]
```

## 6. Pipeline Destroy Flow

```mermaid
flowchart TD
    A[pipeline_destroy node] --> B{node is NULL?}
    B -->|yes| Z[done]
    B -->|no| C[save next node]
    C --> D[clear node.next]
    D --> E{destroy callback exists?}
    E -->|yes| F[node.destroy node]
    E -->|no| G[skip destroy]
    F --> H[node = saved next]
    G --> H
    H --> B
```

## 7. Module Relationship Overview

```mermaid
flowchart LR
    APP[src/app/main.c] --> CM[camera_manager]
    APP --> CF[pipeline_node_factory]

    CM --> CAM[camera]
    CAM --> V4L2[v4l2_device]
    CAM --> PL[pipeline]
    CAM --> FL[frame_listener]

    PL --> PN[pipeline_node]
    CF --> ENC[encoder_node]
    CF --> RTSP[rtsp_node]
    ENC --> PN
    RTSP --> PN

    V4L2 --> FR[Frame]
    PN --> FR
    FL --> FR
```

