# QEMU Guide

This guide provides essential commands and tips for using QEMU.

## Installation

Install QEMU on your system using your distribution's package manager:

-   **Arch Linux:**
    ```bash
    sudo pacman -S qemu
    #Optionally for more architectures
    sudo pacman -S qemu-arch-extra
    ```
-   **Debian/Ubuntu:**
    ```bash
    sudo apt install qemu
    ```
-   **Fedora:**
    ```bash
    sudo dnf install qemu
    ```

## Creating a Virtual Disk Image

Use `qemu-img` to create a virtual hard disk image. The `qcow2` format is recommended as it supports features like snapshots and smaller initial file sizes.

```bash
qemu-img create -f qcow2 Image.img 10G
```
-   `create`: Command to create a new image.
-   `-f qcow2`: Specifies the image format as qcow2.
-   `Image.img`: The name of the output image file.
-   `10G`: The size of the virtual disk (e.g., 10 Gigabytes).

## Launching a Virtual Machine

Use `qemu-system-x86_64` (or the appropriate system for your architecture) to launch a VM. This command includes common options for booting from an ISO and using a disk image.

```bash
qemu-system-x86_64 -enable-kvm -cdrom OS_ISO.iso -boot menu=on -drive file=Image.img -m 2G
```
-   `qemu-system-x86_64`: The QEMU system emulator for x86_64 architecture.
-   `-enable-kvm`: Enables KVM (Kernel-based Virtual Machine) for hardware acceleration (Linux hosts only). This significantly improves performance.
-   `-cdrom OS_ISO.iso`: Mounts an ISO image as a CD-ROM.
-   `-boot menu=on`: Enables the boot menu, allowing you to select the boot device.
-   `-drive file=Image.img`: Attaches the specified disk image file.
-   `-m 2G`: Sets the amount of RAM allocated to the VM (e.g., 2 Gigabytes).

**Tip:** Press `Ctrl + Alt + G` to release the mouse cursor from the VM window. Press `Ctrl + Alt + F` to toggle fullscreen.

## Basic Performance Options

Improve VM performance with these options:

-   `-cpu host`: Configures the VM to use the host CPU model, potentially enabling more features and improving performance.
    ```bash
    -cpu host
    ```
-   `-smp <cores>`: Sets the number of virtual CPU cores for the VM.
    ```bash
    -smp 2
    ```

## Graphics Acceleration

Use the `-vga` option to specify a graphics card emulator. `virtio` is generally recommended for better performance and features.

-   **QXL (2D acceleration, requires host kernel modules):**
    ```bash
    -vga qxl
    ```
-   **VirtIO (Better performance, supports some 3D with `sdl,gl=on`):**
    ```bash
    -vga virtio -display sdl,gl=on
    ```

## Checking System Information

Commands to get information about the system inside the VM:

-   Get architecture name:
    ```bash
    uname -p
    ```
-   View CPU information:
    ```bash
    cat /proc/cpuinfo
    ```

## Checking Virtualization Extension (on Host)

Verify if virtualization extensions (VMX for Intel, SVM for AMD) are enabled on your host machine. A result above 0 indicates support.

```bash
egrep -c '(vmx|svm)' /proc/cpuinfo
```

## Setting up libvirt and virt-manager (Debian/Ubuntu Example)

These steps are for setting up libvirt and virt-manager, which provide a higher-level interface for managing VMs.

1.  Update system package list:
    ```bash
    sudo apt update
    ```
2.  Upgrade installed packages:
    ```bash
    sudo apt upgrade
    ```
3.  Install QEMU, libvirt, and virt-manager:
    ```bash
    sudo apt install qemu-kvm qemu-system qemu-utils python3 python3-pip libvirt-clients libvirt-daemon-system bridge-utils virtinst libvirt-daemon virt-manager -y
    ```
4.  Verify that the libvirtd service is running:
    ```bash
    sudo systemctl status libvirtd.service
    ```
5.  Start and enable the default network (for networking in VMs):
    ```bash
    sudo virsh net-start default
    sudo virsh net-list --all
    sudo virsh net-autostart default
    ```
6.  Check network status:
    ```bash
    sudo virsh net-list --all
    ```
    If autostart is off, run:
    ```bash
    sudo virsh net-autostart default
    ```
7.  Add your user to the necessary libvirt groups to manage VMs without `sudo`:
    ```bash
    sudo usermod -aG libvirt $USER
    sudo usermod -aG libvirt-qemu $USER
    sudo usermod -aG kvm $USER
    sudo usermod -aG input $USER
    sudo usermod -aG disk $USER
    ```
8.  **Reboot** your host machine for group changes to take effect.
9.  Launch Virtual Machine Manager:
    ```bash
    virt-manager
    ```
