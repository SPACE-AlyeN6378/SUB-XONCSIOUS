import numpy as np
import csv

components = {
    "WAV Audio Streaming": "wave_sound.txt",
    "Fluidsynth Rendering": "fs_instrument.txt",
    "Osc-based Synthesizer": "osc_synth.txt",
    "SFZ Rendering": "sfz_instrument.txt",
    "RBJ Biquad Processing": "biquad_process.txt",
    "RBJ Biquad Parameter": "biquad_update.txt",
    "SVF Filter Sweeping": "svf_freq_sweep.txt",
    "Reverb": "reverb.txt",
    "Compressor": "compressor.txt",
    "Echo/Delay": "echo.txt"
}

def analyze(filename):
    with open(filename, "r") as f:
        data = np.array([float(line.strip()) for line in f])

    return {
        "mean": np.mean(data),
        "max": np.max(data),
        "stddev": np.std(data, ddof=1)  # Sample standard deviation
    }

with open("benchmark_results.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)

    # Header
    writer.writerow([
        "Component",
        "Mean (µs)",
        "Max (µs)",
        "Std Dev (µs)"
    ])

    # Data rows
    for name, filename in components.items():
        stats = analyze(filename)

        writer.writerow([
            name,
            round(stats["mean"], 3),
            round(stats["max"], 3),
            round(stats["stddev"], 3)
        ])

print("CSV file written successfully.")


components = {
    "wav": "wave_sound.txt",
    "fs": "fs_instrument.txt",
    "osc": "osc_synth.txt",
    "sfz": "sfz_instrument.txt",
    "rbj": "biquad_process.txt",
    "svf": "svf_freq_sweep.txt",
    "reverb": "reverb.txt",
    "compressor": "compressor.txt",
    "echo": "echo.txt"
}

thread1 = {
    "Track 1": ["osc", "rbj"],
    "Track 2": ["osc", "svf"],
    "Track 3": ["fs", "rbj", "echo"]
}

thread2 = {
    "Track 4": ["wav", "compressor", "echo"],
    "Track 5": ["wav"]
}

thread3 = {
    "Track 6": ["sfz", "rbj", "compressor"]
}

key = "mean"

thread1_time = sum([sum([analyze(components[comp])[key] for comp in ls]) for ls in thread1.values()])
thread2_time = sum([sum([analyze(components[comp])[key] for comp in ls]) for ls in thread2.values()])
thread3_time = sum([sum([analyze(components[comp])[key] for comp in ls]) for ls in thread3.values()])

allTotal = 0
for track, selected in thread1.items():
    times = [analyze(components[comp])[key] for comp in selected]
    total = sum(times)
    print(track + ":", " + ".join([f"{time:.3f}" for time in times]), "=", f"{total:.3f}")

    allTotal += total

print("Total: ", allTotal)

print("Overall:", allTotal + analyze(components["reverb"])[key])

print("Single thread result", thread1_time+thread2_time+thread3_time+analyze(components["reverb"])[key])
# print("Thread 1:", " + ".join([analyze(components[comp])[key] for comp in ls]), thread1_time)
# print("Thread 2:", thread2_time)
# print("Thread 3:", thread3_time)