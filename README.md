# Linux virtual kernel driver

A learning project for Linux character device development, using kernel C and C++ userspace programs. The aim is to build a foundation I can understand and reuse in future projects that need a character device.

The device is virtual, so no physical hardware is required. Instead, a data producer service simulates hardware by reading samples from a file and sending them to `/dev/streamdev0` through the `write()` system call. A separate reader consumes the data through `poll()` and `read()`. The development environment is an ARM64 Linux guest running in QEMU, built with Buildroot.

## How it works

```text
/data.raw → stream_producer → write() → /dev/streamdev0
                                           │
                                     4096-byte FIFO
                                           │
                              poll() / read() → stream_reader
```

- `stream_device.ko` registers a virtual platform device named `stream-device`.
- `stream_driver.ko` binds to it and creates the character device `/dev/streamdev0`.
- `stream_producer` reads signed 16-bit samples from `/data.raw` and writes one sample approximately every 100 ms (10 samples per second).
- The driver copies incoming bytes into a `kfifo`, protects FIFO transfers with a spinlock, and wakes readers waiting for data.
- Blocking reads wait when the FIFO is empty. Nonblocking reads return `EAGAIN`, and `poll()` reports when data is available.
- `stream_reader` opens the device with `O_NONBLOCK`, waits with `poll()`, and prints the samples. Each open file has its own count of bytes read, reported in the kernel log when it closes.

The learning topics include platform device and driver matching, character device registration, file operations, kernel/userspace data transfer, synchronization, wait queues, and resource cleanup. The current implementation uses a userspace producer rather than a kernel timer to simulate incoming hardware data.

The platform driver allocates 4096 bytes of storage for the FIFO queue. It has no knowledge of how many bytes a sample is. If the producer writes 4-byte samples, then the reader has to interpret 4-byte samples as well. For future use, this needs to be adapted depending on the data that is used.

## Requirements

- A Linux host with Bash, Git, Make, a C/C++ compiler, and the host utilities required to build Buildroot.
- Internet access for downloading Buildroot, toolchain, kernel, and package sources.
- Enough disk space and time for a full toolchain and Linux image build.
- `qemu-system-aarch64`, installed on the host or built by Buildroot.

The supplied configuration uses AArch64, Linux **6.18.7**, a C++ toolchain, systemd, and an ext4 root filesystem. Setup clones Buildroot's default branch without pinning a revision. Both `code/compile.sh` and `scripts/build.sh` contain paths tied to kernel version `6.18.7`; update them together if changing the kernel version.

## Build

Run the scripts from the directories shown: they derive repository paths from the current working directory.

Starting at the repository root, configure Buildroot and perform the initial build:

```sh
cd scripts
./setup.sh
cd ../buildroot
make
```

Once the kernel and cross compiler have been built, compile the modules and userspace programs and install them into the guest image:

```sh
cd ../scripts
./build.sh
```

This installs the modules under `/lib/modules/6.18.7/extra/`, the reader and producer under `/usr/bin/`, and the sample file at `/data.raw`. It also installs the producer's systemd unit, module loading configuration, and a udev rule granting the `stream` group access to the device, then rebuilds the image.

Build outputs include:

- `code/build/stream_device.ko` and `code/build/stream_driver.ko`
- `code/build/stream_reader` and `code/build/stream_producer`
- `buildroot/output/images/Image` and `buildroot/output/images/rootfs.ext4`

## Run in QEMU

From the repository's `scripts/` directory:

```sh
./start-qemu.sh
```

If using QEMU supplied by Buildroot, add it to the command's search path:

```sh
PATH="$(pwd)/../buildroot/output/host/bin:$PATH" ./start-qemu.sh
```

The guest uses QEMU's `virt` machine, a Cortex-A53 CPU, 512 MB of RAM, and a serial console. Log in as `root`; the supplied configuration does not set a root password. The modules and producer service are loaded during boot. Start the reader program:

```sh
/usr/bin/stream_reader
```

The reader prints `Opened /dev/streamdev0`, followed by `sample: <value>` lines from the input file. It prints `poll timeout` when no data arrives within five seconds. The producer stops at the end of the file; the reader continues waiting because the device does not signal end of input.

Press `Ctrl+C` to stop the reader. Before unloading either module, stop the producer and close all readers and other device users:

```sh
systemctl stop stream_producer.service
rmmod stream_driver
rmmod stream_device
```

Shut down the guest with `poweroff`, or exit QEMU with `Ctrl+A`, then `X`.
