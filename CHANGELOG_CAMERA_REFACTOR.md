# Camera Code Adjustment Record

Date: 2026-05-30

This document records the adjustments made to the code in the current directory.

## 1. Build Fixes

Initial `make` failed because `src/app/main.c` referenced project types and APIs that were not included or did not match the existing module interfaces.

Changes:

- Added required includes in `src/app/main.c`:
  - `camera.h`
  - `camera_manager.h`
  - `pipeline_node_factory.h`
- Replaced non-existent APIs with existing APIs:
  - `camera_manager_create()` was replaced initially with local `CameraManager` initialization.
  - `camera_create()` was replaced initially with local `Camera` initialization and `camera_open()`.
  - `h264_encoder_node_create()` was replaced with `encoder_node_create()`.
  - `camera_manager_add_camera()` was replaced with `camera_manager_add()`.
- Removed direct use of `camera_set_format()` from `main.c`; camera format setup belongs to `camera_open()` in the current architecture.
- Added standard type includes to `src/core/include/frame.h`:
  - `<stddef.h>`
  - `<stdint.h>`

## 2. Completed Broken Modules

Several files were incomplete or had incorrect content that prevented compilation.

### `src/pipeline/rtsp_node.c`

The file was truncated at an incomplete function declaration.

Changes:

- Added `rtsp_process()`.
- Added `rtsp_node_create()`.
- Matched the existing `PipelineNode` processing model used by `encoder_node.c`.

### `src/v4l2/v4l2_device.c`

The file was missing the beginning of the V4L2 implementation.

Changes:

- Added required system headers:
  - `<fcntl.h>`
  - `<linux/videodev2.h>`
  - `<string.h>`
  - `<sys/ioctl.h>`
  - `<sys/mman.h>`
  - `<unistd.h>`
- Implemented:
  - `v4l2_device_open()`
  - `v4l2_device_set_format()`
- Kept buffer request, stream start, capture, and queue responsibilities in this module.

### `src/v4l2/media_controller.c`

The file contained duplicated and incomplete V4L2 buffer logic that belonged in `v4l2_device.c`.

Changes:

- Rebuilt the file to match `media_controller.h`.
- Implemented:
  - `media_controller_open()`
  - `media_controller_setup_link()`
- Removed duplicated V4L2 capture functions from this module.

### `src/v4l2/include/media_controller.h`

Changes:

- Fixed broken include guard nesting.

## 3. Runtime Logic Fixes

After compilation succeeded, the code was reviewed for runtime correctness.

Changes:

- `camera_open()` now checks each V4L2 setup step and returns failure instead of ignoring errors.
- `camera_poll()` now stops on capture, pipeline, or requeue failure.
- `camera_add_listener()` now checks:
  - null camera pointer
  - null listener pointer
  - listener array overflow
- `camera_manager_add()` now checks:
  - null manager pointer
  - null camera pointer
  - camera array overflow
- `camera_manager_get()` now checks:
  - null manager pointer
  - negative index
  - out-of-range index
- `pipeline_push_frame()` now checks:
  - null `process` callback
  - negative return from node processing
- `encoder_node.c` and `rtsp_node.c` now print `size_t` with `%zu`.

## 4. V4L2 Resource Handling Improvements

Changes in `src/v4l2/v4l2_device.c` and `src/v4l2/include/v4l2_device.h`:

- Added `v4l2_device_close()`.
- Added internal buffer cleanup helper.
- Added buffer count validation against `MAX_BUFFER_COUNT`.
- Added `VIDIOC_QUERYBUF` failure handling.
- Added `mmap()` failure handling.
- Added `VIDIOC_QBUF` failure handling.
- Added captured buffer index validation after `VIDIOC_DQBUF`.
- Filled additional `Frame` metadata:
  - `pixfmt`
  - `timestamp`
- Added queued buffer index validation in `v4l2_device_queue()`.

## 5. Unified Lifecycle Management

A second refactor introduced explicit ownership and destroy paths.

### Ownership Model

- `CameraManager` owns registered `Camera` instances.
- `Camera` owns its `PipelineNode` chain.
- Each `PipelineNode` owns its private data and releases itself through its `destroy` callback.
- `pipeline_destroy()` releases an entire node chain.

### Camera Manager

Files:

- `src/core/include/camera_manager.h`
- `src/core/camera_manager.c`

Added:

- `CameraManager *camera_manager_create(void)`
- `void camera_manager_destroy(CameraManager *mgr)`

Behavior:

- `camera_manager_destroy()` destroys every registered camera and then frees the manager.

### Camera

Files:

- `src/core/include/camera.h`
- `src/core/camera.c`

Added:

- `Camera *camera_create(const char *dev)`
- `void camera_destroy(Camera *cam)`

Behavior:

- `camera_create()` allocates a `Camera` and opens/configures the device.
- `camera_destroy()` closes the V4L2 device, destroys the pipeline, clears state, and frees the camera.
- `camera_set_pipeline()` destroys the existing pipeline before replacing it.
- `camera_close()` now also destroys the camera pipeline.

### Pipeline

Files:

- `src/core/include/pipeline.h`
- `src/core/pipeline.c`
- `src/pipeline/include/pipeline_node.h`

Added:

- `void pipeline_destroy(PipelineNode *node)`
- `PipelineNode.destroy` callback

Behavior:

- `pipeline_destroy()` walks the linked list and calls each node's `destroy` callback.

### Pipeline Nodes

Files:

- `src/pipeline/encoder_node.c`
- `src/pipeline/rtsp_node.c`
- `src/pipeline/include/pipeline_node_factory.h`

Changes:

- `encoder_node_create()` now dynamically allocates a node with `calloc()`.
- `rtsp_node_create()` now dynamically allocates a node with `calloc()`.
- Static singleton nodes were removed.
- Each node provides a destroy callback that calls `free()`.
- Factory prototypes were changed to explicit `void` parameter lists.
- Fixed typo in the pipeline node factory include guard:
  - from `__IPELINE_NODE_FACTORY_H_`
  - to `__PIPELINE_NODE_FACTORY_H_`

## 6. Main Application Flow

File:

- `src/app/main.c`

Changes:

- Uses `camera_manager_create()`.
- Uses `camera_create("/dev/video0")`.
- Creates pipeline nodes dynamically:
  - encoder node
  - RTSP node
- Links pipeline with `encoder->next = rtsp`.
- Gives pipeline ownership to the camera through `camera_set_pipeline()`.
- Gives camera ownership to the manager through `camera_manager_add()`.
- Polls the camera through `camera_manager_get(mgr, 0)`.
- Releases all resources through `camera_manager_destroy(mgr)`.
- Failure paths now clean up already-created resources.

## 7. Git Repository Setup

The normal `.git` path in the current directory is mounted as read-only by the environment, so standard `git init` cannot write there.

Workaround:

- A separate git directory was initialized at `.git-repo`.
- The working tree remains the current directory.

Use these commands for git operations:

```sh
GIT_DIR=.git-repo GIT_WORK_TREE=. git status
GIT_DIR=.git-repo GIT_WORK_TREE=. git log --oneline
GIT_DIR=.git-repo GIT_WORK_TREE=. git diff
```

Commits created:

```text
0582494 Baseline compilable camera framework
fec02e8 Add unified lifecycle management
```

## 8. Build And Verification

Commands run:

```sh
make
```

Result:

```text
camera run in i386 platform compiled success.
```

Additional warning-oriented syntax check:

```sh
gcc -Wall -Wextra \
    -I version \
    -I src/app/include \
    -I src/callback/include \
    -I src/core/include \
    -I src/dma/include \
    -I src/isp/include \
    -I src/pipeline/include \
    -I src/v4l2/include \
    -fsyntax-only \
    src/app/main.c \
    src/callback/frame_listener.c \
    src/core/camera.c \
    src/core/camera_manager.c \
    src/core/pipeline.c \
    src/dma/dmabuf_allocator.c \
    src/isp/rkisp_bridge.c \
    src/pipeline/capture_node.c \
    src/pipeline/encoder_node.c \
    src/pipeline/rtsp_node.c \
    src/v4l2/v4l2_device.c \
    src/v4l2/media_controller.c
```

Result:

- Passed with no output.

Generated binary:

```text
bin/camera
```

## 9. Remaining Recommended Improvements

## 10. Industrial Skeleton Step

The framework was further optimized toward an industrial skeleton.

Changes:

- Added `CameraConfig` so device path, width, height, pixel format, and buffer count are configured through one structure instead of being hardcoded inside `camera_open()`.
- Added `CameraState` with:
  - `CAMERA_STATE_CLOSED`
  - `CAMERA_STATE_OPEN`
  - `CAMERA_STATE_RUNNING`
  - `CAMERA_STATE_ERROR`
- Added state validation for open/start/stop/poll paths.
- Added `camera_stop()` and made `camera_close()` stop a running camera before releasing resources.
- Added V4L2 capability probing with `VIDIOC_QUERYCAP`.
- Added `v4l2_device_stop()` using `VIDIOC_STREAMOFF`.
- Extended `PipelineNode` with `start` and `stop` callbacks.
- Added `pipeline_start()` and `pipeline_stop()`.
- Updated encoder and RTSP nodes to implement no-op start/stop callbacks.
- Updated `main.c` to create the camera from `CameraConfig`.

## 11. zlog Integration

The project now vendors zlog from:

```text
https://github.com/HardySimpson/zlog.git
```

Changes:

- Added zlog source under `3rdparty/zlog`.
- Added `3rdparty/Makefile` so third-party code participates in the existing recursive build.
- Added `3rdparty/zlog/src/Makefile.camera` to compile zlog library objects into the project without building zlog's command-line tools.
- Added `src/log` module:
  - `src/log/include/camera_log.h`
  - `src/log/camera_log.c`
- Added `conf/zlog.conf`.
- Replaced application `printf/fprintf` logging with `CAMERA_LOG_*` macros backed by zlog.
- Replaced encoder and RTSP node debug prints with zlog debug logs.
- `main.c` now initializes logging with:

```c
camera_log_init("conf/zlog.conf");
```

Log level is controlled by `conf/zlog.conf`:

```text
camera.DEBUG >stdout; normal
camera.DEBUG "logs/camera.log", 10MB * 5; normal
```

Change `DEBUG` to `INFO`, `WARN`, `ERROR`, or `FATAL` to adjust runtime log verbosity through configuration.

These were not forced into the current change set because they affect broader design or runtime behavior:

- Add a real RTSP implementation or rename the current RTSP node to a debug/output node.
- Add signal handling in `main.c` so Ctrl-C exits through the normal destroy path.
- Track V4L2 streaming state explicitly to avoid redundant `VIDIOC_STREAMOFF`.
- Decide whether `CameraManager` should always own cameras or support non-owning registration.
- Remove or ignore IDE cache files under `camera.si4project/cache/` if they should not be versioned.
