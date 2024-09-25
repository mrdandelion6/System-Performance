import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

CSV_FILE = "gen/cache_hierarchy.csv"
SCATTER = "assets/scatter.png"
BOXPLOT = "assets/boxplot.png"

def filter_outliers(df, column, threshold=3):
    mean = df[column].mean()
    std = df[column].std()
    return df[(df[column] >= mean - threshold * std) & (df[column] <= mean + threshold * std)]

def plot_scatter(df):
    filtered_df = filter_outliers(df, "latency (nsec)")
    plt.figure(figsize=(10, 5))
    plt.scatter(filtered_df["size (bytes)"], filtered_df["latency (nsec)"], marker='o')
    plt.title("Size vs Latency (Outliers Excluded)")
    plt.xlabel("Size (bytes)")
    plt.ylabel("Latency (nsec)")
    plt.tight_layout()
    plt.savefig(SCATTER)
    plt.close()

def plot_boxplot(data):
    # Ensure data is a DataFrame with 'size (bytes)' and 'latency (nsec)' columns
    if not isinstance(data, pd.DataFrame):
        raise ValueError("Input must be a pandas DataFrame")
    if 'size (bytes)' not in data.columns or 'latency (nsec)' not in data.columns:
        raise ValueError("DataFrame must have 'size (bytes)' and 'latency (nsec)' columns")

    # Define intervals for input size
    max_size = data['size (bytes)'].max()
    intervals = [
        (0, 0.5e7),
        (0.5e7, 0.76e7),
        (0.76e7, max_size)
    ]

    # Group latencies by size intervals
    latencies_by_interval = [
        data[(data['size (bytes)'] >= start) & (data['size (bytes)'] < end)]['latency (nsec)'].tolist()
        for start, end in intervals
    ]

    # Create labels for the intervals
    labels = [
        '0 to 0.5e7',
        '0.5e7 to 0.76e7',
        f'0.76e7 to {max_size:.2e}'
    ]

    # Create box plots
    plt.figure(figsize=(12, 6))
    bp = plt.boxplot(latencies_by_interval, tick_labels=labels, showfliers=False)
    plt.title("Latency Distribution by Input Size")
    plt.xlabel("Input Size Intervals (bytes)")
    plt.ylabel("Latency (nsec)")
    plt.yscale('log')  # Use log scale for latency axis

    # Add median value labels
    for i, line in enumerate(bp['medians']):
        x, y = line.get_xydata()[1]
        plt.text(x, y, f'{y:.2e}', horizontalalignment='center', verticalalignment='bottom')

    plt.tight_layout()
    plt.savefig(BOXPLOT)
    plt.close()

def plot_graphs():
    df = pd.read_csv(CSV_FILE)
    plot_scatter(df)
    plot_boxplot(df)

if __name__ == "__main__":
    plot_graphs()