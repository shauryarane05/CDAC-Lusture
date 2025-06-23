

## Revised and Improved Step-by-Step Guide

Here is a corrected and more detailed version of the guide that addresses the pitfalls of the original.

### Phase 1: Building and Deploying Lustre on Rocky Linux 9

### Our Minimal Cluster Setup:

*   **lustre-mds**: MGS + MDS Node
*   **lustre-oss**: OSS Node
*   **lustre-client**: Client Node

---

### Prerequisites

```bash
# Install virtualization tools
sudo dnf install virt-manager libvirt qemu-kvm -y

# Add your user to the libvirt group to manage VMs without sudo
sudo usermod -aG libvirt $(whoami)
echo "!!! You must now log out and log back in for this change to take effect !!!"

# Enable and start the libvirt service
sudo systemctl enable --now libvirtd
```
**Action:** Log out and log back in. Then launch Virt-Manager.
```bash
virt-manager
```

---

### Step 1: Create a Dedicated Virtual Network

This is the **most important change**. We will create a new, isolated network for our VMs so they can communicate reliably.

1.  In Virt-Manager, go to `Edit` -> `Connection Details` -> `Virtual Networks`.
2.  Click the `+` icon to add a new network.
3.  Give it a name, like `lustre-net`. Click `Forward`.
4.  Set the IPv4 address space to `192.168.56.0/24`. You can disable DHCP as we will use static IPs.
5.  Click `Forward`, then select "Isolated virtual network".
6.  Click `Finish`. The network is now ready.

---

### Step 2: Create Virtual Machines

Create 3 VMs with the following specifications.

| VM Name | Role | OS Disk | **Lustre Disk** | RAM | CPUs | Network |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `lustre-mds` | MGS+MDS | 20 GB | **+ 10 GB** | 2 GB | 2 | `lustre-net` |
| `lustre-oss` | OSS | 20 GB | **+ 25 GB** | 2 GB | 2 | `lustre-net` |
| `lustre-client` | Client | 20 GB | (none) | 2 GB | 1 | `lustre-net` |

**During VM Creation in Virt-Manager:**
1.  **OS**: Select Rocky Linux 9 (Minimal Install).
2.  **Memory/CPU**: Assign as above.
3.  **Disk**: Use the recommended size (e.g., 20 GB) for the OS disk.
4.  **Network**: On the final configuration screen before starting, select **Network Selection** and choose your new `lustre-net` virtual network.
5.  **Add Lustre Disks**: **Before** finishing the creation of `lustre-mds` and `lustre-oss`, check the "Customize configuration before install" box.
    *   Click "Add Hardware".
    *   Select "Storage".
    *   Create a new disk image of the specified size (10 GB for MDS, 25 GB for OSS).
    *   Click "Finish". Now your VM has two virtual disks.
6.  Begin the installation for all three VMs.

---

### Step 3: Post-Install Setup (on all 3 VMs)

```bash
# The network is enabled by default in Rocky 9, but let's be sure
sudo nmcli device connect eth0

# Update system (this might install a new kernel)
sudo dnf update -y

# Install basic tools
sudo dnf install -y vim curl wget net-tools bash-completion tar chrony git rpm-build libtool asciidoc

# CRITICAL: Reboot to ensure you are running the newest kernel
sudo reboot
```

---

### Step 4: Configure Networking & Host Resolution (on all 3 VMs)

After rebooting, configure static IPs.

```bash
# On lustre-mds
sudo nmcli con mod eth0 ipv4.addresses 192.168.56.101/24 ipv4.gateway 192.168.56.1 ipv4.method manual
sudo nmcli con up eth0

# On lustre-oss
sudo nmcli con mod eth0 ipv4.addresses 192.168.56.102/24 ipv4.gateway 192.168.56.1 ipv4.method manual
sudo nmcli con up eth0

# On lustre-client
sudo nmcli con mod eth0 ipv4.addresses 192.168.56.103/24 ipv4.gateway 192.168.56.1 ipv4.method manual
sudo nmcli con up eth0
```

Now, edit `/etc/hosts` on **all three machines** to contain the following:
```
192.168.56.101  lustre-mds
192.168.56.102  lustre-oss
192.168.56.103  lustre-client
```
Test with `ping lustre-mds`, `ping lustre-oss`, etc., from each machine.

---

### Step 5: Disable Firewall and SELinux (on all 3 VMs)

For a lab environment, it's easiest to disable these to eliminate them as sources of error.

```bash
# Stop and disable the firewall
sudo systemctl disable --now firewalld

# Set SELinux to permissive mode (it will only log, not block)
sudo setenforce 0
# Make it permanent across reboots
sudo sed -i 's/SELINUX=enforcing/SELINUX=permissive/g' /etc/selinux/config
```

---

### Step 6: Install Build Dependencies (on lustre-mds)

```bash
# Install kernel headers for the *currently running* kernel
sudo dnf install -y kernel-devel-$(uname -r) kernel-headers-$(uname -r)

# Enable the CRB (CodeReady Builder) repository which contains many build deps
sudo dnf config-manager --set-enabled crb

# Now, install Lustre build dependencies automatically from the .spec file
git clone git://git.lustre.org/lustre-release.git
cd lustre-release
git checkout v2_15_5
sudo dnf builddep -y lustre.spec

# If builddep fails, common missing packages are:
# sudo dnf install -y libyaml-devel libselinux-devel
```

---

### Step 7: Build Lustre RPMs (on lustre-mds)

```bash
# Still inside the lustre-release directory
sh ./autogen.sh
./configure --with-linux=/usr/src/kernels/$(uname -r)
make rpms
```
This will take a while. The RPMs will be in `~/rpmbuild/RPMS/x86_64/`.

---

### Step 8: Install and Configure Servers (MDS and OSS)

#### Copy RPMs:
On `lustre-mds`, copy the RPMs to the OSS node.
```bash
# Note: You may be prompted for the root password on lustre-oss
scp ~/rpmbuild/RPMS/x86_64/*.rpm root@lustre-oss:/root/
```

#### On lustre-mds:
```bash
# Install server and common packages
sudo dnf localinstall -y ~/rpmbuild/RPMS/x86_64/{kmod-lustre-2,lustre-2}*.rpm

# Find your secondary disk (it will likely be vdb or sdb)
lsblk

# Format the MDT on /dev/vdb (replace if different)
sudo mkfs.lustre --mgs --mdt --fsname=lustrefs --index=0 --mgsnode=lustre-mds@tcp /dev/vdb

# Mount the MDT
sudo mkdir /mnt/mdt
sudo mount -t lustre /dev/vdb /mnt/mdt
```

#### On lustre-oss:
```bash
# Install server and common packages from the copied RPMs
sudo dnf localinstall -y /root/{kmod-lustre-2,lustre-2}*.rpm

# Find your secondary disk
lsblk

# Format the OST on /dev/vdb (replace if different)
sudo mkfs.lustre --ost --fsname=lustrefs --index=0 --mgsnode=lustre-mds@tcp /dev/vdb

# Mount the OST
sudo mkdir /mnt/ost
sudo mount -t lustre /dev/vdb /mnt/ost
```
To make mounts permanent, add them to `/etc/fstab` on each respective server.

---

### Step 9: Install and Configure the Client (lustre-client)

#### Copy RPMs:
On `lustre-mds`, copy the required **client** RPMs to the client node.
```bash
scp ~/rpmbuild/RPMS/x86_64/{kmod-lustre-client,lustre-client-2}*.rpm root@lustre-client:/root/
```

#### On lustre-client:
```bash
# Install client packages
sudo dnf localinstall -y /root/*.rpm

# Load the kernel module
sudo modprobe lustre
lsmod | grep lustre

# Create a mount point
sudo mkdir /mnt/lustre

# Mount the file system
sudo mount -t lustre lustre-mds@tcp:/lustrefs /mnt/lustre
```

---

### Step 10: Test and Verify (on lustre-client)

```bash
# Check filesystem status
lfs df -h
# You should see your MDT and OST listed

# Create a test file
touch /mnt/lustre/my_first_lustre_file.txt
echo "Hello Parallel World" > /mnt/lustre/my_first_lustre_file.txt
cat /mnt/lustre/my_first_lustre_file.txt

# Check the file's layout (striping)
lfs getstripe /mnt/lustre/my_first_lustre_file.txt
```

You now have a robust and correctly configured Lustre setup