import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("tests/impulse_response.csv")

time = data["time"].values
x = data["amplitude"].values

# RMS window size
window = 512

rms = np.sqrt(
    np.convolve(x**2, np.ones(window)/window, mode="valid")
)

# Match time axis
rms_time = time[:len(rms)]

# Convert to dB
rms_db = 20 * np.log10(rms + 1e-12)

plt.figure(figsize=(12,5))

plt.plot(rms_time, rms_db)

plt.xlabel("Time (seconds)")
plt.ylabel("Level (dB)")
plt.title("Impulse Response RMS Envelope")
plt.ylim(-120, 5)
plt.grid(True)

plt.show()