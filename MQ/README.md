# Message Queue IPC Example

This directory demonstrates Inter-Process Communication (IPC) using POSIX message queues in C. It includes a sender and receiver program capable of transferring large messages by chunking and reassembling them.

## Files

- `sender.c`: Sends messages (latency or throughput test) to the receiver using a POSIX message queue. Large messages are split into chunks.
- `receiver.c`: Receives message chunks, reassembles them, and prints status when complete.

## How It Works

- Messages larger than the system's maximum queue size (8 KB) are split into chunks.
- Each chunk includes a header with metadata for reassembly: message ID, total chunks, chunk index, and chunk size.
- The receiver tracks incoming chunks and reassembles the full message when all chunks are received.

## Compilation

Compile both programs in the MQ directory:

```bash
gcc sender.c -o sender.o -lrt
gcc receiver.c -o receiver.o -lrt
```

## Execution

1. Start the receiver in one terminal:

	```bash
	./receiver.o
	```

	Expected output:
	```
	Message Queue Receiver ready.
	```

2. Start the sender in another terminal:

	```bash
	./sender.o
	```

	Expected input prompts:
	```
	Menu:
	1. Latency test (small messages)
	2. Throughput test (large messages)
	3. Exit
	Enter choice: 
	Enter message size in bytes (<= 104857600): 
	Enter number of messages: 
	```

	Example output for latency test:
	```
	Latency per message: 0.123456 ms
	```

	Example output for throughput test:
	```
	Throughput: 12.34 MB/s
	```

	The receiver will print chunk and message status, e.g.:
	```
	Received chunk 1/5 for message 0 (8192 bytes)
	Complete message 0 received: 32768 bytes, first char: A
	```

## Notes

- The queue name is `/myqueue` (see `QUEUE_NAME` macro).
- Maximum message size for user data is 100 MB (see `MAX_USER_MSG_SIZE`).
- The sender automatically unlinks the queue after completion.
- The receiver supports up to 100 concurrent messages by default.

## Requirements

- Linux system with POSIX message queue support (`-lrt` library required for linking).

## References

- [man mq_overview](https://man7.org/linux/man-pages/man7/mq_overview.7.html)
- [POSIX Message Queues](https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_open.html)
