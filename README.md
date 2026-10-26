# DxWrapper (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어 (Korean)](readme_kr.md)

A high-performance DirectX wrapper and proxy library optimized specifically for running Direct3D 9 and legacy DirectX games on modern Windows 10 and Windows 11 systems with **ultra-low input latency**, **rock-solid frame pacing**, **zero OS scheduling jitter**, and an **unlocked 1,500 ~ 1,700+ FPS rendering pipeline**.

Tailored and battle-tested for high-refresh-rate (144Hz, 240Hz, 360Hz, 540Hz+) competitive esports environments (such as **KartRider**), this edition completely eliminates the recurring **frame-time spikes (0.8ms -> 1.6ms micro-stutters)** caused by OS timer quantization and thread preemption. Furthermore, it completely bypasses the legacy engine's internal **1,000 FPS frame gate ceiling**, allowing modern multi-core systems (e.g., AMD Ryzen 7 7800X3D + NVIDIA GeForce RTX 50-series) to achieve sustained framerates of **1,500 to 1,700+ FPS** (sub-0.6 ms frame times) with ultra-low display latency (**0.75 ~ 0.85 ms raw**), while granting full control over Windows 10/11 presentation models: **`Hardware: Independent Flip`** (Direct Flip with overlays) and pure exclusive **`Hardware: Legacy Flip`** (DWM bypass).

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

The following side-by-side recordings demonstrate the frame stability, input responsiveness, and elimination of periodic 0.8ms -> 1.6ms frame-time spikes in KartRider when running with DxWrapper versus the legacy DirectX runtime.

### Original (`d3dx9_27.dll`)
> Noticeable periodic frame-time spikes (0.8 ms -> 1.6 ms) and OS scheduler micro-stutters under the legacy presentation pipeline.

https://github.com/user-attachments/assets/2791662b-7665-4540-9d48-99ca38de2f33

### DxWrapper (`d3d9.dll`)
> Completely flat, jitter-free frame pacing locked at ~0.6 ms to 0.8 ms (1,500 ~ 1,700+ FPS) achieved via frame gate bypass, sleep interception, MMCSS thread priority elevation, and DWM bypass.

https://github.com/user-attachments/assets/bb70f855-992b-476a-8d16-837a6ae4bd1e

---

## 2. Installation and Configuration Hierarchy

### Deployment
1. **Remove Legacy DLL**: If `d3dx9_27.dll` exists in your game directory, **delete it**. Retaining a legacy or mismatched `d3dx9_27.dll` in the local game folder can interfere with DirectX runtime loading and cause crash or rendering issues.
2. Copy the compiled `d3d9.dll` and configuration file (`d3d9.ini`) into the directory containing your game executable (e.g., `KartRider.exe`).
3. When the game launches, `d3d9.dll` is automatically loaded into the process, intercepting Direct3D calls and accelerating the rendering pipeline.
4. **Zero-Config Standalone Execution**: Even if `d3d9.ini` is omitted, the DLL internally initializes with hardcoded competitive low-latency defaults (`FlipEx=1`, `DisableDynamicSleep=1`, `BoostRenderThread=1`, `MaxFrameLatency=1`, `DisablePowerThrottling=1`, `DisableLogging=1`, `D3d9to9Ex=0`, `UnlockFrameRate=1`, `RenderThreadAffinity=4`).

### Configuration Search Order
For maximum compatibility and per-game flexibility, the wrapper searches for configuration files in the following order:
1. **`d3d9-[ProcessName].ini`** (Highest priority, e.g., `d3d9-KartRider.ini`)
2. **`d3d9.ini`** (Default configuration)

---

## 3. Core Display and Compatibility Settings

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
  - **Caution**: The D3D9Ex specification removes support for `D3DPOOL_MANAGED`. Games using managed texture pools will experience surface allocation failures (`D3DERR_INVALIDCALL`) or white screen rendering if forced to `1`. Keep this set to `0` for KartRider and similar legacy engines.

---

### `[d3d9]` Section

#### `DisableMaxWindowedMode`
- **Values**: `0` (Independent Flip) | `1` (Legacy Flip)
- **Description**: Directly controls Windows 10/11 **Fullscreen Optimizations (FSO)**.
  - **`0` (Recommended for Streaming, Recording, and In-Game Overlays)**:
    - Leaves FSO active, allowing the swapchain to enter the Desktop Window Manager (DWM) Direct Flip queue.
    - Presentation Mode: **`Hardware: Independent Flip`**
    - Latency: Raw display latency **0.75 ~ 0.85 ms** (1,500 ~ 1,700 FPS)
    - In-game overlays (CapFrameX, PresentMon, Xbox Game Bar, Discord) render seamlessly over the game on a single monitor. Alt-Tab switching is instantaneous with zero screen flickering.
  - **`1` (Recommended for Hardcore Time-Attack / Pure Lowest Latency)**:
    - Disables FSO, forcing the graphics hardware into true exclusive scan-out mode and bypassing DWM completely.
    - Presentation Mode: **`Hardware: Legacy Flip`**
    - Latency: Raw display latency **0.70 ~ 0.80 ms** (1,550 ~ 1,700+ FPS)
    - Saves approximately 0.05 ~ 0.1 ms of frame time by removing DWM token handshakes. Note that desktop overlays will not render on top of the game screen and must be viewed on a secondary monitor.

#### `EnableWindowMode` / `FullscreenWindowMode` / `WindowModeBorder`
- **`EnableWindowMode`**: `0` (Preserve game fullscreen) | `1` (Force windowed)
- **`FullscreenWindowMode`**: `0` (Disabled) | `1` (Borderless fullscreen)
- **`WindowModeBorder`**: `0` (Borderless) | `1` (Show window border)
- **Description**: For titles with built-in fullscreen/windowed toggles (e.g., F11 in KartRider), set all three parameters to `0` to allow the engine to switch cleanly without taskbar interference.

#### `FlipEx`
- **Values**: `0` (Disabled) | `1` (Enabled)
- **Description**: Activates the Direct3D 9Ex `D3DSWAPEFFECT_FLIPEX` presentation model for windowed swapchains. Automatically validates backbuffer counts (`>= 2`), normalizes 16-bit color surfaces to 32-bit `D3DFMT_X8R8G8B8`, and discards unsupported multisampling flags according to D3D9Ex specifications.

---

## 4. Low-Latency and Performance Optimizations

Custom-engineered latency reduction systems designed to eliminate micro-stutters, bypass engine frame rate barriers, and dedicate full hardware execution to the game's rendering thread.

#### `UnlockFrameRate`
- **Values**: `1` (Enabled, Default) | `0` (Disabled)
- **Description**:
  - Legacy game engines frequently enforce a hardcoded 1ms frame gate inside their main update loop (`App::RunFrame` at `0x0102fba6..0x0102fbc0`), skipping subsequent frames rendered in the same integer millisecond (`nowTick == lastFrameTick` -> `jmp 0x1030066`). This places an absolute mathematical ceiling of **exactly 1,000 FPS**.
  - Traditional `.text` bytecode patches (e.g., NOP-ing the conditional jump) violate the client's 30-second self-integrity checksum routine (`0x008a9350`), resulting in immediate disconnects.
  - **Method 2 Virtual Gate Reset**:
    - Leaves game `.text` code 100% untouched (0 bytes modified, 100% anti-cheat and integrity safe).
    - Post-presentation (`ApplyPostPresentFixes`), DxWrapper resets the dynamic runtime variable `lastFrameTick` (`0x013f23a8`) to `0xFFFFFFFF`.
    - On the subsequent frame check, `nowTick != 0xFFFFFFFF` is guaranteed, allowing the engine to render as many frames as the GPU/CPU pipeline can push.
    - Incorporates **Zero-Overhead Inline Reset (Lever C)**: tests write safety with SEH only once upon startup (`TestWriteDword`), and thereafter executes a pure single-cycle direct pointer store (`*s_pLastFrameTick = 0xFFFFFFFF;`, compiled to a single `mov [eax], 0xFFFFFFFF`), saving ~15 CPU instructions per frame and unlocking **1,500 ~ 1,700+ FPS**.
    - Fully safe for physics delta time: physics accumulator safely ignores zero-delta frames (`dt_ms == 0`) with no division-by-zero risk, enabling pure graphical interpolation.

#### `RenderThreadAffinity`
- **Values**: `0` (Automatic / OS Scheduled) or physical core bitmask (e.g., `4` for Core 1)
- **Description**:
  - Pins the primary rendering thread executing Direct3D `Present` to specific CPU physical cores.
  - **Recommended Value for 8-Core CPUs (e.g., AMD Ryzen 7 7800X3D)**: **`4` (0x0004 -> Core 1 / CPU 2)**.
  - Windows automatically assigns OS timer ticks, DPCs, network packet handling, and USB/mouse polling to Core 0 (CPU 0/1). Pinning the render thread to Core 1 (AMD factory Gold Star ★ highest-yield core) isolates it completely from system interrupt preemption, providing an immediate **+30 FPS** gain (1,570 -> 1,600+ FPS) and flattening display latency to **0.75 ~ 0.85 ms**.

#### `DisableDynamicSleep`
- **Values**: `1` (Enabled, Strongly Recommended) | `0` (Disabled)
- **Description**:
  - Legacy game engines frequently call `Sleep(0)` or `Sleep(1)` inside the main rendering loop to curb CPU usage or throttle frame rates.
  - Due to Windows kernel timer quantization, a `Sleep(1)` call often sleeps for 1.0 ms to 2.0 ms, which is the primary cause of periodic frame times jumping from 0.8 ms to 1.6 ms.
  - When enabled:
    - Intercepts `Sleep(0)` and replaces it with CPU-friendly `YieldProcessor()` (`_mm_pause()`), preventing the thread from relinquishing its remaining time quantum.
    - Intercepts micro-sleeps (<= 2 ms) and replaces them with a zero-wait `QueryPerformanceCounter` micro spin loop, keeping the thread fully active without kernel context switches.
  - Intelligently preserves non-zero alertable calls (`SleepEx(0, TRUE)`), ensuring WinSock2 asynchronous overlapped I/O APC callbacks are delivered reliably without network disconnects.

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
  - *Note*: The Direct3D `SetMaximumFrameLatency` API is only available under Direct3D 9Ex. Under standard Direct3D 9 (`D3d9to9Ex = 0`), maximum queued frames are controlled at the driver level (e.g., NVIDIA Low Latency Mode 'Ultra' / AMD Anti-Lag).

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

---

## 5. Supplementary Settings

### Display and Quality Control
- **`EnableVSync = 0`**: Disables vertical synchronization for immediate frame presentation (`D3DPRESENT_INTERVAL_IMMEDIATE`).
- **`LimitPerFrameFPS = 0`**: Unlocks the wrapper-level frame rate limiter for uncapped rendering.
- **`OverrideRefreshRate = 0`**: Uses the native display refresh rate (e.g., 240Hz, 360Hz, 540Hz). Set to a specific number to force a custom refresh rate.
- **`AntiAliasing = 0` / `AnisotropicFiltering = 0`**: Retains in-game graphic quality settings.

### Diagnostics and System Control
- **`DisableLogging = 1`**: Disables debug log generation to eliminate disk I/O overhead. (Set to `0` to generate `d3d9-[ProcessName].log` during troubleshooting).
- **`RealDllPath = AUTO`**: Automatically locates and hooks the native system `SysWOW64\d3d9.dll`.
- **`FixPerfCounterUptime = 0`**: Resolves high-uptime timer overflow bugs on legacy systems (set to `1` if needed).

---

## 6. Recommended Profiles

All core latency optimizations (1,000 FPS frame gate unlock, core affinity isolation, sleep interception, MMCSS thread priority boosting, power throttling prevention) are active across both profiles. **The sole distinction is the value of `DisableMaxWindowedMode`**.

### Profile Comparison Matrix

| Feature | Profile 1 (Streaming / Recording / Overlays) | Profile 2 (Hardcore Time-Attack / Tournament) |
| :--- | :---: | :---: |
| **Key Setting** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **Presentation Mode** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **Average Framerate** | **1,500 ~ 1,650 FPS** | **1,550 ~ 1,700+ FPS** |
| **Display Latency (Raw)** | **0.75 ~ 0.85 ms** | **0.70 ~ 0.80 ms** |
| **Single-Monitor Overlays** | Supported (renders directly over game) | Not supported (requires secondary monitor) |
| **Alt-Tab Switching** | Instantaneous (no screen flicker) | Traditional fullscreen display flicker |
| **Recommended Users** | Streamers, content creators, casual players | Competitive time-attack players, tournament setups |

---

### Reference Configuration (`d3d9.ini`)

```ini
[Compatibility]
D3d9to9Ex                  = 0          ;; Keep 0: Prevents D3DPOOL_MANAGED white-screen issues
EnableD3d9Wrapper          = 1

[d3d9]
EnableWindowMode           = 0
FullscreenWindowMode       = 0
WindowModeBorder           = 0
FlipEx                     = 1
DisableDynamicSleep        = 1
DisablePowerThrottling     = 1
BoostRenderThread          = 1
MaxFrameLatency            = 1
SpinWaitPacing             = 1
BypassFpuReset             = 1
RenderThreadAffinity       = 4          ;; Core 1 (AMD 7800X3D Gold Star core, avoids Core 0 DPC/ISR)
UnlockFrameRate            = 1          ;; Unlocks engine 1,000 FPS ceiling (Method 2 + Lever C)
LimitPerFrameFPS           = 0          ;; 0 = Uncapped maximum framerate
DisableLogging             = 1

;; ======================================================================
;; [Core Presentation Switch]
;;   DisableMaxWindowedMode = 0  -> Profile 1 (Independent Flip, Overlays, ~0.80ms, 1500-1650 FPS)
;;   DisableMaxWindowedMode = 1  -> Profile 2 (Legacy Flip, Pure Scan-Out, ~0.75ms, 1550-1700 FPS)
;; ======================================================================
DisableMaxWindowedMode     = 0
```

For the complete list of all 133 configuration parameters, refer to [Configuration (advanced).md](Configuration%20(advanced).md).

---

## 7. Developer Build Guide

The build system is configured for MSVC v145 (Visual Studio 2026 / 2022) targeting x86 Win32, stripping all unused legacy subsystems (DirectDraw, DirectSound, DirectInput, Direct3D 8) into an ultra-lean binary (~657 KB).

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
- `Release\d3d9.dll` (~657 KB)
- `Release\d3d9.ini`

---

## 8. License and Credits

This project is distributed under the zlib license.

- **DxWrapper Base Engine**: Copyright (C) 2025 Elisha Riedlinger
- **Low-Latency & FSO Optimization Layer**: Copyright (C) 2026 KaitouJoker
- **Third-Party Libraries**:
  - `d3d8to9` by Patrick Mours
  - `Hooking.Patterns` by ThirteenAG
  - `Detours` by Microsoft
