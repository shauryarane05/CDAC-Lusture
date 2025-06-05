# BeeGFS Installation & Setup Guide (Ubuntu 22.04, x86_64)

---

##  Overview

**BeeGFS** (formerly FhGFS) is a high-performance parallel file system designed for performance-critical environments such as High Performance Computing (HPC), data analytics, and large-scale storage solutions. It allows multiple clients to access file data simultaneously, improving throughput and scalability.

This guide assumes:

* You are using Ubuntu 22.04 (x86\_64)
* You have `sudo` access and a working internet connection
* You are comfortable with basic Linux command-line operations
* All nodes can resolve each other's hostnames via DNS or `/etc/hosts`


### Key Features of BeeGFS

- **Parallel Access:** Distributes files across multiple storage servers for high throughput.
- **Modular Design:** Allows roles to be split across different nodes or combined.
- **Scalability:** Supports adding nodes without downtime.
- **POSIX Compatibility:** Standard file system interface.
- **Enterprise Features:** Optional licensing for advanced features.
- **Monitoring:** Optional InfluxDB-based monitoring for real-time stats.

---

## BeeGFS Architecture and Roles

BeeGFS is composed of several server and client components, each serving a specific role:

| Role                  | Description                                         | Package(s)                |
|-----------------------|-----------------------------------------------------|---------------------------|
| Management Server     | Cluster config & coordination                       | `beegfs-mgmtd`            |
| Metadata Server       | Directory structure & file metadata                 | `beegfs-meta`             |
| Storage Server        | Stores file data (chunks)                           | `beegfs-storage`          |
| Client                | Kernel module for mounting BeeGFS                   | `beegfs-client`           |
| Enterprise Features   | Licensing & advanced features                       | `libbeegfs-license`       |
| RDMA Support         | High-speed networking (optional)                    | `libbeegfs-ib`            |
| Monitoring Server     | Real-time stats (InfluxDB-based)                    | `beegfs-mon`              |
| Admin Tools           | CLI tools, checkers, utilities                      | `beegfs-tools`, `beegfs-utils` |

---

## Step 1: Add BeeGFS Repository

### 1. Import GPG Key

```bash
sudo wget https://www.beegfs.io/release/beegfs_8.0/gpg/GPG-KEY-beegfs -O /etc/apt/trusted.gpg.d/beegfs.asc
```

### 2. Add BeeGFS APT Repository for Ubuntu 22.04 (Jammy)

```bash
sudo wget https://www.beegfs.io/release/beegfs_8.0/dists/beegfs-jammy.list -O /etc/apt/sources.list.d/beegfs.list
sudo apt update
```

> If you encounter 404 errors, verify that `/etc/apt/sources.list.d/beegfs.list` contains:
>
> ```
> deb [signed-by=/etc/apt/trusted.gpg.d/beegfs.asc] https://www.beegfs.io/release/beegfs_8.0 jammy non-free
> ```


---

## Step 2: Install BeeGFS Packages

Install only the components needed for each node role.

| Node Role           | Installation Command                                       |
| ------------------- | ---------------------------------------------------------- |
| Management Server   | `sudo apt install beegfs-mgmtd`                            |
| Metadata Server     | `sudo apt install beegfs-meta`                             |
| Storage Server      | `sudo apt install beegfs-storage`                          |
| Client              | `sudo apt install linux-headers-$(uname -r) beegfs-client` |
| Admin Tools         | `sudo apt install beegfs-tools beegfs-utils`               |
| RDMA Support        | `sudo apt install libbeegfs-ib`                            |
| Enterprise Features | `sudo apt install libbeegfs-license`                       |
| Monitoring Server   | `sudo apt install beegfs-mon`                              |

> After installing the client, you may need to rebuild the kernel module:
>
> ```bash
> sudo dpkg-reconfigure beegfs-client
> ```

---
## Step 3: Configure BeeGFS

All configs are in `/etc/beegfs/`:

| Component         | Config File                        |
|-------------------|------------------------------------|
| Management Server | `/etc/beegfs/beegfs-mgmtd.conf`    |
| Metadata Server   | `/etc/beegfs/beegfs-meta.conf`     |
| Storage Server    | `/etc/beegfs/beegfs-storage.conf`  |
| Client            | `/etc/beegfs/beegfs-client.conf`   |

**Key settings:**
- Set the management server's hostname/IP in all configs.
- Ensure all nodes can resolve each other (DNS/hosts).
- For advanced options, see [BeeGFS docs](https://www.beegfs.io/wiki/).

---

## Step 4: Enable & Start Services

Enable and start the relevant services on each node:

```bash
# Example: Management Server
sudo systemctl enable --now beegfs-mgmtd

# Metadata Server
sudo systemctl enable --now beegfs-meta

# Storage Server
sudo systemctl enable --now beegfs-storage

# Client
sudo systemctl enable --now beegfs-client
```

---

## Step 5: Mount BeeGFS Filesystem on Clients

On the client machine:

```bash
sudo mkdir -p /mnt/beegfs
sudo mount -t beegfs beegfs-node01 /mnt/beegfs
```

Make sure `beegfs-node01` matches the value of `sysMgmtdHost` set in `/etc/beegfs/beegfs-client.conf`.

To make the mount persistent across reboots, add the following to `/etc/fstab`:

```
beegfs-node01 /mnt/beegfs beegfs defaults 0 0
```

---

## Step 6: Verify BeeGFS Setup

Run a simple test on a client node:

```bash
touch /mnt/beegfs/testfile
ls -l /mnt/beegfs
```

You should see `testfile` listed with no errors.

To check the cluster state using admin tools:

```bash
beegfs-ctl --listnodes
beegfs-ctl --listtargets
```

---

## References

* Official Documentation: [https://www.beegfs.io/wiki/](https://www.beegfs.io/wiki/)
* Download Page: [https://www.beegfs.io/wiki/Download](https://www.beegfs.io/wiki/Download)
* Community GitHub: [https://github.com/just4crafts/BeeGFS](https://github.com/just4crafts/BeeGFS)
