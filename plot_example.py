# OS 24 EX1 - Memory Latency Plot
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the data (replace with your CSV file path)
data = pd.read_csv('leestupid.csv', header=None)
# Convert to numpy array for plotting
data = data.to_numpy()

# Cache sizes in bytes (from lscpu output)
l1_size = 32 * 1024  # 32 KiB per core
l2_size = 256 * 1024  # 256 KiB per core
l3_size = 9 * 1024 * 1024  # 9 MiB shared

# Create the plot
plt.figure(figsize=(10, 6))

# Plot both access patterns
plt.plot(data[:, 0], data[:, 1], label="Random access", color='blue')
plt.plot(data[:, 0], data[:, 2], label="Sequential access", color='orange')

# Use logarithmic scales for both axes
plt.xscale('log')
plt.yscale('log')

# Add vertical lines for cache sizes
plt.axvline(x=l1_size, label=f"L1 (32 KiB)", c='r')
plt.axvline(x=l2_size, label=f"L2 (256 KiB)", c='g')
plt.axvline(x=l3_size, label=f"L3 (9 MiB)", c='brown')

# Optional: Add page table eviction threshold for bonus
# Get page size
page_size = 4096  # Default, usually 4KiB on most systems
# Address size (from lscpu: 64-bit system)
address_size = 8  # 8 bytes for 64-bit system
# Calculate page table eviction threshold
page_eviction_threshold = (1/2) * (page_size/address_size) * l3_size
plt.axvline(x=page_eviction_threshold, label=f"Page Table Eviction ({page_eviction_threshold/1024/1024:.1f} MiB)", c='magenta')

# Add labels and title
plt.legend()
plt.title("Latency as a function of array size on Intel(R) Core(TM) i5-8500")
plt.ylabel("Latency (ns log scale)")
plt.xlabel("Bytes allocated (log scale)")

# Save the plot
plt.savefig('memory_latency_plot.png', dpi=300, bbox_inches='tight')

# Show the plot
plt.show()