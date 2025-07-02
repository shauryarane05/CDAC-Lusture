Lustre Installation Guide for Fedora with ldiskfs
This guide outlines the process of installing Lustre on a Fedora system using the ldiskfs backend, targeting the latest kernel version (e.g., 6.15). It includes steps to handle compatibility issues, apply specific patches, and resolve common errors encountered during the process. The steps are based on efforts to port Lustre using ldiskfs after challenges with ZFS integration.
Prerequisites

A Fedora system with root access.
Internet connectivity for downloading packages and repositories.
Basic familiarity with Linux commands, Git, and kernel module compilation.
Ensure the system is updated: sudo dnf update.

Step-by-Step Installation
1. Clone the Lustre Repository
Clone the official Lustre repository from Whamcloud to obtain the source code.
git clone git://git.whamcloud.com/fs/lustre-release.git
cd lustre-release

Purpose: This retrieves the latest Lustre source code from the Whamcloud repository, which serves as the base for applying patches and building Lustre.
Note: Ensure Git is installed (sudo dnf install git).
2. Apply Necessary Patches
Cherry-pick specific commits from the Whamcloud Gerrit review system to ensure compatibility with kernel 6.15 and ldiskfs. The following commits are critical:

LU-19137: Compatibility updates for kernel 6.15.
LU-19142: OSD-ldiskfs journal commit callback.
LU-19135: Server support for kernel 6.15.
LU-19141: Convert some checks to parallel (optional, tested with kernel 6.15.3).

Run the following commands to apply these patches:
git fetch https://review.whamcloud.com/fs/lustre-release refs/changes/70/59970/1 && git cherry-pick FETCH_HEAD
git fetch https://review.whamcloud.com/fs/lustre-release refs/changes/72/59972/1 && git cherry-pick FETCH_HEAD
git fetch https://review.whamcloud.com/fs/lustre-release refs/changes/73/59973/1 && git cherry-pick FETCH_HEAD
# Optional
git fetch https://review.whamcloud.com/fs/lustre-release refs/changes/69/59969/1 && git cherry-pick FETCH_HEAD

Purpose: These patches update Lustre to work with kernel 6.15, addressing compatibility issues in the build system, ldiskfs, and journal handling.
Note: Verify the commits are applied correctly using git log. The commit IDs should match:

37721332d0 (LU-19137)
2aff2807fb (LU-19142)
114b38492d (LU-19135)
efbe12d92a (LU-19141)
82f2bdb17a (LU-19098, already in master)

3. Enable Fedora Source Repositories
Enable the Fedora source repositories to download the kernel source RPM, which is needed for ldiskfs compatibility.
sudo dnf config-manager --setopt=fedora-source.enabled=true
sudo dnf config-manager --setopt=updates-source.enabled=true
sudo dnf clean all
sudo dnf makecache

Purpose: This enables the Fedora source repositories and refreshes the package metadata to allow downloading source RPMs.
Note: Ensure dnf is installed and configured properly.
4. Upgrade to the Latest Kernel
Download and install the latest kernel to ensure compatibility with the patched Lustre source.
dnf download --source kernel
sudo dnf upgrade --refresh
rpm -q kernel
sudo reboot

Purpose: This downloads the latest kernel source RPM, upgrades the system to the latest kernel, verifies the kernel version, and reboots to apply the new kernel.
Note: After reboot, confirm the kernel version with uname -r (should be 6.15.x or higher).
5. Extract and Install ext4 Source
Extract the ext4 filesystem source from the kernel source tarball and copy it to the kernel development directory.
rpm -ivh kernel-core-*.src.rpm
SOURCE_TARBALL=$(ls $HOME/rpmbuild/SOURCES/linux-*.tar.xz)
tar Jxf ${SOURCE_TARBALL} --wildcards '*/fs/ext4'
EXTRACTED_DIR=$(ls -d linux-*/)
sudo cp -r $HOME/${EXTRACTED_DIR}/fs/ext4 /usr/src/kernels/$(uname -r)/fs/
ls /usr/src/kernels/$(uname -r)/fs/ext4

Purpose: This installs the kernel source RPM, extracts the ext4 filesystem source (used by ldiskfs), and copies it to the kernel development directory for Lustre to use during compilation.
Note: Verify the ext4 directory exists in /usr/src/kernels/$(uname -r)/fs/ to ensure the copy was successful.
6. Install e2fsprogs and Development Libraries
Set up a custom DNF repository for Lustre’s e2fsprogs and install the required packages.
sudo nano /etc/yum.repos.d/lustre-e2fsprogs.repo

Add the following content to the file:
[Lustre-e2fsprogs]
name=Lustre-e2fsprogs for EL9
baseurl=https://downloads.whamcloud.com/public/e2fsprogs/latest/el9/
gpgcheck=0
enabled=1

Save and exit, then install the packages:
sudo dnf install e2fsprogs
sudo dnf install e2fsprogs-devel

Purpose: This configures a repository for Lustre’s customized e2fsprogs, which is required for ldiskfs support, and installs the necessary runtime and development libraries.
Note: The repository is set for EL9 (Enterprise Linux 9), which is compatible with Fedora for this purpose. gpgcheck=0 disables GPG verification due to the lack of a public key.
7. Configure and Build Lustre
Prepare and configure the Lustre source for compilation with ldiskfs support, explicitly disabling ZFS and utilities to avoid errors.
sh autogen.sh
./configure --with-zfs=no --enable-ldiskfs --disable-utils

Purpose: 

autogen.sh: Generates the build system files (e.g., configure script).
./configure: Configures Lustre to use ldiskfs instead of ZFS and disables utilities that may cause compilation errors.

Note: The --disable-utils flag is used to avoid issues in the utilities code, which may not be needed for basic Lustre functionality.
8. Handle Errors in liblnetconfig.c
If compilation fails due to errors in liblnetconfig.c, remove references to this file from the build system.

Identify and edit the relevant Makefiles and linking files (e.g., lustre/utils/Makefile.am, lustre/lnet/Makefile.am, or similar).
Remove or comment out references to liblnetconfig.c.
Re-run the configuration:

sh autogen.sh
./configure --with-zfs=no --enable-ldiskfs --disable-utils

Purpose: This resolves compilation errors related to liblnetconfig.c, which may not be compatible with the current setup.
Note: Be cautious when modifying Makefiles, as incorrect changes can break the build. Back up files before editing.
9. Compile and Install Lustre
Compile the Lustre source and install the resulting modules.
sudo make install

Purpose: This compiles the Lustre source code and installs the kernel modules and associated files to the system.
Note: Ensure sufficient disk space and memory are available, as compilation can be resource-intensive.
10. Verify Module Installation
Check that Lustre modules are installed in the correct directory.
ls /lib/modules/$(uname -r)/fs/lustre/

Purpose: This confirms that the Lustre kernel modules (e.g., lustre.ko, ldiskfs.ko) are correctly installed.
Note: If no modules appear, revisit the compilation and configuration steps for errors.
11. Update Module Dependencies
Update the module dependency tree to ensure modprobe can locate the Lustre modules.
sudo depmod -a

Purpose: This updates the kernel module dependency database, allowing the system to load Lustre modules using modprobe.
12. Load the Lustre Module
Load the Lustre kernel module to enable the filesystem.
sudo modprobe lustre

Purpose: This loads the Lustre module into the running kernel, making the filesystem available for use.
Note: If the module fails to load, check dmesg or /var/log/messages for errors and revisit the configuration steps.
Testing
To verify the installation, run the Lustre sanity tests on a single node (as tested with kernel 6.15.3):
cd lustre-release/lustre/tests
sh sanity.sh tartó

**Purpose**: The `sanity.sh` script runs a suite of tests to ensure Lustre is functioning correctly with `ldiskfs`.

**Note**: Ensure a test environment is set up (e.g., loopback devices for `ldiskfs`). Refer to the [Lustre documentation](https://doc.lustre.org/) for detailed setup instructions.

## Troubleshooting
- **ZFS/ldiskfs Detection Issue**: Lustre autodetects ZFS or `ldiskfs`. If neither is present, ensure `e2fsprogs` and `e2fsprogs-devel` are installed and the `ext4` source is correctly copied.
- **Compilation Errors**: If errors persist in `liblnetconfig.c`, double-check the Makefile edits and consider disabling additional utilities or modules causing issues.
- **Kernel Compatibility**: Ensure the kernel version matches the patched Lustre source (6.15.x). Mismatches can cause module loading failures.
- **Module Not Found**: If `modprobe lustre` fails, verify the module path and run `depmod -a` again.

## Additional Notes
- The patches applied are specific to kernel 6.15 and `ldiskfs`. For other kernel versions, consult the Whamcloud Gerrit for relevant patches.
- The `--disable-utils` flag avoids compilation issues but may limit some Lustre features. Re-enable utilities if needed after resolving errors.
- For production use, consult the [Lustre documentation](https://doc.lustre.org/) for advanced configuration and performance tuning.

## References
- [Whamcloud Lustre Repository](https://git.whamcloud.com/fs/lustre-release.git)
- [Whamcloud Gerrit](https://review.whamcloud.com/)
- [Lustre Documentation](https://doc.lustre.org/)
- [e2fsprogs Repository](https://downloads.whamcloud.com/public/e2fsprogs/latest/el9/)
link to my notion -https://www.notion.so/PORTING-LUSTRE-FINALE-224a8210e16e8046ab80db7b2316a3ea