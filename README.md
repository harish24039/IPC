# Inter-Process Communication (IPC) Benchmark Suite

This repository contains a comprehensive benchmarking suite for comparing different Inter-Process Communication (IPC) mechanisms in C. The project implements and evaluates three primary IPC methods: **Message Queues**, **Shared Memory**, and **UNIX Domain Sockets**.

## 🎯 Overview

This project provides practical implementations and performance analysis of different IPC mechanisms, measuring both **latency** and **throughput** characteristics across varying data sizes. Each implementation includes sender/receiver programs that can handle both small messages (latency testing) and large data transfers (throughput testing).


```

## 🚀 Quick Start

### Prerequisites

- GCC compiler
- POSIX-compliant system (Linux)
- Python 3.x with matplotlib (for plotting)

### Compilation

Each IPC method can be compiled independently:

#### Message Queues
```bash
cd MQ/
gcc sender.c -o sender -lrt
gcc receiver.c -o receiver -lrt
```

#### Shared Memory
```bash
cd SM/
gcc writer.c -o writer -lrt
gcc reader.c -o reader -lrt
```

#### UNIX Domain Sockets
```bash
cd Socket/
gcc sender.c -o sender
gcc receiver.c -o receiver
```

### Execution

For each IPC method:
1. Start the receiver/reader in one terminal
2. Start the sender/writer in another terminal
3. Follow the interactive menu to run latency or throughput tests

## 📊 Performance Analysis

### Key Findings

- **Shared Memory**: Highest throughput (~7GB/s), lowest latency for large data
- **UNIX Domain Sockets**: Moderate performance, good balance of features
- **Message Queues**: Lower throughput but excellent for small message passing

### Benchmarking Features

- **Latency Testing**: Measures round-trip time for small messages
- **Throughput Testing**: Measures data transfer rates for large payloads
- **Scalable Message Sizes**: Supports messages from 1KB to 100MB
- **Chunking Support**: Automatic message segmentation for size limits

## 🔧 Technical Implementation

### Message Queues (POSIX)
- Uses POSIX message queues (`mqueue.h`)
- Implements automatic chunking for large messages
- Maximum single message: 8KB (system limit)
- Handles messages up to 100MB through chunking

### Shared Memory (POSIX)
- Uses POSIX shared memory (`shm_open`, `mmap`)
- Atomic operations for synchronization
- Memory barriers for data consistency
- Supports up to 110MB shared region

### UNIX Domain Sockets
- Stream-based communication
- Built-in flow control and reliability
- No message size limitations
- Connection-oriented protocol

## 📈 Visualization

The `Plots/` directory contains:
- Performance comparison graphs
- Latency vs. block size analysis
- Throughput vs. block size analysis
- Python script for generating custom plots

Generate plots:
```bash
cd Plots/
python3 plots.py
```

## 🎓 Educational Value

This project demonstrates:
- **System Programming**: Low-level IPC implementations
- **Performance Analysis**: Benchmarking methodologies
- **Concurrency**: Multi-process synchronization
- **POSIX Standards**: Cross-platform system programming
- **Data Visualization**: Performance metric presentation

## 📝 Usage Examples

### Latency Testing
Test small message round-trip times to measure communication overhead.

### Throughput Testing
Transfer large data blocks to measure maximum data transfer rates.

### Comparative Analysis
Run the same tests across all three IPC methods to compare performance characteristics.

## 🔍 Implementation Details

Each IPC method includes:
- Interactive menu system
- Configurable message sizes
- Performance timing measurements
- Error handling and cleanup
- Detailed status reporting

## 📚 Documentation

- Individual README files in each subdirectory
- Inline code comments
- Comprehensive report in `Report/` directory
- Performance analysis and conclusions

## 🤝 Contributing

This is an academic project demonstrating IPC concepts. Feel free to:
- Extend the benchmarking suite
- Add new IPC mechanisms
- Improve performance measurements
- Enhance visualization capabilities

## 📄 License

This project is for educational purposes. Please refer to your institution's academic policies regarding code sharing and attribution.

---

**Author**: Harish Hatmode  
**Course**: Networks for AI/ML
**Assignment**: IPC Performance Analysis
