# OS 24 EX1
import pandas as pd
import matplotlib.pyplot as plt

# Read your data
data = pd.read_csv(r"./new_measure.csv")
data = data.to_numpy()

# Set cache sizes in bytes from your lscpu output
# For L1d and L2, we use per-core values since the test is single-threaded
l1_size = 32 * 1024      # 32 KiB per core (L1d is 128 KiB / 4 instances)
l2_size = 256 * 1024     # 256 KiB per core (1 MiB / 4 instances)
l3_size = 6 * 1024 * 1024  # 6 MiB total (shared)

# Calculate page table eviction threshold
page_size = 4096         # From getconf PAGE_SIZE
address_size = 8         # 8 bytes for 64-bit system
threshold = 0.5 * (page_size/address_size) * l3_size  # ~1.5 GiB

# Create the plot
plt.figure(figsize=(10, 6))
plt.plot(data[:, 0], data[:, 1], label="Random access")
plt.plot(data[:, 0], data[:, 2], label="Sequential access")
plt.xscale('log')
plt.yscale('log')

# Add cache size lines
plt.axvline(x=l1_size, label="L1 (32 KiB)", c='r')
plt.axvline(x=l2_size, label="L2 (256 KiB)", c='g')
plt.axvline(x=l3_size, label="L3 (6 MiB)", c='brown')

# Add page table eviction threshold for bonus (magenta color as shown in Figure 2)
plt.axvline(x=threshold, label="Page Table Eviction Threshold", c='magenta')

plt.legend()
plt.title("Latency as a function of array size on Intel Core i5-6600")
plt.ylabel("Latency (ns)")
plt.xlabel("Bytes allocated (log scale)")
plt.grid(True, which="both", ls="--", alpha=0.3)
plt.tight_layout()
plt.savefig("memory_latency_plot.png", dpi=300)
plt.show()