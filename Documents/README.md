# Camera Framework

This project is a C camera framework skeleton based on V4L2 capture, a simple pipeline model, and zlog-based logging.

## Directory Layout

```text
.
├── Makefile                 # Top-level build entry
├── Makefile.build           # Recursive object build rule
├── bin/                     # Build output directory
├── conf/
│   └── zlog.conf            # Runtime logging configuration
├── logs/                    # Runtime log directory
├── src/
│   ├── app/                 # Application entry
│   ├── framework/           # Framework layer
│   │   ├── callback/        # Frame listener abstraction
│   │   ├── core/            # Camera, manager, pipeline core
│   │   └── pipeline/        # Pipeline node implementations
│   ├── hal/                 # Hardware abstraction layer
│   │   ├── dma/             # DMA heap and DMA-BUF fd sharing helpers
│   │   ├── isp/             # Sensor, media graph, RKISP, AIQ, HDR control
│   │   └── v4l2/            # V4L2 device wrapper
│   ├── utils/
│   │   └── log/             # zlog wrapper
│   ├── thread/              # Thread pool and worker abstraction
│   ├── message/             # Event bus and camera events
│   ├── codec/               # Codec extension module placeholder
│   ├── network/             # Network extension module placeholder
│   └── ai/                  # AI extension module placeholder
└── 3rdparty/
    └── zlog/                # Vendored zlog source
```

## Build Requirements

Required tools:

```sh
gcc
make
ld
ar
```

Required Linux headers:

```text
linux/videodev2.h
```

The default target is built for the current Linux host with `ARCH=i386` in `Makefile`. The build currently uses the host `gcc` unless `CROSS_COMPILE` is configured.

## Runtime Pipeline Model

The capture path uses an internal frame queue so slow encoder or network output does not block `camera_poll()` directly:

```text
V4L2 capture -> FrameQueue -> ThreadPool worker -> encoder -> rtsp -> V4L2 queue
```

`FrameQueue` is defined in `src/framework/core/include/buffer_queue.h` with 32 frame slots. Worker execution is provided by `src/thread`. Camera lifecycle and frame status notifications are published through `src/message` as events such as `CAMERA_EVENT_FRAME_READY`, `CAMERA_EVENT_LOST`, `CAMERA_EVENT_STREAM_ON`, and `CAMERA_EVENT_STREAM_OFF`.

The current camera pipeline uses one worker in the pipeline pool. The module boundary is prepared so future capture, encoder, RTSP, and AI work can be split into independent worker pools or dedicated threads.

Pipeline nodes use graph-style outputs rather than a single linear `next` pointer. A node can fan out one frame to multiple downstream nodes:

```text
capture -> isp -> rtsp
              -> ai
              -> recorder
```

`Frame` is structured around DMA-BUF sharing. The primary handle is `dma_fd`; `vaddr` is only the optional CPU mapping:

```c
typedef struct {
    int dma_fd;
    void *vaddr;
    size_t size;
    int width;
    int height;
    uint32_t pixfmt;
} Frame;
```

The current V4L2 path still uses MMAP buffers, so it sets `dma_fd = -1` and fills `vaddr`. The DMA HAL already provides `dma_heap`, `dmabuf_export`, `dmabuf_import`, and `dmabuf_sync` modules for the RK3588-style V4L2/RGA/MPP/RKNN shared-fd path.

The ISP HAL is split so sensor-specific work does not leak into the framework:

```text
sensor manager -> media graph -> rkisp bridge -> aiq controller
                                -> hdr controller
```

Sensor models such as OV13855, IMX415, and IMX462 should be selected through `SensorConfig` and media entity configuration instead of changing the framework code.

## Build

From the project root:

```sh
make
```

Expected success output:

```text
camera run in i386 platform compiled success.
```

Generated binary:

```text
bin/camera
```

Generated version header:

```text
version/version.h
```

## Clean

Basic clean:

```sh
make clean
```

Full clean:

```sh
make distclean
```

Note: the current `clean` rule is basic. If stale object files remain, use:

```sh
find . -name "*.o" -o -name "*.d" -o -name "built-in.o"
```

and remove stale build artifacts as needed.

## Deployment

Deploy at least these files/directories together:

```text
bin/camera
conf/zlog.conf
logs/
```

The application currently opens:

```text
/dev/video0
```

The camera parameters are currently configured in `src/app/main.c`:

```c
CameraConfig config = {
    .device = "/dev/video0",
    .width = 1920,
    .height = 1080,
    .pixfmt = V4L2_PIX_FMT_NV12,
    .buffer_count = 4,
};
```

Before running, make sure:

```sh
test -e /dev/video0
mkdir -p logs
```

Run from the project root:

```sh
./bin/camera
```

You can also specify the log config and camera device explicitly:

```sh
./bin/camera --log-conf conf/zlog.conf --device /dev/video0
```

Show command line help:

```sh
./bin/camera --help
```

Supported command line options:

```text
--log-conf PATH  zlog config file path, default: conf/zlog.conf
--device PATH    V4L2 device node, default: /dev/video0
--help           show command line help
```

Invalid arguments return non-zero and print an error plus usage. Examples:

```text
Error: unknown argument
Error: --log-conf requires a config file path
Error: --device requires a V4L2 device path
```

Example:

```sh
./bin/camera --log-conf conf/zlog.conf --device /dev/video0
```

If the selected device does not exist, the program exits with:

```text
failed to open camera device
```

Check available V4L2 devices:

```sh
ls -l /dev/video*
```

If deploying to another directory, keep this layout:

```text
deploy-root/
├── bin/camera
├── conf/zlog.conf
└── logs/
```

Run from `deploy-root`:

```sh
./bin/camera --log-conf conf/zlog.conf --device /dev/video0
```

## Logging

Logging is implemented with zlog. The application initializes logging with:

```c
camera_log_init("conf/zlog.conf");
```

The wrapper lives in:

```text
src/utils/log/include/camera_log.h
src/utils/log/camera_log.c
```

Use these macros in project code:

```c
CAMERA_LOG_DEBUG("message");
CAMERA_LOG_INFO("message");
CAMERA_LOG_WARN("message");
CAMERA_LOG_ERROR("message");
CAMERA_LOG_FATAL("message");
```

## zlog Configuration

Runtime logging config:

```text
conf/zlog.conf
```

Current config:

```text
[global]
strict init = true
buffer min = 1024
buffer max = 2MB
rotate lock file = self
default format = "%d(%F %T.%l) %-6V [%p] %c %f:%L - %m%n"

[formats]
normal = "%d(%F %T.%l) %-6V [%p] %c %f:%L - %m%n"

[rules]
camera.DEBUG >stdout; normal
camera.DEBUG "logs/camera.log", 10MB * 5; normal
```

### `[global]`

`strict init = true`

Requires all referenced categories and rules to initialize correctly. If the config is invalid, `camera_log_init()` fails and the application exits.

`buffer min = 1024`

Minimum internal log buffer size.

`buffer max = 2MB`

Maximum internal log buffer size.

`rotate lock file = self`

Uses zlog's internal/self lock behavior for log rotation.

`default format = "..."`

Default log output format if a rule does not specify another format.

### `[formats]`

Defines named log formats.

Current format:

```text
normal = "%d(%F %T.%l) %-6V [%p] %c %f:%L - %m%n"
```

Meaning:

- `%d(%F %T.%l)`: timestamp with date, time, and milliseconds
- `%-6V`: log level, left aligned
- `%p`: process id
- `%c`: zlog category
- `%f:%L`: source file and line number
- `%m`: log message
- `%n`: newline

### `[rules]`

Rules decide which category and level go to which output.

Current stdout rule:

```text
camera.DEBUG >stdout; normal
```

Meaning:

- Category: `camera`
- Minimum level: `DEBUG`
- Output: stdout
- Format: `normal`

Current file rule:

```text
camera.DEBUG "logs/camera.log", 10MB * 5; normal
```

Meaning:

- Category: `camera`
- Minimum level: `DEBUG`
- Output file: `logs/camera.log`
- Rotate when file reaches `10MB`
- Keep `5` rotated log files
- Format: `normal`

## Configure Log Level

Change the level in `conf/zlog.conf`.

For verbose debug logs:

```text
camera.DEBUG >stdout; normal
camera.DEBUG "logs/camera.log", 10MB * 5; normal
```

For normal production info logs:

```text
camera.INFO >stdout; normal
camera.INFO "logs/camera.log", 10MB * 5; normal
```

For warnings and errors only:

```text
camera.WARN >stdout; normal
camera.WARN "logs/camera.log", 10MB * 5; normal
```

For errors only:

```text
camera.ERROR >stdout; normal
camera.ERROR "logs/camera.log", 10MB * 5; normal
```

The category name must remain `camera` unless `camera_log_init()` is changed.

## Camera Configuration

Camera configuration defaults are currently defined in `src/app/main.c`.

The `device` field can be overridden at runtime:

```sh
./bin/camera --device /dev/video2
```

Other camera fields are still compiled into the default `CameraConfig`.

Fields:

```c
typedef struct
{
    const char *device;
    int width;
    int height;
    unsigned int pixfmt;
    int buffer_count;
} CameraConfig;
```

Meaning:

- `device`: V4L2 device node, for example `/dev/video0`; can be overridden by `--device`
- `width`: capture width
- `height`: capture height
- `pixfmt`: V4L2 pixel format, for example `V4L2_PIX_FMT_NV12`
- `buffer_count`: number of MMAP buffers, maximum `MAX_BUFFER_COUNT`

The current app does not yet parse width, height, pixel format, or buffer count from a file or CLI.

## Runtime Notes

- The process must have permission to open the V4L2 device node.
- The device must support `V4L2_CAP_VIDEO_CAPTURE`.
- The device must support `V4L2_CAP_STREAMING`.
- The requested pixel format and resolution must be supported by the device or driver.
- The current encoder and RTSP nodes are framework placeholders; they log frame sizes but do not encode or stream media yet.

## Git Notes

In this environment, the standard `.git` path is mounted read-only. This project uses a separate git directory:

```sh
GIT_DIR=.git-repo GIT_WORK_TREE=. git status
GIT_DIR=.git-repo GIT_WORK_TREE=. git log --oneline
```
