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
This downloads and installs the GPG key used to verify the authenticity of BeeGFS packages.

### 2. Add BeeGFS APT Repository for Ubuntu 22.04 (Jammy)

```bash
sudo wget https://www.beegfs.io/release/beegfs_8.0/dists/beegfs-jammy.list -O /etc/apt/sources.list.d/beegfs.list
sudo apt update
```
This adds the BeeGFS repository to your system so that apt can fetch and install packages from it.

```bash 
apt install apt-transport-https

```
Ensures APT can fetch packages securely via HTTPS.

```bash
apt update
```
Refreshes the package index so the system recognizes the newly added BeeGFS packages.

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

To enable support for remote direct memory access (RDMA) based on the `OFED`ibverbs API, please install the additional `libbeegfs-ib` package.

To use enterprise features such as storage pools or quotas, please install the `libbeegfs-license` package on the management node and download your BeeGFS license to `/etc/beegfs/license.pem`.

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


#### Management Service
By default the management service will store its data at /var/lib/beegfs/mgmtd.sqlite. Its main task is keeping track of file system configuration and state including the list of nodes, targets, pools, mirrors, etc.

To initialize the database for a new BeeGFS installation run:
```bash
$ ssh root@node02 
/opt/beegfs/sbin/beegfs-mgmtd --init
```

The management service requires Configuring TLS.  For a test system TLS could simply be disabled by setting tls-disable  = true in /etc/beegfs/beegfs-mgmtd.toml, but this is discouraged for production.
Some network communication in BeeGFS now defaults to using TLS encryption,


#### Metadata Service
The metadata service needs to know where it can store its data and where the management service is running. Typically, you will have multiple metadata services running on different machines.

Optionally, you can also define a custom numeric metadata service ID 
```bash 
$ ssh root@node02
$ /opt/beegfs/sbin/beegfs-setup-meta -p /data/beegfs/beegfs_meta -s 2 -m node01
```

#### Storage Service
The storage service needs to know where it can store its data and how to reach the management server.

Optionally, you can also define a custom numeric storage service ID and numeric storage target ID (both in range 1..65535).
```bash 
$ ssh root@node03
$ /opt/beegfs/sbin/beegfs-setup-storage -p /mnt/myraid1/beegfs_storage -s 3 -i 301 -m node01
```
To add a second storage target on this same machine:
```bash
$ /opt/beegfs/sbin/beegfs-setup-storage -p /mnt/myraid2/beegfs_storage -s 3 -i 302
```
#### Client
The client node is where you access the BeeGFS filesystem.
The client needs to know where the management service is running.
    1. Initialize BeeGFS Client
    ```
    $ /opt/beegfs/sbin/beegfs-setup-client -m node01
    ```
    -m node01 specifies the hostname of the management server.

    2. Ensure Correct Management Host in Config
    ```
    nano /etc/beegfs/beegfs-client.conf
    ```

    set:
    ```
    sysMgmtdHost = node01

    ```

    3.Create a Mount Directory and Mount File System

    ```
    sudo mkdir -p /mnt/beegfs
    sudo mount -t beegfs beegfs-node01 /mnt/beegfs
    ```

    4. Persistent Mount (On Reboot)
    To auto-mount the BeeGFS file system on startup, add this line to /etc/fstab:
    ```
    beegfs-node01 /mnt/beegfs beegfs defaults 0 0
    ```

Make sure `beegfs-node01` matches the value of `sysMgmtdHost` set in `/etc/beegfs/beegfs-client.conf`.

To make the mount persistent across reboots, add the following to `/etc/fstab`:

```
beegfs-node01 /mnt/beegfs beegfs defaults 0 0
```


#### Connection Auth

It is highly recommended to secure the BeeGFS installation by enabling connection based authentication. Services will not start if no connAuthFile is configured. To run a system without connection authentication, connDisableAuthentication must be set to true in all service configuration files and auth-disable must be set to true in the Management Service configuration file.

    1. Create a file which contains a shared secret
    ```bash 
    $ dd if=/dev/random of=/etc/beegfs/conn.auth bs=128 count=1
    ``` 
    Generates a 128-byte random secret.

    2. Restrict access to the file
    ```bash
    chown root:root /etc/beegfs/conn.auth
    chmod 400 /etc/beegfs/conn.auth
    ```
    If you want non-root users (e.g., monitoring tools) to use BeeGFS commands:
    ```bash 
    chown root:beegfs /etc/beegfs/conn.auth
    ```
    3. Copy the file to all cluster nodes
    Every node (mgmtd, metadata, storage, client, monitor) must have the same /etc/beegfs/conn.auth.
    Use scp or another secure method:
    ```bash
    scp /etc/beegfs/conn.auth user@other-node:/etc/beegfs/conn.auth

    ```

    4. Configure each node to use it
    In all BeeGFS config files (like beegfs-mgmtd.toml, beegfs-meta.conf, etc.), add:
    ```bash
    connAuthFile = /etc/beegfs/conn.auth
    ```
    This tells each service to use the shared secret file for verifying connections.

    5. Restart the services




---

## Step 4: Enable & Start Services

Enable and start the relevant services on each node:

```bash
# Example: Management Server
sudo systemctl start --now beegfs-mgmtd

# Metadata Server
sudo systemctl start --now beegfs-meta

# Storage Server
sudo systemctl start --now beegfs-storage

# Client
sudo systemctl start --now beegfs-client
```

---


## Step 5: Check Connectivity 
This step ensures your client node is correctly communicating with the BeeGFS services (mgmtd, metadata, storage) over the intended routes and protocols.


The new `beegfs` CLI uses environment variables, To persist these across reboots or shells, you add them to ~/.bashrc.
    1. Set the Management Server Address:
    ```bash
    ssh node04
    echo "export BEEGFS_MGMTD_ADDR='<IP-OR-HOSTNAME>:8010'" >> ~/.bashrc
    source ~/.bashrc
    ```

    2. Disable TLS and Auth (Testing Only!)
    ```bash
    echo "export BEEGFS_TLS_DISABLE='true'" >> ~/.bashrc
    echo "export BEEGFS_AUTH_DISABLE='true'" >> ~/.bashrc
    source ~/.bashrc
    ```


    check connectivity:
    ```bash
    beegfs node list --with-nics    #lists nodes and network interfaces
    beegfs health net   #shows what connections the client is using
    beegfs health df    # shows available space and inode info 
    beegfs health check     #detects any common setup issues
    ```
---

## References

* Official Documentation: [https://www.beegfs.io/wiki/](https://www.beegfs.io/wiki/)
* Download Page: [https://www.beegfs.io/wiki/Download](https://www.beegfs.io/wiki/Download)
* Community GitHub: [https://github.com/just4crafts/BeeGFS](https://github.com/just4crafts/BeeGFS)
