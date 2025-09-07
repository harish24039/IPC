import matplotlib.pyplot as plt
import numpy as np

# Data for Shared Memory
sm_block_size_kb = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
sm_latency_ms = [0.004318, 0.004107, 0.004445, 0.003611, 0.004372, 0.004443, 0.008887, 0.014692, 0.023005, 0.048482, 0.097129]

sm_block_size_mb = [1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
sm_throughput_mbs = [14665.34, 4019.14, 5332.93, 5727.87, 6044.28, 6324.45, 6356.21, 6556.33, 6818.2, 6962.06, 7050.19]

# Data for Socket
socket_block_size_kb = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
socket_latency_ms = [0.043, 0.055, 0.046, 0.054, 0.074, 0.077, 0.092, 0.107, 0.167, 0.337, 0.532]

socket_block_size_mb = [1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
socket_throughput_mbs = [875.21, 1417.90, 1458.01, 1628.36, 1582.63, 1714.84, 2043.18, 1740.23, 1839.73, 1869.29, 1899.09]

# Data for Message Queue
mq_block_size_kb = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
mq_latency_ms = [0.010975, 0.022179, 0.017751, 0.051311, 0.089289, 0.154802, 0.306202, 0.123081, 0.529142, 0.833492, 1.904443]

mq_block_size_mb = [1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
mq_throughput_mbs = [578.93, 554.15, 558.88, 498.98, 525.55, 552.16, 543.10, 588.20, 592.73, 571.56, 601.83]

# Create subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

# Plot 1: Latency vs Block Size
ax1.plot(sm_block_size_kb, sm_latency_ms, 'b-o', label='Shared Memory', linewidth=2, markersize=6)
ax1.plot(socket_block_size_kb, socket_latency_ms, 'r-s', label='Socket', linewidth=2, markersize=6)
ax1.plot(mq_block_size_kb, mq_latency_ms, 'g-^', label='Message Queue', linewidth=2, markersize=6)

ax1.set_xlabel('Block Size (KB)', fontsize=12)
ax1.set_ylabel('Latency (ms)', fontsize=12)
ax1.set_title('IPC Latency Comparison', fontsize=14, fontweight='bold')
ax1.legend(fontsize=10)
ax1.grid(True, alpha=0.3)
ax1.set_xscale('log')
ax1.set_yscale('log')

# Plot 2: Throughput vs Block Size
ax2.plot(sm_block_size_mb, sm_throughput_mbs, 'b-o', label='Shared Memory', linewidth=2, markersize=6)
ax2.plot(socket_block_size_mb, socket_throughput_mbs, 'r-s', label='Socket', linewidth=2, markersize=6)
ax2.plot(mq_block_size_mb, mq_throughput_mbs, 'g-^', label='Message Queue', linewidth=2, markersize=6)

ax2.set_xlabel('Block Size (MB)', fontsize=12)
ax2.set_ylabel('Throughput (MB/s)', fontsize=12)
ax2.set_title('IPC Throughput Comparison', fontsize=14, fontweight='bold')
ax2.legend(fontsize=10)
ax2.grid(True, alpha=0.3)

# Adjust layout and save
plt.tight_layout()
plt.savefig('ipc_comparison.png', dpi=300, bbox_inches='tight')
plt.show()

print("Plots have been generated and saved as 'ipc_comparison.png'")