import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("tests/impulse_response.csv")

time = data["time"].values
x = data["amplitude"].values


# -------------------------
# RMS amplitude envelope
# -------------------------

window = 512

rms = np.sqrt(
    np.convolve(x**2, np.ones(window)/window, mode="valid")
)

rms_time = time[:len(rms)]

# Normalize relative to initial energy
rms_db = 20 * np.log10(rms / np.max(rms) + 1e-12)


# -------------------------
# Plot
# -------------------------

plt.figure(figsize=(12, 8))


# dB envelope
plt.subplot(2, 1, 1)

plt.plot(rms_time, rms_db)

plt.xlabel("Time (seconds)")
plt.ylabel("Level (dB)")
plt.title("RMS Energy Decay Envelope")

plt.ylim(-80, 5)
plt.grid(True)


# Time domain
plt.subplot(2, 1, 2)

markerline, stemlines, baseline = plt.stem(time, x)

plt.setp(stemlines, linewidth=0.3)
plt.setp(markerline, visible=False)
plt.setp(baseline, linewidth=0.3)

plt.xlabel("Time (seconds)")
plt.ylabel("Amplitude")
plt.title("Impulse Response")

max_amp = np.max(np.abs(x))
plt.ylim(0.0, max_amp * 1.2)

# Zoom early reflections
plt.xlim(0, 10.0)

plt.grid(True)


plt.tight_layout()
plt.show()