# Booting Fedora Linux on RISC-V with QEMU

This guide provides step-by-step instructions for booting Fedora Server on RISC-V architecture using QEMU virtualization.

## Overview

This setup uses QEMU to emulate a RISC-V 64-bit system and boot Fedora Server 42 with U-Boot as the bootloader. The configuration provides a virtual machine with 4 CPU cores, 8GB RAM, and network connectivity.

## Prerequisites

Before starting, ensure you have the following installed on your system:

- **QEMU**: RISC-V system emulation support
- **U-Boot**: Bootloader for RISC-V systems
- **Fedora Server Image**: RISC-V 64-bit raw disk image

### Installing QEMU (if not already installed)

```bash
# On Ubuntu/Debian
sudo apt update
sudo apt install qemu-system-misc

# On Fedora/RHEL
sudo dnf install qemu-system-riscv

# On Arch Linux
sudo pacman -S qemu-system-riscv
```

## File Structure

Your working directory should contain the following files:

```
qemu_booting/
├── qemurun_new.sh                                           # Boot script
├── u-boot-spl.bin                                          # U-Boot SPL (Secondary Program Loader)
├── u-boot.itb                                              # U-Boot Image Tree Blob
├── Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw  # Fedora disk image
├── Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw.xz  # Compressed image
└── uboot-images-riscv64-2025.04-2.0.riscv64.fc42.noarch.rpm       # U-Boot package
```

## Step-by-Step Boot Process

### Step 1: Download and Prepare the Fedora Image

1. **Download the Fedora Server Image**:
   ```bash
   cd /home/ranegod/qemu_booting
   
   # Download the Fedora Server RISC-V image (compressed)
   wget https://dl.fedoraproject.org/pub/fedora/linux/development/rawhide/Server/riscv64/images/Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw.xz
   ```

2. **Extract the Fedora Image**:
   ```bash
   cd /home/ranegod/qemu_booting
   
   # Extract the compressed image using unxz
   unxz Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw.xz
   ```

3. **Verify File Permissions**:
   ```bash
   # Make the boot script executable
   chmod +x qemurun_new.sh
   
   # Check that all required files are present
   ls -la u-boot-spl.bin u-boot.itb *.raw qemurun_new.sh
   ```

### Step 2: Understanding the Boot Script

The `qemurun_new.sh` script contains the following QEMU configuration:

```bash
IMAGEFILE=Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw

qemu-system-riscv64 \
  -bios u-boot-spl.bin \
  -nographic \
  -machine virt \
  -smp 4 \
  -m 8G \
  -device loader,file=u-boot.itb,addr=0x80200000 \
  -object rng-random,filename=/dev/urandom,id=rng0 \
  -device virtio-rng-device,rng=rng0 \
  -device virtio-blk-device,drive=hd0 \
  -drive file=${IMAGEFILE},format=raw,id=hd0,if=none \
  -device virtio-net-device,netdev=usernet \
  -netdev user,id=usernet,hostfwd=tcp::10000-:22
```

#### Parameter Explanation:

- **`-bios u-boot-spl.bin`**: Uses U-Boot SPL as the BIOS/firmware
- **`-nographic`**: Runs without graphical display (console mode)
- **`-machine virt`**: Uses QEMU's generic virtual machine
- **`-smp 4`**: Configures 4 CPU cores
- **`-m 8G`**: Allocates 8GB of RAM
- **`-device loader,file=u-boot.itb,addr=0x80200000`**: Loads U-Boot proper at memory address 0x80200000
- **`-object rng-random`** and **`-device virtio-rng-device`**: Provides hardware random number generation
- **`-device virtio-blk-device,drive=hd0`**: Creates a virtual block device
- **`-drive file=${IMAGEFILE}...`**: Attaches the Fedora disk image
- **`-device virtio-net-device`** and **`-netdev user`**: Configures network with port forwarding
- **`hostfwd=tcp::10000-:22`**: Forwards host port 10000 to guest port 22 (SSH)

### Step 3: Boot Fedora Linux

1. **Run the Boot Script**:
   ```bash
   ./qemurun_new.sh
   ```

2. **Alternative: Run QEMU Directly**:
   ```bash
   qemu-system-riscv64 \
     -bios u-boot-spl.bin \
     -nographic \
     -machine virt \
     -smp 4 \
     -m 8G \
     -device loader,file=u-boot.itb,addr=0x80200000 \
     -object rng-random,filename=/dev/urandom,id=rng0 \
     -device virtio-rng-device,rng=rng0 \
     -device virtio-blk-device,drive=hd0 \
     -drive file=Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw,format=raw,id=hd0,if=none \
     -device virtio-net-device,netdev=usernet \
     -netdev user,id=usernet,hostfwd=tcp::10000-:22
   ```

### Step 4: Boot Sequence

When you run the script, you'll see the following boot sequence:

1. **U-Boot SPL Loading**: Initial bootloader starts
2. **U-Boot Proper Loading**: Main bootloader loads from memory address 0x80200000
3. **Kernel Loading**: U-Boot loads the Linux kernel from the disk image
4. **Fedora Boot**: Fedora Server starts with systemd initialization
5. **Login Prompt**: System presents login prompt

### Step 5: System Access

#### Console Access
- The system runs in console mode (`-nographic` flag)
- Use the console directly for system interaction
- To exit QEMU: Press `Ctrl+A`, then `X`

#### SSH Access (if configured)
- SSH is forwarded from host port 10000 to guest port 22
- Connect from host system:
  ```bash
  ssh -p 10000 username@localhost
  ```

## Boot Options and Customization

### Memory Configuration
To change memory allocation, modify the `-m` parameter:
```bash
-m 4G    # 4GB RAM
-m 16G   # 16GB RAM
```

### CPU Configuration
To change CPU core count, modify the `-smp` parameter:
```bash
-smp 2   # 2 CPU cores
-smp 8   # 8 CPU cores
```

### Network Configuration
The default configuration provides:
- NAT networking for internet access
- SSH forwarding on port 10000
- To change SSH port forwarding:
  ```bash
  -netdev user,id=usernet,hostfwd=tcp::2222-:22  # Use port 2222
  ```

### Storage Configuration
- The raw disk image provides the root filesystem
- To add additional storage:
  ```bash
  -drive file=additional-disk.raw,format=raw,id=hd1,if=none \
  -device virtio-blk-device,drive=hd1
  ```

## Troubleshooting

### Common Issues

1. **Permission Denied**:
   ```bash
   chmod +x qemurun_new.sh
   ```

2. **File Not Found**:
   - Verify all files are in the correct directory
   - Check file names match exactly

3. **QEMU Not Found**:
   ```bash
   # Check if QEMU is installed
   which qemu-system-riscv64
   
   # Install if missing (Ubuntu/Debian)
   sudo apt install qemu-system-misc
   ```

4. **Boot Hangs**:
   - Ensure U-Boot files are correct for RISC-V
   - Verify disk image is not corrupted
   - Check available system memory

5. **Network Issues**:
   - Firewall might block port forwarding
   - Try different port numbers
   - Check if SSH service is running in guest

### Performance Tips

1. **Enable KVM** (if available on RISC-V host):
   ```bash
   -enable-kvm
   ```

2. **Optimize Memory**:
   - Use appropriate memory size for your system
   - Consider memory ballooning for dynamic allocation

3. **Storage Performance**:
   - Use SSD storage for host disk images
   - Consider using qcow2 format for better performance

## Additional Resources

- **QEMU RISC-V Documentation**: https://www.qemu.org/docs/master/system/target-riscv.html
- **U-Boot Documentation**: https://docs.u-boot.org/
- **Fedora RISC-V**: https://fedoraproject.org/wiki/Architectures/RISC-V

## File Descriptions

- **`u-boot-spl.bin`**: Secondary Program Loader - first stage bootloader
- **`u-boot.itb`**: U-Boot Image Tree Blob - contains U-Boot proper and device tree
- **`*.raw`**: Raw disk image containing Fedora Server root filesystem
- **`qemurun_new.sh`**: Automated boot script with optimized QEMU parameters

---

**Note**: This setup is configured for Fedora Server 42 on RISC-V 64-bit architecture. Ensure your system supports RISC-V emulation before proceeding.
