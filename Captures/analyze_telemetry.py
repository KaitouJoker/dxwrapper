#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DxWrapper x64 General - PresentMon Telemetry Benchmark Analyzer
Extracts and computes rendering throughput, frametime pacing, jitter,
inter-frame ripple, display latency, and hardware queue timings from
raw PresentMon capture CSV datasets.
"""

import os
import sys
import csv
import math

try:
    import numpy as np
    HAVE_NUMPY = True
except ImportError:
    HAVE_NUMPY = False


def percentile(sorted_data, p):
    """Linear interpolation percentile matching numpy / Excel standards."""
    if not sorted_data:
        return 0.0
    if HAVE_NUMPY:
        return float(np.percentile(sorted_data, p))
    k = (len(sorted_data) - 1) * (p / 100.0)
    f = math.floor(k)
    c = math.ceil(k)
    if f == c:
        return sorted_data[int(k)]
    return sorted_data[int(f)] * (c - k) + sorted_data[int(c)] * (k - f)


def load_telemetry(csv_path):
    """Loads and computes key telemetry metrics from PresentMon CSV."""
    if not os.path.exists(csv_path):
        raise FileNotFoundError(f"Telemetry file not found: {csv_path}")

    target_cols = [
        'TimeInSeconds', 'MsBetweenPresents', 'MsUntilDisplayed',
        'MsCPUBusy', 'MsCPUWait', 'MsGPUTime', 'MsGPULatency'
    ]
    raw = {c: [] for c in target_cols}
    spikes_1ms = 0
    spikes_2ms = 0
    spikes_5ms = 0

    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            p_str = row.get('MsBetweenPresents', '')
            if not p_str or p_str == 'NA':
                continue
            try:
                p = float(p_str)
            except ValueError:
                continue

            if p >= 5.0:
                spikes_5ms += 1
            elif p >= 2.0:
                spikes_2ms += 1
            elif p >= 1.0:
                spikes_1ms += 1

            for c in target_cols:
                val = row.get(c, '')
                if val and val != 'NA':
                    try:
                        raw[c].append(float(val))
                    except ValueError:
                        pass

    ft = raw['MsBetweenPresents']
    if not ft:
        raise ValueError(f"No valid frametime data found in {csv_path}")

    total_frames = len(ft)
    times = raw['TimeInSeconds']
    duration = (times[-1] - times[0]) if len(times) > 1 else (sum(ft) / 1000.0)
    s_ft = sorted(ft)
    ft_mean = sum(ft) / total_frames
    avg_fps = 1000.0 / ft_mean

    # FPS distribution
    fps_list = [1000.0 / x for x in ft if x > 0]
    s_fps = sorted(fps_list)

    # Frametime Jitter (Standard Deviation)
    variance = sum((x - ft_mean) ** 2 for x in ft) / total_frames
    std_ft = math.sqrt(variance)

    # Frame-to-Frame Ripple (Consecutive frame delta |t_i - t_{i-1}|)
    ripples = [abs(ft[i] - ft[i - 1]) for i in range(1, total_frames)]
    mean_ripple = (sum(ripples) / len(ripples)) if ripples else 0.0

    def col_mean(name):
        arr = raw[name]
        return (sum(arr) / len(arr)) if arr else 0.0

    def col_p99(name):
        arr = raw[name]
        return percentile(sorted(arr), 99) if arr else 0.0

    return {
        'path': csv_path,
        'filename': os.path.basename(csv_path),
        'total_frames': total_frames,
        'duration': duration,
        'avg_fps': avg_fps,
        'peak_fps': max(fps_list) if fps_list else 0.0,
        'low_1pct_fps': percentile(s_fps, 1),
        'low_5pct_fps': percentile(s_fps, 5),
        'min_fps': min(fps_list) if fps_list else 0.0,
        'ft_mean': ft_mean,
        'ft_p50': percentile(s_ft, 50),
        'ft_p95': percentile(s_ft, 95),
        'ft_p99': percentile(s_ft, 99),
        'ft_p99_9': percentile(s_ft, 99.9),
        'ft_max': s_ft[-1],
        'ft_jitter_sigma': std_ft,
        'ft_ripple_delta': mean_ripple,
        'disp_lat_mean': col_mean('MsUntilDisplayed'),
        'disp_lat_p99': col_p99('MsUntilDisplayed'),
        'cpu_busy_mean': col_mean('MsCPUBusy'),
        'cpu_busy_p99': col_p99('MsCPUBusy'),
        'gpu_time_mean': col_mean('MsGPUTime'),
        'gpu_time_p99': col_p99('MsGPUTime'),
        'gpu_lat_mean': col_mean('MsGPULatency'),
        'gpu_lat_p99': col_p99('MsGPULatency'),
        'spikes_1ms': spikes_1ms + spikes_2ms + spikes_5ms,
        'spikes_2ms': spikes_2ms + spikes_5ms,
        'spikes_5ms': spikes_5ms,
    }


def format_delta(v1, v2, unit='', lower_better=True, precision=1):
    diff = v2 - v1
    pct = (diff / v1 * 100.0) if v1 != 0 else 0.0
    sign = '+' if diff > 0 else ''
    if precision == 1:
        diff_str = f"{sign}{diff:.1f}"
    elif precision == 3:
        diff_str = f"{sign}{diff:.3f}"
    else:
        diff_str = f"{sign}{diff}"
    return f"**{diff_str} {unit} ({sign}{pct:.1f}%)**" if unit else f"**{diff_str} ({sign}{pct:.1f}%)**"


def print_comparison_table(b, o, title="PresentMon Telemetry Benchmark Comparison"):
    """Prints GitHub Flavored Markdown comparison table matching repository docs."""
    print(f"\n### {title}\n")
    print(f"- **Baseline Data**: `{b['filename']}` ({b['total_frames']:,} frames, {b['duration']:.2f}s)")
    print(f"- **Optimized Data**: `{o['filename']}` ({o['total_frames']:,} frames, {o['duration']:.2f}s)\n")
    print("| Metric | Baseline | Optimized | Delta (Change) |")
    print("| :--- | :---: | :---: | :--- |")

    rows = [
        ("Average Framerate", f"{b['avg_fps']:.1f} FPS", f"**{o['avg_fps']:.1f} FPS**",
         format_delta(b['avg_fps'], o['avg_fps'], 'FPS', lower_better=False, precision=1)),
        ("Peak Instantaneous FPS", f"{b['peak_fps']:.1f} FPS", f"**{o['peak_fps']:.1f} FPS**",
         format_delta(b['peak_fps'], o['peak_fps'], 'FPS', lower_better=False, precision=1)),
        ("1% Low Framerate", f"{b['low_1pct_fps']:.1f} FPS", f"**{o['low_1pct_fps']:.1f} FPS**",
         format_delta(b['low_1pct_fps'], o['low_1pct_fps'], 'FPS', lower_better=False, precision=1)),
        ("5% Low Framerate", f"{b['low_5pct_fps']:.1f} FPS", f"**{o['low_5pct_fps']:.1f} FPS**",
         format_delta(b['low_5pct_fps'], o['low_5pct_fps'], 'FPS', lower_better=False, precision=1)),
        ("Minimum Framerate", f"{b['min_fps']:.1f} FPS", f"**{o['min_fps']:.1f} FPS**",
         format_delta(b['min_fps'], o['min_fps'], 'FPS', lower_better=False, precision=1)),
        ("Average Frametime", f"{b['ft_mean']:.3f} ms", f"**{o['ft_mean']:.3f} ms ({o['ft_mean']*1000:.0f} µs)**",
         format_delta(b['ft_mean'], o['ft_mean'], 'ms', lower_better=True, precision=3)),
        ("Frametime Jitter / Std Dev ($\\sigma$)", f"{b['ft_jitter_sigma']:.3f} ms ({b['ft_jitter_sigma']*1000:.1f} µs)", f"**{o['ft_jitter_sigma']:.3f} ms ({o['ft_jitter_sigma']*1000:.1f} µs)**",
         format_delta(b['ft_jitter_sigma'], o['ft_jitter_sigma'], 'ms', lower_better=True, precision=3)),
        ("Frame-to-Frame Ripple (Mean $\\Delta t$)", f"{b['ft_ripple_delta']:.3f} ms ({b['ft_ripple_delta']*1000:.1f} µs)", f"**{o['ft_ripple_delta']:.3f} ms ({o['ft_ripple_delta']*1000:.1f} µs)**",
         format_delta(b['ft_ripple_delta'], o['ft_ripple_delta'], 'ms', lower_better=True, precision=3)),
        ("Frametime 99th Percentile (p99)", f"{b['ft_p99']:.3f} ms", f"**{o['ft_p99']:.3f} ms**",
         format_delta(b['ft_p99'], o['ft_p99'], 'ms', lower_better=True, precision=3)),
        ("Frametime 99.9th Percentile (p99.9)", f"{b['ft_p99_9']:.3f} ms", f"**{o['ft_p99_9']:.3f} ms**",
         format_delta(b['ft_p99_9'], o['ft_p99_9'], 'ms', lower_better=True, precision=3)),
        ("Maximum Frametime Spike", f"{b['ft_max']:.3f} ms", f"**{o['ft_max']:.3f} ms**",
         format_delta(b['ft_max'], o['ft_max'], 'ms', lower_better=True, precision=3)),
        ("Spikes $\\ge$ 1.0 ms across 60s", f"{b['spikes_1ms']:,} frames ({b['spikes_1ms']/b['total_frames']*100:.1f}%)", f"**{o['spikes_1ms']:,} frames**",
         format_delta(b['spikes_1ms'], o['spikes_1ms'], 'frames', lower_better=True, precision=0)),
        ("Spikes $\\ge$ 5.0 ms across 60s", f"{b['spikes_5ms']:,} spikes", f"**{o['spikes_5ms']:,} spikes**",
         format_delta(b['spikes_5ms'], o['spikes_5ms'], 'spikes', lower_better=True, precision=0)),
    ]

    for metric, base_val, opt_val, delta in rows:
        print(f"| **{metric}** | {base_val} | {opt_val} | {delta} |")


def main():
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')

    script_dir = os.path.dirname(os.path.abspath(__file__))

    if len(sys.argv) == 3:
        b = load_telemetry(sys.argv[1])
        o = load_telemetry(sys.argv[2])
        print_comparison_table(b, o)
    elif len(sys.argv) == 2:
        d = load_telemetry(sys.argv[1])
        print(f"\n### PresentMon Telemetry Summary: {d['filename']}\n")
        print(f"- Total Frames: {d['total_frames']:,}")
        print(f"- Duration: {d['duration']:.2f} s")
        print(f"- Average FPS: {d['avg_fps']:.2f} FPS")
        print(f"- Peak Instantaneous FPS: {d['peak_fps']:.1f} FPS")
        print(f"- 1% Low FPS: {d['low_1pct_fps']:.1f} FPS")
        print(f"- 5% Low FPS: {d['low_5pct_fps']:.1f} FPS")
        print(f"- Min FPS: {d['min_fps']:.1f} FPS")
        print(f"- Mean Frametime: {d['ft_mean']:.4f} ms")
        print(f"- Frametime Median (p50): {d['ft_p50']:.4f} ms")
        print(f"- Frametime p99: {d['ft_p99']:.4f} ms")
        print(f"- Frametime p99.9: {d['ft_p99_9']:.4f} ms")
        print(f"- Max Spike: {d['ft_max']:.4f} ms")
        print(f"- Frametime Jitter (σ): {d['ft_jitter_sigma']:.4f} ms ({d['ft_jitter_sigma']*1000:.1f} µs)")
        print(f"- Inter-frame Ripple (Δt): {d['ft_ripple_delta']:.4f} ms ({d['ft_ripple_delta']*1000:.1f} µs)")
        print(f"- Mean Display Latency: {d['disp_lat_mean']:.4f} ms")
        print(f"- Mean CPU Busy Time: {d['cpu_busy_mean']:.4f} ms")
        print(f"- Mean GPU Execution Time: {d['gpu_time_mean']:.4f} ms")
        print(f"- Spikes >= 1.0ms: {d['spikes_1ms']} frames")
        print(f"- Spikes >= 5.0ms: {d['spikes_5ms']} frames")
    else:
        # Default mode: compare default archived datasets in Captures directory
        f_legacy = os.path.join(script_dir, 'pmcap-KartRider.exe-261106-075244.csv')
        f_8777 = os.path.join(script_dir, 'pmcap-KartRider.exe-261106-085359.csv')

        if os.path.exists(f_legacy) and os.path.exists(f_8777):
            b = load_telemetry(f_legacy)
            o = load_telemetry(f_8777)
            print_comparison_table(b, o, title="KartRider x64: Legacy Runtime vs DxWrapper v1.9.8777.25")
        else:
            print("Usage:")
            print("  python analyze_telemetry.py <capture.csv>")
            print("  python analyze_telemetry.py <baseline.csv> <optimized.csv>")


if __name__ == '__main__':
    main()
