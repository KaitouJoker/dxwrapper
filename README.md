# DirectX 11 (DXGI) Low-Latency Proxy Wrapper

A dedicated, high-performance 64-bit DirectX 11 (DXGI) proxy wrapper designed to minimize presentation latency, eliminate CPU/GPU scheduling jitter, and enforce true Hardware Independent Flip on Windows 10 and Windows 11.

---

## Architecture and Core Pillars

### 1. True Hardware: Independent Flip (Direct DWM Bypass)
Standard windowed and borderless DirectX 11 games often fall back to `Composed: Copy with GPU GDI` or `Composed: Flip`, subjecting frames to the Desktop Window Manager (DWM) composition queue, which adds 1 to 2 frames of display latency.

This wrapper intercepts swapchain creation across all DXGI pathways:
- `IDXGIFactory::CreateSwapChain`
- `IDXGIFactory2::CreateSwapChainForHwnd`
- `D3D11CreateDeviceAndSwapChain`

Key presentation modifications:
- Automatically upgrades swapchain `SwapEffect` to `DXGI_SWAP_EFFECT_FLIP_DISCARD`.
- Enforces a minimum backbuffer count of 2.
- Adds `DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING` and passes `DXGI_PRESENT_ALLOW_TEARING` to `IDXGISwapChain::Present`.
- Enables variable refresh rate (G-Sync, FreeSync, Adaptive-Sync) with zero composition overhead.
- Promotes the window to native `Hardware: Independent Flip` (verified via Intel PresentMon, CapFrameX, and Special K).

### 2. Operating System and Kernel Scheduler Optimizations
- **Windows Power Throttling / EcoQoS Disabler**:
  Invokes `SetProcessInformation` with `ProcessPowerThrottling` (`PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION | PROCESS_POWER_THROTTLING_EXECUTION_SPEED`). Prevents the Windows thread scheduler from parking or demoting game threads to low-power efficiency cores (E-cores).
- **Dynamic Sleep Hooking**:
  Hooks `kernel32!Sleep` and `SleepEx` using Microsoft Detours.
  - Replaces `Sleep(0)` with non-yielding CPU pause instructions (`YieldProcessor()`), avoiding thread scheduling quanta preemption.
  - Converts micro-sleep calls (1ms to 2ms) into high-precision `QueryPerformanceCounter` (QPC) spin-wait loops.
- **MMCSS and Thread Priority Elevation**:
  Elevates the render thread priority to `THREAD_PRIORITY_TIME_CRITICAL` and registers the thread with the Multimedia Class Scheduler Service (MMCSS) under the `Games` profile.
- **Sub-Millisecond System Timer**:
  Requests the maximum timer resolution supported by the kernel (0.5ms via `NtSetTimerResolution` and `timeBeginPeriod(1)`).

### 3. Direct3D 11 Pipeline and GPU Queue Minimization
- **Maximum Frame Latency**:
  Queries `IDXGIDevice1` from the Direct3D 11 device and invokes `SetMaximumFrameLatency(1)`, constraining the GPU command submission queue to a single pre-rendered frame.
- **GPU Thread Priority**:
  Sets `IDXGIDevice1::SetGPUThreadPriority(7)` to guarantee immediate GPU hardware queue scheduling for game draw calls.
- **Spin-Wait Frame Pacing**:
  Replaces standard OS timer waits with microsecond-accurate QPC spin loops for stable frame times and reduced input-to-photon latency.

### 4. Strict 64-Bit Clean Architecture
- Specifically compiled for `x64` architectures.
- All x86, 32-bit assembly, ARM, and obsolete legacy graphics backends (DirectDraw, Direct3D 8, Direct3D 9) have been cleanly isolated or removed.
- Self-contained Microsoft Detours source integration without external binary runtime dependencies.

---

## Configuration Guide (`dxgi.ini`)

Place `dxgi.ini` in the same directory as `dxgi.dll` and the target game executable.

```ini
[General]
RealDllPath                 = AUTO
WrapperMode                 = AUTO
LoadCustomDllPath           = 
ExcludeProcess              = 
IncludeProcess              = 
RunProcess                  = 
WaitForProcess              = 0
DisableLogging              = 1

[dxgi]
ForceFlipModel              = 1
AllowTearing                = 1
MaxFrameLatency             = 1
SetGPUThreadPriority        = 7

[Performance]
DisableDynamicSleep         = 1
DisablePowerThrottling      = 1
BoostRenderThread           = 1
SpinWaitPacing              = 1
RenderThreadAffinity        = 0
```

### Parameter Reference

| Setting | Default | Description |
| :--- | :---: | :--- |
| `ForceFlipModel` | `1` | Upgrades swapchain to `DXGI_SWAP_EFFECT_FLIP_DISCARD` with at least 2 buffers. |
| `AllowTearing` | `1` | Enables tearing flag for G-Sync/FreeSync and true unbuffered presentation. |
| `MaxFrameLatency` | `1` | Maximum pre-rendered frame queue depth (1 = lowest input lag). |
| `SetGPUThreadPriority` | `7` | GPU context scheduling priority (range: 0 to 7; 7 = real-time). |
| `DisableDynamicSleep` | `1` | Intercepts `Sleep` to eliminate quantum scheduling stalls. |
| `DisablePowerThrottling` | `1` | Disables Windows EcoQoS power throttling across all game threads. |
| `BoostRenderThread` | `1` | Elevates render thread priority to `TIME_CRITICAL` and registers with MMCSS. |
| `SpinWaitPacing` | `1` | Employs high-precision QPC busy loops for frame pacing. |
| `RenderThreadAffinity` | `0` | Bitmask specifying CPU core affinity (0 = OS default scheduling). |
| `DisableLogging` | `1` | Disables file logging for minimum runtime overhead. |

---

## Building from Source

### Requirements
- Windows 10 / Windows 11 (64-bit)
- Visual Studio 2022 (Community, Professional, or Enterprise) or Visual Studio Build Tools
- Desktop development with C++ workload (MSVC v143 or v144 x64 toolset, Windows 10/11 SDK)

### Build Commands
1. Clone the repository:
   ```cmd
   git clone https://github.com/KaitouJoker/wintun.git dxwrapper-dx11
   cd dxwrapper-dx11
   git checkout DX11-x64
   ```
2. Run the automated build script:
   ```cmd
   build_dx11.bat nopause
   ```
3. Compiled artifacts will be placed in the `Release\` folder:
   - `Release\dxgi.dll`
   - `Release\dxgi.ini`

---

## Verification

To verify that the proxy wrapper is operating correctly:
1. Copy `Release\dxgi.dll` and `Release\dxgi.ini` to the game's executable directory.
2. Launch the game.
3. Open Intel PresentMon or CapFrameX:
   - Verify `Presentation Mode` reports `Hardware: Independent Flip` or `Hardware: Composed: Independent Flip`.
   - Verify zero composition queue latency.
