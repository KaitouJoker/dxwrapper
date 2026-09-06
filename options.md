# DxWrapper 설정 옵션 상세 가이드 (Options Guide)

본 문서는 DxWrapper Low-Latency Edition의 설정 파일(`d3d9.ini` 또는 `dxwrapper.ini`)에서 사용할 수 있는 모든 옵션의 동작 원리와 권장 설정값을 상세히 정리한 가이드입니다.

---

## 목차
1. [주요 설정 (Major Settings)](#1-주요-설정-major-settings)
   - [호환성 섹션: Compatibility](#호환성-섹션-compatibility)
   - [디스플레이 및 프레젠테이션 섹션: d3d9](#디스플레이-및-프레젠테이션-섹션-d3d9)
2. [성능 및 초저지연 설정 (Advanced Latency & Performance)](#2-성능-및-초저지연-설정-advanced-latency--performance)
3. [부가 설정 (Supplementary Settings)](#3-부가-설정-supplementary-settings)
   - [디스플레이 및 그래픽 품질 제어](#디스플레이-및-그래픽-품질-제어)
   - [시스템 및 진단 제어](#시스템-및-진단-제어)
4. [옵션 요약 및 권장값 테이블](#4-옵션-요약-및-권장값-테이블)
5. [용도별 권장 프리셋](#5-용도별-권장-프리셋)

---

## 1. 주요 설정 (Major Settings)

렌더링 파이프라인의 인터페이스 및 디스플레이 프레젠테이션 방식을 결정하는 핵심 파라미터입니다.

### 호환성 섹션: [Compatibility]

#### EnableD3d9Wrapper
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `1`
- **동작 원리**: Direct3D 9 API 가로채기(Hooking) 및 래퍼 엔진을 활성화합니다. 본 프로젝트에서 구현된 모든 저지연 패치와 하드웨어 제어 기능을 사용하기 위해 반드시 `1`로 설정해야 합니다.

#### D3d9to9Ex
- **설정값**: `0` (비활성화) / `1` (D3D9Ex 승격)
- **기본값/권장값**: `0`
- **동작 원리**: 
  - `0`: 네이티브 Direct3D 9 인터페이스를 유지합니다. 카트라이더 등 `D3DPOOL_MANAGED` 메모리 풀을 사용하는 구형 게임 엔진에서 텍스처 백화(White Screen) 현상 없이 정상적인 그래픽 렌더링 및 안정적인 F11 창모드/전체화면 전환을 보장합니다.
  - `1`: 게임의 D3D9 호출을 내부적으로 `Direct3DCreate9Ex` 및 `CreateDeviceEx`로 강제 승격합니다. D3D9Ex의 하드웨어 프레임 레이턴시 제어를 활용할 수 있으나, D3DPOOL_MANAGED를 지원하지 않는 구형 게임에서는 텍스처 결함이 발생할 수 있습니다.

#### D3d9on12
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `0`
- **동작 원리**: Microsoft의 D3D9On12 매핑 계층을 활성화하여 D3D9 명령을 DirectX 12로 실시간 변환합니다. 카트라이더와 같이 구형 고정기능 파이프라인(FFP, Texture Stage States)을 사용하는 게임에서는 셰이더 변환 실패로 인해 회색 화면(Gray Screen)만 출력될 수 있으므로 `0`으로 유지해야 합니다.

---

### 디스플레이 및 프레젠테이션 섹션: [d3d9]

#### DisableMaxWindowedMode
- **설정값**: `0` (FSO 활성화 / Independent Flip) / `1` (FSO 비활성화 / Legacy Flip)
- **기본값/권장값**: `0` (방송/오버레이 녹화 시) 또는 `1` (0.1ms 극한의 타임어택 시)
- **동작 원리**: Windows 10/11의 **전체 화면 최적화 (Fullscreen Optimizations, FSO)**를 제어합니다.
  - **`0` 설정 시 (`Hardware: Independent Flip`)**:
    - Windows FSO가 정상 활성화되어 DWM(Desktop Window Manager)의 다이렉트 플립 경로를 탑니다.
    - 99% 프레임 타임: 약 1.35ms (약 740 FPS)
    - 특징: CapFrameX, PresentMon, Xbox Game Bar 등 **프레임 측정 오버레이가 게임 화면 위에 실시간으로 표출**되므로 단일 모니터 화면 캡처 및 영상 녹화가 완벽히 지원됩니다. Alt-Tab 전환 시 화면 깜빡임이 없습니다.
  - **`1` 설정 시 (`Hardware: Legacy Flip`)**:
    - Windows FSO를 강제로 차단하고 DWM 통신을 완전히 생략하는 순수 하드웨어 독점 전체화면(FSE)으로 동작합니다.
    - 99% 프레임 타임: 약 1.25ms (약 800 FPS)
    - 특징: DWM 토큰 교환 오버헤드(~0.1ms)가 생략되어 극단적인 반응속도를 확보합니다. 단, 데스크톱 오버레이가 게임 화면 위에 표시되지 않으므로 보조 모니터를 통해 확인해야 합니다.

#### EnableWindowMode
- **설정값**: `0` (원본 유지) / `1` (강제 창모드 변환)
- **기본값/권장값**: `0`
- **동작 원리**: 게임의 디바이스 생성 파라미터를 강제로 `Windowed = TRUE`로 덮어씁니다. 카트라이더와 같이 인게임 F11 키로 전체화면/창모드를 자체 전환하는 게임에서는 `0`으로 두어야 작업 표시줄 간섭 없이 원본 전체화면으로 동작합니다.

#### FullscreenWindowMode
- **설정값**: `0` (비활성화) / `1` (테두리 없는 전체 창모드)
- **기본값/권장값**: `0`
- **동작 원리**: 창모드 상태의 게임 창을 모니터 해상도에 맞춰 화면 전체 크기로 확장합니다.

#### WindowModeBorder
- **설정값**: `0` (창 테두리 숨김) / `1` (창 테두리 표시)
- **기본값/권장값**: `0`
- **동작 원리**: 창모드 구동 시 타이틀 바(Caption) 및 외곽 테두리(Border)의 표시 여부를 결정합니다.

#### FlipEx
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `0` (D3d9to9Ex=0 환경 시)
- **동작 원리**: Direct3D 9Ex 환경에서 저지연 큐잉 모델인 `D3DSWAPEFFECT_FLIPEX`를 활성화합니다. `D3d9to9Ex = 0`일 때는 자동으로 비활성화됩니다.

---

## 2. 성능 및 초저지연 설정 (Advanced Latency & Performance)

간헐적인 프레임 스파이크(1.1ms → 1.6ms 튐)를 원천 차단하고 상시 최대 렌더링 성능을 보장하기 위해 본 에디션에서 독자 구현된 핵심 저지연 기능입니다.

#### DisableDynamicSleep
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `1` (강력 권장)
- **동작 원리**:
  - 구형 게임 루프가 CPU 점유율 절약 등을 목적으로 호출하는 `Sleep(0)` 및 `Sleep(1)`을 가로챕니다.
  - Windows 스케줄러의 최소 타임 슬라이스 한계로 인해 `Sleep(1)`은 실제 1.0ms ~ 2.0ms 동안 스레드를 잠재워 주기적 프레임 스파이크를 유발합니다.
  - `DisableDynamicSleep = 1` 활성화 시:
    - `Sleep(0)` 호출을 CPU 캐시 친화적 인스트럭션인 `_mm_pause()`로 즉시 치환합니다.
    - `Sleep(1)` 호출을 커널 대기가 없는 **QPC(QueryPerformanceCounter) 초정밀 마이크로 스핀 루프**로 대체하여 스레드 슬립을 원천 방지하고 상시 최대 렌더링 상태를 유지합니다.

#### BoostRenderThread
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `1` (강력 권장)
- **동작 원리**:
  - 렌더링 스레드의 우선순위를 `THREAD_PRIORITY_TIME_CRITICAL`로 격상합니다.
  - Windows **MMCSS (Multimedia Class Scheduler Service)**에 스레드를 `'Games'` 프로필로 등록하여 백그라운드 프로세스에 의해 렌더링 파이프라인이 중단되거나 지연되는 현상을 방지합니다.

#### DisablePowerThrottling
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `1` (강력 권장)
- **동작 원리**:
  - Intel 12세대 이상(하이브리드 아키텍처) 및 최신 AMD CPU 환경에서 Windows의 **EcoQoS / 전원 스로틀링** 정책을 프로세스 레벨에서 영구 해제합니다.
  - 메인 게임 스레드가 효율 코어(E-Core)로 강등되거나 CPU 클럭이 강제 다운되는 것을 원천 차단하여 성능 코어(P-Core)에서 상시 최고 부스트 클럭을 유지하도록 보장합니다.

#### MaxFrameLatency
- **설정값**: `1` (저지연 권장) ~ `3`
- **기본값/권장값**: `1`
- **동작 원리**: GPU 렌더링 큐에 사전 대기할 수 있는 최대 백버퍼 프레임 수를 제한합니다. `1`로 설정 시 CPU-GPU 간 대기 프레임이 1개로 최소화되어 입력 장치 조작이 화면에 표시되는 **인풋-투-포톤(Input-to-Photon) 지연시간이 극소화**됩니다.

#### SpinWaitPacing
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `1`
- **동작 원리**: GPU 하드웨어 이벤트 쿼리(`D3DQUERYTYPE_EVENT`)를 발행하고, 프레임 제출 전 GPU 완료 대기 루프를 커널 슬립 대신 나노초 단위 마이크로 스핀-폴링으로 처리합니다. 드라이버 커널이 절전/대기 모드로 빠지는 것을 방지하여 균일한 프레임 페이싱을 유지합니다.

#### BypassFpuReset
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `1`
- **동작 원리**: Direct3D 9 디바이스 생성 시 x87 FPU 제어 단어(Control Word)를 매번 초기화하는 불필요한 CPU 오버헤드를 건너뜁니다(`D3DCREATE_FPU_PRESERVE` 효과).

#### RenderThreadAffinity
- **설정값**: `0` (자동 / 전체 코어) 또는 코어 비트마스크 (예: `0x000F` = 0~3번 코어)
- **기본값/권장값**: `0`
- **동작 원리**: 렌더링 스레드를 특정 고성능 물리 코어에 영구 고정하고자 할 때 비트마스크를 지정합니다. 특수한 CPU 튜닝 환경이 아닐 경우 `0`으로 두는 것이 가장 안전합니다.

---

## 3. 부가 설정 (Supplementary Settings)

### 디스플레이 및 그래픽 품질 제어

#### EnableVSync
- **설정값**: `0` (비활성화) / `1` (수직동기화 켬)
- **기본값/권장값**: `0`
- **동작 원리**: `0` 설정 시 프레임 생성 즉시 화면에 버퍼를 송출하는 `D3DPRESENT_INTERVAL_IMMEDIATE` 모드로 동작하여 디스플레이 대기 지연시간을 없앱니다.

#### ForceVsyncMode
- **설정값**: `0` (비활성화) / `1` (강제)
- **기본값/권장값**: `0`
- **동작 원리**: 게임 내부의 VSync 요청을 무시하고 `EnableVSync`에 지정된 값을 강제로 적용합니다.

#### LimitPerFrameFPS
- **설정값**: `0` (무제한) 또는 목표 FPS 숫자 (예: `240`)
- **기본값/권장값**: `0`
- **동작 원리**: 래퍼 레벨의 초정밀 FPS 리미터를 동작시킵니다. 카트라이더와 같이 무제한 프레임 렌더링을 지향하는 경우 `0`으로 설정합니다.

#### OverrideRefreshRate
- **설정값**: `0` (모니터 기본 주사율 사용) 또는 목표 Hz 숫자 (예: `240`)
- **기본값/권장값**: `0`
- **동작 원리**: 전체화면 진입 시 특정 모니터 주사율을 강제로 인가합니다.

#### AntiAliasing & AnisotropicFiltering
- **설정값**: `0` (게임 설정 준수) 또는 강제 레벨 숫자
- **기본값/권장값**: `0`
- **동작 원리**: 게임 내부 그래픽 설정과 드라이버 제어판 설정을 따르도록 `0`으로 유지합니다.

---

### 시스템 및 진단 제어

#### DisableLogging
- **설정값**: `0` (로깅 활성화) / `1` (로깅 끔)
- **기본값/권장값**: `1` (배포 및 실사용 시)
- **동작 원리**: 디버그 로그 파일(`d3d9-[프로세스명].log`) 생성을 차단하여 디스크 I/O 오버헤드를 완전히 없앱니다. 문제 발생 시 `0`으로 변경하면 디바이스 생성 및 프레젠테이션 단계별 상세 진단 로그가 기록됩니다.

#### RealDllPath
- **설정값**: `AUTO` (자동) 또는 실제 시스템 DLL의 절대 경로
- **기본값/권장값**: `AUTO`
- **동작 원리**: Windows 정식 시스템 라이브러리(`C:\Windows\SysWOW64\d3d9.dll`)를 자동으로 탐색하여 로드합니다.

#### FixPerfCounterUptime
- **설정값**: `0` (비활성화) / `1` (활성화)
- **기본값/권장값**: `0`
- **동작 원리**: PC 부팅 시간이 길어질 때 카운터 오버플로로 인해 발생하는 구형 게임의 타이머 버그를 패치합니다.

---

## 4. 옵션 요약 및 권장값 테이블

| 섹션 | 옵션 파라미터명 | 권장값 | 설명 요약 |
| :--- | :--- | :---: | :--- |
| `[Compatibility]` | **EnableD3d9Wrapper** | `1` | D3D9 래퍼 엔진 활성화 |
| `[Compatibility]` | **D3d9to9Ex** | `0` | D3D9 네이티브 유지 (구형 텍스처 백화 방지) |
| `[Compatibility]` | **D3d9on12** | `0` | D3D12 변환 끔 (회색 화면 방지) |
| `[d3d9]` | **DisableMaxWindowedMode** | `0` 또는 `1` | `0`: Independent Flip(오버레이/녹화), `1`: Legacy Flip(1.25ms) |
| `[d3d9]` | **EnableWindowMode** | `0` | 인게임 F11 네이티브 전체화면 유지 |
| `[d3d9]` | **FullscreenWindowMode** | `0` | 해상도 강제 확장 비활성화 |
| `[d3d9]` | **WindowModeBorder** | `0` | 창 테두리 숨김 |
| `[d3d9]` | **FlipEx** | `0` | D3D9Ex FlipEx 비활성화 |
| `[d3d9]` | **DisableDynamicSleep** | `1` | Sleep(0/1) QPC 마이크로스핀 치환 (1.6ms 스파이크 제거) |
| `[d3d9]` | **BoostRenderThread** | `1` | 렌더 스레드 Time-Critical & MMCSS 'Games' 등록 |
| `[d3d9]` | **DisablePowerThrottling** | `1` | Windows EcoQoS 해제 (P-Core 상시 최고 클럭) |
| `[d3d9]` | **MaxFrameLatency** | `1` | GPU 사전 렌더 큐 1프레임 최소화 |
| `[d3d9]` | **SpinWaitPacing** | `1` | GPU 이벤트 쿼리 나노초 스핀 페이싱 |
| `[d3d9]` | **BypassFpuReset** | `1` | 불필요한 x87 FPU 리셋 오버헤드 생략 |
| `[d3d9]` | **RenderThreadAffinity** | `0` | 렌더 스레드 CPU 선호도 자동 |
| `[d3d9]` | **EnableVSync** | `0` | 즉시 표시 (D3DPRESENT_INTERVAL_IMMEDIATE) |
| `[d3d9]` | **LimitPerFrameFPS** | `0` | 래퍼 레벨 프레임 제한 해제 (무제한) |
| `[General]` | **DisableLogging** | `1` | 디스크 I/O 제거 (디버깅 필요 시 `0`) |
| `[General]` | **RealDllPath** | `AUTO` | 정식 SysWOW64 DLL 자동 로드 |

---

## 5. 용도별 권장 프리셋

### 프로필 1: 방송 / 영상 녹화 & 오버레이 모드 (현재 기본 적용)
> **단일 모니터 화면 캡처 및 CapFrameX/PresentMon 오버레이 표출에 최적화**

```ini
[Compatibility]
D3d9to9Ex                  = 0
EnableD3d9Wrapper          = 1

[d3d9]
EnableWindowMode           = 0
FullscreenWindowMode       = 0
WindowModeBorder           = 0
FlipEx                     = 0
DisableMaxWindowedMode     = 0
DisableDynamicSleep        = 1
DisablePowerThrottling     = 1
BoostRenderThread          = 1
MaxFrameLatency            = 1
SpinWaitPacing             = 1
BypassFpuReset             = 1
DisableLogging             = 1
```
- **프레젠테이션 모드**: `Hardware: Independent Flip`
- **99% 프레임 타임**: ~1.35ms
- **특징**: F11 전체화면/창모드 정상 작동, 작업 표시줄 숨김, 게임 화면 위 프레임 오버레이 표출 가능, 빠른 Alt-Tab.

---

### 프로필 2: 극한의 타임어택 / 대회용 모드 (Legacy Flip)
> **0.1ms의 DWM 통신 지연조차 허용하지 않는 극한의 초저지연 세팅**

```ini
[Compatibility]
D3d9to9Ex                  = 0
EnableD3d9Wrapper          = 1

[d3d9]
EnableWindowMode           = 0
FullscreenWindowMode       = 0
WindowModeBorder           = 0
FlipEx                     = 0
DisableMaxWindowedMode     = 1
DisableDynamicSleep        = 1
DisablePowerThrottling     = 1
BoostRenderThread          = 1
MaxFrameLatency            = 1
SpinWaitPacing             = 1
BypassFpuReset             = 1
DisableLogging             = 1
```
- **프레젠테이션 모드**: `Hardware: Legacy Flip` (순수 독점 전체화면)
- **99% 프레임 타임**: ~1.25ms (약 800 FPS)
- **특징**: DWM 개입 제로, 오버레이는 보조 모니터 또는 별도 앱 창에서 확인 필요.
