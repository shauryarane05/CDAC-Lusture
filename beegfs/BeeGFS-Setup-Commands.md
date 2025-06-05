# BeeGFS Complete Setup Guide (Single Machine)

This document provides a complete, documented list of all key commands used to manually set up **BeeGFS management, metadata and storage services on a single machine** without creating partitions.

---

## 🧭 BeeGFS Management Node Setup Guide

### 📌 Purpose of Management Service

The **BeeGFS Management Service** is responsible for:

* Tracking the BeeGFS file system's metadata: node list, target mappings, mirror groups, storage pools, and quotas.
* Storing the system configuration database (`mgmtd.sqlite`).
* Not performance-critical unless you're using quotas with frequent updates.

By default, it uses:

```
/var/lib/beegfs/mgmtd.sqlite
```

---

### 🔧 Step 1: Create Directory for Management Database (Optional)

```bash
sudo mkdir -p /data/beegfs/mgmtd
```

**Why**: If you want to store the management database in a custom location instead of the default `/var/lib/beegfs/`.

---

### ⚙️ Step 2: Configure the Management Node

Edit the config file:

```bash
sudo nano /etc/beegfs/beegfs-mgmtd.toml
```

Add/modify these lines for a **test system**:

```toml
# Optional: Custom database file location
db-file = "/data/beegfs/mgmtd/mgmtd.sqlite"

# Disable TLS (for local testing only)
tls-disable = true

# Disable connection-based authentication (not for production)
auth-disable = true
```

**Why**: 
- `tls-disable = true` lets you skip generating TLS certs for testing
- `auth-disable = true` disables client-node identity verification
- These are **unsafe** in production but okay for test/dev

---

### 🔄 Step 3: Initialize the Management Database

> This must be done **before starting the service** for the first time.

```bash
sudo /opt/beegfs/sbin/beegfs-mgmtd --init
```

**What this does**:
- Creates and initializes the `mgmtd.sqlite` file (management database)
- Required for the management daemon to run successfully

---

### ▶️ Step 4: Start and Enable the Management Service

```bash
sudo systemctl start beegfs-mgmtd
sudo systemctl enable beegfs-mgmtd
```

**Check service status**:
```bash
sudo systemctl status beegfs-mgmtd
```

**View logs** (optional):
```bash
journalctl -u beegfs-mgmtd --no-pager
```

**Why**: This starts the management service and enables it to start automatically on boot.

---

## ✅ Metadata & Storage Setup Process

### 🔧 Step 5: Create Directories for Metadata & Storage

```bash
sudo mkdir -p /data/beegfs/beegfs_meta
sudo mkdir -p /data/beegfs/beegfs_storage
```

**Why**: BeeGFS stores internal data on disk. Since we're not creating separate partitions, we create directories on the root (`/`) filesystem instead.

---

### 🔐 Step 6: Ensure Permissions Are Correct

```bash
sudo chown -R root:root /data/beegfs/beegfs_meta
sudo chown -R root:root /data/beegfs/beegfs_storage
```

**Why**: We're running BeeGFS as `root`, so giving ownership to `root` ensures write access. You'd use `beegfs:beegfs` here only if the service runs as a non-root user.

---

### ⚙️ Step 7: Setup Metadata Service

```bash
sudo /opt/beegfs/sbin/beegfs-setup-meta -p /data/beegfs/beegfs_meta -s 2 -m localhost
```

**Explanation of options**:
- `-p`: Path where metadata will be stored (must be unique per service)
- `-s 2`: ID of this metadata service (any unique number)
- `-m localhost`: Address of management node (same machine here)

**Why**: This initializes the metadata directory and registers the service with the management daemon.

---

### ⚙️ Step 8: Setup Storage Service

```bash
sudo /opt/beegfs/sbin/beegfs-setup-storage -p /data/beegfs/beegfs_storage -s 3 -i 301 -m localhost
```

**Explanation of options**:
- `-p`: Path to store actual file data
- `-s 3`: ID of this storage service
- `-i 301`: ID of this specific storage target (you can have multiple targets)
- `-m localhost`: Management node location

**Why**: Initializes storage backend and connects to the management node.

---

### 🛠 Step 9: Edit Configuration Files (Optional)

> These are only needed if you want to **change settings**, like where BeeGFS should look for the storage paths.

**Configuration Files**:
- `/etc/beegfs/beegfs-meta.conf`
- `/etc/beegfs/beegfs-storage.conf`

Inside these files, you'd see or add:

```ini
storeMetaDirectory = /data/beegfs/beegfs_meta
storeStorageDirectory = /data/beegfs/beegfs_storage
```

**Why**: The setup commands already put these in the correct files, but you can edit manually if needed.

---

### ▶️ Step 10: Start the Metadata & Storage Services

```bash
sudo systemctl start beegfs-meta
sudo systemctl start beegfs-storage
```

**Why**: This starts the BeeGFS metadata and storage services using systemd.

**To enable them on boot**:

```bash
sudo systemctl enable beegfs-meta
sudo systemctl enable beegfs-storage
```

---

### 🧪 Step 11: Check Logs and Status (Optional)

**Check service status**:
```bash
sudo systemctl status beegfs-meta
sudo systemctl status beegfs-storage
```

**View detailed logs**:
```bash
journalctl -u beegfs-meta --no-pager
journalctl -u beegfs-storage --no-pager
```

**Why**: Verify the services started correctly and are connecting to the management node.

---

### ⏹️ Step 12: Stop All Services

**To stop all services**:
```bash
sudo systemctl stop beegfs-mgmtd
sudo systemctl stop beegfs-meta
sudo systemctl stop beegfs-storage
```

**To disable them from auto-starting on boot**:
```bash
sudo systemctl disable beegfs-mgmtd
sudo systemctl disable beegfs-meta
sudo systemctl disable beegfs-storage
```

**Why**: Use these commands when you need to stop all BeeGFS services for maintenance, troubleshooting, or system shutdown.

---

## 📋 Quick Reference Commands

### Start All Services
```bash
sudo systemctl start beegfs-mgmtd
sudo systemctl start beegfs-meta
sudo systemctl start beegfs-storage
```

### Stop All Services
```bash
sudo systemctl stop beegfs-mgmtd
sudo systemctl stop beegfs-meta
sudo systemctl stop beegfs-storage
```

### Check Status
```bash
sudo systemctl status beegfs-mgmtd
sudo systemctl status beegfs-meta
sudo systemctl status beegfs-storage
```

### View Logs
```bash
journalctl -u beegfs-mgmtd --no-pager
journalctl -u beegfs-meta --no-pager
journalctl -u beegfs-storage --no-pager
```

---

## 🗂️ Directory Structure Created

After setup, your directory structure will look like:

```
/data/beegfs/
├── mgmtd/              # Management database directory (optional custom location)
│   └── mgmtd.sqlite    # Management database file
├── beegfs_meta/        # Metadata storage directory
└── beegfs_storage/     # File data storage directory
```

---

## 🛑 Production Security Considerations

For production systems:

* Do **not** use `tls-disable` or `auth-disable` in the management configuration.
* Instead, configure proper TLS certificates and client authentication as outlined in the BeeGFS Admin Guide.
* Use dedicated partitions instead of directories for better performance and isolation.

---

## 📝 Notes

1. **Single Machine Setup**: This guide is for running management, metadata, and storage services on the same machine.
2. **No Partitions**: We're using directories instead of dedicated partitions for simplicity.
3. **Root User**: Services run as root - adjust permissions if using a different user.
4. **Service IDs**: Make sure to use unique IDs (-s and -i parameters) if setting up multiple services.
5. **Management Database**: The management service must be initialized before starting for the first time.
6. **Service Order**: Start management service first, then metadata and storage services.

---

## 🔗 Related Documentation

- For client setup and mounting, see the BeeGFS client documentation
- For production deployments, consider using dedicated partitions instead of directories
- For advanced configuration options, see the BeeGFS Admin Guide
- For multi-node setups, configure each service on separate machines
