import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

CSV_FILE = "gen/cache_hierarchy.csv"
OUTPUT_FILE = "assets/graphs.png"

def plot_graphs():
    df = pd.read_csv(CSV_FILE)
    fig, ax = plt.subplots(figsize=(10, 5))  # Create a single subplot

    ax.plot(df["size (bytes)"], df["latency (nsec)"], marker='o')
    ax.set_title("Size vs Latency")
    ax.set_xlabel("Size (bytes)")
    ax.set_ylabel("Latency (nsec)")

    plt.savefig(OUTPUT_FILE)

if __name__ == "__main__":
    plot_graphs()