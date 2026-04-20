
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

##  Core Features

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
## Experimental Demonstration

### 1. Supervisor Startup & Container Initialization
![supervisor_terminal_output](https://github.com/user-attachments/assets/bb6c1773-3ce9-4591-8268-d6945597a2ea)
Shows the supervisor process launching and starting multiple containers.

---

### 2. Container Listing (CLI Output)
![container_listing](https://github.com/user-attachments/assets/4997ef6f-1062-4845-a36e-3d846cec5e4e)

Displays container IDs and states using the `engine ps` command.


---

### 3. Kernel-Level Monitoring Output
![kernel_monitor](https://github.com/user-attachments/assets/5cd398bc-d863-40b8-91d6-606d4f8aae81)

Shows kernel module logs tracking processes and memory usage.


---

### 4. Container Logging Output
![logging_out](https://github.com/user-attachments/assets/1920f426-eee7-4f86-8685-ba248da8d097)

Demonstrates per-container logs being written and stored.


---

### 5. CPU vs IO Scheduling Behavior
![cpu_vs_io](https://github.com/user-attachments/assets/7365b036-c43e-439b-a711-dd20bdb350a6)

Shows difference in CPU usage between CPU-bound and IO-bound workloads.


---

### 6. CPU Hog Execution Logs
![logging_out](https://github.com/user-attachments/assets/92a17ac3-66c6-47f2-af2d-7491d6cca9ea)

Displays continuous CPU workload execution inside container.

---

### 7. System Cleanup & Termination
![cleanup_logs](https://github.com/user-attachments/assets/e55b32cf-6843-46ac-bcb8-7224cf3e53fc)

Shows proper shutdown with no zombie processes.


---

### 8. Supervisor Request Handling
![supervisor_requests](https://github.com/user-attachments/assets/b01c2c05-19d6-41bc-801e-bd20e5c7d73f)

Shows supervisor receiving and processing multiple requests.


---

### 9. Nice Values Scheduling (Priority Control)
![nice_values png](https://github.com/user-attachments/assets/2a8dc82c-9e6c-4733-ba6a-a156fba38504)

Demonstrates scheduling priority using nice values.






