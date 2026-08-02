import pandas as pd
import matplotlib.pyplot as plt

# Read CSV
df = pd.read_csv("tests/compressor_plot.csv")

# Plot
plt.figure(figsize=(10, 4))
plt.plot(df["Time"], df["Input"], linestyle='--', label="Input")
plt.plot(df["Time"], df["Output"], label="Compressed Output")
plt.plot(df["Time"], df["Gain"], label="Gain Reduction")

plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title("Compressor Test")
plt.grid(True)
plt.legend()

plt.tight_layout()
plt.show()