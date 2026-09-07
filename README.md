# DxWrapper (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어 (Korean)](readme_kr.md)

A high-performance DirectX wrapper and proxy library optimized specifically for running Direct3D 9 and legacy DirectX games on modern Windows 10 and Windows 11 systems with **ultra-low input latency**, **rock-solid frame pacing**, and **zero OS scheduling jitter**.

Tailored and battle-tested for high-refresh-rate (144Hz to 240Hz+) competitive esports environments (such as **KartRider**), this edition completely eliminates the recurring **frame-time spikes (1.1ms -> 1.6ms micro-stutters)** caused by OS timer quantization and thread preemption, while granting full control over Windows 10/11 presentation models: **`Hardware: Independent Flip`** (Direct Flip with overlays) and pure exclusive **`Hardware: Legacy Flip`** (DWM bypass).

---

## Table of Contents
1. [Installation and Configuration Hierarchy](#1-installation-and-configuration-hierarchy)
2. [Core Display and Compatibility Settings](#2-core-display-and-compatibility-settings)
3. [Low-Latency and Performance Optimizations](#3-low-latency-and-performance-optimizations)
4. [Supplementary Settings](#4-supplementary-settings)
5. [Recommended Profiles](#5-recommended-profiles)
6. [Developer Build Guide](#6-developer-build-guide)
7. [License and Credits](#7-license-and-credits)

---

## 1. Installation and Configuration Hierarchy

### Deployment
1. Copy the compiled `d3d9.dll` and configuration file (`d3d9.ini`) into the directory containing your game executable (e.g., `KartRider.exe`).
2. When the game launches, `d3d9.dll` is automatically loaded into the process, intercepting Direct3D calls and accelerating the rendering pipeline.
3. **Zero-Config Standalone Execution**: Even if `d3d9.ini` is omitted, the DLL internally initializes with hardcoded competitive low-latency defaults (`FlipEx=1`, `DisableDynamicSleep=1`, `BoostRenderThread=1`, `MaxFrameLatency=1`, `DisablePowerThrottling=1`, `DisableLogging=1`, `D3d9to9Ex=0`).

### Configuration Search Order
For maximum compatibility and per-game flexibility, the wrapper searches for configuration files in the following order:
1. **`d3d9-[ProcessName].ini`** (Highest priority, e.g., `d3d9-KartRider.ini`)
2. **`d3d9.ini`** (Default configuration)

---

## 2. Core Display and Compatibility Settings

These settings dictate the rendering pipeline architecture and swapchain presentation model.

### `[Compatibility]` Section

#### `EnableD3d9Wrapper`
- **Values**: `0` (Disabled) | `1` (Enabled, Default)
- **Description**: Enables Direct3D 9 API interception and the proxy wrapping engine. Must remain set to `1` for all optimization features to take effect.

#### `D3d9to9Ex`
- **Values**: `0` (Standard Direct3D 9, Recommended) | `1` (Upgrade to Direct3D 9Ex)
- **Description**:
  - `0`: Retains the native Direct3D 9 device interface while applying all low-latency hooks and patches. Guarantees full compatibility and texture rendering in legacy game engines relying on `D3DPOOL_MANAGED` memory pools, avoiding white/gray screen rendering bugs during resolution and window mode (F11) switching.
  - `1`: Promotes all game D3D9 device calls directly to `Direct3DCreate9Ex` and `CreateDeviceEx`.
  - **Caution**: The D3D9Ex specification removes support for `D3DPOOL_MANAGED`. Games using managed texture pools will experience surface allocation failures (`D3DERR_INVALIDCALL`) or white textures if forced to `1`. Keep this set to `0` for KartRider and similar legacy engines.

---

### `[d3d9]` Section

#### `DisableMaxWindowedMode`
- **Values**: `0` (Independent Flip) | `1` (Legacy Flip)
- **Description**: Directly controls Windows 10/11 **Fullscreen Optimizations (FSO)**.
  - **`0` (Recommended for Streaming, Recording, and In-Game Overlays)**:
    - Leaves FSO active, allowing the swapchain to enter the Desktop Window Manager (DWM) Direct Flip queue.
    - Presentation Mode: **`Hardware: Independent Flip`**
    - 99th Percentile Frame Time: **~1.35 ms** (~740 FPS)
    - In-game overlays (CapFrameX, PresentMon, Xbox Game Bar, Discord) render seamlessly over the game on a single monitor. Alt-Tab switching is instantaneous with zero screen flickering.
  - **`1` (Recommended for Hardcore Time-Attack / Pure Lowest Latency)**:
    - Disables FSO, forcing the graphics hardware into true exclusive scan-out mode and bypassing DWM completely.
    - Presentation Mode: **`Hardware: Legacy Flip`**
    - 99th Percentile Frame Time: **~1.25 ms** (~800 FPS)
    - Saves approximately 0.1 ms of frame time by removing DWM token handshakes. Note that desktop overlays will not render on top of the game screen and must be viewed on a secondary monitor.

#### `EnableWindowMode` / `FullscreenWindowMode` / `WindowModeBorder`
- **`EnableWindowMode`**: `0` (Preserve game fullscreen) | `1` (Force windowed)
- **`FullscreenWindowMode`**: `0` (Disabled) | `1` (Borderless fullscreen)
- **`WindowModeBorder`**: `0` (Borderless) | `1` (Show window border)
- **Description**: For titles with built-in fullscreen/windowed toggles (e.g., F11 in KartRider), set all three parameters to `0` to allow the engine to switch cleanly without taskbar interference.

#### `FlipEx`
- **Values**: `0` (Disabled) | `1` (Enabled)
- **Description**: Activates the Direct3D 9Ex `D3DSWAPEFFECT_FLIPEX` presentation model for windowed swapchains. Automatically validates backbuffer counts (`>= 2`), normalizes 16-bit color surfaces to 32-bit `D3DFMT_X8R8G8B8`, and discards unsupported multisampling flags according to D3D9Ex specifications.

---

## 3. Low-Latency and Performance Optimizations

Custom-engineered latency reduction systems designed to eliminate micro-stutters and dedicate full hardware execution to the game's rendering thread.

#### `DisableDynamicSleep`
- **Values**: `1` (Enabled, Strongly Recommended) | `0` (Disabled)
- **Description**:
  - Legacy game engines frequently call `Sleep(0)` or `Sleep(1)` inside the main rendering loop to curb CPU usage or throttle frame rates.
  - Due to Windows kernel timer quantization, a `Sleep(1)` call often sleeps for 1.0 ms to 2.0 ms, which is the primary cause of periodic frame times jumping from 1.1 ms to 1.6 ms.
  - When enabled:
    - Intercepts `Sleep(0)` and replaces it with CPU-friendly `YieldProcessor()` (`_mm_pause()`), preventing the thread from relinquishing its remaining time quantum.
    - Intercepts micro-sleeps (<= 2 ms) and replaces them with a zero-wait `QueryPerformanceCounter` micro spin loop, keeping the thread fully active without kernel context switches.

#### `BoostRenderThread`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Dynamically detects the thread executing `Present` / `PresentEx` and promotes its priority to `THREAD_PRIORITY_TIME_CRITICAL` (level 15).
  - Disables scheduler priority boost decay via `SetThreadPriorityBoost(hThread, FALSE)`.
  - Registers the render thread with Windows **MMCSS (Multimedia Class Scheduler Service)** under the `"Games"` profile via `AvSetMmThreadCharacteristicsA`, preventing background services from pre-empting the game rendering loop.

#### `DisablePowerThrottling`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Permanently disables Windows 10/11 **Power Throttling (EcoQoS / Efficiency Mode)** on the game process via `SetProcessInformation(ProcessPowerThrottling)`.
  - Prevents the OS from demoting rendering threads to Efficiency Cores (E-Cores) on Intel 12th/13th/14th Gen or AMD hybrid CPUs, keeping threads pinned to Performance Cores (P-Cores) at maximum boost clocks.

#### `MaxFrameLatency`
- **Values**: `1` (Minimum Latency, Default) to `3`
- **Description**:
  - Limits the maximum number of frames the driver can queue ahead in the GPU backbuffer pipeline.
  - Setting this to `1` ensures the CPU does not render subsequent frames until the current frame has cleared the GPU, minimizing **input-to-photon latency**.

#### `SpinWaitPacing`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Employs a hardware event query (`D3DQUERYTYPE_EVENT`) before presentation.
  - If the GPU is lagging slightly behind the CPU, the wrapper performs a micro spin-wait (up to 200 microseconds with `YieldProcessor()`) instead of letting the graphics driver drop into an OS kernel wait state (`WaitForSingleObject`).
  - Maintains steady, jitter-free frame delivery.

#### `BypassFpuReset`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Bypasses redundant Direct3D 9 `_controlfp` hardware floating-point control word resets on every frame presentation, saving critical CPU cycles in the presentation path.

#### `RenderThreadAffinity`
- **Values**: `0` (Automatic / All Cores) or a core bitmask (e.g., `0x000F` for cores 0-3)
- **Description**: Binds the primary rendering thread to specific high-performance physical CPU cores.

---

## 4. Supplementary Settings

### Display and Quality Control
- **`EnableVSync = 0`**: Disables vertical synchronization for immediate frame presentation (`D3DPRESENT_INTERVAL_IMMEDIATE`).
- **`LimitPerFrameFPS = 0`**: Unlocks the wrapper-level frame rate limiter for uncapped rendering.
- **`OverrideRefreshRate = 0`**: Uses the native display refresh rate (e.g., 240Hz). Set to a specific number to force a custom refresh rate.
- **`AntiAliasing = 0` / `AnisotropicFiltering = 0`**: Retains in-game graphic quality settings.

### Diagnostics and System Control
- **`DisableLogging = 1`**: Disables debug log generation to eliminate disk I/O overhead. (Set to `0` to generate `d3d9-[ProcessName].log` during troubleshooting).
- **`RealDllPath = AUTO`**: Automatically locates and hooks the native system `SysWOW64\d3d9.dll`.
- **`FixPerfCounterUptime = 0`**: Resolves high-uptime timer overflow bugs on legacy systems (set to `1` if needed).

---

## 5. Recommended Profiles

All core latency optimizations (sleep interception, MMCSS thread priority boosting, power throttling prevention) are active across both profiles. **The sole distinction is the value of `DisableMaxWindowedMode`**.

### Profile Comparison Matrix

| Feature | Profile 1 (Streaming / Recording / Overlays) | Profile 2 (Hardcore Time-Attack / Tournament) |
| :--- | :---: | :---: |
| **Key Setting** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **Presentation Mode** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **99th Percentile Frame Time** | ~1.35 ms (~740 FPS) | ~1.25 ms (~800 FPS) |
| **Single-Monitor Overlays** | Supported (renders directly over game) | Not supported (requires secondary monitor) |
| **Alt-Tab Switching** | Instantaneous (no screen flicker) | Traditional fullscreen display flicker |
| **Recommended Users** | Streamers, content creators, casual players | Competitive time-attack players, tournament setups |

---

### Reference Configuration (`d3d9.ini`)

```ini
[Compatibility]
D3d9to9Ex                  = 0
EnableD3d9Wrapper          = 1

[d3d9]
EnableWindowMode           = 0
FullscreenWindowMode       = 0
WindowModeBorder           = 0
FlipEx                     = 0
DisableDynamicSleep        = 1
DisablePowerThrottling     = 1
BoostRenderThread          = 1
MaxFrameLatency            = 1
SpinWaitPacing             = 1
BypassFpuReset             = 1
DisableLogging             = 1

;; ======================================================================
;; [Core Presentation Switch]
;;   DisableMaxWindowedMode = 0  -> Profile 1 (Independent Flip, Overlays, ~1.35ms)
;;   DisableMaxWindowedMode = 1  -> Profile 2 (Legacy Flip, Pure Scan-Out, ~1.25ms)
;; ======================================================================
DisableMaxWindowedMode     = 0
```

For the complete list of all 133 configuration parameters, refer to [Configuration (advanced).md](Configuration%20(advanced).md).

---

## 6. Developer Build Guide

The build system is configured for MSVC v145 (Visual Studio 2026 / 2022) targeting x86 Win32, stripping all unused legacy subsystems (DirectDraw, DirectSound, DirectInput, Direct3D 8) into an ultra-lean binary (~650 KB).

### One-Click Build
Run the automated batch script from the repository root:
```cmd
build_d3d9.bat
```

To run in automated CI/CD scripts without user prompts:
```cmd
build_d3d9.bat nopause
```

### Build Artifacts
- `Release\d3d9.dll` (~656 KB)
- `Release\d3d9.ini`

---

## 7. License and Credits

This project is distributed under the zlib license.

- **DxWrapper Base Engine**: Copyright (C) 2025 Elisha Riedlinger
- **Low-Latency & FSO Optimization Layer**: Copyright (C) 2026 KaitouJoker
- **Third-Party Libraries**:
  - `d3d8to9` by Patrick Mours
  - `Hooking.Patterns` by ThirteenAG
  - `Detours` by Microsoft
