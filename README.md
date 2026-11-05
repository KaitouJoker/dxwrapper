# DxWrapper x64 General (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어 (Korean)](readme_kr.md)

A universal (Intel / AMD Universal) high-performance DirectX wrapper and proxy library specifically engineered for running 64-bit (x64 Native) Direct3D 9 games on modern Windows 10 and Windows 11 systems with **ultra-low latency**, **rock-solid frame pacing**, **zero OS scheduling jitter**, and an unlocked **2,600+ FPS rendering pipeline**.

Tailored and battle-tested for high-refresh-rate (144Hz, 240Hz, 360Hz, 540Hz+) competitive esports environments (such as **KartRider x64**), this edition completely eliminates the recurring **frame-time spikes and 1,000ms periodic micro-stutters** caused by OS scheduler transitions and timer quantization. Furthermore, it completely bypasses the 64-bit engine's internal **1,000 FPS frame gate ceiling**, allowing modern systems (AMD Ryzen and Intel 12th/13th/14th Gen Core CPUs + NVIDIA GeForce RTX series) to sustain framerates of **2,600+ FPS** (~0.38ms frame times) with ultra-low display latency (**0.55 ~ 0.70ms raw latency**). It also incorporates Windows 10/11 **DirectFlip / Independent Flip** swapchain elevation (`SetSwapEffectShim = 1`) to bypass the Desktop Window Manager (DWM) 3-frame buffering queue.

---

## Table of Contents
1. [Performance Benchmark Comparison](#1-performance-benchmark-comparison)
2. [Installation and Configuration Hierarchy](#2-installation-and-configuration-hierarchy)
3. [Core Display and Compatibility Settings](#3-core-display-and-compatibility-settings)
4. [Low-Latency and Performance Optimizations](#4-low-latency-and-performance-optimizations)
5. [Supplementary Settings](#5-supplementary-settings)
6. [Recommended Profiles](#6-recommended-profiles)
7. [Developer Build Guide](#7-developer-build-guide)
8. [License and Credits](#8-license-and-credits)

---

## 1. Performance Benchmark Comparison

Below is the verified 60-second PresentMon telemetry benchmark in KartRider x64, comparing the default legacy 64-bit DirectX runtime (`d3dx9_43.dll`) against the optimized **DxWrapper x64 General (`d3d9.dll` v1.9.8770.25)**:

| Metric | Legacy Runtime (`d3dx9_43.dll`) | DxWrapper x64 General (`d3d9.dll`) | Delta / Improvement |
| :--- | :---: | :---: | :--- |
| **Average Framerate** | 948.9 FPS (1,000 FPS hard-capped) | **2,700.7 FPS** | **+1,751.8 FPS (+184.6% throughput gain)** |
| **Peak Instantaneous FPS** | 1,331.4 FPS | **2,999.4 FPS** | **Max throughput reached (2,999.4 FPS)** |
| **1% Low Framerate** | 651.8 FPS | **2,182.9 FPS** | **+1,531.1 FPS (+234.9% consistency gain)** |
| **5% Low Framerate** | 674.0 FPS | **2,437.2 FPS** | **+1,763.2 FPS (+261.6% consistency gain)** |
| **Minimum Framerate** | 103.0 FPS | **436.1 FPS** | **+333.1 FPS (4.2x baseline increase)** |
| **Average Frametime** | 1.054 ms | **0.370 ms (370 µs)** | **-0.684 ms (-64.9% latency reduction)** |
| **Frametime 99th Percentile (p99)** | 1.534 ms | **0.458 ms** | **-1.076 ms (-70.1% latency reduction)** |
| **Frametime 99.9th Percentile (p99.9)** | 1.608 ms | **0.554 ms** | **-1.054 ms (-65.5% latency reduction)** |
| **Maximum Frametime Spike** | 9.704 ms (scheduler stall) | **2.293 ms** | **-7.412 ms (-76.4% variance reduction)** |
| **Spikes $\ge$ 1.0 ms across 60s** | 41,899 frames (73.7% of total) | **2 frames** | **99.998% sub-millisecond pacing consistency** |
| **Spikes $\ge$ 5.0 ms across 60s** | 14 spikes (periodic stutter) | **0 spikes** | **0 spikes (100% elimination of periodic stalls)** |

---

## 2. Installation and Configuration Hierarchy

### Deployment
1. **Remove Legacy DLL**: If `d3dx9_43.dll` exists in your game directory (from legacy wrappers or unofficial patches), **delete it** or back it up. Retaining an outdated or mismatched `d3dx9_43.dll` in the game folder can interfere with DirectX runtime loading and cause crashes or rendering issues.
2. Copy the compiled 64-bit `d3d9.dll` and configuration file (`d3d9.ini`) into the directory containing your game executable (e.g., `KartRider.exe`).
3. When the game launches, 64-bit `d3d9.dll` is automatically loaded into the process, intercepting Direct3D calls and accelerating the rendering pipeline.
4. **Zero-Config Standalone Execution**: Even if `d3d9.ini` is omitted, the DLL internally initializes with hardcoded competitive esports defaults (`FlipEx=1`, `DisableDynamicSleep=0`, `BoostRenderThread=1`, `MaxFrameLatency=1`, `DisablePowerThrottling=1`, `DisableLogging=1`, `D3d9to9Ex=0`, `UnlockFrameRate=1`, `DisableMaxWindowedMode=1`, `RenderThreadAffinity=0`, `RenderThreadIdealCore=1`, `GpuSchedulingPriority=0`, `TimerResolution100ns=5000`, `SetSwapEffectShim=1`, `SpinWaitPacing=0`), operating at full performance out-of-the-box.

### Configuration Search Order
For per-game flexibility, the wrapper searches for configuration files in the following order:
1. **`d3d9-[ProcessName].ini`** (Highest priority, e.g., `d3d9-KartRider.ini`)
2. **`d3d9.ini`** (Default configuration)

---

## 3. Core Display and Compatibility Settings

These settings dictate the rendering pipeline architecture and swapchain presentation model.

### `[Compatibility]` Section

#### `EnableD3d9Wrapper`
- **Values**: `0` (Disabled) | `1` (Enabled, Default)
- **Description**: Enables Direct3D 9 API interception and proxy wrapping. Must remain set to `1` for all optimization features to take effect.

#### `D3d9to9Ex`
- **Values**: `0` (Standard Direct3D 9, Recommended) | `1` (Upgrade to Direct3D 9Ex)
- **Description**:
  - `0`: Retains native Direct3D 9 while applying all low-latency hooks. Guarantees full texture rendering stability and flawless resolution/window mode (F11) switching in engines relying on `D3DPOOL_MANAGED` memory pools (e.g., KartRider).
  - `1`: Promotes Direct3D 9 device creation to `Direct3DCreate9Ex` / `CreateDeviceEx`.
  - **Caution**: The D3D9Ex specification removes support for `D3DPOOL_MANAGED`. Games using managed texture pools will experience surface allocation failures (`D3DERR_INVALIDCALL`) or white screen rendering if set to `1`. Always use `0` for KartRider.

---

### `[d3d9]` Section

#### `SetSwapEffectShim`
- **Values**: `0` (Disabled) | `1` (DirectFlip Elevation, Default)
- **Description**: Invokes Windows 10/11 DWM `Direct3D9SetSwapEffectUpgradeShim(1)` to elevate `D3DSWAPEFFECT_DISCARD` swapchains to **DirectFlip / Independent Flip** even in windowed modes. Completely bypasses DWM's 3-frame composition queue, reducing display latency from >5ms down to **under 1ms** and restoring hardware presentation telemetry (e.g., PresentMon, Reflex overlays).

#### `DisableMaxWindowedMode`
- **Values**: `1` (Legacy Flip, Default) | `0` (Independent Flip)
- **Description**: Controls Windows 10/11 **Fullscreen Optimizations (FSO)**.
  - **`1` (Recommended for Pure Lowest Latency / Time-Attack, Default)**: Disables FSO, forcing the GPU into pure hardware scanout mode (`Hardware: Legacy Flip`) for maximum 2,700+ FPS and sub-0.25ms display latency.
  - **`0` (Recommended for Streaming, Recording, and In-Game Overlays)**: Leaves FSO active to enter the DWM DirectFlip queue (`Hardware: Independent Flip`). In-game overlays (CapFrameX, PresentMon, Discord) display normally, and Alt-Tab switching is instantaneous without display flickering.

#### `EnableWindowMode` / `FullscreenWindowMode` / `WindowModeBorder`
- **Values**: All set to `0` (Recommended)
- **Description**: For games with built-in fullscreen/windowed toggles (e.g., F11 in KartRider), setting all three to `0` allows the engine to handle mode switches cleanly without taskbar interference.

#### `FlipEx`
- **Values**: `0` (Disabled) | `1` (Enabled, Default)
- **Description**: Enables the Direct3D 9Ex `D3DSWAPEFFECT_FLIPEX` presentation model for windowed swapchains.

---

## 4. Low-Latency and Performance Optimizations

Custom-engineered latency reduction systems designed to eliminate micro-stutters and achieve 2,600+ FPS across Intel and AMD architectures.

#### `UnlockFrameRate`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Completely bypasses the hardcoded 1ms frame gate (`nowTick == lastFrameTick`) in the 64-bit client main loop.
  - Scans 64-bit RIP-relative instructions (`8B 3D [disp32] FF 15 ...`) to automatically resolve the target counter variable (`0x1498179bc`).
  - Leaves code (`.text`) 100% untouched (0 bytes modified), resetting only the `.data` tick counter to `0xFFFFFFFF` post-presentation. Passes anti-cheat memory integrity checks (BlackCipher) with zero false positives.
  - Employs a single-cycle inline reset to completely remove the 1,000 FPS engine ceiling, achieving sustained **2,600+ FPS**.

#### `BoostRenderThread`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Dynamically elevates the primary Direct3D `Present` render thread priority to `THREAD_PRIORITY_ABOVE_NORMAL` (level 9).
  - **MMCSS ("Games") Removed**: Windows MMCSS 1,000ms periodic resource reclamation (`SystemResponsiveness`) is bypassed, eliminating 1-second periodic micro-stutters.
  - **Starvation Avoidance**: Uses a safe elevated priority instead of `TIME_CRITICAL` (level 15), completely resolving the **500ms+ freezing issue** on 6-core Intel CPUs (e.g., i5-12400F, i5-13400F) where background loader and anti-cheat threads were starved.

#### `RenderThreadAffinity`
- **Values**: `0` (Automatic / Intel P-Core Auto-Pinning, Default) or physical core bitmask
- **Description**:
  - When set to `0`, queries Windows logical processor topology via `GetLogicalProcessorInformationEx`.
  - On **Intel Hybrid architectures (12th/13th/14th Gen Core CPUs)**, automatically excludes E-cores and pins the render thread exclusively to **P-cores (Performance Cores)**.
  - On AMD Ryzen and non-hybrid Intel CPUs, allows the OS scheduler to distribute threads evenly, preventing single-core saturation stutters.

#### `RenderThreadIdealCore`
- **Values**: `1` (Core 1/CPU 2, Recommended Default) | `0` (Disabled) | `>=2` (Explicit logical CPU index)
- **Description**:
  - Sets the ideal processor hint for the render thread via `SetThreadIdealProcessorEx`.
  - Shifts the render thread away from Core 0/1 (where OS DPC, interrupts, and timer ticks concentrate), reducing scheduler preemption and boosting 1% Low FPS by ~100 FPS without hard core lockouts.

#### `SpinWaitPacing`
- **Values**: `0` (Disabled, Default / Recommended for 2,600+ FPS) | `1` (Enabled)
- **Description**:
  - The DirectFlip pipeline (`SetSwapEffectShim = 1`) already maintains GPU queue latency below 1 frame (<1ms).
  - Setting this to `0` removes redundant GPU fence spin-waiting, maximizing rendering throughput to reach **2,600+ FPS**.

#### `DisableDynamicSleep`
- **Values**: `0` (Disabled, Recommended Default) | `1` (Enabled)
- **Description**:
  - With `UnlockFrameRate` bypassing the 1ms frame gate at the memory level, the main render loop does not invoke sleep calls.
  - Setting this to `0` ensures background worker threads (audio, network, anti-cheat) can idle properly, avoiding CPU core waste caused by busy-spinning.

#### `DisablePowerThrottling`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Permanently disables Windows 10/11 **Power Throttling (EcoQoS / Efficiency Mode)** on the process, preventing rendering threads from being downgraded to low-power states.

#### `MaxFrameLatency`
- **Values**: `1` (Minimum Latency, Default)
- **Description**: Limits driver backbuffer queueing to 1 frame, minimizing input-to-photon latency.

---

## 5. Supplementary Settings

### Display and Quality Control
- **`EnableVSync = 0`**: Disables vertical synchronization for immediate frame presentation (`D3DPRESENT_INTERVAL_IMMEDIATE`).
- **`LimitPerFrameFPS = 0`**: Disables the wrapper-level frame rate limiter.
- **`OverrideRefreshRate = 0`**: Adopts the native display refresh rate (e.g., 240Hz, 360Hz, 540Hz).

### Diagnostics and System Control
- **`DisableLogging = 1`**: Disables runtime log writes to eliminate disk I/O overhead. (Set to `0` only when diagnosing issues).
- **`HandleExceptions = 0`**: Prevents unnecessary VEH hook interception of normal access violations used by BlackCipher (`BlackCall64.aes`) integrity checks, avoiding crashes.

---

## 6. Recommended Profiles

All primary optimizations (x64 1,000 FPS frame gate unlock, Intel Hybrid P-core pinning, DirectFlip DWM bypass, power throttling suppression) are active by default.

### Profile Comparison Matrix

| Feature | Default Profile (Time-Attack Ultra-Low Latency) | Windowed / Overlay Profile |
| :--- | :---: | :---: |
| **Key Setting** | **`DisableMaxWindowedMode = 1`** | **`DisableMaxWindowedMode = 0`** |
| **Presentation Mode** | `Hardware: Legacy Flip` | `Hardware: Independent Flip` |
| **Average Framerate** | **2,500 ~ 2,700+ FPS** | **2,400 ~ 2,660+ FPS** |
| **Display Latency (Raw)** | **0.21 ~ 0.25 ms** | **0.55 ~ 0.70 ms** |
| **In-Game Overlay Support** | Secondary monitor recommended | Supported (PresentMon, CapFrameX overlay on screen) |
| **Alt-Tab Switching** | Fullscreen mode transition flicker | Instantaneous (no screen flicker) |
| **Target Audience** | Competitive esports, time-attack record setting | Streaming, single-monitor recording, overlay users |

---

### Standard Configuration (`d3d9.ini`)

```ini
[Compatibility]
D3d9to9Ex                  = 0          ;; Keep 0: Prevents texture white-screen issues
EnableD3d9Wrapper          = 1

[d3d9]
EnableWindowMode           = 0
FullscreenWindowMode       = 0
WindowModeBorder           = 0
FlipEx                     = 1
DisableDynamicSleep        = 0          ;; Keep 0: Ensures background threads idle properly
DisablePowerThrottling     = 1
BoostRenderThread          = 1          ;; 1: Safe priority elevation (prevents 1000ms & 500ms stutters)
MaxFrameLatency            = 1
SpinWaitPacing             = 0          ;; 0: Maximize throughput to 2,600+ FPS
RenderThreadAffinity       = 0          ;; 0: Auto-pin to Intel P-cores, balanced on AMD
RenderThreadIdealCore       = 1          ;; 1: Hint scheduler to Core 1/CPU 2 (+100 FPS 1% Low)
GpuSchedulingPriority       = 0          ;; 0: Default GPU priority, 1: D3DKMT High
UnlockFrameRate            = 1          ;; 1: Unlock x64 1,000 FPS engine ceiling (2,600+ FPS)
SetSwapEffectShim          = 1          ;; 1: DirectFlip elevation (bypasses DWM buffer, <1ms latency)
LimitPerFrameFPS           = 0
DisableLogging             = 1

;; ======================================================================
;; [Core Presentation Switch]
;;   DisableMaxWindowedMode = 1  -> Legacy Flip (Pure hardware scan-out, ultra-low latency, Default)
;;   DisableMaxWindowedMode = 0  -> Independent Flip (Overlays supported, instant Alt-Tab)
;; ======================================================================
DisableMaxWindowedMode     = 1
```

---

## 7. Developer Build Guide

Configured for MSVC v145 (Visual Studio 2026 / 2022) targeting 64-bit native (`x64 Release`).

### One-Click Build
Run the batch script from the repository root:
```cmd
build_d3d9_x64.bat
```

To run in automated CI/CD pipelines without prompts:
```cmd
build_d3d9_x64.bat nopause
```

### Build Artifacts
- `Release\d3d9.dll` (~810 KB, x64 native binary)
- `Release\d3d9.ini`

---

## 8. License and Credits

This project is distributed under the zlib license.

- **DxWrapper Base Engine**: Copyright (C) 2025 Elisha Riedlinger
- **x64 Porting & Low-Latency Optimization Layer**: Copyright (C) 2026 KaitouJoker
- **Third-Party Libraries**:
  - `d3d8to9` by Patrick Mours
  - `Hooking.Patterns` by ThirteenAG
  - `Detours` by Microsoft
