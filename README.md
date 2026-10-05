# Linux virtual kernel driver

A learning project for Linux kernel development in C, with a C++ userspace reader. It runs an ARM64 Linux system in QEMU, built with Buildroot, so no physical device is required.

The current implementation simulates a streaming device:

- `stream_device.ko` registers a virtual platform device named `stream-device`.
- `stream_driver.ko` binds to that device and creates `/dev/streamdev0`.
- A kernel timer generates an increasing 32-bit sample roughly every 100 ms and stores it in a 4096-byte FIFO. Samples are dropped when the FIFO is full.
- The driver supports blocking reads, nonblocking reads (`EAGAIN` when empty), and `poll()` notifications through a wait queue.
- `stream_reader` opens the device in nonblocking mode, waits with `poll()`, and prints the samples.

Readers consume a shared FIFO; each open file also tracks its own byte count. The learning goals include moving from C++ to kernel C, understanding driver lifecycle and synchronization, and eventually exploring device trees and hardware interrupts. Currently, the device is registered by a module and sampling uses a timer.

## Requirements

- A Linux host with Bash, Git, GNU Make, GCC/G++, and standard Buildroot build utilities.
- Internet access for cloning Buildroot and downloading toolchain, kernel, and package sources.
- Disk space and time for a full toolchain and Linux image build; the first build is substantially longer than subsequent builds.
- `qemu-system-aarch64`, either installed on the host or supplied by Buildroot.

For example, on Debian/Ubuntu, install the host dependencies with:

```sh
sudo apt update
sudo apt install build-essential git wget which sed binutils diffutils \
  bash patch gzip bzip2 perl tar cpio unzip rsync file bc findutils gawk \
  python3 libssl-dev libncurses-dev qemu-system-arm
```

The supplied configuration targets AArch64, Linux **6.18.7**, a Buildroot C++ toolchain, systemd, and an ext4 root filesystem. `code/compile.sh` also hardcodes the Linux `6.18.7` build directory; update it if you change the configured kernel version. Setup clones Buildroot's default branch rather than pinning a revision.

## Build

Start in the repository root. The scripts derive paths from the current working directory, so run them from the directories shown below.

First, configure Buildroot and build the kernel, cross compiler, and root filesystem:

```sh
cd scripts
./setup.sh
cd ../buildroot
make
```

Then build the modules and reader and include them in the guest image:

```sh
cd ../scripts
./build.sh
```

`build.sh` runs `code/compile.sh`, copies both modules into the guest's `/` and the reader into `/usr/bin`, then rebuilds the image. Outputs include:

- `code/build/stream_device.ko`
- `code/build/stream_driver.ko`
- `code/build/stream_reader`
- `buildroot/output/images/Image`
- `buildroot/output/images/rootfs.ext4`

Run these builds as your regular host user.

## Run in QEMU

From the repository's `scripts/` directory:

```sh
./start-qemu.sh
```

This boots QEMU's `virt` machine with a Cortex-A53 CPU, 512 MB of RAM, and a serial console in your terminal. If QEMU is not installed globally, use the version built by Buildroot:

```sh
PATH="$(pwd)/../buildroot/output/host/bin:$PATH" ./start-qemu.sh
```

At the guest login prompt, log in as `root` (the default configuration has no root password). Inside the guest, load the modules and start the reader:

```sh
insmod /stream_device.ko
insmod /stream_driver.ko
ls -l /dev/streamdev0
stream_reader
```

Expected output looks like:

```text
Opened /dev/streamdev0
sample: 0
sample: 1
sample: 2
```

The timer starts when the driver binds, so the first sample and any initial backlog depend on how long you wait before starting the reader.

Press `Ctrl+C` to stop the reader. Close all readers before unloading the modules, and unload the driver before the platform device:

```sh
rmmod stream_driver
rmmod stream_device
```

Use `dmesg` to inspect probe, open, close, and removal messages. To shut down the guest, run `poweroff`; to exit QEMU directly, press `Ctrl+A`, then `X`.

## Development

After editing the code, stop QEMU, run `./build.sh` from `scripts/`, and boot the updated image again. Module loading is manual on each boot.

To compile only the modules and reader without rebuilding the guest image, run from the repository root:

```sh
cd code
./compile.sh
```

The initial Buildroot build must already be complete. The Makefile also accepts explicit build paths:

```sh
make ARCH=arm64 \
  KDIR=/path/to/configured/kernel/build \
  CROSS_COMPILE=/path/to/aarch64-toolchain-prefix-
```

Modules must be built against the kernel used by the guest. The Makefile target named `test` builds the `stream_reader` executable; it does not run an automated test suite.

## Repository layout

| Path | Purpose |
| --- | --- |
| `code/stream_device.c` | Virtual platform device registration |
| `code/stream_driver.c` | Platform driver registration and file operations |
| `code/include/stream_driver.h` | Shared structures and declarations |
| `code/src/` | Probe, removal, read, poll, open, release, and timer callbacks |
| `code/stream_reader.cc` | C++ userspace sample reader |
| `code/Makefile`, `code/compile.sh` | Module and reader builds |
| `configs/buildroot_config` | Buildroot configuration |
| `scripts/` | Setup, image rebuild, and QEMU launch scripts |
| `buildroot/` | Locally cloned Buildroot tree (ignored by Git) |

## Troubleshooting

- **Missing kernel directory or cross compiler:** complete the initial `make` in `buildroot/` before running `scripts/build.sh` or `code/compile.sh`.
- **Paths point outside the repository:** run scripts from `scripts/` and `compile.sh` from `code/`.
- **`qemu-system-aarch64: command not found`:** install host QEMU or use the Buildroot `PATH` command above.
- **Reader cannot open `/dev/streamdev0`:** check that both modules loaded successfully and inspect `dmesg` for probe errors.
- **Module version errors:** rebuild against the guest's kernel and regenerate the image before rebooting.
