# DxWrapper x64 General (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어 (Korean)](readme_kr.md)

64비트(x64 Native) DirectX 9 기반 게임을 최신 Windows 10/11 환경에서 **초저지연(Low-Latency)**, **완벽한 프레임 페이싱(Rock-solid Frame Pacing)**, **OS 스케줄러 지터 원천 제거(Zero Scheduling Jitter)**, 그리고 **2,600+ FPS 초고프레임 렌더링 파이프라인**으로 구동하기 위해 특화 개발된 범용(Intel / AMD Universal) 고성능 DirectX 래퍼 라이브러리입니다.

특히 **카트라이더(KartRider x64)** 와 같은 고주사율(144Hz, 240Hz, 360Hz, 540Hz+) 타임어택/경쟁전 환경에서 발생하는 **주기적 프레임 스파이크 및 1,000ms 주기 마이크로 스터터링을 원천 차단** 하고, 64비트 엔진 내부에 하드코딩된 **1,000 FPS 프레임 게이트 제약을 완전히 우회** 하여 최신 시스템(AMD Ryzen 및 Intel 12/13/14세대 Core 프로세서 + NVIDIA GeForce RTX 시리즈)에서 **2,600+ FPS**(0.38ms 프레임 타임) 및 초저 디스플레이 지연 시간(**0.55 ~ 0.70ms Raw Latency**)을 상시 유지합니다. 또한 Windows 10/11의 **DirectFlip / Independent Flip** 업그레이드 심(`SetSwapEffectShim = 1`)을 통해 DWM 3프레임 버퍼링 큐를 원천 우회합니다.

---

## 목차
1. [성능 비교 영상 (Performance Demonstration)](#1-성능-비교-영상-performance-demonstration)
2. [설치 및 INI 파일 로딩 구조](#2-설치-및-ini-파일-로딩-구조)
3. [주요 설정 (Core Display & Compatibility Settings)](#3-주요-설정-core-display--compatibility-settings)
4. [추가 설정 - 성능 및 초저지연 (Low-Latency & Performance Optimizations)](#4-추가-설정---성능-및-초저지연-low-latency--performance-optimizations)
5. [부가 설정 (Supplementary Settings)](#5-부가-설정-supplementary-settings)
6. [용도별 권장 프리셋 (Recommended Profiles)](#6-용도별-권장-프리셋-recommended-profiles)
7. [개발자 빌드 가이드 (Developer Build Guide)](#7-개발자-빌드-가이드-developer-build-guide)
8. [라이선스 및 크레딧](#8-라이선스-및-크레딧)

---

## 1. 성능 비교 영상 (Performance Demonstration)

아래 영상은 기존 64비트 레거시 런타임(`d3dx9_43.dll`)과 최적화된 DxWrapper(`d3d9.dll`) 환경에서 카트라이더 실행 시의 프레임 타임 안정성, 인풋 반응성 및 주기적 스파이크 튐 현상 제거 효과를 직접 비교한 녹화 영상입니다.

### 기존 레거시 런타임 (`d3dx9_43.dll`)
> 레거시 프레젠테이션 환경에서 주기적인 프레임 타임 스파이크 및 OS 스케줄러 전환에 따른 미세 버벅임 발생.

https://github.com/user-attachments/assets/1d350200-094a-451f-8b3a-af26f0d09b87

### 최적화된 DxWrapper (`d3d9.dll`)
> x64 프레임 게이트 해제, DWM DirectFlip 우회, 인텔 하이브리드 P코어 자동 고정 및 안전한 스케줄링을 통해 끊김 없이 약 0.38ms ~ 0.6ms (2,600+ FPS)로 완벽하게 평탄화된 프레임 페이싱 유지.

https://github.com/user-attachments/assets/f270993d-ed86-4d61-a123-00b3e891eb22

---

## 2. 설치 및 INI 파일 로딩 구조

### 배포 및 설치
1. **기존 레거시 DLL 정리**: 게임 설치 폴더에 비공식 패치용으로 남아있는 구형 래퍼 파일이 존재할 경우 삭제하거나 백업해 주십시오.
2. 빌드된 `d3d9.dll`과 설정 파일(`d3d9.ini`)을 게임 실행 파일(예: `KartRider.exe`)이 위치한 폴더에 복사합니다.
3. 게임 실행 시 64비트 `d3d9.dll`이 프로세스에 자동으로 로드되며 Direct3D 호출을 가로채 하드웨어 파이프라인을 가속합니다.
4. **Zero-Config 독립 실행**: `d3d9.ini` 설정 파일이 없어도 DLL 내부에 e스포츠 경쟁전 최적화 기본값(`FlipEx=1`, `DisableDynamicSleep=0`, `BoostRenderThread=1`, `MaxFrameLatency=1`, `DisablePowerThrottling=1`, `DisableLogging=1`, `D3d9to9Ex=0`, `UnlockFrameRate=1`, `RenderThreadAffinity=0`, `SetSwapEffectShim=1`, `SpinWaitPacing=0`)이 하드코딩되어 있어 설정 파일 없이도 동일하게 최대 성능으로 작동합니다.

### INI 설정 파일 탐색 우선순위
본 버전은 프로세스별 맞춤 설정을 위해 설정 파일 자동 탐색 메커니즘을 지원합니다:
1. **`d3d9-[프로세스명].ini`** (최우선 순위, 예: `d3d9-KartRider.ini`)
2. **`d3d9.ini`** (기본 설정 파일)

---

## 3. 주요 설정 (Core Display & Compatibility Settings)

렌더링 파이프라인의 핵심 인터페이스 및 스왑체인 디스플레이 프레젠테이션 모델을 결정하는 파라미터입니다.

### `[Compatibility]` 섹션

#### `EnableD3d9Wrapper`
- **값**: `0` (비활성화) | `1` (활성화, 기본값)
- **동작 원리**: Direct3D 9 API 가로채기(Hooking) 및 래퍼 엔진을 활성화합니다. 본 프로젝트의 모든 최적화 기능을 적용하기 위해 반드시 `1`로 유지해야 합니다.

#### `D3d9to9Ex`
- **값**: `0` (표준 Direct3D 9, 권장) | `1` (Direct3D 9Ex 승격)
- **동작 원리**: 
  - `0`: 네이티브 Direct3D 9 인터페이스를 유지하며 모든 저지연 패치를 적용합니다. 카트라이더 등 `D3DPOOL_MANAGED` 메모리 풀을 사용하는 구형 엔진에서 완벽한 텍스처 렌더링 및 해상도/창모드(F11) 전환 안정성을 보장합니다.
  - `1`: 게임의 D3D9 디바이스 생성을 강제로 `Direct3DCreate9Ex` / `CreateDeviceEx`로 승격합니다.
  - **주의**: D3D9Ex 규격에서는 `D3DPOOL_MANAGED` 지원이 완전히 제거되었습니다. 구형 텍스처 관리 방식을 사용하는 게임에서 강제로 `1`을 설정할 경우 서피스 할당 오류(`D3DERR_INVALIDCALL`) 또는 화면 백화(White Screen) 현상이 발생합니다. 카트라이더 환경에서는 반드시 `0`을 사용하십시오.

---

### `[d3d9]` 섹션

#### `SetSwapEffectShim`
- **값**: `0` (비활성화) | `1` (DirectFlip 승격, 기본값)
- **동작 원리**: Windows 10/11 DWM의 `Direct3D9SetSwapEffectUpgradeShim(1)` API를 호출하여 `D3DSWAPEFFECT_DISCARD` 스왑체인을 창모드에서도 **DirectFlip / Independent Flip**으로 승격합니다. DWM의 3프레임 컴포지션 버퍼링을 원천 우회하여 인풋 지연 시간을 5ms 이상에서 **1ms 미만**으로 대폭 단축하며, 하드웨어 텔레메트리(PresentMon, Reflex 오버레이)를 복원합니다.

#### `DisableMaxWindowedMode`
- **값**: `0` (Independent Flip, 기본값) | `1` (Legacy Flip)
- **동작 원리**: Windows 10/11의 **전체 화면 최적화 (Fullscreen Optimizations, FSO)** 활성화 여부를 직접 제어합니다.
  - **`0` (권장: 방송 / 영상 녹화 / 단일 모니터 오버레이 사용 시)**: FSO를 활성화하여 DWM Direct Flip 큐를 탑니다. CapFrameX, PresentMon, Discord 등 인게임 오버레이가 정상 표시되며 Alt-Tab 전환이 즉시 이루어집니다.
  - **`1` (권장: 극한의 타임어택)**: FSO를 차단하고 순수 하드웨어 스캔아웃 모드로 진입합니다.

#### `EnableWindowMode` / `FullscreenWindowMode` / `WindowModeBorder`
- **값**: 모두 `0` (권장)
- **동작 원리**: 카트라이더처럼 인게임 F11 키를 통해 자체적으로 전체화면/창모드를 전환하는 게임의 경우, 세 옵션을 모두 `0`으로 두어야 작업 표시줄 간섭 없이 원본과 동일하게 꽉 찬 화면이 구현됩니다.

#### `FlipEx`
- **값**: `0` (비활성화) | `1` (활성화, 기본값)
- **동작 원리**: Direct3D 9Ex 환경에서 창모드 스왑체인을 위한 `D3DSWAPEFFECT_FLIPEX` 프레젠테이션 모델을 활성화합니다.

---

## 4. 추가 설정 - 성능 및 초저지연 (Low-Latency & Performance Optimizations)

인텔/AMD 전 기종에서 마이크로 스터터와 프리징을 완전히 박멸하고 2,600+ FPS를 달성하기 위해 특화 설계된 최적화 시스템입니다.

#### `UnlockFrameRate`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - 카트라이더 x64 클라이언트 메인 루프에 하드코딩된 1ms 프레임 제한(`nowTick == lastFrameTick`)을 원천 우회합니다.
  - 64비트 RIP-상대 주소 계산식(`8B 3D [disp32] FF 15 ...`)을 정밀 스캔하여 타깃 변수(`0x1498179bc`)를 자동 검출합니다.
  - 코드(`.text`) 영역을 1바이트도 수정하지 않고 프레임 직후 `.data`의 틱 카운터만 `0xFFFFFFFF`로 리셋하므로 넥슨 보안 모듈(BlackCipher)의 메모리 무결성 검증을 100% 무반응 통과합니다.
  - 단일 사이클 인라인 리셋으로 엔진 프레임 제한(1,000 FPS)을 완벽히 해제하여 **2,600+ FPS** 출력을 실현합니다.

#### `BoostRenderThread`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - Direct3D `Present`를 수행하는 메인 렌더 스레드의 우선순위를 `THREAD_PRIORITY_ABOVE_NORMAL`(레벨 9)로 지능형 승격합니다.
  - **MMCSS("Games") 제거**: Windows MMCSS의 1,000ms 주기 시스템 리소스 회수(`SystemResponsiveness`) 메커니즘을 배제하여 1초 주기 미세 끊김을 원천 차단했습니다.
  - **스레드 기아 방지**: `TIME_CRITICAL`(레벨 15) 대신 안전한 상위 우선순위를 채택하여 인텔 6코어(i5-12400F, i5-13400F) 환경에서 백그라운드 리소스 로더와 보안 스레드가 멈추는 **500ms 이상 프리징 현상을 완전히 해결**했습니다.

#### `RenderThreadAffinity`
- **값**: `0` (자동 / P코어 자동 할당, 기본값) 또는 물리 코어 비트마스크
- **동작 원리**:
  - `0` 설정 시 Windows 논리 프로세서 토폴로지(`GetLogicalProcessorInformationEx`)를 검사합니다.
  - **인텔 하이브리드 아키텍처(12/13/14세대 Core 프로세서)** 감지 시, 렌더 스레드를 E코어(효율 코어)에서 완전히 배제하고 **P코어(성능 코어) 전용 마스크**에 자동 고정합니다.
  - AMD 및 단일 구조 인텔 CPU에서는 OS 스케줄러가 코어를 유연하게 분배하도록 하여 단일 코어 과부하로 인한 스터터링을 차단합니다.

#### `SpinWaitPacing`
- **값**: `0` (비활성화, 기본값 / 2600+ FPS 권장) | `1` (활성화)
- **동작 원리**:
  - `SetSwapEffectShim = 1`을 통한 DirectFlip 파이프라인이 이미 GPU 큐 지연시간을 1프레임 이하(<1ms)로 완벽히 제어합니다.
  - `0`으로 설정하여 불필요한 GPU 펜스 스핀 대기를 제거함으로써 **2,600+ FPS의 최대 처리량**을 달성합니다.

#### `DisableDynamicSleep`
- **값**: `0` (비활성화, 권장 기본값) | `1` (활성화)
- **동작 원리**:
  - 1ms 프레임 게이트는 이미 `UnlockFrameRate`가 메모리 레벨에서 완벽히 해제하므로 렌더 루프에서 Sleep이 발생하지 않습니다.
  - `0`으로 설정하여 백그라운드 워커 스레드(오디오, 네트워크, 안티치트)가 필요 시 정상적으로 유휴 상태에 진입하도록 보장하며, 비지 스핀으로 인한 CPU 코어 낭비를 방지합니다.

#### `DisablePowerThrottling`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - Windows 10/11의 **전원 스로틀링(Power Throttling / EcoQoS)** 기능을 영구 비활성화하여 렌더 스레드가 절전 모드로 강등되는 것을 방지합니다.

#### `MaxFrameLatency`
- **값**: `1` (최소 지연시간, 기본값)
- **동작 원리**: 드라이버의 백버퍼 대기 큐를 1프레임으로 제한하여 인풋-투-포톤(Input-to-Photon) 지연 시간을 최소화합니다.

---

## 5. 부가 설정 (Supplementary Settings)

### 디스플레이 및 품질 제어
- **`EnableVSync = 0`**: 수직 동기화를 즉시 해제하여 무제한 프레젠테이션(`D3DPRESENT_INTERVAL_IMMEDIATE`)을 보장합니다.
- **`LimitPerFrameFPS = 0`**: 래퍼 레벨의 프레임 제한을 해제합니다.
- **`OverrideRefreshRate = 0`**: 모니터의 기본 최고 주사율(예: 240Hz, 360Hz, 540Hz)을 그대로 적용합니다.

### 진단 및 시스템 제어
- **`DisableLogging = 1`**: 런타임 디스크 I/O 발생을 원천 차단하여 로그 기록으로 인한 스터터링을 방지합니다. (문제 분석 시에만 `0`으로 변경).
- **`HandleExceptions = 0`**: 넥슨 BlackCipher(`BlackCall64.aes`)의 무결성 검사용 정상 AV 예외에 불필요한 전역 VEH 가로채기를 차단하여 충돌을 방지합니다.

---

## 6. 용도별 권장 프리셋 (Recommended Profiles)

모든 핵심 최적화(x64 1,000 FPS 프레임 게이트 해제, 인텔 하이브리드 P코어 자동 할당, DirectFlip DWM 우회, 전원 스로틀링 차단)가 기본 적용되어 있습니다.

### 프리셋 비교 요약

| 항목 | 기본 프로필 (범용 경쟁전 / 방송 / 오버레이) | 타임어택 극저지연 프로필 |
| :--- | :---: | :---: |
| **핵심 설정** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **프레젠테이션 모드** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **평균 프레임레이트** | **2,400 ~ 2,660+ FPS** | **2,500 ~ 2,700+ FPS** |
| **디스플레이 지연 시간** | **0.55 ~ 0.70 ms** | **0.50 ~ 0.65 ms** |
| **인게임 오버레이 표시** | 지원 (PresentMon, CapFrameX 화면 위 출력) | 비지원 (보조 모니터 출력 필요) |
| **Alt-Tab 전환** | 즉각 전환 (화면 깜빡임 없음) | 전체화면 모드 전환 깜빡임 발생 |
| **권장 대상** | 일반/경쟁전 유저, 스트리머, 오버레이 사용자 | 대회/타임어택 0.01초 랩타임 기록 단축용 |

---

### 표준 권장 설정 파일 (`d3d9.ini`)

```ini
[Compatibility]
D3d9to9Ex                  = 0          ;; 0 유지: 카트라이더 텍스처 백화 현상 방지
EnableD3d9Wrapper          = 1

[d3d9]
EnableWindowMode           = 0
FullscreenWindowMode       = 0
WindowModeBorder           = 0
FlipEx                     = 1
DisableDynamicSleep        = 0          ;; 0 유지: 백그라운드 스레드 정상 유휴 보장
DisablePowerThrottling     = 1
BoostRenderThread          = 1          ;; 1: 안전한 우선순위 승격 (1000ms 끊김 및 500ms 프리징 차단)
MaxFrameLatency            = 1
SpinWaitPacing             = 0          ;; 0: 최대 2,600+ FPS 달성
RenderThreadAffinity       = 0          ;; 0: 인텔 P코어 자동 감지 및 안전 분배
UnlockFrameRate            = 1          ;; 1: x64 1,000 FPS 엔진 제한 해제 (2600+ FPS)
SetSwapEffectShim          = 1          ;; 1: DirectFlip 승격 (DWM 버퍼 우회, <1ms 레이턴시)
LimitPerFrameFPS           = 0
DisableLogging             = 1

;; ======================================================================
;; [프레젠테이션 모드 전환]
;;   DisableMaxWindowedMode = 0  -> Independent Flip (오버레이 지원, 즉각 Alt-Tab)
;;   DisableMaxWindowedMode = 1  -> Legacy Flip (순수 하드웨어 스캔아웃, 극저지연)
;; ======================================================================
DisableMaxWindowedMode     = 0
```

---

## 7. 개발자 빌드 가이드 (Developer Build Guide)

본 프로젝트는 MSVC v145 (Visual Studio 2026 / 2022) 도구 모음을 사용하여 64비트 네이티브(`x64 Release`) 아키텍처로 컴파일됩니다.

### 원클릭 자동 빌드
저장소 루트에서 배치 스크립트를 실행합니다:
```cmd
build_d3d9_x64.bat
```

CI/CD 파이프라인이나 자동화 스크립트에서 프롬프트 없이 실행 시:
```cmd
build_d3d9_x64.bat nopause
```

### 빌드 결과물
- `Release\d3d9.dll` (~810 KB, x64 네이티브 바이너리)
- `Release\d3d9.ini`

---

## 8. 라이선스 및 크레딧

본 프로젝트는 zlib 라이선스 하에 배포됩니다.

- **DxWrapper Base Engine**: Copyright (C) 2025 Elisha Riedlinger
- **x64 Porting & Low-Latency Optimization Layer**: Copyright (C) 2026 KaitouJoker
- **Third-Party Libraries**:
  - `d3d8to9` by Patrick Mours
  - `Hooking.Patterns` by ThirteenAG
  - `Detours` by Microsoft
