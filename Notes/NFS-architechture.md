# NFS(Network File System)

![image.png](attachment:948cccb0-7d5a-4e16-b290-25de6c6920ce:image.png)

1. **Distributed file systems** 
It allows clients to access files as if they were stored locally, even though they may be physically located on different servers. DFSs are used to improve scalability, performance, and fault tolerance.
2. Some popolar DFS
    1. **Network File System (NFS)**: NFS is a popular DFS that is widely used in Unix and Linux environments. It uses a client-server architecture, where the server exports directories to 
    clients, and the clients can mount the exported directories.
    2. **Hadoop Distributed File System (HDFS)**
    3. **GlusterFS**
    4. **Ceph**
3. Desirable properties of DFS
    - **Transparency**: The DFS should be transparent to the client, meaning that the client
    should be able to access files in the DFS as if they were local files.
    - **Support for concurrent clients**: The DFS should be able to support multiple client processes accessing the same file concurrently.
    - **Replication**: The DFS should store multiple copies of the file on different servers to improve fault tolerance and performance.
    - **Scalability**: The DFS should be able to scale to support a large number of clients and files.
    - **Security**: The DFS should provide security features to protect files from unauthorized access.
    - **Performance**: The DFS should provide good performance for both read and write operations.
4. **Concurrent Accesses in DFS**
    1. **One-Copy Update Semantics:** There maybe multiple copies of the files across various servers but we want the clients to see the latest updated version of the replicas and the replicas should sync up immediately.whether a file is replicated or not, clients accessing the file should see consistent and coherent data. 
    2. **At most once operation**: An at most once 
    operation is an operation that is guaranteed to be executed at most 
    once, even if there are failures in the system. At most once operations 
    are useful for applications where idempotency is important, such as 
    financial applications.
        1. means when we tranfer 100 rupees to my friend and if the request to the server is not processed we dont want it to retry automatically it gives up after one attempt . as we dont want 100 rupees to be transfered again.
        2. but in the case of sensors info logging if the server doesnt get the info we want it to try again and again 
        3. idempotency means repetion doesnt change the outcome
    3. **At least once operation**: An at least once
     operation is an operation that is guaranteed to be executed at least 
    once, even if there are failures in the system. At least once operations
     are useful for applications where durability is important, such as 
    logging applications.
5.  **Security in DFS**
    1. **Access control lists (ACLs)** are a way of specifying who has access to a particular resource and
    what type of access they have. ACLs are typically associated with
    individual resources, such as files and directories.
        1. This is when files control who all can access them in what way like a guestlist
    2. **Capability lists** are a way of specifying what resources a particular user has access to
    and what type of access they have. Capability lists are typically
    associated with individual users. Capabilities are indivisible units of
    authority. Each capability grants the holder permission to perform a
    specific operation on a specific resource.
        1. this is when the user have the list of which all files it can access in what way it is like a keycard
6. **Mount in Unix-like operating systems**
    1. **Unix-like operating systems, the term “mount” refers to the process of 
    associating a file system with a particular directory in the system’s 
    directory tree.** This allows the files and directories within that file 
    system to be accessed by users and applications as if they were part of 
    the overall directory structure.
    2.  mount point is an existing directory in the file system hierarchy where
     the contents of a separate file system are attached. When a file system
     is mounted at a specific mount point, the files and directories in that
     file system become accessible through that directory.Once
     a file system is mounted, you can navigate to the specified mount point
     and access the files and directories within that file system as if they
     were part of the local file system. This flexibility allows Unix-like 
    operating systems to manage various storage devices and network 
    resources seamlessly.
    3. For example, if a client mounts the directory `/dev/sda1` into the directory `/mnt/external/sda1`(may be using `sudo mount -t ntfs /mnt/external/sda1`), then the client can access the files inside sda1 directory i.e.,`/mnt/external/sda1/foo.txt`.

**Network File System (NFS)**

A **Remote Procedure Call (RPC)** is a way for one computer (a **client**) to **run a function on another computer (a server)** *as if it were local*.

- The files are on a different machine.
- You can’t run `open()` or `read()` directly on that remote machine.
- So the client system sends a **message (RPC)** to the server saying:
    
    > “Please do this file operation on my behalf.”
    > 

The server:

- Performs it
- Sends back the result
- And the client OS shows it to you like it happened locally

---

 It is a client-server architecture, where the server exports directories to clients, and the clients can mount the exported directories.

NFS uses a remote procedure call (RPC) protocol to communicate between the client and server. This makes it relatively easy to implement and deploy.

![image.png](attachment:7fcee8a6-b31f-4ac3-87fa-106e47b13993:image.png)

## NFS Client System

The NFS client system is integrated with the kernel (OS). It performs RPCs 
to the NFS server system for DFS operations, such as reading and writing
 files, creating and deleting files, and moving and renaming files.

## NFS Server System

The NFS server system plays the role of both flat file service and 
directory service. It allows mounting of files and directories. Server files are mounted to client system

Mounting files and directories does not clone (copy) the files. It simply creates a symbolic link to the files on the server. This means that the client can access the files without having to download them to the client machine.

**Virtual File System (VFS) Module**

## ✅ 1. **File Descriptor (`fd`)**

- **What it is**: A simple **integer** given to your program when it opens a file.
- **Who uses it**: Your **user-space process**.
- **What it points to**: A `struct file` in the kernel.

## ✅ 2. **`struct file`**

- **What it is**: A kernel structure that represents your **open file**.
- **Created by**: The **kernel** when you open a file.

## ✅ 3. **VNode (Virtual Node)**

> Linux doesn't literally use the term "vnode" — it uses dentry + inode, but we’ll call it vnode for simplicity.
> 
- **What it is**: VFS's abstract view of a file.
- **Why it exists**: So the kernel can treat **all file systems the same** — ext4, FAT, NFS, etc.
- **Points to**: The actual **inode** or **remote NFS server + inode**
- **Used by**: `struct file` to access file metadata.

## ✅ 4. **Inode (Index Node)**

- **What it is**: The **core metadata** about a file on the disk.
- **Stored on**: The actual file system (ext4, NFS, etc.).
- **Contains**: File size, owner, timestamps, disk block locations.

## ✅ 5. **Dentry (Directory Entry)**

- **What it is**: Maps a **file name** to an **inode**.
- **Used for**: Path lookup (`/home/shaurya/file.txt`)
1. VFS Is an abstraction between kernel and filesystems
2. it allows process to access flies via file descriptors
3. This makes local and remote files indistinguishable to processes, which gives transparency.
4. The VFS keeps a data structure for each mounted file system. This data structure contains information about the file system, such as its type and location.
5. The VFS also keeps a data structure called a v-node for all open files. The v-node is a cache of information about the file, such as its size, permissions, and location.
6. If the file is local, the v-node points to the local disk i-node. If the file is remote, the v-node contains the address of the remote NFS server.
7. VFS decides whether to route the request to the local file system or to the NFS client system. 
8. This decision is made based on the file handle of the file.
- When a file is accessed, VFS checks the **superblock** of the mounted file system.
- The superblock stores the **file system type** (e.g., ext4, nfs).
- If the type is **NFS (remote)**, VFS uses the **file handle** provided by the NFS server.
- If the type is **local (ext4, xfs, etc.)**, VFS uses the **inode** from the local disk.
1.  The VFS uses the file handle to identify the file system on which the file is located and to determine which file system operations to perform.
2. 

Here is an example of how the VFS works:

- A process opens a file using the open() system call.
- The kernel uses the file path to find the file system that the file is located on.
- The kernel then calls the VFS to open the file.
- The VFS checks to see if the file is local or remote.
- If the file is local, the VFS calls the local file system to open the file.
- If the file is remote, the VFS calls the NFS client system to open the file.
- Once the file is open, the VFS returns a file descriptor to the process.
- The process can then use the file descriptor to read and write to the file.

![image.png](attachment:9bbd7ec7-56e7-4bd9-b788-4c3a25673981:image.png)

The VFS hides the details of how the file is located and accessed from the process. This makes the process code more portable and easier to maintain. (v-node → file descriptor → inode). This means that the vnode is at the highest level, followed by the file descriptor, and then the inode.

---

There are two main flavours of writes in NFS:

- **Delayed write**: With delayed write, the server writes the data to memory and then
flushes it to disk every 30 seconds (or via the Unix sync operation).
This is faster than write-through, but it is not as consistent. If the
server crashes before the data is flushed to disk, the data may be lost.
it sends write sucessful after laoding to ram only
- **Write-through**: With write-through, the server writes the data to disk immediately
before acknowledging the client request. This is more consistent than
delayed write, but it may be slower.

### . **Speed Boost**

- Writing to **RAM** is **much faster** than writing to disk.
- Delayed write lets the server **acknowledge the client immediately** without waiting for the slow disk operation.

### 2. **Batching Writes**

- If multiple clients or operations write to the same file, the server can **merge those writes** in memory and **write once to disk**.
- This reduces disk load and improves performance.

### ✅ Use delayed write when:

- Speed matters more than perfect reliability
- Like video streaming, log files, or temporary data

### ✅ Use write-through when:

- Data **must not be lost**
- Like bank transactions, configuration files, or medical records

---

- **Asynchronous I/O**: NFS servers can use asynchronous I/O to improve performance.
Asynchronous I/O allows the server to continue processing other requests while it is waiting for I/O operations to complete.
- **Multiple threads**: NFS servers can use multiple threads to improve performance. This
allows the server to handle multiple client requests simultaneously.
- **Load balancing**: NFS servers can be load balanced to distribute the load across multiple servers. This can improve performance and reliability.

### 🔧 Example:

### Situation:

3 clients are writing files to an NFS server:

- Client A writes `fileA.txt`
- Client B writes `fileB.txt`
- Client C writes `fileC.txt`

---

### 🐢 Without Asynchronous I/O (Synchronous):

1. Server gets write from Client A → waits until it’s written to disk
2. Then it handles Client B → waits
3. Then Client C → waits

Total time = time for A + B + C (slow, especially if disk is busy)

---

### 🚀 With Asynchronous I/O:

## Client Caching

Client
 caching is similar to server caching in that it stores some of the 
recently-accessed blocks (of files and directories) in memory. However, 
there are a few key differences:

- Client caching is performed by the NFS client, while server caching is performed by the NFS server.
- Client caching is used to improve the performance of read operations, while
server caching is used to improve the performance of both read and write operations.
- Client caching can lead to inconsistencies, while server caching is always consistent.
1. Server receives write from Client A → **starts writing to disk** in background
2. Meanwhile, it immediately accepts request from Client B and **starts** its write
3. Then handles Client C similarly
4. Disk writes **happen in parallel** while the server continues to serve clients

## Client Caching

Client
 caching is similar to server caching in that it stores some of the 
recently-accessed blocks (of files and directories) in memory. However, 
there are a few key differences:

- Client caching is performed by the NFS client, while server caching is performed by the NFS server.
- Client caching is used to improve the performance of read operations, while
server caching is used to improve the performance of both read and write operations.
- Client caching can lead to inconsistencies, while server caching is always consistent.

---

## ✅ **Client-Side Caching for Writes in NFS – Short Notes**

### 🧠 How it works:

- When a client writes to a file on an NFS share:
    1. The data is first written to the **client’s cache (RAM)** — this is called a **delayed write**.
    2. The client **sends the data to the server later**.
    3. The **server writes it to disk**, then **acknowledges** the write.

### 📋 Cache Validation:

- Each block in the client cache has:
    - **Tc**: When the cache entry was last validated with server
    - **Tm**: When the server last modified that block
- Cache is valid if:
    
    `T - Tc < t`
    
    *(t = freshness interval, e.g., 3–30s for files, 30–60s for directories in Solaris)*
    

---

## ⚡ Why delayed writes improve performance:

- **Faster response**: Writing to RAM is quicker than waiting for disk + network.
- **Batched writes**: Multiple small writes can be grouped → fewer network calls.
- **Redundant write elimination**: Only final state is sent → less I/O.

---

## ⚠️ Trade-off:

- If client crashes **before sending** → server never gets the data → **data loss**.

Use **write-through** for critical applications that must avoid this risk.

---

## ❓ Clarification:

> "If data is cached on client, how is performance better?"
> 

Because:

- **RAM is much faster** than disk.
- Instead of writing across a **slow network** to a **remote disk**, the client writes to **local memory**.
- This gives instant response to the user/app.
- The actual network+disk operation happens in the background.

🧠 Think of it as **"saving locally first, sending later"** — just like Gmail saving drafts instantly before syncing them online.