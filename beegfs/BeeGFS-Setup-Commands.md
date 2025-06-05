# BeeGFS Metadata & Storage Node Setup (Single Machine)

This document provides a complete, documented list of all key commands used to manually set up **BeeGFS metadata and storage services on a single machine** without creating partitions.

---

## ✅ Complete Setup Process

### 🔧 Step 1: Create Directories for Metadata & Storage

```bash
sudo mkdir -p /data/beegfs/beegfs_meta
sudo mkdir -p /data/beegfs/beegfs_storage
```

**Why**: BeeGFS stores internal data on disk. Since we're not creating separate partitions, we create directories on the root (`/`) filesystem instead.

---

### 🔐 Step 2: Ensure Permissions Are Correct

```bash
sudo chown -R root:root /data/beegfs/beegfs_meta
sudo chown -R root:root /data/beegfs/beegfs_storage
```

**Why**: We're running BeeGFS as `root`, so giving ownership to `root` ensures write access. You'd use `beegfs:beegfs` here only if the service runs as a non-root user.

---

### ⚙️ Step 3: Setup Metadata Service

```bash
sudo /opt/beegfs/sbin/beegfs-setup-meta -p /data/beegfs/beegfs_meta -s 2 -m localhost
```

**Explanation of options**:
- `-p`: Path where metadata will be stored (must be unique per service)
- `-s 2`: ID of this metadata service (any unique number)
- `-m localhost`: Address of management node (same machine here)

**Why**: This initializes the metadata directory and registers the service with the management daemon.

---

### ⚙️ Step 4: Setup Storage Service

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

### 🛠 Step 5: Edit Configuration Files (Optional)

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

### ▶️ Step 6: Start the Services

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

### 🧪 Step 7: Check Logs and Status (Optional)

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

### ⏹️ Step 8: Stop the Services

**To stop the services**:
```bash
sudo systemctl stop beegfs-meta
sudo systemctl stop beegfs-storage
```

**To disable them from auto-starting on boot**:
```bash
sudo systemctl disable beegfs-meta
sudo systemctl disable beegfs-storage
```

**Why**: Use these commands when you need to stop the BeeGFS services for maintenance, troubleshooting, or system shutdown.

---

## 📋 Quick Reference Commands

### Start Services
```bash
sudo systemctl start beegfs-meta
sudo systemctl start beegfs-storage
```

### Stop Services
```bash
sudo systemctl stop beegfs-meta
sudo systemctl stop beegfs-storage
```

### Check Status
```bash
sudo systemctl status beegfs-meta
sudo systemctl status beegfs-storage
```

### View Logs
```bash
journalctl -u beegfs-meta --no-pager
journalctl -u beegfs-storage --no-pager
```

---

## 🗂️ Directory Structure Created

After setup, your directory structure will look like:

```
/data/beegfs/
├── beegfs_meta/        # Metadata storage directory
└── beegfs_storage/     # File data storage directory
```

---

## 📝 Notes

1. **Single Machine Setup**: This guide is for running both metadata and storage services on the same machine.
2. **No Partitions**: We're using directories instead of dedicated partitions for simplicity.
3. **Root User**: Services run as root - adjust permissions if using a different user.
4. **Service IDs**: Make sure to use unique IDs (-s and -i parameters) if setting up multiple services.
5. **Management Node**: This assumes you have a BeeGFS management node running on localhost.

---

## 🔗 Related Documentation

- For management node setup, see the BeeGFS management documentation
- For client setup and mounting, see the BeeGFS client documentation
- For production deployments, consider using dedicated partitions instead of directories
