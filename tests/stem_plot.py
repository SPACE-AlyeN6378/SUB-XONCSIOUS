import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
data = pd.read_csv("tests/impulse_response.csv")

time = data["time"]
amplitude = data["amplitude"]

plt.figure(figsize=(12, 5))

markerline, stemlines, baseline = plt.stem(time, amplitude)

plt.setp(stemlines, linewidth=0.3)
plt.setp(markerline, visible=False)
plt.setp(baseline, linewidth=0.3)

plt.xlabel("Time (seconds)")
plt.ylabel("Amplitude")
plt.title("Impulse Response")
plt.grid(True)

plt.show()