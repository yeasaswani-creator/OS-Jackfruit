# 📦 Multi-Container Runtime System

## 👥 Team Information

- **Name:** SANGADALA YEASASWANI  
  **SRN:** PES2UG24CS441  

- **Name:** SHAYNA ZAIDI GUPTA  
  **SRN:** PES2UG24CS465  

---

## 📖 System Overview

This project implements a lightweight container runtime for Linux systems, developed in C, with the objective of exploring key operating system concepts such as process isolation, scheduling, and kernel-level monitoring.

The system enables execution of multiple containers in parallel using Linux namespaces, ensuring isolated process environments. A persistent supervisor process orchestrates container lifecycle operations, including creation, execution, monitoring, and termination.

Additionally, the design integrates both user-space and kernel-space components, providing visibility into system behavior and resource usage.

---

## 🚀 Core Features

### 🔹 Namespace-Based Isolation
Containers run in independent execution environments with separate process trees and filesystem views.

### 🔹 Supervisor-Based Control
A central supervisor manages all container operations and maintains system coordination.

### 🔹 Per-Container Logging System
Each container writes output to its own log file for monitoring and debugging.

### 🔹 Kernel-Level Monitoring (LKM)
A Loadable Kernel Module tracks memory usage and enforces limits.

### 🔹 Workload Simulation
Includes CPU-bound (`cpu_hog`) and I/O-bound (`io_pulse`) workloads for scheduling analysis.

### 🔹 Complete Lifecycle Management
Handles container startup, execution, and cleanup without leaving residual processes.

---

## ⚙️ Build, Load & Execution Steps

### 🔹 1. Build the Project
```bash
cd boilerplate
make
