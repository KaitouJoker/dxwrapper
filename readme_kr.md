# DxWrapper (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어](readme_kr.md)

DirectX 9 및 구형 DirectX 기반 게임을 최신 Windows 10/11 환경에서 **초저지연(Low-Latency)** 및 **최고의 프레임 안정성**으로 구동하기 위해 특화 개발된 고성능 DirectX 래퍼 라이브러리입니다.

특히 **카트라이더(KartRider)**와 같은 고주사율(144Hz ~ 240Hz+) 타임어택/경쟁전 환경에서 발생하는 **주기적 프레임 스파이크(1.1ms → 1.6ms 튐 현상)를 원천 차단**하고, Windows 10/11의 최신 디스플레이 기술인 **`Hardware: Independent Flip`** 및 순수 독점 전체화면 **`Hardware: Legacy Flip`**을 완벽하게 제어할 수 있도록 최적화되었습니다.

---

## 목차
1. [성능 비교 영상 (Performance Demonstration)](#1-성능-비교-영상-performance-demonstration)
2. [설치 및 INI 파일 로딩 구조](#2-설치-및-ini-파일-로딩-구조)
3. [주요 설정 (Major Settings)](#3-주요-설정-major-settings)
4. [추가 설정 - 성능 및 초저지연 (Advanced Latency & Performance)](#4-추가-설정---성능-및-초저지연-advanced-latency--performance)
5. [부가 설정 (Supplementary Settings)](#5-부가-설정-supplementary-settings)
6. [용도별 권장 프리셋 (Recommended Profiles)](#6-용도별-권장-프리셋-recommended-profiles)
7. [개발자 빌드 가이드 (Developer Build Guide)](#7-개발자-빌드-가이드-developer-build-guide)
8. [라이선스 및 크레딧](#8-라이선스-및-크레딧)

---

## 1. 성능 비교 영상 (Performance Demonstration)

아래 영상은 기존 레거시 런타임(`d3dx9_27.dll`)과 최적화된 DxWrapper(`d3d9.dll`) 환경에서 카트라이더 실행 시의 프레임 타임 안정성, 인풋 반응성 및 주기적 스파이크(1.1ms → 1.6ms) 튐 현상 제거 효과를 직접 비교한 녹화 영상입니다.

### 기존 레거시 런타임 (`d3dx9_27.dll`)
> 레거시 프레젠테이션 환경에서 주기적인 프레임 타임 스파이크(1.1ms → 1.6ms) 및 OS 스케줄러 전환에 따른 미세 버벅임 발생.

https://github.com/user-attachments/assets/2791662b-7665-4540-9d48-99ca38de2f33

### 최적화된 DxWrapper (`d3d9.dll`)
> OS 동적 Sleep 가로채기, MMCSS 실시간 렌더 스레드 승격, DWM 우회를 통해 스파이크 없이 약 1.1ms (900+ FPS)로 완벽하게 평탄화된 프레임 페이싱 유지.

https://github.com/user-attachments/assets/7d7afbc1-af99-4283-916f-f025c07ac0d6

---

## 2. 설치 및 INI 파일 로딩 구조

### 배포 및 설치
1. **기존 레거시 DLL 삭제**: 게임 설치 폴더에 기존 `d3dx9_27.dll` 파일이 존재할 경우 **반드시 삭제**해 주십시오. 구버전 또는 비정상적인 `d3dx9_27.dll`이 게임 폴더에 남아있으면 DirectX 런타임 로딩 간섭으로 인해 실행 오류나 렌더링 충돌이 발생할 수 있습니다.
2. 빌드된 `d3d9.dll`과 설정 파일(`d3d9.ini`)을 게임 실행 파일(예: `KartRider.exe`)이 위치한 폴더에 복사합니다.
3. 게임 실행 시 DLL이 프로세스에 자동으로 로드되며 하드웨어 파이프라인을 가속합니다.
4. **Zero-Config 독립 실행**: `d3d9.ini` 설정 파일이 없어도 DLL 내부에 최적화 기본값이 하드코딩되어 동일하게 최대 성능으로 작동합니다.

### INI 설정 파일 탐색 우선순위
본 버전은 사용자의 편의를 위해 설정 파일 자동 탐색 메커니즘을 지원합니다:
1. **`d3d9-[프로세스명].ini`** (예: `d3d9-KartRider.ini` - 프로세스 전용 설정 최우선)
2. **`d3d9.ini`** (기본 설정 파일)

---

## 3. 주요 설정 (Major Settings)

렌더링 파이프라인의 핵심 인터페이스 및 디스플레이 프레젠테이션 모델을 결정하는 파라미터입니다.

### `[Compatibility]` 섹션

#### `EnableD3d9Wrapper`
- **값**: `0` (비활성화) / `1` (활성화, 기본값 권장)
- **동작 원리**: Direct3D 9 API 가로채기(Hooking) 및 래퍼 엔진을 활성화합니다. 본 프로젝트의 모든 최적화 기능을 적용하기 위해 반드시 `1`로 설정해야 합니다.

#### `D3d9to9Ex`
- **값**: `0` (기본값 권장) / `1` (D3D9Ex 승격)
- **동작 원리**: 
  - `0`: 네이티브 Direct3D 9 인터페이스를 유지하며 모든 저지연 패치를 적용합니다. 카트라이더 등 `D3DPOOL_MANAGED` 메모리 풀을 사용하는 구형 엔진에서 완벽한 화면 렌더링 및 F11(전체화면/창모드) 전환 안정성을 보장합니다.
  - `1`: 게임의 D3D9 호출을 강제로 `Direct3DCreate9Ex` / `CreateDeviceEx`로 승격합니다.
  - **주의**: D3D9Ex는 사양상 `D3DPOOL_MANAGED` 풀이 제거되었으므로, 구형 텍스처 관리 방식을 사용하는 게임에서는 화면 백화(White Screen) 현상이 발생할 수 있습니다. 카트라이더 환경에서는 반드시 `0`을 사용하십시오.

---

### `[d3d9]` 섹션

#### `DisableMaxWindowedMode`
- **값**: `0` (Independent Flip) / `1` (Legacy Flip)
- **동작 원리**: Windows 10/11의 **전체 화면 최적화 (Fullscreen Optimizations, FSO)** 활성화 여부를 직접 제어합니다.
  - **`0` (권장: 방송 / 영상 녹화 / 오버레이 필요 시)**:
    - FSO를 활성화하여 Windows Desktop Window Manager(DWM)의 다이렉트 플립 파이프라인을 탑니다.
    - 프레젠테이션 모드: **`Hardware: Independent Flip`**
    - 99% 프레임 타임: **약 1.35ms** (약 740 FPS)
    - 특징: CapFrameX, PresentMon, Xbox Game Bar 등 **프레임 측정 오버레이가 게임 화면 위에 실시간 렌더링**되므로 단일 모니터 화면 캡처 및 영상 녹화가 완벽히 지원됩니다. Alt-Tab 전환 시 화면 깜빡임이 없습니다.
  - **`1` (권장: 극한의 타임어택 / 0.1ms 극단적 단축 필요 시)**:
    - FSO를 강제 차단하고 DWM을 완전히 우회하는 순수 하드웨어 스캔아웃 모드로 진입합니다.
    - 프레젠테이션 모드: **`Hardware: Legacy Flip`**
    - 99% 프레임 타임: **약 1.25ms** (약 800 FPS)
    - 특징: DWM 토큰 핸드셰이크가 생략되어 프레임 타임이 약 0.1ms 더 단축됩니다. 단, 데스크톱 오버레이가 게임 화면 위에 그려지지 않으므로 별도 서브 모니터에서 오버레이를 확인해야 합니다.

#### `EnableWindowMode` & `FullscreenWindowMode` & `WindowModeBorder`
- **`EnableWindowMode`**: `0` (게임 원본 전체화면 유지) / `1` (강제 창모드 변환)
- **`FullscreenWindowMode`**: `0` (비활성화) / `1` (테두리 없는 전체 창모드)
- **`WindowModeBorder`**: `0` (창 테두리 숨김) / `1` (창 테두리 표시)
- **동작 원리**:
  - 카트라이더처럼 인게임 F11 키를 통해 자체적으로 전체화면/창모드를 전환하는 게임의 경우, 세 옵션을 모두 `0`으로 두어야 작업 표시줄 간섭 없이 원본과 동일하게 꽉 찬 전체화면이 구현됩니다.

#### `FlipEx`
- **값**: `0` (비활성화) / `1` (활성화)
- **동작 원리**: Direct3D 9Ex 환경에서 `D3DSWAPEFFECT_FLIPEX` 프레젠테이션 모델을 활성화합니다. 창모드 상태에서 큐잉 지연을 줄일 때 사용되며, `D3d9to9Ex = 0` 환경에서는 자동으로 비활성화됩니다.

---

## 4. 추가 설정 - 성능 및 초저지연 (Advanced Latency & Performance)

간헐적인 프레임 스파이크를 없애고 CPU/GPU 하드웨어 자원을 상시 100% 렌더링에 집중시키기 위해 본 에디션에서 독자 구현된 초저지연 기능입니다.

#### `DisableDynamicSleep`
- **값**: `1` (기본값, 강력 권장) / `0` (비활성화)
- **동작 원리**:
  - 구형 게임 엔진은 프레임 레이트 안정화나 CPU 점유율 절약을 위해 메인 렌더 루프 내부에서 `Sleep(0)` 또는 `Sleep(1)`을 주기적으로 호출합니다.
  - Windows 스케줄러의 타임 슬라이스 한계로 인해 `Sleep(1)`은 실제 1.0ms ~ 2.0ms 동안 스레드를 잠재우며, 이것이 **1.1ms로 유지되던 프레임 타임이 간헐적으로 1.6ms까지 튀는 스파이크의 결정적 원인**이었습니다.
  - `DisableDynamicSleep = 1` 활성화 시:
    - `Sleep(0)` 호출을 CPU 캐시 친화적인 `_mm_pause()` (x86 PAUSE 인스트럭션)로 즉시 치환합니다.
    - `Sleep(1)` 호출을 커널 대기가 없는 **QPC(QueryPerformanceCounter) 초정밀 마이크로 스핀 루프**로 대체하여 스레드 슬립을 원천 방지하고 상시 최대 렌더링 상태를 유지합니다.

#### `BoostRenderThread`
- **값**: `1` (기본값 권장) / `0` (비활성화)
- **동작 원리**:
  - DirectX 디바이스가 생성되고 프레임 제출(`Present`)이 일어나는 렌더링 스레드의 우선순위를 `THREAD_PRIORITY_TIME_CRITICAL`로 격상합니다.
  - Windows **MMCSS (Multimedia Class Scheduler Service)**에 스레드를 `'Games'` 프로필로 등록하여 OS 백그라운드 태스크나 타 프로세스에 의해 렌더링 파이프라인이 선점(Preempt)되는 현상을 방지합니다.

#### `DisablePowerThrottling`
- **값**: `1` (기본값 권장) / `0` (비활성화)
- **동작 원리**:
  - 최신 Intel 12세대 이상(하이브리드 아키텍처) 및 AMD CPU 환경에서 Windows의 **EcoQoS / 전원 스로틀링** 정책을 프로세스 레벨에서 영구 해제합니다.
  - 메인 게임 스레드가 절전 코어(E-Core)로 강등 배치되거나 클럭이 다운되는 것을 원천 차단하여 P-Core에서 상시 최고 부스트 클럭을 유지하도록 보장합니다.

#### `MaxFrameLatency`
- **값**: `1` (기본값 권장, 저지연) ~ `3`
- **동작 원리**:
  - GPU 렌더링 큐에 사전 대기할 수 있는 최대 백버퍼 프레임 수를 제한합니다.
  - `1`로 설정 시 CPU가 다음 프레임을 준비하기 전에 이전 프레임의 GPU 렌더링이 완료되도록 동기화하여 키보드/마우스 입력이 모니터 화면에 반영되는 **인풋-투-포톤(Input-to-Photon) 지연시간을 최소화**합니다.

#### `SpinWaitPacing`
- **값**: `1` (기본값 권장) / `0` (비활성화)
- **동작 원리**:
  - GPU 하드웨어 이벤트 쿼리(`D3DQUERYTYPE_EVENT`)를 발행하고, 다음 프레임 제출 전 GPU 완료 대기 루프를 커널 슬립 대신 나노초 단위 마이크로 스핀-폴링으로 처리합니다.
  - GPU 드라이버 커널 스케줄러가 대기 상태로 전환되는 것을 막아 균일하고 부드러운 프레임 페이싱을 달성합니다.

#### `BypassFpuReset`
- **값**: `1` (기본값 권장) / `0` (비활성화)
- **동작 원리**:
  - Direct3D 9 디바이스 생성 시 x87 FPU 제어 단어(Control Word)를 매번 초기화하는 오버헤드를 건너뜁니다(`D3DCREATE_FPU_PRESERVE` 효과).

#### `RenderThreadAffinity`
- **값**: `0` (자동 / 전체 코어 스케줄링) 또는 코어 비트마스크 (예: `0x000F` = 0~3번 코어)
- **동작 원리**: 렌더링 스레드를 특정 고성능 물리 코어(P-Core)에 영구 고정하고자 할 때 사용합니다.

---

## 5. 부가 설정 (Supplementary Settings)

### 디스플레이 및 그래픽 품질 제어
- **`EnableVSync = 0`**: 수직동기화를 해제하고 즉시 화면에 프레임을 송출합니다(`D3DPRESENT_INTERVAL_IMMEDIATE`).
- **`LimitPerFrameFPS = 0`**: 래퍼 레벨 FPS 제한 해제 (무제한 렌더링).
- **`OverrideRefreshRate = 0`**: 모니터의 기본 주사율(예: 240Hz)을 그대로 사용. 특정 주사율 강제 시 해당 Hz 값 입력.
- **`AntiAliasing = 0` / `AnisotropicFiltering = 0`**: 게임 내 자체 그래픽 설정을 따르도록 `0`으로 유지.

### 시스템 및 진단 제어
- **`DisableLogging = 1`**: 디버그 로그 파일 생성을 비활성화하여 디스크 I/O 오버헤드를 완전히 제거합니다. (디버깅이 필요할 경우 `0`으로 변경 시 `d3d9-[프로세스명].log` 파일이 생성됩니다.)
- **`RealDllPath = AUTO`**: 시스템 정식 `SysWOW64\d3d9.dll`을 자동으로 탐색하여 로드합니다.
- **`FixPerfCounterUptime = 0`**: 장시간 PC 부팅 시 카운터 오버플로 패치 (필요 시 `1`).

---

## 6. 용도별 권장 프리셋 (Recommended Profiles)

모든 저지연 최적화(Sleep 마이크로스핀, MMCSS 우선순위 부스트, 전원 스로틀링 해제 등)는 공통으로 적용되며, **두 프로필 간의 유일한 차이는 `DisableMaxWindowedMode` 값 하나뿐**입니다.

### 프로필 차이점 비교표

| 비교 항목 | 프로필 1 (방송 / 영상 녹화 / 오버레이) | 프로필 2 (극한의 타임어택 / 대회용) |
| :--- | :---: | :---: |
| **핵심 변경 옵션** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **프레젠테이션 모드** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **99% 프레임 타임** | 약 1.35ms (약 740 FPS) | 약 1.25ms (약 800 FPS) |
| **단일 모니터 오버레이 표출** | 지원 (게임 화면 위에 실시간 렌더링) | 미지원 (보조 모니터 또는 별도 창 확인) |
| **Alt-Tab 전환 반응** | 즉시 전환 (화면 깜빡임 없음) | 전체화면 깜빡임 발생 |
| **권장 대상** | 방송 스트리머, 유튜브 녹화, 일반 플레이어 | 기록 갱신용 타임어택, 대회 출전 선수 |

---

### 통합 최적화 설정 (`d3d9.ini`)

아래 설정을 기본 베이스로 사용하며, 목적에 맞춰 **`DisableMaxWindowedMode`** 값만 `0` 또는 `1`로 전환하십시오:

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
;; [핵심 전환 옵션] 아래 값 하나로 동작 모드가 결정됩니다:
;;   DisableMaxWindowedMode = 0  -> 프로필 1 (Independent Flip, 오버레이 표출, ~1.35ms)
;;   DisableMaxWindowedMode = 1  -> 프로필 2 (Legacy Flip, 극한 타임어택, ~1.25ms)
;; ======================================================================
DisableMaxWindowedMode     = 0
```

---

## 7. 개발자 빌드 가이드 (Developer Build Guide)

본 프로젝트는 MSVC v145 (Visual Studio 2026 / 2022) x86 환경에서 불필요한 외부 의존성을 제거하고 650KB 대의 초경량 단일 바이너리로 컴파일되도록 빌드 파이프라인이 구성되어 있습니다.

### 원클릭 빌드
프로젝트 루트 디렉토리에서 배치 스크립트를 실행합니다:
```cmd
build_d3d9.bat
```

### 산출물 위치
- `Release\d3d9.dll` (약 656 KB)
- `Release\d3d9.ini`

---

## 8. 라이선스 및 크레딧

본 소프트웨어는 zlib 라이선스 하에 배포됩니다.

- **DxWrapper Base Engine**: Copyright (C) 2025 Elisha Riedlinger
- **Low-Latency & FSO Optimization Layer**: Copyright (C) 2026 KaitouJoker
- **Third-Party Components**:
  - `d3d8to9` by Patrick Mours
  - `Hooking.Patterns` by ThirteenAG
  - `Detours` by Microsoft
