![supervisor_terminal_output](https://github.com/user-attachments/assets/2cd27f5b-5b18-4310-a298-87a91beb17d9)#  Multi-Container Runtime System

## Team Information

- **Name:** SANGADALA YEASASWANI  
  **SRN:** PES2UG24CS441  

- **Name:** SHAYNA ZAIDI GUPTA  
  **SRN:** PES2UG24CS465  

---

##  System Overview

This project implements a lightweight container runtime for Linux systems, developed in C, with the objective of exploring key operating system concepts such as process isolation, scheduling, and kernel-level monitoring.

The system enables execution of multiple containers in parallel using Linux namespaces, ensuring isolated process environments. A persistent supervisor process orchestrates container lifecycle operations, including creation, execution, monitoring, and termination.

Additionally, the design integrates both user-space and kernel-space components, providing visibility into system behavior and resource usage.

---

## 🚀 Core Features

### . Namespace-Based Isolation
Containers run in independent execution environments with separate process trees and filesystem views.

### . Supervisor-Based Control
A central supervisor manages all container operations and maintains system coordination.

### . Per-Container Logging System
Each container writes output to its own log file for monitoring and debugging.

### . Kernel-Level Monitoring (LKM)
A Loadable Kernel Module tracks memory usage and enforces limits.

### . Workload Simulation
Includes CPU-bound (`cpu_hog`) and I/O-bound (`io_pulse`) workloads for scheduling analysis.

### . Complete Lifecycle Management
Handles container startup, execution, and cleanup without leaving residual processes.

---

## ⚙️ Build, Load & Execution Steps

### 1. Build the Project
```bash
cd boilerplate
make
```

### 2. Load Kernel Module
```bash
sudo insmod monitor.ko
ls -l /dev/container_monitor
```

### 3. Start Supervisor
```bash
sudo ./engine supervisor ./rootfs-base
```

### 4. Prepare Containers
```bash
cp -a ./rootfs-base ./rootfs-alpha
cp -a ./rootfs-base ./rootfs-beta
```

### 5. Start Containers
```bash
sudo ./engine start alpha ./rootfs-alpha /cpu_hog
sudo ./engine start beta ./rootfs-beta /io_pulse
```

### 6. Inspect Containers
```bash
sudo ./engine ps
sudo ./engine logs alpha
```

### 7. Stop Containers & Cleanup
```bash
sudo ./engine stop alpha
sudo ./engine stop beta
sudo rmmod monitor
```
## 📸 Experimental Demonstration

### 1. Supervisor Startup & Container Initialization
Shows the supervisor process launching and starting multiple containers.
![supervisor_terminal_output](https://github.com/user-attachments/assets/bb6c1773-3ce9-4591-8268-d6945597a2ea)


---

### 2. Container Listing (CLI Output)
Displays container IDs and states using the `engine ps` command.


---

### 3. Kernel-Level Monitoring Output
Shows kernel module logs tracking processes and memory usage.


---

### 4. Container Logging Output
Demonstrates per-container logs being written and stored.


---

### 5. CPU vs IO Scheduling Behavior
Shows difference in CPU usage between CPU-bound and IO-bound workloads.


---

### 6. CPU Hog Execution Logs
Displays continuous CPU workload execution inside container.


---

### 7. System Cleanup & Termination
Shows proper shutdown with no zombie processes.


---

### 8. Performance Analysis Graph
Illustrates relationship between connections and load time.


---

### 9. Supervisor Request Handling
Shows supervisor receiving and processing multiple requests.


---

### 10. Nice Values Scheduling (Priority Control)
Demonstrates scheduling priority using nice values.






