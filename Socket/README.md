# Sockets IPC Benchmark

This program demonstrates **Interprocess Communication (IPC)** using **UNIX domain sockets**.  
It includes two programs:

- **receiver.c** → Waits for connection, receives data, sends acknowledgment (`ACK`).  
- **sender.c** → Connects to receiver, allows user to test **latency** or **throughput**.  

The programs remain active until the user explicitly chooses the **Exit option** in the sender.

---

## 📂 Files

```
Sockets/
├── sender.c
├── receiver.c
├── README.md
```

---

## ⚙️ Compilation
Use `gcc` (or any C compiler):

```bash
gcc sender.c -o sender
gcc receiver.c -o receiver
```

---

## ▶️ Execution

Start the receiver in one terminal:

```bash
./receiver
```

Start the sender in another terminal:

```bash
./sender
```

---

## 📝 Usage

When you run the sender, you'll see a menu:

```
Select mode:
1. Latency (round-trip)
2. Throughput (one-way)
3. Exit
Choice:
```

### Latency mode

- Enter the size of message (bytes).
- Sender sends 1000 messages of this size.
- Receiver replies with ACK after each message.
- Program measures round-trip time, divides by 2 → reports average one-way latency in ms.

### Throughput mode

- Enter the size of message (bytes).
- Sender sends one large block of data.
- Receiver replies with ACK once.
- Program measures total transfer time and reports throughput in MB/s.

### Exit mode

- Sender closes connection and terminates.
- Receiver automatically detects closure and exits.

---

## 📊 Example Runs

### Latency test (1 KB message)
```
Select mode:
1. Latency (round-trip)
2. Throughput (one-way)
3. Exit
Choice: 1
Enter data size in bytes: 1024
Average one-way latency: 0.035 ms
```

### Throughput test (10 MB message)
```
Select mode:
1. Latency (round-trip)
2. Throughput (one-way)
3. Exit
Choice: 2
Enter data size in bytes: 10485760
Throughput: 920.45 MB/s (11.300 ms for 10485760 bytes)
```

---

## 🔧 Notes

- Uses UNIX domain sockets (local IPC only).
- Socket path: `/tmp/ipc_socket`
- Make sure no stale socket file exists before running (program handles this with `unlink`).
- Recommended to run on the same machine for accurate results.
