#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DxWrapper x64 General - PresentMon Telemetry Metrics Extractor
Reads a single PresentMon CSV capture file and outputs an exact,
rigorous table of measured rendering throughput, frametime pacing,
jitter, ripple, display latency, and hardware queue timings.
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
        raise FileNotFoundError(f"File not found: {csv_path}")

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
        raise ValueError(f"No valid MsBetweenPresents data found in {csv_path}")

    total_frames = len(ft)
    times = raw['TimeInSeconds']
    duration = (times[-1] - times[0]) if len(times) > 1 else (sum(ft) / 1000.0)
    s_ft = sorted(ft)
    ft_mean = sum(ft) / total_frames
    avg_fps = 1000.0 / ft_mean

    # FPS distribution
    fps_list = [1000.0 / x for x in ft if x > 0]
    s_fps = sorted(fps_list)

    # Frametime Jitter (Standard Deviation σ)
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
        'filesize_mb': os.path.getsize(csv_path) / (1024 * 1024),
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


def print_single_telemetry(d):
    """Outputs a markdown table of all measured metrics for a single capture."""
    print("=" * 80)
    print(f"PresentMon Telemetry Analysis: {d['filename']}")
    print(f"Path: {d['path']}")
    print(f"Size: {d['filesize_mb']:.2f} MB | Frames: {d['total_frames']:,} | Duration: {d['duration']:.2f}s")
    print("=" * 80 + "\n")

    print("| 측정 항목 (Metric) | 실측값 (Measured Value) |")
    print("| :--- | :---: |")

    rows = [
        ("평균 프레임레이트 (Average Framerate)", f"**{d['avg_fps']:.1f} FPS**"),
        ("최고 순간 프레임 (Peak Instantaneous FPS)", f"**{d['peak_fps']:.1f} FPS**"),
        ("하위 1% 프레임 (1% Low Framerate)", f"**{d['low_1pct_fps']:.1f} FPS**"),
        ("하위 5% 프레임 (5% Low Framerate)", f"**{d['low_5pct_fps']:.1f} FPS**"),
        ("최저 프레임 (Minimum Framerate)", f"**{d['min_fps']:.1f} FPS**"),
        ("평균 프레임타임 (Average Frametime)", f"**{d['ft_mean']:.3f} ms ({d['ft_mean']*1000:.0f} µs)**"),
        ("프레임타임 중앙값 (Median p50)", f"**{d['ft_p50']:.3f} ms**"),
        ("프레임타임 상위 95% (p95 Frametime)", f"**{d['ft_p95']:.3f} ms**"),
        ("프레임타임 상위 99% (p99 Frametime)", f"**{d['ft_p99']:.3f} ms**"),
        ("프레임타임 상위 99.9% (p99.9 Frametime)", f"**{d['ft_p99_9']:.3f} ms**"),
        ("프레임타임 표준편차 (Jitter $\\sigma$)", f"**{d['ft_jitter_sigma']:.3f} ms ({d['ft_jitter_sigma']*1000:.1f} µs)**"),
        ("연속 프레임 간 리플 (Frame Ripple $\\Delta t$)", f"**{d['ft_ripple_delta']:.3f} ms ({d['ft_ripple_delta']*1000:.1f} µs)**"),
        ("최대 프레임타임 튐 (Max Frametime Spike)", f"**{d['ft_max']:.3f} ms**"),
        ("디스플레이 지연 시간 (Display Latency Mean)", f"**{d['disp_lat_mean']:.3f} ms**"),
        ("디스플레이 지연 시간 상위 99% (Display Latency p99)", f"**{d['disp_lat_p99']:.3f} ms**"),
        ("CPU 연산 시간 (CPU Submission Time Mean)", f"**{d['cpu_busy_mean']:.3f} ms**"),
        ("CPU 연산 시간 상위 99% (CPU Submission Time p99)", f"**{d['cpu_busy_p99']:.3f} ms**"),
        ("GPU 실행 시간 (GPU Execution Time Mean)", f"**{d['gpu_time_mean']:.3f} ms**"),
        ("GPU 실행 시간 상위 99% (GPU Execution Time p99)", f"**{d['gpu_time_p99']:.3f} ms**"),
        ("GPU 지연 시간 (GPU Latency Mean)", f"**{d['gpu_lat_mean']:.3f} ms**"),
        ("GPU 지연 시간 상위 99% (GPU Latency p99)", f"**{d['gpu_lat_p99']:.3f} ms**"),
        ("1.0ms 이상 스파이크 프레임 수 (Spikes >= 1.0ms)", f"**{d['spikes_1ms']:,}회 ({d['spikes_1ms']/d['total_frames']*100:.3f}%)**"),
        ("5.0ms 이상 프레임 지연 횟수 (Spikes >= 5.0ms)", f"**{d['spikes_5ms']:,}회**"),
    ]

    for metric, val in rows:
        print(f"| **{metric}** | {val} |")


def main():
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')

    if len(sys.argv) < 2:
        # If no arguments provided, guide usage or check default file in script dir
        script_dir = os.path.dirname(os.path.abspath(__file__))
        default_file = os.path.join(script_dir, 'pmcap-KartRider.exe-261106-085359.csv')
        if os.path.exists(default_file):
            print(f"[Notice] 인수가 지정되지 않아 최신 캡처 기본 파일({os.path.basename(default_file)})을 분석합니다.")
            print(f"사용법: python {os.path.basename(__file__)} <분석할_PresentMon_데이터.csv>\n")
            target_path = default_file
        else:
            print(f"사용법: python {os.path.basename(__file__)} <분석할_PresentMon_데이터.csv>")
            sys.exit(1)
    else:
        target_path = sys.argv[1]

    d = load_telemetry(target_path)
    print_single_telemetry(d)


if __name__ == '__main__':
    main()
