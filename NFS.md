# Network File System (NFS) Setup Guide

## What is NFS?

Network File System (NFS) is a distributed file-sharing protocol that allows files to be stored and retrieved across a network. It enables a client to access files on a remote server as if they were local. NFS operates at the application layer and is typically implemented over the TCP/IP protocol suite. It is supported on operating systems like Microsoft Windows, Linux, and Oracle Solaris, as well as cloud platforms such as Amazon Elastic File System, Microsoft Azure NFS file shares, and Google Cloud Filestore.

NFS can share devices like hard disks, solid-state drives, tape drives, printers, and other peripherals attached to the host file system. Authorized users can access these resources seamlessly from their client machines.

### NFS Operation Models

- **Remote Access Model**: The file stays on the server, and the client sends requests to access it. The server processes the request and sends back the results.
- **Upload/Download Model**: The file is transferred to the client, where it is modified locally. Once done, the updated file is sent back to the server, replacing the original.

## Versions of NFS

NFS has evolved through multiple versions, each improving functionality and performance:

- **Sun Network Filesystem**: Released March 1984.
- **NFSv2**: Released March 1989.
- **NFSv3**: Released June 1995.
- **NFSv4**: Released April 2003.
- **NFSv4.1**: Released January 2010.
- **NFSv4.2**: Released November 2016.

Versions starting from NFSv2 are generally compatible after client-server negotiation.

## Benefits of NFS

- **Open Protocol**: NFS is an open standard, documented in internet specifications, and freely accessible.
- **Cost-Effective**: Utilizes existing network infrastructure, minimizing setup costs.
- **Distributed**: Supports distributed file systems, reducing reliance on removable media.
- **Secure**: Limits the use of removable media (e.g., CDs, USB drives), enhancing security.

## Setting Up NFS on Ubuntu

### Configuring the NFS Server

#### Step 1: Install NFS Kernel Server

1. Check the Ubuntu version:

   ```
   lsb_release -a
   ```
2. Update system packages:

   ```
   sudo apt update
   ```
3. Install the NFS kernel server package (includes `nfs-common` and `rpcbind`):

   ```
   sudo apt install nfs-kernel-server
   ```
4. Check the NFS server status:

   ```
   sudo systemctl status nfs-kernel-server
   ```

   Ensure the server is active.
5. Enable the NFS server to start on boot:

   ```
   sudo systemctl enable nfs-kernel-server
   ```

#### Step 2: Create an NFS Export Directory

1. Create a directory to share with clients:

   ```
   sudo mkdir /nfsshare
   ```
2. Set permissions to allow read, write, and execute access for all clients:

   ```
   sudo chmod 777 -R /nfsshare
   ```

#### Step 3: Grant NFS Share Access to Clients

1. Edit the `/etc/exports` file to define access permissions:

   ```
   sudo vim /etc/exports
   ```

   or

   ```
   sudo gedit /etc/exports
   ```
2. Add an entry for a single client (e.g., IP `192.168.67.206`):

   ```
   /nfsshare 192.168.67.206(rw,sync,no_root_squash,no_subtree_check)
   ```

   For multiple clients, add each on a new line:

   ```
   /nfsshare client_IP_1(rw,sync,no_root_squash,no_subtree_check)
   /nfsshare client_IP_2(rw,sync,no_root_squash,no_subtree_check)
   ```

   **Options Explained**:
   - `rw`: Read and write access.
   - `sync`: Changes are written to disk before being applied.
   - `no_root_squash`: Allows the client’s root user to retain root privileges.
   - `no_subtree_check`: Disables subtree checking for better performance.

#### Step 4: Export the NFS Share Directory

1. Export the shared directory:

   ```
   sudo exportfs -a
   ```
2. Restart the NFS server to apply changes:

   ```
   sudo systemctl restart nfs-kernel-server
   ```

#### Step 5: Allow NFS Access Through the Firewall

1. Allow NFS traffic from the client IP (port 2049 by default):

   ```
   sudo ufw allow from 192.168.67.206 to any port nfs
   ```
2. Enable or reload the firewall and check its status:

   ```
   sudo ufw enable
   sudo ufw status
   ```

   Verify that port 2049 is open.

### Configuring the NFS Client

#### Step 1: Install the NFS-Common Package

1. Update the client system:

   ```
   sudo apt update
   ```
2. Install the `nfs-common` package:

   ```
   sudo apt install nfs-common
   ```

#### Step 2: Create an NFS Mount Point

1. Create a directory to mount the remote NFS share:

   ```
   sudo mkdir /nfs
   ```

#### Step 3: Mount the NFS Share

1. Mount the NFS share from the server (e.g., server IP `192.168.69.173`):

   ```
   sudo mount 192.168.69.173:/nfsshare /nfs
   ```

#### Step 4: Test the NFS Share

1. Create a test file in the mounted directory on the client:

   ```
   sudo touch /nfs/file
   ```
2. On the server, verify the file’s existence:

   ```
   ls /nfsshare
   ```

   The file should appear, confirming the NFS setup works.

## Removing NFS Services

To uninstall NFS services:

1. Remove the NFS server package:

   ```
   sudo apt-get remove nfs-kernel-server
   ```
2. Remove the NFS client package:

   ```
   sudo apt-get remove nfs-common
   ```

## Conclusion

NFS, developed by Sun Microsystems in 1980, enables file and folder sharing between Linux/Unix systems over a network. It allows remote hosts to interact with shared file systems as if they were local. Setting up NFS requires configuring an NFS server and client on separate machines. Originally designed for local area networks, NFS has evolved to support various distributed file systems, making it a versatile protocol for network file sharing.