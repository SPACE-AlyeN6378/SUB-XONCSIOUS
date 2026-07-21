import pandas as pd
import matplotlib.pyplot as plt
import os

# Read CSV
data = pd.read_csv("tests/frequency_response.csv")

# Extract columns
frequency = data["Input Frequency"]
gain = data["Gain (dB)"]

# Plot
plt.figure(figsize=(8, 5))

plt.semilogx(frequency, gain)

plt.xlabel("Frequency (Hz)")
plt.ylabel("Gain (dB)")
plt.title("Biquad Frequency Response")

plt.grid(True, which="both")
plt.xlim(20, 20000)

plt.show()

os.remove("tests/frequency_response.csv")