# DxWrapper x64 General (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어 (Korean)](readme_kr.md)

A universal (Intel / AMD Universal) high-performance DirectX wrapper and proxy library specifically engineered for running 64-bit (x64 Native) Direct3D 9 games on modern Windows 10 and Windows 11 systems with **ultra-low latency**, **rock-solid frame pacing**, **zero OS scheduling jitter**, and an unlocked **2,600+ FPS rendering pipeline**.

Tailored and battle-tested for high-refresh-rate (144Hz, 240Hz, 360Hz, 540Hz+) competitive esports environments (such as **KartRider x64**), this edition completely eliminates the recurring **frame-time spikes and 1,000ms periodic micro-stutters** caused by OS scheduler transitions and timer quantization. Furthermore, it completely bypasses the 64-bit engine's internal **1,000 FPS frame gate ceiling**, allowing modern systems (AMD Ryzen and Intel 12th/13th/14th Gen Core CPUs + NVIDIA GeForce RTX series) to sustain framerates of **2,600+ FPS** (~0.38ms frame times) with ultra-low display latency (**0.55 ~ 0.70ms raw latency**). It also incorporates Windows 10/11 **DirectFlip / Independent Flip** swapchain elevation (`SetSwapEffectShim = 1`) to bypass the Desktop Window Manager (DWM) 3-frame buffering queue.

---

## Table of Contents
1. [Performance Demonstration (Video)](#1-performance-demonstration-video)
2. [Installation and Configuration Hierarchy](#2-installation-and-configuration-hierarchy)
3. [Core Display and Compatibility Settings](#3-core-display-and-compatibility-settings)
4. [Low-Latency and Performance Optimizations](#4-low-latency-and-performance-optimizations)
5. [Supplementary Settings](#5-supplementary-settings)
6. [Recommended Profiles](#6-recommended-profiles)
7. [Developer Build Guide](#7-developer-build-guide)
8. [License and Credits](#8-license-and-credits)

---

## 1. Performance Demonstration (Video)

The following side-by-side recordings demonstrate the frame-time stability, input responsiveness, and complete elimination of periodic spikes in KartRider x64 when running with the optimized DxWrapper versus the legacy 64-bit DirectX runtime.

### Legacy Runtime (`d3dx9_43.dll`)
> Noticeable periodic frame-time spikes and micro-stutters caused by legacy presentation queues and OS thread rescheduling.

https://github.com/user-attachments/assets/1d350200-094a-451f-8b3a-af26f0d09b87

### Optimized DxWrapper (`d3d9.dll`)
> Completely flat, jitter-free frame pacing sustained at ~0.38ms to 0.6ms (2,600+ FPS), achieved via x64 frame gate bypass, DWM DirectFlip elevation, Intel Hybrid P-core auto-pinning, and safe thread scheduling.

https://github.com/user-attachments/assets/f270993d-ed86-4d61-a123-00b3e891eb22

---

## 2. Installation and Configuration Hierarchy

### Deployment
1. **Remove Legacy DLL**: If `d3dx9_43.dll` exists in your game directory (from legacy wrappers or unofficial patches), **delete it** or back it up. Retaining an outdated or mismatched `d3dx9_43.dll` in the game folder can interfere with DirectX runtime loading and cause crashes or rendering issues.
2. Copy the compiled 64-bit `d3d9.dll` and configuration file (`d3d9.ini`) into the directory containing your game executable (e.g., `KartRider.exe`).
3. When the game launches, 64-bit `d3d9.dll` is automatically loaded into the process, intercepting Direct3D calls and accelerating the rendering pipeline.
4. **Zero-Config Standalone Execution**: Even if `d3d9.ini` is omitted, the DLL internally initializes with hardcoded competitive esports defaults (`FlipEx=1`, `DisableDynamicSleep=0`, `BoostRenderThread=1`, `MaxFrameLatency=1`, `DisablePowerThrottling=1`, `DisableLogging=1`, `D3d9to9Ex=0`, `UnlockFrameRate=1`, `RenderThreadAffinity=0`, `SetSwapEffectShim=1`, `SpinWaitPacing=0`), operating at full performance out-of-the-box.

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
- **Values**: `0` (Independent Flip, Default) | `1` (Legacy Flip)
- **Description**: Controls Windows 10/11 **Fullscreen Optimizations (FSO)**.
  - **`0` (Recommended for Streaming, Recording, and In-Game Overlays)**: Leaves FSO active to enter the DWM DirectFlip queue (`Hardware: Independent Flip`). In-game overlays (CapFrameX, PresentMon, Discord) display normally, and Alt-Tab switching is instantaneous without display flickering.
  - **`1` (Recommended for Pure Lowest Latency / Time-Attack)**: Disables FSO, forcing the GPU into pure hardware scanout mode (`Hardware: Legacy Flip`).

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

| Feature | Default Profile (General / Streaming / Overlays) | Time-Attack Ultra-Low Latency Profile |
| :--- | :---: | :---: |
| **Key Setting** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **Presentation Mode** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **Average Framerate** | **2,400 ~ 2,660+ FPS** | **2,500 ~ 2,700+ FPS** |
| **Display Latency (Raw)** | **0.55 ~ 0.70 ms** | **0.50 ~ 0.65 ms** |
| **In-Game Overlay Support** | Supported (PresentMon, CapFrameX overlay on screen) | Not supported (secondary monitor required) |
| **Alt-Tab Switching** | Instantaneous (no screen flicker) | Fullscreen mode transition flicker |
| **Target Audience** | Competitive esports, streamers, overlay users | Time-attack record setting, tournament setups |

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
UnlockFrameRate            = 1          ;; 1: Unlock x64 1,000 FPS engine ceiling (2,600+ FPS)
SetSwapEffectShim          = 1          ;; 1: DirectFlip elevation (bypasses DWM buffer, <1ms latency)
LimitPerFrameFPS           = 0
DisableLogging             = 1

;; ======================================================================
;; [Core Presentation Switch]
;;   DisableMaxWindowedMode = 0  -> Independent Flip (Overlays supported, instant Alt-Tab)
;;   DisableMaxWindowedMode = 1  -> Legacy Flip (Pure hardware scan-out, ultra-low latency)
;; ======================================================================
DisableMaxWindowedMode     = 0
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
