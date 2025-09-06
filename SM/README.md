# Shared Memory IPC Benchmark

This program demonstrates **Interprocess Communication (IPC)** using **POSIX shared memory**.  
It includes two programs:

- **writer.c** → Creates shared memory region, writes data, measures performance.  
- **reader.c** → Connects to shared memory region, reads data, displays received messages.  

The programs use atomic operations and memory barriers for synchronization between processes.

---

## 📂 Files

```
Shmem/
├── writer.c
├── reader.c
├── README.md
```

---

## ⚙️ Compilation
Use `gcc` (or any C compiler) with real-time library:

```bash
gcc writer.c -o writer -lrt
gcc reader.c -o reader -lrt
```

**Note:** The `-lrt` flag is required for POSIX shared memory functions.

---

## ▶️ Execution

Start the reader in one terminal:

```bash
./reader
```

Start the writer in another terminal:

```bash
./writer
```

---

## 📝 Usage

When you run the writer, you'll see a menu:

```
Menu:
1. Latency test (small messages)
2. Throughput test (large message)
3. Exit
Enter choice:
```

### Latency Test

- Enter message size in bytes
- Enter number of messages to send
- Writer sends multiple small messages sequentially
- Measures average time per message
- Reports latency per message in milliseconds

### Throughput Test

- Enter message size in bytes  
- Enter number of messages to send
- Writer sends data and measures total transfer time
- Reports throughput in MB/s

### Reader Behavior

- Continuously monitors shared memory region
- Displays number of bytes received and first character of each message
- Shows different characters as writer cycles through A-Z pattern

---

## 🔧 Technical Details

### Shared Memory Configuration
- **Shared memory name:** `/myshm`
- **Maximum size:** ~110 MB
- **Synchronization:** Atomic operations with spin locks

### Data Structure
```c
struct shm_region {
    size_t size;        // Size of current message
    int ready;          // Synchronization flag
    char buffer[1];     // Flexible array for data
};
```

### Synchronization Mechanism
- Writer uses `__sync_lock_test_and_set()` to acquire lock
- Reader clears the ready flag after processing
- Memory barriers ensure proper ordering of operations

---

## 📊 Example Runs

### Latency Test (1 KB messages)
```
Menu:
1. Latency test (small messages)
2. Throughput test (large message)
3. Exit
Enter choice: 1
Enter message size in bytes: 1024
Enter number of messages: 1000
Latency per message: 0.002500 ms
```

### Throughput Test (10 MB message)
```
Menu:
1. Latency test (small messages)
2. Throughput test (large message)
3. Exit
Enter choice: 2
Enter message size in bytes: 10485760
Enter number of messages: 1
Throughput: 1250.75 MB/s
```

### Reader Output
```
Shared memory receiver ready.
Received 1024 bytes. First char: A
Received 1024 bytes. First char: B
Received 1024 bytes. First char: C
...
```

---

## 🔧 Notes

- **Platform:** POSIX-compliant systems (Linux, macOS, etc.)
- **Cleanup:** Writer automatically removes shared memory on exit
- **Performance:** Shared memory typically faster than sockets for large data transfers
- **Synchronization:** Uses busy-waiting (spin locks) for low latency
- **Memory:** Pre-allocates large shared memory region for maximum performance

### Troubleshooting
- If "Permission denied" error occurs, try running with appropriate permissions
- If shared memory persists after crash, manually remove with: `rm /dev/shm/myshm` (Linux)
- On macOS, shared memory is automatically cleaned up by the system

---

## 🚀 Performance Characteristics

Shared memory IPC typically offers:
- **Lower latency** than socket-based communication
- **Higher throughput** for large data transfers
- **Zero-copy** data sharing between processes
- **Minimal kernel overhead** after initial setup
