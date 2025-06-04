# Guide to SSH and SCP for Remote Access and File Transfer

This guide provides a practical, hands-on approach to using **SSH (Secure Shell)** for secure remote access and **SCP (Secure Copy Protocol)** for transferring files between systems. It’s designed for beginners and intermediate users, with clear instructions, real-world examples, and troubleshooting tips. Whether you’re managing a server or transferring files securely, this guide will help you navigate SSH and SCP with confidence.

---

## Table of Contents

1. [What Are SSH and SCP?](#1-what-are-ssh-and-scp)
2. [Before You Begin](#2-before-you-begin)
3. [Installing SSH Tools](#3-installing-ssh-tools)
4. [Starting and Managing the SSH Server](#4-starting-and-managing-the-ssh-server)
5. [Verifying SSH and Network Setup](#5-verifying-ssh-and-network-setup)
6. [Using SSH: Core Commands](#6-using-ssh-core-commands)
7. [Using SCP: File Transfer Basics](#7-using-scp-file-transfer-basics)
8. [Advanced SSH Techniques](#8-advanced-ssh-techniques)
9. [Advanced SCP Techniques](#9-advanced-scp-techniques)
10. [Handling Firewall and Network Issues](#10-handling-firewall-and-network-issues)
11. [Setting Up SSH Keys for Passwordless Access](#11-setting-up-ssh-keys-for-passwordless-access)
12. [Securing Your SSH Setup](#12-securing-your-ssh-setup)
13. [Troubleshooting Common Problems](#13-troubleshooting-common-problems)
14. [Quick Reference: Common Commands](#14-quick-reference-common-commands)

---

## 1. What Are SSH and SCP?

- **SSH (Secure Shell)**: A protocol for securely logging into a remote system and running commands as if you were physically at the machine. It encrypts all communication, protecting against eavesdropping and tampering.
- **SCP (Secure Copy Protocol)**: A tool built on SSH to securely transfer files between a local and remote machine, or even between two remote machines.
- **Default Port**: Both use port **22** for communication, though this can be customized.

**Why Use Them?**  
SSH is ideal for managing servers, automating tasks, or accessing remote systems securely. SCP complements SSH by enabling secure file transfers without needing additional software like FTP.

---

## 2. Before You Begin

To use SSH and SCP effectively, ensure:

- Both your local and remote machines are connected to a network (local or internet).
- The remote machine has an **SSH server** (`sshd`) installed and running.
- You have a valid user account and credentials (username/password or SSH key) for the remote machine.
- Firewall settings allow traffic on port 22 (or your custom SSH port).
- You’re comfortable with basic command-line operations.

**Note**: Always verify the remote machine’s IP address or hostname before connecting.

---

## 3. Installing SSH Tools

Most systems come with an SSH client pre-installed, but you may need to install the SSH server on the remote machine.

### On Ubuntu/Debian:

1. **Check for SSH client**:
   ```bash
   which ssh
   ```
   If it returns a path (e.g., `/usr/bin/ssh`), the client is installed.

2. **Install SSH client** (if missing):
   ```bash
   sudo apt update
   sudo apt install openssh-client
   ```

3. **Install SSH server** (for the remote machine to accept connections):
   ```bash
   sudo apt update
   sudo apt install openssh-server
   ```

### On CentOS/RHEL:

1. **Install SSH client and server**:
   ```bash
   sudo yum install openssh-clients openssh-server
   ```

### On macOS:

- The SSH client is built-in. Use `ssh` in the Terminal.
- To enable an SSH server:
  ```bash
  sudo systemsetup -setremotelogin on
  ```

---

## 4. Starting and Managing the SSH Server

After installing the SSH server, you need to ensure it’s running and set to start on boot.

### Commands:

- **Start the SSH server**:
  ```bash
  sudo systemctl start ssh
  ```

- **Enable SSH to start on boot**:
  ```bash
  sudo systemctl enable ssh
  ```

- **Check server status**:
  ```bash
  sudo systemctl status ssh
  ```
  Look for `active (running)` in the output. If it’s not running, check logs with `journalctl -u ssh`.

---

## 5. Verifying SSH and Network Setup

Before connecting, confirm the SSH server is accessible and the network is configured correctly.

- **Check if SSH is listening on port 22**:
  ```bash
  sudo ss -tlnp | grep :22
  ```
  You should see output indicating `sshd` is bound to port 22.

- **Verify firewall settings** (using UFW on Ubuntu):
  ```bash
  sudo ufw status
  ```
  If SSH is blocked, allow it:
  ```bash
  sudo ufw allow ssh
  sudo ufw enable
  ```

- **Find the remote machine’s IP address**:
  ```bash
  ip a
  ```
  Note the IP (e.g., `192.168.1.100`) for SSH connections.

---

## 6. Using SSH: Core Commands

### Connect to a Remote Machine:
```bash
ssh username@remote_ip
```
- Replace `username` with your remote user account (e.g., `john`).
- Replace `remote_ip` with the IP address or hostname (e.g., `192.168.1.100` or `server.example.com`).

**Example**:
```bash
ssh john@192.168.1.100
```

### Connect Using a Custom Port:
If the SSH server uses a non-standard port (e.g., 2222):
```bash
ssh -p 2222 username@remote_ip
```

### Exit an SSH Session:
Type `exit` or press `Ctrl+D`.

---

## 7. Using SCP: File Transfer Basics

SCP makes it easy to transfer files securely. Below are common commands with examples.

### Copy a Local File to a Remote Machine:
```bash
scp /path/to/local/file.txt username@remote_ip:/remote/path/
```
**Example**:
```bash
scp documents/report.pdf john@192.168.1.100:/home/john/
```

### Copy a Remote File to Your Local Machine:
```bash
scp username@remote_ip:/remote/path/file.txt /local/path/
```
**Example**:
```bash
scp john@192.168.1.100:/home/john/report.pdf ./downloads/
```

### Copy an Entire Directory (Recursively):
- **Local to remote**:
  ```bash
  scp -r /local/path/directory username@remote_ip:/remote/path/
  ```
  **Example**:
  ```bash
  scp -r ./project john@192.168.1.100:/home/john/
  ```

- **Remote to local**:
  ```bash
  scp -r username@remote_ip:/remote/path/directory /local/path/
  ```

### Use a Custom Port with SCP:
```bash
scp -P 2222 /local/file.txt username@remote_ip:/remote/path/
```

**Tip**: Always include the trailing `/` when specifying directories to avoid errors.

---

## 8. Advanced SSH Techniques

### Run a Single Command Remotely:
Execute a command on the remote machine without starting an interactive session:
```bash
ssh username@remote_ip "ls -la /home/username"
```
**Example**:
```bash
ssh john@192.168.1.100 "df -h"
```

### Enable Verbose Output for Debugging:
```bash
ssh -v username@remote_ip
```
The `-v` flag shows detailed connection info, useful for troubleshooting.

### SSH Agent Forwarding:
Access another server through the remote machine:
```bash
ssh -A username@remote_ip
```
This is useful for scenarios like accessing a private Git repository from a jump server.

### Keep SSH Sessions Alive:
Add this to your `~/.ssh/config` to prevent timeouts:
```plaintext
Host *
  ServerAliveInterval 60
  ServerAliveCountMax 3
```

---

## 9. Advanced SCP Techniques

### Limit Bandwidth Usage:
Prevent SCP from consuming too much network bandwidth (limit in KB/s):
```bash
scp -l 500 /local/file.txt username@remote_ip:/remote/path/
```
**Example** (limit to 500 KB/s):
```bash
scp -l 500 large_file.iso john@192.168.1.100:/home/john/
```

### Resume Interrupted Transfers with `rsync`:
If an SCP transfer fails, use `rsync` over SSH to resume:
```bash
rsync -P --rsh=ssh /local/file username@remote_ip:/remote/path/
```
**Example**:
```bash
rsync -P --rsh=ssh ./backup.tar.gz john@192.168.1.100:/home/john/
```

### Preserve File Attributes:
Use the `-p` flag to preserve timestamps and permissions:
```bash
scp -p /local/file.txt username@remote_ip:/remote/path/
```

---

## 10. Handling Firewall and Network Issues

### Test if Port 22 is Open:
```bash
nc -zv remote_ip 22
```
- **Success**: Shows `succeeded!`.
- **Failure**: Indicates a firewall block or SSH server not running.

### Check Network Connectivity:
```bash
ping -c 4 remote_ip
```
- Sends 4 packets to test if the remote machine is reachable.

### Common Firewall Commands (UFW):
- Allow SSH:
  ```bash
  sudo ufw allow 22
  ```
- Deny SSH (e.g., to restrict access temporarily):
  ```bash
  sudo ufw deny 22
  ```
- Reset firewall rules (careful!):
  ```bash
  sudo ufw reset
  ```

---

## 11. Setting Up SSH Keys for Passwordless Access

Using SSH keys eliminates the need to enter a password for each connection, improving both security and convenience.

### Step 1: Generate an SSH Key Pair (on your local machine):
```bash
ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
```
- Press `Enter` to accept the default file location (`~/.ssh/id_rsa`).
- Optionally, set a passphrase for extra security.

### Step 2: Copy the Public Key to the Remote Machine:
```bash
ssh-copy-id username@remote_ip
```
- Enter your password when prompted.
- This adds your public key to `~/.ssh/authorized_keys` on the remote machine.

### Step 3: Test Passwordless Login:
```bash
ssh username@remote_ip
```
You should log in without a password prompt.

**Manual Key Copy (if `ssh-copy-id` isn’t available)**:
1. Copy the public key:
   ```bash
   cat ~/.ssh/id_rsa.pub
   ```
2. Paste it into `~/.ssh/authorized_keys` on the remote machine.

---

## 12. Securing Your SSH Setup

To minimize vulnerabilities:

- **Use SSH Keys**: Passwords are more susceptible to brute-force attacks.
- **Change the Default Port**: Edit `/etc/ssh/sshd_config` to use a non-standard port (e.g., 2222):
  ```plaintext
  Port 2222
  ```
  Restart the SSH server:
  ```bash
  sudo systemctl restart ssh
  ```
- **Disable Root Login**: In `/etc/ssh/sshd_config`, set:
  ```plaintext
  PermitRootLogin no
  ```
- **Limit User Access**: Specify allowed users in `/etc/ssh/sshd_config`:
  ```plaintext
  AllowUsers username1 username2
  ```
- **Install `fail2ban`**: Block repeated failed login attempts:
  ```bash
  sudo apt install fail2ban
  ```
- **Keep Software Updated**:
  ```bash
  sudo apt update && sudo apt upgrade
  ```
- **Use Two-Factor Authentication (2FA)**: Configure tools like Google Authenticator for SSH.

---

## 13. Troubleshooting Common Problems

| Issue                          | Likely Cause                              | Solution                                                                 |
|--------------------------------|-------------------------------------------|--------------------------------------------------------------------------|
| Connection times out           | Network down or firewall blocking port 22 | Verify network with `ping`, allow port 22 with `ufw allow 22`.           |
| Connection refused             | SSH server not running                    | Start the server: `sudo systemctl start ssh`.                            |
| Permission denied (publickey)  | SSH keys misconfigured                   | Verify key in `~/.ssh/authorized_keys`, check permissions (`chmod 600`). |
| Host key verification failed   | Remote server’s key changed              | Remove old key: `ssh-keygen -R remote_ip`, then reconnect.               |
| Slow transfers or high latency | Network congestion                       | Test with `ping` or `traceroute`, consider limiting bandwidth with `-l`. |

**Pro Tip**: Check SSH logs for clues:
```bash
sudo tail -f /var/log/auth.log
```

---

## 14. Quick Reference: Common Commands

| Command                                         | Purpose                                           |
|-------------------------------------------------|---------------------------------------------------|
| `ssh username@remote_ip`                        | Connect to a remote machine                       |
| `scp file.txt username@remote_ip:/path/`        | Copy a local file to a remote machine             |
| `scp username@remote_ip:/path/file.txt /local/` | Copy a remote file to the local machine           |
| `sudo systemctl start ssh`                      | Start the SSH server                              |
| `sudo systemctl enable ssh`                     | Enable SSH to start on boot                       |
| `sudo ss -tlnp | grep :22`                     | Confirm SSH is listening on port 22               |
| `sudo ufw allow ssh`                            | Allow SSH traffic through the firewall            |
| `ssh-keygen -t rsa -b 4096`                     | Generate an SSH key pair                          |
| `ssh-copy-id username@remote_ip`                | Copy public key for passwordless login            |
| `nc -zv remote_ip 22`                           | Test if port 22 is open                           |
| `ping -c 4 remote_ip`                           | Test network connectivity                         |

---

