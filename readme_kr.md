# DxWrapper (Low-Latency & Modern Windows Optimization Edition)

[English](README.md) | [한국어 (Korean)](readme_kr.md)

DirectX 9 및 구형 DirectX 기반 게임을 최신 Windows 10/11 환경에서 **초저지연(Low-Latency)**, **완벽한 프레임 페이싱(Rock-solid Frame Pacing)**, **OS 스케줄러 지터 원천 제거(Zero Scheduling Jitter)**, 그리고 **1,500 ~ 1,700+ FPS 초고프레임 렌더링 파이프라인**으로 구동하기 위해 특화 개발된 고성능 DirectX 래퍼 라이브러리입니다.

특히 **카트라이더(KartRider)** 와 같은 고주사율(144Hz, 240Hz, 360Hz, 540Hz+) 타임어택/경쟁전 환경에서 발생하는 **주기적 프레임 스파이크(0.8ms → 1.6ms 튐 현상)를 원천 차단** 하고, 구형 엔진 내부에 하드코딩된 **1,000 FPS 프레임 게이트 제약을 완전히 우회** 하여 최신 멀티코어 시스템(예: AMD Ryzen 7 7800X3D + NVIDIA GeForce RTX 50 시리즈)에서 **1,500 ~ 1,700+ FPS**(0.58 ~ 0.62ms 프레임 타임) 및 초저 디스플레이 지연 시간(**0.75 ~ 0.85ms Raw Latency**)을 상시 유지합니다. 또한 Windows 10/11의 최신 디스플레이 기술인 **`Hardware: Independent Flip`**(오버레이 지원 Direct Flip)과 순수 독점 전체화면 **`Hardware: Legacy Flip`**(DWM 완전 우회)을 완벽하게 제어할 수 있습니다.

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

아래 영상은 기존 레거시 런타임(`d3dx9_27.dll`)과 최적화된 DxWrapper(`d3d9.dll`) 환경에서 카트라이더 실행 시의 프레임 타임 안정성, 인풋 반응성 및 주기적 스파이크(0.8ms → 1.6ms) 튐 현상 제거 효과를 직접 비교한 녹화 영상입니다.

### 기존 레거시 런타임 (`d3dx9_27.dll`)
> 레거시 프레젠테이션 환경에서 주기적인 프레임 타임 스파이크(0.8ms → 1.6ms) 및 OS 스케줄러 전환에 따른 미세 버벅임 발생.

https://github.com/user-attachments/assets/2791662b-7665-4540-9d48-99ca38de2f33

### 최적화된 DxWrapper (`d3d9.dll`)
> 엔진 1,000 FPS 프레임 게이트 해제, OS 동적 Sleep 가로채기, MMCSS 실시간 렌더 스레드 승격, DWM 우회를 통해 지터 없이 약 0.6ms ~ 0.8ms (1,500 ~ 1,700+ FPS)로 완벽하게 평탄화된 프레임 페이싱 유지.

https://github.com/user-attachments/assets/bb70f855-992b-476a-8d16-837a6ae4bd1e

---

## 2. 설치 및 INI 파일 로딩 구조

### 배포 및 설치
1. **기존 레거시 DLL 삭제**: 게임 설치 폴더에 기존 `d3dx9_27.dll` 파일이 존재할 경우 **반드시 삭제**해 주십시오. 구버전 또는 비정상적인 `d3dx9_27.dll`이 게임 폴더에 남아있으면 DirectX 런타임 로딩 간섭으로 인해 실행 오류나 렌더링 충돌이 발생할 수 있습니다.
2. 빌드된 `d3d9.dll`과 설정 파일(`d3d9.ini`)을 게임 실행 파일(예: `KartRider.exe`)이 위치한 폴더에 복사합니다.
3. 게임 실행 시 DLL이 프로세스에 자동으로 로드되며 Direct3D 호출을 가로채 하드웨어 파이프라인을 가속합니다.
4. **Zero-Config 독립 실행**: `d3d9.ini` 설정 파일이 없어도 DLL 내부에 e스포츠 경쟁전 최적화 기본값(`FlipEx=1`, `DisableDynamicSleep=1`, `BoostRenderThread=1`, `MaxFrameLatency=1`, `DisablePowerThrottling=1`, `DisableLogging=1`, `D3d9to9Ex=0`, `UnlockFrameRate=1`, `RenderThreadAffinity=4`)이 하드코딩되어 있어 설정 파일 없이도 동일하게 최대 성능으로 작동합니다.

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

#### `DisableMaxWindowedMode`
- **값**: `0` (Independent Flip) | `1` (Legacy Flip)
- **동작 원리**: Windows 10/11의 **전체 화면 최적화 (Fullscreen Optimizations, FSO)** 활성화 여부를 직접 제어합니다.
  - **`0` (권장: 방송 / 영상 녹화 / 단일 모니터 오버레이 사용 시)**:
    - FSO를 활성화하여 Windows Desktop Window Manager(DWM)의 다이렉트 플립 파이프라인을 탑니다.
    - 프레젠테이션 모드: **`Hardware: Independent Flip`**
    - 지연 시간: Raw 디스플레이 레이턴시 **0.75 ~ 0.85 ms** (1,500 ~ 1,650 FPS)
    - 특징: CapFrameX, PresentMon, Xbox Game Bar, Discord 등 **인게임 오버레이가 게임 화면 위에 실시간 렌더링**됩니다. Alt-Tab 전환 시 화면 깜빡임 없이 즉시 전환됩니다.
  - **`1` (권장: 극한의 타임어택 / 대회용 극단적 저지연 필요 시)**:
    - FSO를 강제 차단하고 DWM을 완전히 우회하는 순수 하드웨어 스캔아웃 모드로 진입합니다.
    - 프레젠테이션 모드: **`Hardware: Legacy Flip`**
    - 지연 시간: Raw 디스플레이 레이턴시 **0.70 ~ 0.80 ms** (1,550 ~ 1,700+ FPS)
    - 특징: DWM 토큰 핸드셰이크가 생략되어 프레임 타임이 약 0.05 ~ 0.1ms 추가 단축됩니다. 단, 데스크톱 오버레이가 게임 화면 위에 그려지지 않으므로 별도 보조 모니터에서 오버레이를 확인해야 합니다.

#### `EnableWindowMode` / `FullscreenWindowMode` / `WindowModeBorder`
- **`EnableWindowMode`**: `0` (게임 원본 전체화면 유지) | `1` (강제 창모드 변환)
- **`FullscreenWindowMode`**: `0` (비활성화) | `1` (테두리 없는 전체 창모드)
- **`WindowModeBorder`**: `0` (창 테두리 숨김) | `1` (창 테두리 표시)
- **동작 원리**: 카트라이더처럼 인게임 F11 키를 통해 자체적으로 전체화면/창모드를 전환하는 게임의 경우, 세 옵션을 모두 `0`으로 두어야 작업 표시줄 간섭 없이 원본과 동일하게 꽉 찬 전체화면이 구현됩니다.

#### `FlipEx`
- **값**: `0` (비활성화) | `1` (활성화, 기본값)
- **동작 원리**: Direct3D 9Ex 환경에서 창모드 스왑체인을 위한 `D3DSWAPEFFECT_FLIPEX` 프레젠테이션 모델을 활성화합니다. 백버퍼 개수(`>= 2`) 검증, 16비트 서피스의 32비트 `D3DFMT_X8R8G8B8` 정규화, D3D9Ex 규격에 맞춘 멀티샘플링 플래그 보정을 자동으로 수행합니다.

---

## 4. 추가 설정 - 성능 및 초저지연 (Low-Latency & Performance Optimizations)

간헐적인 프레임 스파이크를 없애고 엔진 내장 1,000 FPS 제약을 돌파하며, CPU/GPU 하드웨어 자원을 상시 100% 렌더링에 집중시키기 위해 독자 구현된 고성능 레이턴시 제어 기능입니다.

#### `UnlockFrameRate`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - 구형 게임 엔진은 메인 업데이트 루프(`App::RunFrame` at `0x0102fba6..0x0102fbc0`) 내부에서 1ms 단위 프레임 게이트를 두어, 동일한 정수 밀리초 내에 렌더링되는 후속 프레임을 스킵합니다(`nowTick == lastFrameTick` -> `jmp 0x1030066`). 이로 인해 엔진 자체적으로 **정확히 1,000 FPS 상한선**이 강제됩니다.
  - 기존 방식처럼 `.text` 바이너리 코드를 직접 패치(조건부 점프 NOP 처리 등)하면 클라이언트의 30초 무결성 체크섬 루틴(`0x008a9350`)에 감지되어 접속이 강제 종료됩니다.
  - **방법 2 가상 프레임 게이트 우회 (Virtual Gate Reset)**:
    - 게임의 `.text` 코드 영역을 전혀 수정하지 않습니다 (0바이트 변조, 100% 안티치트/무결성 안전).
    - 프레임 프레젠테이션 직후(`ApplyPostPresentFixes`), DxWrapper가 동적 런타임 변수인 `lastFrameTick` (`0x013f23a8`)을 `0xFFFFFFFF`로 리셋합니다.
    - 엔진의 다음 프레임 검사 시 `nowTick != 0xFFFFFFFF`가 항상 보장되어, 하드웨어가 낼 수 있는 최대 성능으로 프레임을 무제한 렌더링합니다.
    - **레버 C 무오버헤드 인라인 리셋 (Zero-Overhead Inline Reset)**: 프로그램 기동 시 단 1회 SEH 메모리 쓰기 안전성 검사(`TestWriteDword`)를 수행한 후, 매 프레임 단일 사이클 직접 포인터 저장(`*s_pLastFrameTick = 0xFFFFFFFF;`, 단 한 줄의 `mov [eax], 0xFFFFFFFF` 어셈블리로 컴파일)을 수행합니다. 프레임당 약 15개의 CPU 명령어를 절약하여 **1,500 ~ 1,700+ FPS** 성능을 달성합니다.
    - 물리 엔진 델타 타임 안전성: 물리 누적 루프가 제로 델타 프레임(`dt_ms == 0`)을 나눗셈 오류 없이 안전하게 스킵하므로, 물리 틱 꼬임 없이 순수 그래픽 보간 프레임만 극한으로 향상됩니다.

#### `RenderThreadAffinity`
- **값**: `0` (자동 / OS 스케줄링) 또는 코어 비트마스크 (예: `4` = Core 1)
- **동작 원리**:
  - Direct3D `Present`를 수행하는 메인 렌더링 스레드를 특정 CPU 물리 코어에 영구 고정합니다.
  - **AMD Ryzen 7 7800X3D 등 8코어 권장값**: **`4` (0x0004 -> Core 1 / CPU 2)**.
  - Windows는 기본적으로 OS 타이머 틱, DPC, 네트워크 패킷 인터럽트, 마우스/키보드 USB 폴링을 Core 0 (CPU 0/1)에 집중 할당합니다. 렌더 스레드를 Core 1(AMD 공장 출고 최고 수율 골드 스타 ★ 코어)에 격리 배치함으로써 시스템 인터럽트 간섭을 완전히 배제하고 즉각적인 **+30 FPS 향상**(1,570 → 1,600+ FPS)과 디스플레이 지연 시간 **0.75 ~ 0.85 ms 평탄화**를 달성합니다.

#### `DisableDynamicSleep`
- **값**: `1` (활성화, 강력 권장) | `0` (비활성화)
- **동작 원리**:
  - 구형 게임 엔진은 CPU 점유율 절약이나 프레임 안정화를 위해 메인 루프 내부에서 `Sleep(0)` 또는 `Sleep(1)`을 주기적으로 호출합니다.
  - Windows 커널 타이머 양자화 한계로 인해 `Sleep(1)`은 실제 1.0ms ~ 2.0ms 동안 스레드를 재우며, 이것이 **0.8ms로 유지되던 프레임 타임이 간헐적으로 1.6ms까지 튀는 주기적 스파이크의 주원인**입니다.
  - `DisableDynamicSleep = 1` 활성화 시:
    - `Sleep(0)` 호출을 CPU 친화적인 `YieldProcessor()` (`_mm_pause()`)로 치환하여 스레드가 남은 타임 퀀텀을 반납하지 않도록 방지합니다.
    - 마이크로 슬립(<= 2ms)을 커널 대기 없는 **QPC(QueryPerformanceCounter) 초정밀 마이크로 스핀 루프**로 대체하여 스레드 슬립을 원천 차단합니다.
    - WinSock2 비동기 오버랩 I/O 완료 콜백을 수신하는 얼러터블 슬립(`SleepEx(0, TRUE)`)은 정상 보존하여 30초 네트워크 연결 끊김 현상을 완벽히 방지합니다.

#### `BoostRenderThread`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - 프레임을 제출하는 렌더링 스레드의 우선순위를 `THREAD_PRIORITY_TIME_CRITICAL` (Level 15)로 승격합니다.
  - `SetThreadPriorityBoost(hThread, FALSE)`를 통해 OS 스케줄러의 우선순위 감쇠(Decay)를 방지합니다.
  - Windows **MMCSS (Multimedia Class Scheduler Service)**에 스레드를 `'Games'` 프로필(`AvSetMmThreadCharacteristicsA`)로 등록하여 백그라운드 서비스에 의해 렌더링 파이프라인이 선점(Preempt)되는 현상을 방지합니다.

#### `DisablePowerThrottling`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - Windows 10/11의 **전원 스로틀링(Power Throttling / EcoQoS / 효율 모드)**을 프로세스 레벨(`SetProcessInformation`)에서 영구 차단합니다.
  - Intel 12세대 이상 하이브리드 아키텍처 및 AMD CPU에서 메인 렌더링 스레드가 고효율 코어(E-Core)로 강등되거나 클럭이 다운되는 것을 방지하고 P-Core 최고 부스트 클럭을 유지합니다.

#### `MaxFrameLatency`
- **값**: `1` (최저 지연, 기본값) ~ `3`
- **동작 원리**:
  - 드라이버가 GPU 백버퍼 큐에 미리 대기시킬 수 있는 최대 프레임 수를 제한합니다.
  - `1`로 설정 시 현재 프레임의 GPU 처리가 완료될 때까지 CPU가 다음 프레임을 준비하지 않도록 동기화하여 키보드/마우스 입력이 화면에 반영되는 **인풋-투-포톤(Input-to-Photon) 지연 시간을 최소화**합니다.
  - *참고*: Direct3D의 `SetMaximumFrameLatency` API는 Direct3D 9Ex 환경에서만 직접 호출됩니다. D3D9 표준 모드(`D3d9to9Ex = 0`)에서는 그래픽 드라이버 제어판(예: NVIDIA 저지연 모드 'Ultra' / AMD Anti-Lag) 설정을 통해 GPU 큐 대기 프레임이 제어됩니다.

#### `SpinWaitPacing`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - 프레젠테이션 전 하드웨어 이벤트 쿼리(`D3DQUERYTYPE_EVENT`)를 발행합니다.
  - GPU가 CPU보다 살짝 뒤처지는 경우, 드라이버가 OS 커널 대기 상태(`WaitForSingleObject`)로 빠지는 대신 최대 200마이크로초 동안 `YieldProcessor()` 나노초 스핀-폴링으로 대기하여 균일하고 지터 없는 프레임 페이싱을 유지합니다.

#### `BypassFpuReset`
- **값**: `1` (활성화, 기본값) | `0` (비활성화)
- **동작 원리**:
  - 매 프레임 프레젠테이션 시 Direct3D 9가 수행하는 불필요한 x87 FPU 제어 단어(Control Word) 초기화 오버헤드를 건너뛰어 프레젠테이션 경로의 CPU 사이클을 절약합니다.

---

## 5. 부가 설정 (Supplementary Settings)

### 디스플레이 및 그래픽 품질 제어
- **`EnableVSync = 0`**: 수직동기화를 해제하고 즉시 화면에 프레임을 송출합니다(`D3DPRESENT_INTERVAL_IMMEDIATE`).
- **`LimitPerFrameFPS = 0`**: 래퍼 레벨 FPS 제한 해제 (무제한 렌더링).
- **`OverrideRefreshRate = 0`**: 모니터의 기본 주사율(예: 240Hz, 360Hz, 540Hz)을 그대로 사용. 특정 주사율 강제 시 해당 Hz 값 입력.
- **`AntiAliasing = 0` / `AnisotropicFiltering = 0`**: 게임 내 자체 그래픽 설정을 따르도록 `0`으로 유지.

### 시스템 및 진단 제어
- **`DisableLogging = 1`**: 디버그 로그 파일 생성을 비활성화하여 디스크 I/O 오버헤드를 완전히 제거합니다. (문제 분석 시 `0`으로 변경하면 `d3d9-[프로세스명].log` 파일이 생성됩니다.)
- **`RealDllPath = AUTO`**: 시스템 정식 `SysWOW64\d3d9.dll`을 자동으로 탐색하여 로드합니다.
- **`FixPerfCounterUptime = 0`**: 장시간 PC 부팅 시 카운터 오버플로 패치 (필요 시 `1`).

---

## 6. 용도별 권장 프리셋 (Recommended Profiles)

1,000 FPS 프레임 게이트 해제, 전용 코어 격리, Sleep 마이크로스핀, MMCSS 우선순위 부스트, 전원 스로틀링 해제 등 모든 핵심 최적화는 두 프로필에 공통으로 적용됩니다. **두 프로필 간의 유일한 차이는 `DisableMaxWindowedMode` 값 하나뿐**입니다.

### 프로필 차이점 비교표

| 비교 항목 | 프로필 1 (방송 / 영상 녹화 / 오버레이) | 프로필 2 (극한의 타임어택 / 대회용) |
| :--- | :---: | :---: |
| **핵심 변경 옵션** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **프레젠테이션 모드** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **평균 프레임 레이트** | **1,500 ~ 1,650 FPS** | **1,550 ~ 1,700+ FPS** |
| **디스플레이 지연 시간 (Raw)** | **0.75 ~ 0.85 ms** | **0.70 ~ 0.80 ms** |
| **단일 모니터 오버레이 표출** | 지원 (게임 화면 위에 실시간 렌더링) | 미지원 (보조 모니터 또는 별도 창 확인) |
| **Alt-Tab 전환 반응** | 즉시 전환 (화면 깜빡임 없음) | 전체화면 깜빡임 발생 |
| **권장 대상** | 방송 스트리머, 유튜브 녹화, 일반 플레이어 | 기록 갱신용 타임어택, 대회 출전 선수 |

---

### 통합 최적화 설정 (`d3d9.ini`)

아래 설정을 기본 베이스로 사용하며, 목적에 맞춰 **`DisableMaxWindowedMode`** 값만 `0` 또는 `1`로 전환하십시오:

```ini
[Compatibility]
D3d9to9Ex                  = 0          ;; 0 유지: D3DPOOL_MANAGED 화면 백화 현상 방지
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
RenderThreadAffinity       = 4          ;; Core 1 격리 (AMD 7800X3D 골든 코어, Core 0 DPC/인터럽트 회피)
UnlockFrameRate            = 1          ;; 엔진 1,000 FPS 프레임 게이트 해제 (방법 2 + 레버 C)
LimitPerFrameFPS           = 0          ;; 0 = 무제한 최대 프레임
DisableLogging             = 1

;; ======================================================================
;; [핵심 프레젠테이션 전환 옵션]
;;   DisableMaxWindowedMode = 0  -> 프로필 1 (Independent Flip, 오버레이 지원, ~0.80ms, 1500~1650 FPS)
;;   DisableMaxWindowedMode = 1  -> 프로필 2 (Legacy Flip, 순수 스캔아웃, ~0.75ms, 1550~1700 FPS)
;; ======================================================================
DisableMaxWindowedMode     = 0
```

133가지 전체 고급 설정 항목은 [Configuration (advanced).md](Configuration%20(advanced).md) 문서를 참고하십시오.

---

## 7. 개발자 빌드 가이드 (Developer Build Guide)

본 프로젝트는 MSVC v145 (Visual Studio 2026 / 2022) x86 환경에서 불필요한 레거시 서브시스템(DirectDraw, DirectSound, DirectInput, Direct3D 8)을 제거하고 약 657KB 대의 초경량 단일 바이너리로 컴파일되도록 최적화되어 있습니다.

### 원클릭 빌드
프로젝트 루트 디렉토리에서 배치 스크립트를 실행합니다:
```cmd
build_d3d9.bat
```

자동화 CI/CD 환경에서 사용자 프롬프트 대기 없이 실행하려면:
```cmd
build_d3d9.bat nopause
```

### 산출물 위치
- `Release\d3d9.dll` (약 657 KB)
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
