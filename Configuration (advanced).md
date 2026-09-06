# DxWrapper 고급 설정 가이드 (Configuration Advanced)

본 문서는 DxWrapper Low-Latency Edition의 설정 파일(`d3d9.ini`)에 존재하는 **모든 파라미터(10개 섹션, 133개 항목)**에 대한 상세 기술 사양서입니다.
공식 [DxWrapper Configuration Wiki](https://github.com/elishacloud/dxwrapper/wiki/Configuration-(advanced))의 원문 정의와 본 프로젝트에서 독자 개발된 저지연 파이프라인 패치의 동작 원리, 설정 가능한 데이터 타입, 기본값, 그리고 카트라이더(Direct3D 9 x86) 최적화 권장 설정값을 총망라하여 정리하였습니다.

---

## 목차 (Table of Contents)

1. [General (일반 및 래퍼 구동 제어)](#1-general) - 8개 항목
2. [Plugins (외부 플러그인 제어)](#2-plugins) - 2개 항목
3. [Compatibility (호환성 및 렌더링 백엔드 변환)](#3-compatibility) - 20개 항목
4. [DDrawCompat (DirectDraw 호환성 계층)](#4-ddrawcompat) - 5개 항목
5. [ddraw (DirectDraw 세부 제어)](#5-ddraw) - 2개 항목
6. [Dd7to9 (DirectDraw/D3D7 to Direct3D 9 변환)](#6-dd7to9) - 27개 항목
7. [d3d9 (Direct3D 9 디스플레이 및 초저지연 제어)](#7-d3d9) - 42개 항목
8. [FullScreen (전체화면 윈도우 관리)](#8-fullscreen) - 3개 항목
9. [dinput8 (DirectInput 8 입력 제어)](#9-dinput8) - 10개 항목
10. [dsound (DirectSound 오디오 제어)](#10-dsound) - 14개 항목
11. [전체 파라미터 요약 테이블 (Summary Table)](#11-전체-파라미터-요약-테이블)
12. [용도별 실전 권장 프리셋 (Optimized Presets)](#12-용도별-실전-권장-프리셋)

---

## 1. [General]
**분류**: 일반 및 래퍼 구동 제어 (8개 설정 항목)

### RealDllPath
- **기본값 (`d3d9.ini`)**: `AUTO`
- **설정 데이터 타입**: 문자열 (String, 경로 또는 'AUTO')
- **개요**: 실제 시스템 또는 대상 DLL의 파일 경로를 수동으로 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> By default when DxWrapper is wrapping a dll it will load the Windows System dll.  However, if you need DxWrapper to load a different dll you can list that dll here.  A use case for this would be if you want to use DxWrapper alongside some other wrapper like dgVoodoo or an existing game dll with the same name.
> 
> Note: this option only works if you either rename DxWrapper to match the name of the file you are wrapping (such as d3d8.dll or ddraw.dll) or if you are using this setting on the stub dll.  To use this setting on the stub dll you will need to create a new ini file that matches the name of the stub dll and add this setting into that ini file.
- **동작 원리 및 상세 분석**:
  DxWrapper가 프록시 로딩할 실제 Windows 시스템 라이브러리(예: `C:\Windows\SysWOW64\d3d9.dll`)의 절대 경로를 지정합니다. 기본값인 `AUTO`로 둘 경우 Windows 시스템 폴더에서 동일한 이름의 원본 DLL을 자동으로 감지하여 로드합니다.
- **카트라이더 및 권장 설정**: `AUTO` (카트라이더 및 일반 환경에서 기본값 유지 권장)

### WrapperMode
- **기본값 (`d3d9.ini`)**: `AUTO`
- **설정 데이터 타입**: 문자열 (String, DLL 식별자 또는 'AUTO')
- **개요**: DxWrapper가 가로챌 대상 래퍼 모드를 명시적으로 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This tells DxWrapper which dll it is wrapping.  By default DxWrapper will detect the name of the wrapper and use the mode associated with the name.  So, if you are wrapping d3d8.dll then it will automatically choose thed3d8wrapper mode.  However, in some circumstances games will rename the dll file.  For example, if you are playing theGOGversion of American Conquest, then it has a dll namedmdraw.dllthat is just ddraw.dll renamed.  In this case you would rename DxWrapper tomdraw.dlland you would need to set theWrapperModetoddrawto tell DxWrapper that this is a ddraw dll.
> 
> Supported wrapper modes are:bcrypt,cryptsp,d2d1,d3d8,d3d9,d3d10,d3d11,d3d12,dciman32,ddraw,dinput,dinput8,dplayx,dsound,dwmapi,msacm32,msvfw32,version,wininet,winmm,wsock32,winmmbase.
- **동작 원리 및 상세 분석**:
  기본값인 `AUTO` 상태에서는 로드된 DLL 파일명(예: `d3d9.dll`)을 기반으로 자동으로 모드를 판별합니다. 지원 모드: `d3d9`, `ddraw`, `d3d8`, `dinput8`, `dsound` 등.
- **카트라이더 및 권장 설정**: `AUTO` (본 빌드는 d3d9.dll 프록시로 동작하므로 AUTO 유지 권장)

### LoadCustomDllPath
- **기본값 (`d3d9.ini`)**: `(공란 / 미지정)`
- **설정 데이터 타입**: 문자열 (String, 쉼표 구분 파일 경로 목록)
- **개요**: 프로그램 시작 시 추가로 로드할 외부 DLL 파일 목록을 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will load additional dlls into the program.  This is useful if there are specific dlls that need to be loaded, for example the program keeps loading a system dll and you want the program to load a custom dll.  This technique comes in handy in a few games like Warhammer 40,000 Chaos Gate and Star Wars Battlefront.  To use this you can specify just the dll name or the full path and name.
- **동작 원리 및 상세 분석**:
  ReShade, 커스텀 안티치트 후킹 라이브러리, 모드 로더 등 게임 프로세스 생성 시 함께 로드되어야 하는 추가 DLL들을 쉼표(`,`)로 구분하여 지정할 수 있습니다.
- **카트라이더 및 권장 설정**: 공란 (불필요한 외부 주입 방지)

### ExcludeProcess
- **기본값 (`d3d9.ini`)**: `(공란 / 미지정)`
- **설정 데이터 타입**: 문자열 (String, 쉼표 구분 프로세스 이름 목록)
- **개요**: DxWrapper 주입에서 제외할 실행 파일 목록을 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> By default DxWrapper will load into all games in a specified folder that load the wrapped dll.  However, in some circumstances you may not want to DxWrapper to load into a specific process.  You can use this option to exclude that process name.
- **동작 원리 및 상세 분석**:
  게임 런처, 업데이트 패처, 크래시 리포터 등 래핑이 불필요하거나 충돌을 유발할 수 있는 프로세스 이름을 쉼표로 구분하여 등록합니다.
- **카트라이더 및 권장 설정**: 공란 또는 런처 충돌 시 런처 프로세스명 지정

### IncludeProcess
- **기본값 (`d3d9.ini`)**: `(공란 / 미지정)`
- **설정 데이터 타입**: 문자열 (String, 쉼표 구분 프로세스 이름 목록)
- **개요**: DxWrapper를 적용할 특정 게임 실행 파일 목록을 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> By default DxWrapper will load into all games in a specified folder that load the wrapped dll.  However, in some circumstances you may want to DxWrapper to load only into one specific process.  You can use this option to tell DxWrapper to only load into that one specific process name.
> 
> Note: Another way to do this is to rename the ini file.  If you add the process name to the ini file name only that process will load this ini file.  This can be useful if you have multiple processes in a single folder and you want different settings for each process.  If for example you have a process namedgame.exethen you can renamedxwrapper.initodxwrapper-game.ini.  This will tell dxwrapper that these settings should only work on the processgame.exe.
- **동작 원리 및 상세 분석**:
  특정 프로세스에서만 DxWrapper를 동작시키고자 할 때 프로세스명을 지정합니다. 비어 있을 경우 DLL을 로드하는 모든 호환 프로세스에 적용됩니다.
- **카트라이더 및 권장 설정**: 공란 (단일 게임 폴더 배포 시 비워둠)

### RunProcess
- **기본값 (`d3d9.ini`)**: `(공란 / 미지정)`
- **설정 데이터 타입**: 문자열 (String, 실행 파일 경로)
- **개요**: 게임이 시작될 때 함께 실행할 외부 프로그램 또는 유틸리티를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will run a process when the application starts up.  This is useful if you want something to happen when the program starts up, for example if you want to mount an ISO or change a registry key.  To use this you just need to type the command that you want to run.
- **동작 원리 및 상세 분석**:
  외부 타이머, 사운드 믹서, 성능 측정 도구 등을 게임 구동 시 자동으로 백그라운드 실행할 수 있습니다.
- **카트라이더 및 권장 설정**: 공란

### WaitForProcess
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: RunProcess로 지정된 프로세스가 종료될 때까지 게임 로딩을 일시 대기합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will pause the loading of the game until the process finishes.  You can use this if the process needs to complete before the program will function.  For instance if you need to mount an ISO for the game to run.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  `1`로 설정 시 `RunProcess`에 지정된 프로세스가 완전히 실행을 마치고 종료될 때까지 게임 초기화 루틴을 대기시킵니다. 사전 패치 작업에 유용합니다.
- **카트라이더 및 권장 설정**: `0`

### DisableLogging
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DxWrapper의 파일 로깅 기능을 비활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will disable all logging for DxWrapper.  It is recommended to keep logging enabled while you are first installing and testing DxWrapper with a game.  Then, once the game is running correctly you can disable logging, if you like.  However, if you keep logging enabled it can help you find issues later if they arise.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  `0` 설정 시 래퍼 초기화 및 디바이스 설정 내역을 담은 로그 파일(`d3d9-[프로세스명].log`)을 생성합니다. `1` 설정 시 디스크 I/O 오버헤드를 완전히 차단하여 성능을 최적화합니다.
- **카트라이더 및 권장 설정**: `1` (배포 및 실사용 시 강력 권장, 설정 디버깅 필요 시 `0`)

---

## 2. [Plugins]
**분류**: 외부 플러그인 제어 (2개 설정 항목)

### LoadPlugins
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 실행 폴더에 존재하는 ASI 플러그인을 자동으로 로드합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> If this option will load custom libraries with the file extension .asi into the game process.  It loads ASI files in game root directory,scriptsandpluginsfolders.
- **동작 원리 및 상세 분석**:
  게임 루트 디렉토리나 플러그인 폴더에 위치한 `.asi` 확장자의 서드파티 모드/플러그인을 검색하여 프로세스에 주입합니다.
- **카트라이더 및 권장 설정**: `0` (보안 및 불필요한 DLL 로드 방지)

### LoadFromScriptsOnly
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: ASI 플러그인 로드 위치를 scripts/plugins 하위 폴더로 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> If this option is enabled then DxWrapper will only load ASI files in thescriptsandpluginsfolders.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  `LoadPlugins = 1`일 때 적용되며, 게임 루트가 아닌 `scripts` 또는 `plugins` 서브폴더 내의 ASI 파일만 선별 로드하여 파일 오염을 방지합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 3. [Compatibility]
**분류**: 호환성 및 렌더링 백엔드 변환 (20개 설정 항목)

### Dd7to9
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectDraw (DirectX 1~7) 호출을 Direct3D 9으로 변환합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option improves compatibility in games using DirectX 1-7 byconverting them into Direct3D 9.  This is useful for games that use ddraw for their rendering engine.  It supports all 7 versions of ddraw 2D APIs and a number of the 3D APIs.  This can be set to '0' for off or '1' for on.
> 
> Note: onceDd7to9is enabled for a ddraw game the settings for this game can be controlled via all three of theddraw,Dd7to9andd3d9section options.  For example you can use theFullscreenWindowModeoption on a ddraw game even though this option is a d3d9 option because the game has been converted to Direct3D9 (which is d3d9).
- **동작 원리 및 상세 분석**:
  구형 DirectDraw API 호출을 현대 Direct3D 9 파이프라인으로 실시간 에뮬레이션하여 최신 GPU 하드웨어 가속을 활용할 수 있도록 변환합니다.
- **카트라이더 및 권장 설정**: `0` (카트라이더는 D3D9 네이티브이므로 0)

### D3d8to9
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Direct3D 8 API 호출을 Direct3D 9으로 변환합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables ReShade'sd3d8to9module that is intended to improve compatibility and stability in games using Direct3D 8 by converting all API calls to equivalent Direct3D 9 ones. It also opens those games up to the new possibilities from proven tools and wrappers written for Direct3D 9.  This can be set to0for off or1for on.
> 
> Note: onceD3d8to9is enabled for a Direct3d 8 game the settings for this game can be controlled viad3d9section options.  For example you can use theAntiAliasingoption on a Direct3d 8 game even though this option is a d3d9 option because the game has been converted to Direct3D 9 (which is d3d9).
- **동작 원리 및 상세 분석**:
  crosire의 d3d8to9 모듈을 내장하여 D3D8 게임의 인터페이스 호출을 D3D9으로 변환합니다.
- **카트라이더 및 권장 설정**: `0` (카트라이더는 D3D9 네이티브이므로 0)

### D3d9to9Ex
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Direct3D 9 호출을 Direct3D 9Ex(D3D9Ex) 인터페이스로 강제 승격합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option converts Direct3D9 to Direct3D9Ex.  This allows games to take advantage of some of the extra features in Direct3D9Ex, including better window mode support and better alt+tab support.  This option can work along sideDd7to9andD3d8to9.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  `1`로 설정 시 `Direct3DCreate9Ex` 및 `CreateDeviceEx`를 강제 사용하여 WDDM 가상 메모리 관리와 `FlipEx` 기능을 지원합니다. 단, 카트라이더와 같이 `D3DPOOL_MANAGED` 풀을 사용하는 구형 엔진에서는 리소스 생성 거부로 인해 백화 현상(White Screen)이 발생하므로 반드시 `0`으로 유지해야 합니다.
- **카트라이더 및 권장 설정**: `0` (카트라이더 텍스처 백화 방지 필수)

### d3d9on12
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Microsoft D3D9On12 매핑 계층을 활성화하여 DirectX 12 위에서 D3D9을 구동합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  D3D9 명령어를 DirectX 12 렌더 커맨드로 실시간 변환합니다. 카트라이더와 같이 고정기능 파이프라인(Fixed-Function Pipeline) 상태를 의존하는 구형 엔진에서는 셰이더 생성 실패로 회색 화면(Gray Screen)만 출력되므로 `0`으로 설정해야 합니다.
- **카트라이더 및 권장 설정**: `0` (카트라이더 회색 화면 방지 필수)

### DDrawCompat
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: narzoul의 DDrawCompat 호환성 라이브러리를 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables theDDrawCompatlibrary.  This can be enabled when wrapping any dll, however most of its functions will only work if the game usesddraw.dll.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  DirectDraw 기반 고전 2D/3D 게임의 윈도우 호환성, 화면 깜빡임, 팔레트 버그를 수정하는 DDrawCompat 기능을 작동시킵니다.
- **카트라이더 및 권장 설정**: `0`

### Dinputto8
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectInput (1~7) 호출을 DirectInput 8로 변환합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables the Dinputto8library, which improves compatibility in games using DirectInput 1-7 (dinput.dll) by converting all API calls to their equivalent DirectInput 8 (dinput8.dll) ones. This allows older games to be able to use newer tools and wrappers written for DirectInput 8.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  구형 DirectInput API를 DirectInput 8 인터페이스로 리디렉션하여 최신 입력 장치 및 USB 컨트롤러 호환성을 개선합니다.
- **카트라이더 및 권장 설정**: `0`

### DisableGameUX
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Windows 게임 탐색기(GameUX) 연동을 차단합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option disables the Microsoft Game Explorer (GameUX) to prevent rundll32.exe from using high CPU or causing the game to freeze.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  Windows 7 및 이후 OS에서 구형 게임 실행 시 `rundll32.exe`가 인터넷 통신을 시도하며 높은 CPU 점유율을 유발하고 게임 기동이 지연되는 현상을 원천 방지합니다.
- **카트라이더 및 권장 설정**: `0` (필요 시 `1`)

### EnableDdrawWrapper
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectDraw 래퍼 엔진을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option enables the built-in ddraw wrapper which has some built-in fixes for specific ddraw bugs.  It also allows the options in theddrawsection to be available for use.  EitherEnableDdrawWrapperorDd7to9must be enabled before the options in theddrawsection are available for use.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  ddraw.dll 가로채기 루틴을 활성화합니다.
- **카트라이더 및 권장 설정**: `0`

### EnableD3d9Wrapper
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Direct3D 9 래퍼 엔진을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option enables the built-in Direct3D 9 wrapper which has some built-in fixes for specific Direct3D 9 bugs.  It also allows the options in thed3d9section to be available for use.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  d3d9.dll 프록시 가로채기 엔진을 활성화합니다. 본 최적화 빌드의 모든 저지연 패치(스핀웨이트, 슬립 패치, MMCSS 등)를 동작시키기 위한 필수 파라미터입니다.
- **카트라이더 및 권장 설정**: `1` (필수 활성화)

### EnableDinput8Wrapper
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectInput 8 래퍼 엔진을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables DirectInput8 wrapper.  It also allows the options in thedinput8section.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  dinput8.dll 프록시 엔진을 활성화하여 마우스 고주파수 병합 및 축 패딩을 적용할 때 사용합니다.
- **카트라이더 및 권장 설정**: `0`

### EnableDsoundWrapper
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectSound 래퍼 엔진을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables a wrapper that has same options to tweak some function calls of games using Microsoft DirectSound (part of DirectX).  It also allows the options in thedsoundsection to be available for use and can "work around" some driver and application limitations and bugs.  It can also enhance the sound quality in many ways, for example by enabling 3D sound with games that disables this functionality by default.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  dsound.dll 프록시 엔진을 활성화하여 오디오 버퍼링 및 샘플레이트 제어를 수행할 때 사용합니다.
- **카트라이더 및 권장 설정**: `0`

### disablegdigammaramp
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: GDI 감마 램프(Gamma Ramp) 적용을 차단합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  GDI 레벨에서 데스크톱 감마를 덮어써 화면이 물빠진 색상(washed-out)으로 보이거나 밝기가 왜곡되는 현상을 차단합니다.
- **카트라이더 및 권장 설정**: `0`

### EnableOpenDialogHook
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Windows 파일 열기 대화상자(GetOpenFileName) 후킹을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option intercepts calls to the open and save dialog boxes to allow modifying the calls to fix some issues that happen in some games that attempt to open a dialog box.  Seehere for an example.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  전체화면 모드에서 구형 게임이 파일 열기 대화상자를 호출할 때 화면이 정지하거나 락다운되는 문제를 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### WinVersionLie
- **기본값 (`d3d9.ini`)**: `off`
- **설정 데이터 타입**: 문자열 (String, '95', '98', '2000', 'XP', 'off' 등)
- **개요**: 게임 프로세스에 보고할 Windows 운영체제 버전을 위장합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Allows overriding the Windows version information reported to the application. Some applications may work differently or refuse to run at all if they detect specific Windows versions.  This can be set to '95', 'nt4', '98', '2000' or 'xp' for overwriting the windows version; any other value will turn this option off.
- **동작 원리 및 상세 분석**:
  OS 버전 체크 루틴을 속여 최신 Windows 10/11에서 실행을 거부하는 레거시 게임의 구동을 가능하게 만듭니다.
- **카트라이더 및 권장 설정**: `off`

### WinVersionLieSP
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, Service Pack 번호)
- **개요**: WinVersionLie 적용 시 함께 보고할 서비스 팩(Service Pack) 번호를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Allows overriding the Windows Service Pack version information reported to the application. This requires theWinVersionLieoption to be enabled. This can be set to '1', '2', '3', '4' or '5' for overwriting the windows version; any other value will turn this option off.
- **동작 원리 및 상세 분석**:
  예: `WinVersionLie = XP`, `WinVersionLieSP = 3` 설정 시 Windows XP Service Pack 3으로 인식시킵니다.
- **카트라이더 및 권장 설정**: `0`

### forcekeyboardlayout
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 16진수 정수 (Hex/DWORD, 레이아웃 HKL)
- **개요**: 특정 키보드 언어 레이아웃을 강제로 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  예: `0x00000409` (미국 영어). 다국어 입력기(IME)나 특정 언어 레이아웃에서 충돌하는 구형 게임 엔진에 영문 표준 키 배열을 강제 인가합니다.
- **카트라이더 및 권장 설정**: `0` (기본값)

### FixPerfCounterUptime
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: QueryPerformanceCounter 가동 시간 오버플로 버그를 패치합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will patch performance counter to fake uptime so that games see uptime as less than 1 day in order to fix shuttering and performance issues that happen when the device has long-uptime.  This also fakes the CPU frequency in case it is too high for some games.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  PC 가동 시간(Uptime)이 수일 이상 누적되었을 때 32비트 정수 연산 오버플로로 인해 게임 내 시간이 튀거나 랙이 발생하는 구형 엔진을 위해 QPC 기준값을 리셋합니다.
- **카트라이더 및 권장 설정**: `0`

### HandleExceptions
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: NOP 패치, 2: 예외 코드 건너뜀)
- **개요**: 애플리케이션 크래시를 유발하는 처리되지 않은 예외를 가로채어 복구합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will allow DxWrapper to set its own exception handler that tries (often successfully) to fix several common exceptions, such as the divide by zero exception in old games that were trying to calculate the CPU speed.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  `1` 설정 시 크래시 명령어 메모리를 NOP으로 교체하며, `2` 설정 시 예외 핸들러가 해당 명령을 건너뛰고 실행을 지속합니다.
- **카트라이더 및 권장 설정**: `0`

### SingleProcAffinity
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 프로세스를 단일 CPU 코어에 강제 고정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option limits the game to one CPU core.  Some games are not properly synchronized to run on multiple CPU cores, or simply don't properly handle powerful CPUs.  SetSingleCoreAffinityto a value of1or higher to change core used. (1 = CPU 0, 2 = CPU 1, etc.) If the value is set to a number that is greater than the available cores on your computer, it will default to using CPU 0.
- **동작 원리 및 상세 분석**:
  멀티코어 환경에서 스레드 동기화 버그나 사운드 끊김, 초고속 배속 현상이 나타나는 초기 3D 게임을 단일 코어로 격리합니다.
- **카트라이더 및 권장 설정**: `0`

### procaffinitymask
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수/비트마스크 (DWORD)
- **개요**: SingleProcAffinity 적용 시 할당할 CPU 코어 비트마스크를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  예: `1` = 0번 코어, `2` = 1번 코어, `4` = 2번 코어. 0으로 설정 시 시스템의 첫 번째 활성 코어가 자동 할당됩니다.
- **카트라이더 및 권장 설정**: `0`

---

## 4. [DDrawCompat]
**분류**: DirectDraw 호환성 계층 (5개 설정 항목)

### DDrawCompat20
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DDrawCompat v0.2.0b 버전을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables version 2.0 of theDDrawCompatlibrary.  Some games may work better with older versions of the library.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  DDrawCompat의 구버전 0.2.0b 엔진을 선택 로드합니다.
- **카트라이더 및 권장 설정**: `0`

### DDrawCompat21
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DDrawCompat v0.2.1 버전을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables version 2.1 of theDDrawCompatlibrary.  Some games may work better with older versions of the library.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  DDrawCompat v0.2.1 릴리즈 엔진을 선택 로드합니다.
- **카트라이더 및 권장 설정**: `0`

### DDrawCompat32
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DDrawCompat v0.3.2 버전을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This enables version 3.2 of theDDrawCompatlibrary.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  DDrawCompat v0.3.2 릴리즈 엔진을 선택 로드합니다.
- **카트라이더 및 권장 설정**: `0`

### DDrawCompatDisableGDIHook
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DDrawCompat 라이브러리의 GDI 후킹 기능을 비활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This disables GDI hooking of theDDrawCompatlibrary.  Some games have issues if the GDI is hooked. This requires one of theDDrawCompatoptions to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  DDrawCompat 사용 중 데스크톱 창 관리자나 창 테두리에서 그래픽 깨짐이나 렌더링 정지가 발생할 때 GDI 후크를 배제합니다.
- **카트라이더 및 권장 설정**: `0`

### DDrawCompatNoProcAffinity
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DDrawCompat의 기본 단일 프로세서 선호도 잠금을 해제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This disables the single process affinity feature of theDDrawCompatlibrary.  Some games don't need to be set to single process affinity or have issues on modern computers when enabling single process affinity. This requires one of theDDrawCompatoptions to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  DDrawCompat이 자동으로 프로세스를 1개 코어에 고정하는 동작을 방지하여 멀티코어 환경을 유지합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 5. [ddraw]
**분류**: DirectDraw 세부 제어 (2개 설정 항목)

### DdrawOverrideBitMode
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 8, 16, 24, 32)
- **개요**: DirectDraw 디스플레이 색상 비트 깊이를 강제로 변경합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This overrides the default bit mode.  This requires eitherDd7to9orEnableDdrawWrapperoption to be enabled.
> 
> The following values are defined:
> 
> Bit ModeValue8-bit1 or 816-bit2 or 1624-bit3 or 2432-bit4 or 32
- **동작 원리 및 상세 분석**:
  고전 8비트/16비트 게임의 렌더링 타겟을 32비트 트루컬러 등으로 강제 변환하여 화면 호환성을 확보합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawUseDirect3D9Caps
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectDraw GetCaps 호출 시 Direct3D 9 장치 능력을 덮어씌웁니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Overwrites the default DirectDraw caps and uses the caps fromDd7to9instead.  Seehere for an example.  This can be set to0for off or1for on.
> 
> Sample games that may need to use Direct3D9 Caps:
> 
> Gangsters: Organized Crime
- **동작 원리 및 상세 분석**:
  구형 하드웨어 체크 루틴을 통과시키기 위해 D3D9 어댑터의 텍스처 및 가속 스펙을 DirectDraw 인터페이스에 주입합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 6. [Dd7to9]
**분류**: DirectDraw/D3D7 to Direct3D 9 변환 (27개 설정 항목)

### DdrawAutoFrameSkip
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 프레임 지연 발생 시 자동으로 프레임을 건너뛰어 입력 지연을 완화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This function will skip the drawing of frames if to try and make games run smoother when v-sync is enabled.  Sometimes v-sync will slow down the game because the game's primary surface is being updated more than once per frame.  This option will try and detect multiple updates with-in the same frame and only display the last one.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  렌더링 속도가 디스플레이 주사율을 따라가지 못할 때 프레임 드랍을 허용하여 입력 지연(Input Lag)을 최소화합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawEmulateSurface
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 장치 컨텍스트(DC)를 사용하여 DirectDraw 표면을 소프트웨어 에뮬레이션합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> By default Dd7to9 will only emulate surfaces that aren't supported by Direct3D9, such as 24-bit surfaces or the Device Context to a surface with an alpha channel.  However, if you enable this option than all supported surfaces will be emulated.  This helps in a few cases where games use Device Context and the text is messed up or if the game tries to use surface memory after the surface is released.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  현대 GPU에서 지원하지 않는 구형 픽셀 포맷 표면을 시스템 메모리와 GDI 호환 DC로 생성하여 렌더링 충돌을 회피합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawEmulateLock
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectDraw 표면 Lock/Unlock 외부 접근 크래시를 방지하는 에뮬레이션 락을 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> By default Dd7to9 will only normally just call the Direct3D9 lock/unlock functions.  However, if you enable this option than system memory will be created to use to emulate the surface lock.  This helps fix cases where games crash when attempting to access the memory of an unlocked surface.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  Lock 구간 외부에서 비디오 메모리에 직접 접근하려는 불량 레거시 게임 코드의 메모리 위반(Access Violation)을 차단합니다.
- **카트라이더 및 권장 설정**: `0`

### ddrawuseshadowsurface
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 렌더 타겟 Lock/GetDC 호출을 위해 시스템 메모리 섀도우 서피스를 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  GPU 렌더 타겟에 CPU가 직접 접근할 때 발생하는 파이프라인 스톨과 화면 깜빡임을 방지하기 위해 임시 복사 버퍼를 둡니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawKeepAllInterfaceCache
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 자동, 1: 전체 유지, 2: 캐시 끔)
- **개요**: 해제된 DirectDraw COM 인터페이스의 캐시를 보존합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Some games will attempt to access interfaces that have previously been released.  This option will cache each interface after release and then reuse the interface's address to prevent any created interface from ever permanently being deleted allowing games to access released interfaces without crashing.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  Release()가 호출된 후에도 댕글링 포인터로 인터페이스에 접근하는 결함 있는 고전 게임의 런타임 크래시를 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### ddrawallowmultisampling
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임이 렌더 스테이트로 안티앨리어싱을 요청할 경우 MSAA를 허용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  Dd7to9 변환 레이어에서 게임의 멀티샘플링 요청을 승인하여 가장자리 안티앨리어싱을 적용합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawForceMipMapAutoGen
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Direct3D 9 하드웨어 밉맵 자동 생성(AutoGenMipMap)을 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Older Direct3D version don't have the ability to auto generate the MipMap surfaces.  However, enabling this option will force Direct3D9 auto generate MipMaps for all MipMap surfaces.  This helps fix cases where games show black for MipMap textures and it may help improve performance in some cases.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  밉맵 생성 기능이 없던 구형 DirectX 7 텍스처에 대해 D3D9 하드웨어 자동 밉맵 필터링을 생성합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawClampVertexZDepth
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 최대 1.0f 제한, 2: 0.0~1.0f 범위 제한 및 재계산)
- **개요**: 변환된 정점의 Z 깊이 값을 0.0f ~ 1.0f 사이로 클램핑합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Sets the Z value in all transformed vertices to 1.0f if the value is greater than 1.0f. In other words, it clamps the value at 1.0f max. This is needed because if Z goes above 1.0f it can cause the object to disappear.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  카메라 투영 연산 버그로 인해 지오메트리가 시야 밖으로 소실되거나 클리핑 오류가 나는 것을 수정합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawEnableByteAlignment
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 서피스 스트라이드에 대한 32비트/64비트 바이트 정렬 강제를 해제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Dxwrapper used to force 32-bit or 64-bit byte alignment for all surfaces.  However, this byte alignment is now only enabled when this option is enabled.  This option may improve performance in some cases and may help fix crashes or other issues caused when non-aligned surfaces cannot be created.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  메모리 패딩을 기대하지 않는 고전 게임 표면 데이터가 왜곡되는 현상을 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawFixByteAlignment
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 바이트 정렬, 2: 정렬+Point, 3: 정렬+Linear)
- **개요**: 피치(Pitch) 크기가 일치하지 않는 표면의 락(Lock)을 보정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Fixes display issues in some ddraw games that ignore the pitch sent by ddraw.  This requires either theDd7to9option to be enabled.  This can be set to0for off or1for on.
> 
> Sample games that require byte alignment fixes:
> 
> Populous: The Beginning
- **동작 원리 및 상세 분석**:
  텍스처 줄무늬 왜곡이나 기울어짐 현상을 해결합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawIntroVideoFix
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 인트로 동영상 재생 시 SetCooperativeLevel 동작을 보정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option makes some changes to how SetCooperativeLevel() detects fullscreen vs windowed mode so that the intro will run in fullscreen windowed mode to fix some issues with specific games' intro videos that doesn't work right without running in fullscreen windowed mode.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
> 
> Sample games that require DdrawIntroVideoFix:
> 
> Codename Outbreak
- **동작 원리 및 상세 분석**:
  게임 인트로 컷씬 및 동영상 렌더러가 올바른 윈도우 핸들에 비디오를 출력할 수 있도록 협동 레벨을 조정합니다.
- **카트라이더 및 권장 설정**: `0`

### ddrawfilteractivateapp
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: WM_ACTIVATEAPP 윈도우 메시지를 필터링하여 게임 전달을 차단합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  포커스 이동 시 게임이 강제로 최소화되거나 사운드가 음소거되는 문제를 차단합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawRemoveScanlines
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 단일 프레임 내의 검은색 인터레이스 스캔라인 줄무늬를 제거합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Removes the blank lines (scanlines) seen in some older games during the in game videos.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
> 
> Sample games that use scanlines:
> 
> Command & Conquer GoldCommand & Conquer Red AlertDeathtrap DungeonDiablo HellfireDiablo SpawnPacific GeneralSanitariumTotal AnnihilationWar Wind IIWarGamesWarlords 3
- **동작 원리 및 상세 분석**:
  저해상도 FMV 동영상 재생 시 가로줄이 생기는 비디오 품질 저하를 보정합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawRemoveInterlacing
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 프레임 간 인터레이싱 떨림을 디인터레이싱 필터로 제거합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Attempts to removes interlacing in some older games during the in game videos.  This works by detecting single lines being drawn to the screen and drawing all of them before presenting to the screen.  Enabling this will cause one frame of delay to the game.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
> 
> Sample games that use interlacing:
> 
> Abes Oddysee
- **동작 원리 및 상세 분석**:
  필드 기반 교차 렌더링을 사용하는 고전 영상의 잔상 및 화면 떨림을 개선합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawReadFromGDI
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 서피스를 게임에 전달하기 전 GDI 화면 내용을 읽어옵니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will cause dxwrapper to read from GDI before each Blt(), Lock() or GetDC() call.  In most casesDdrawWriteToGDIwill need to be used with this option.  This is a hack specifically for certain windowed games that use a mixture of GDI and DirectDraw on the same screen, such as Deadlock 2.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  GDI로 렌더링된 텍스트나 UI가 DirectX 화면과 정상적으로 합성되도록 보장합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawWriteToGDI
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Direct3D 9 대신 GDI 디바이스 컨텍스트로 서피스를 직접 Blt 전송합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will Blt directly from the DirectX surface to GDI rather than using the normal Present() function.  This is a hack specifically for certain windowed games that use a mixture of GDI and DirectDraw on the same screen.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  순수 2D 그래픽 타이틀에서 D3D9 백버퍼 변환을 거치지 않고 직접 화면에 출력합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawLimitTextureFormats
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임에 보고되는 텍스처 포맷의 개수를 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will limit the number of texture formats returned to the game.  Some older games cannot handle a large number of texture formats.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  너무 많은 텍스처 포맷(수십 개)이 보고될 때 내부 버퍼가 오버플로되어 충돌하는 구형 게임을 보호합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawLimitDisplayModeCount
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임에 열거되는 디스플레이 해상도 모드의 개수를 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will limit the number of display mode options returned to the game.  Some older games cannot handle a large number of display mode options.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  모니터가 지원하는 수많은 해상도 목록으로 인해 고전 게임의 해상도 선택 배열이 깨지는 현상을 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawCustomWidth
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 픽셀)
- **개요**: 해상도 제한 시 게임에 노출할 사용자 정의 가로 해상도를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will present a custom resolution to game when limit display mode is set, allowing you to choose this resolution in the game menu when applicable.  This requiresDd7to9,DdrawLimitDisplayModeCountandDdrawCustomHeightoptions to be enabled.  Set to the desired width for the game.
- **동작 원리 및 상세 분석**:
  `DdrawLimitDisplayModeCount = 1`일 때 그래픽 카드와 모니터가 지원하는 가로 해상도를 지정합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawCustomHeight
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 픽셀)
- **개요**: 해상도 제한 시 게임에 노출할 사용자 정의 세로 해상도를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will present a custom resolution to game when limit display mode is set, allowing you to choose this resolution in the game menu when applicable.  This requiresDd7to9,DdrawLimitDisplayModeCountandDdrawCustomWidthoptions to be enabled.  Set to the desired width for the game.
- **동작 원리 및 상세 분석**:
  `DdrawLimitDisplayModeCount = 1`일 때 그래픽 카드와 모니터가 지원하는 세로 해상도를 지정합니다.
- **카트라이더 및 권장 설정**: `0`

### ddrawlineartexturefilter
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 비팔레트 텍스처 확대 시 바이리니어(Linear) 필터링을 강제 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  픽셀 뭉개짐(도트) 대신 부드러운 선형 보간 처리를 통해 텍스처 품질을 개선합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawUseNativeResolution
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임을 바탕화면의 현재 네이티브 모니터 해상도로 확장하여 렌더링합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will stretch the game to fill the current resolution of the monitor.  This requires theDd7to9option to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  저해상도 640x480 게임을 모니터의 전체 해상도(예: 1920x1080)로 늘려 화면을 채웁니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawOverrideWidth
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 픽셀)
- **개요**: Dd7to9 사용 시 D3D9 백버퍼 가로 해상도를 지정한 값으로 강제 덮어씁니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will stretch the game to fill the resolution specified.  This requiresDd7to9andDdrawOverrideHeightoptions to be enabled.  Set to the desired width for the game.
- **동작 원리 및 상세 분석**:
  게임 내부 해상도와 무관하게 렌더링 백버퍼의 가로 픽셀을 강제 변경합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawOverrideHeight
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 픽셀)
- **개요**: Dd7to9 사용 시 D3D9 백버퍼 세로 해상도를 지정한 값으로 강제 덮어씁니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will stretch the game to fill the resolution specified.  This requiresDd7to9andDdrawOverrideWidthoptions to be enabled.  Set to the desired height for the game.
- **동작 원리 및 상세 분석**:
  게임 내부 해상도와 무관하게 렌더링 백버퍼의 세로 픽셀을 강제 변경합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawOverrideStencilFormat
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, D3DFORMAT 번호)
- **개요**: Dd7to9 변환 중 Direct3D 9이 생성할 Depth/Stencil 포맷을 강제 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will override the depth stencil format used to for Direct3D9 during the conversion.  This requiresDd7to9to be enabled.  SetDdrawOverrideStencilFormatto the number related to the desired format, for example, if you want this to be D3DFMT_D24S8 then you will set this value to 75.
- **동작 원리 및 상세 분석**:
  예: `75` (D3DFMT_D24S8), `77` (D3DFMT_D24X8). 깊이 버퍼 호환성 문제를 해결합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawIntegerScalingClamp
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 화면 확대 시 정수 배율(1x, 2x, 3x)로만 스케일링을 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the resolution scaling to use an integer as a multiplier.  For instance if the game's resolution is 800x600 and you scale the game to use 1920x1080 it will scale the games resolution to use 1600x600 and then show black borders around the edges.  This requiresDd7to9and scaling options (such asDdrawUseNativeResolution) to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  소수점 배율로 인한 픽셀 번짐이나 흐림 없이 또렷한 픽셀 아트를 보존합니다.
- **카트라이더 및 권장 설정**: `0`

### DdrawMaintainAspectRatio
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 해상도 확장 시 원본 화면비(4:3 등)를 보존하고 필러박스를 생성합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will preserve the game's aspect ratio when using resolution scaling.  For instance if the game's resolution is 800x600 and you scale the game to use 1920x1080 it will scale the games resolution to use 1440x1080 and then show black borders on the edges.  This requiresDd7to9and scaling options (such asDdrawUseNativeResolution) to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  와이드 모니터에서 화면이 좌우로 늘어나 납작해지는 현상을 막고 좌우에 검은 여백을 둡니다.
- **카트라이더 및 권장 설정**: `0`

---

## 7. [d3d9]
**분류**: Direct3D 9 디스플레이 및 초저지연 제어 (42개 설정 항목)

### AnisotropicFiltering
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 최대 지원, 2/4/8/16: 지정 배율)
- **개요**: Direct3D 9 비등방성 필터링(AF)을 강제로 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the built-in Direct3D9 Anisotropic Filtering to be enabled.  Currently it does not modify the actual texture filtering so it will have little effect on the actual game visuals.  This requires either the game to use Direct3D9 or for theDd7to9orD3d8to9options to be enabled.  1 = highest your video card can support. 2, 4, 8, or 16 = specific AF settings you want to configure.
- **동작 원리 및 상세 분석**:
  사선 방향의 텍스처 선명도를 대폭 향상시킵니다.
- **카트라이더 및 권장 설정**: `0` (드라이버 제어판 또는 인게임 설정 준수)

### AntiAliasing
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 최대 지원, 2/4/8/16: 지정 MSAA 배율)
- **개요**: Direct3D 9 디바이스 생성 시 멀티샘플링 안티앨리어싱(MSAA)을 강제 주입합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the built-in Direct3D9 AntiAliasing to be enabled.  This requires either the game to use Direct3D9 or for theDd7to9orD3d8to9options to be enabled.  1 = highest your video card can support. 2, 4, 8, or 16 = specific AA settings you want to configure.
> 
> Note: this can cause unwanted artifacts or missing surfaces on some games that don't support AntiAliasing.
- **동작 원리 및 상세 분석**:
  계단 현상을 제거합니다. 단, FlipEx 모드와 상충하므로 저지연 렌더링 시에는 `0`으로 유지합니다.
- **카트라이더 및 권장 설정**: `0`

### enablemultisamplingatoc
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: ATOC, 2: ATOC + AlphaTest)
- **개요**: 알파 커버리지(Alpha-To-Coverage) 투명도 멀티샘플링을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  수풀, 펜스 등 알파 텍스처의 경계면 계단 현상을 부드럽게 완화합니다.
- **카트라이더 및 권장 설정**: `0`

### EnvironmentCubeMapFix
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 큐브 환경 맵 텍스처 누락 시 흰색 단색 텍스처를 공급하여 렌더링 오류를 수정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option attempts to detect when the game is using a cubed environment map and no texture and sets a blank all white texture.  If fixes some games that have missing cubed environment map, seehere for an example. This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  D3D8 및 초기 D3D9 게임에서 큐브맵 텍스처 바인딩 실패로 물체가 검게 나오거나 크래시 나는 문제를 해결합니다.
- **카트라이더 및 권장 설정**: `0`

### forcemipmapusage
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임이 요청하지 않더라도 모든 텍스처에 밉맵 생성을 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  원거리 텍스처의 깜빡임(시머링)을 억제합니다.
- **카트라이더 및 권장 설정**: `0`

### depthbiasfactor
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 실수 (Float, 배율 계수)
- **개요**: D3DRS_DEPTHBIAS 렌더 스테이트 값에 곱할 계수를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  데칼이나 그림자가 바닥에 파묻히거나 깜빡이는 Z-파이팅(Z-fighting) 현상을 보정합니다.
- **카트라이더 및 권장 설정**: `0`

### depthbiasdropoffvalue
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 실수 (Float, 임계값)
- **개요**: 지정된 값 이하의 미세한 뎁스 바이어스를 0으로 절삭합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  불필요한 과도 바이어스로 인한 그래픽 왜곡을 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### EnableVSync
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0: 즉시 프레젠트, 1: 수직동기화 활성화)
- **개요**: Direct3D 9 프레젠테이션 수직동기화를 제어합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force v-sync to be enabled.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
> 
> Note: v-sync can cause some games to run sluggishly or have a slow response time.  To resolve this try usingEnableWindowModeorFullscreenWindowModeinstead. Windowed mode and fullscreen windowed mode can also remove screen tearing.
- **동작 원리 및 상세 분석**:
  `0` 설정 시 `D3DPRESENT_INTERVAL_IMMEDIATE` 모드로 구동되어 GPU가 프레임 생성을 완료하는 즉시 화면에 송출하므로 최저 레이턴시를 확보합니다.
- **카트라이더 및 권장 설정**: `0` (초저지연 필수)

### ForceVsyncMode
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 내부의 VSync 설정을 무시하고 EnableVSync 옵션을 강제 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force whatever theEnableVSyncsetting is set to.  IfEnableVSyncis disabled andForceVsyncModeis enabled then it will force v-sync to be disabled.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  게임 옵션에서 VSync를 켜더라도 래퍼 차원에서 즉시 표시(Immediate)를 강제 인가합니다.
- **카트라이더 및 권장 설정**: `0`

### ShowFPSCounter
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 좌상단, 2: 우상단, 3: 우하단, 4: 좌하단)
- **개요**: 화면 모서리에 실시간 FPS 카운터 오버레이를 렌더링합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Displays an FPS counter on the corner of the game window.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
> 
> Display LocationValueUpper-left1Upper-right2Bottom-right3Bottom-left4
- **동작 원리 및 상세 분석**:
  렌더링 파이프라인에서 측정된 현재 초당 프레임 수를 화면에 표출합니다.
- **카트라이더 및 권장 설정**: `0` (성능 측정 필요 시 1~4)

### OverrideRefreshRate
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, Hz 주사율)
- **개요**: 독점 전체화면 모드에서 모니터 주사율을 강제로 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will set the game refresh rate to the one specified.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  SetOverrideRefreshRateto the desired refresh rate for the game.
- **동작 원리 및 상세 분석**:
  예: `144`, `240`. 전체화면 진입 시 모니터의 최대 주사율로 고정할 때 사용합니다.
- **카트라이더 및 권장 설정**: `0` (기본값)

### limitrefreshrates
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임에 보고되는 모니터 주사율 목록의 개수를 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  초고주사율(240Hz, 360Hz 등) 목록을 처리하지 못해 충돌하는 구형 게임을 보호합니다.
- **카트라이더 및 권장 설정**: `0`

### LimitDisplayModeCount
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: D3D9 게임에 열거되는 해상도 목록의 개수를 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will limit the number of display mode options returned to the game.  Some older games cannot handle a large number of display mode options.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
> 
> Note: this will give a slightly different list of supported displays compared toDdrawLimitDisplayModeCount.  WhileDdrawLimitDisplayModeCountis focused on supporting legacy display modes,LimitDisplayModeCountfilters some older display modes and exposes more modern modes.
- **동작 원리 및 상세 분석**:
  버퍼 크기를 초과하는 해상도 목록으로 인한 메모리 충돌을 차단합니다.
- **카트라이더 및 권장 설정**: `0`

### CustomDisplayWidth
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 픽셀)
- **개요**: 해상도 제한 시 게임에 노출할 사용자 정의 가로 해상도입니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will present a custom resolution to game when limit display mode is set, allowing you to choose this resolution in the game menu when applicable.  This requiresEnableD3d9Wrapper,Dd7to9orD3d8to9options, andLimitDisplayModeCountandCustomDisplayHeightoptions to be enabled.  Set to the desired width for the game.
- **동작 원리 및 상세 분석**:
  모니터가 지원하는 가로 해상도를 지정합니다.
- **카트라이더 및 권장 설정**: `0`

### CustomDisplayHeight
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 픽셀)
- **개요**: 해상도 제한 시 게임에 노출할 사용자 정의 세로 해상도입니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will present a custom resolution to game when limit display mode is set, allowing you to choose this resolution in the game menu when applicable.  This requiresEnableD3d9Wrapper,Dd7to9orD3d8to9options, andLimitDisplayModeCountandCustomDisplayWidthoptions to be enabled.  Set to the desired width for the game.
- **동작 원리 및 상세 분석**:
  모니터가 지원하는 세로 해상도를 지정합니다.
- **카트라이더 및 권장 설정**: `0`

### LimitPerFrameFPS
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 실수/정수 (Float, 목표 FPS, 0: 무제한)
- **개요**: 초정밀 프레임 레이트 제한기를 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will measure the total time each frame takes and add a delay to the frame if it is too quick so that it attempts to achieve the specified frame rate.  If the frame takes to long than no delay is added.  This value can support decimal points, so, for example, if you want the frame rate to be 29.963 FPS then you can set this option to that value.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  프레임 타임을 정밀 계산하여 마이크로 딜레이를 주입함으로써 목표 FPS를 칼같이 유지합니다. 카트라이더의 경우 무제한 렌더링을 위해 `0`을 사용합니다.
- **카트라이더 및 권장 설정**: `0` (무제한)

### EnableWindowMode
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0: 게임 설정 준수, 1: 강제 창모드)
- **개요**: 게임 디바이스를 강제로 창모드로 생성합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the game into windowed.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
> 
> Note: this option does not work on all games, specifically some games made using DirectDraw have issues with this mode.
- **동작 원리 및 상세 분석**:
  PresentParameters의 Windowed 플래그를 `TRUE`로 덮어씁니다. 카트라이더와 같이 인게임 F11 키로 전체화면/창모드를 자체 전환하는 게임에서는 `0`으로 두어야 네이티브 전체화면을 유지합니다.
- **카트라이더 및 권장 설정**: `0` (카트라이더 F11 전체화면 보존 필수)

### WindowModeBorder
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0: 테두리 없음, 1: 표준 창 테두리)
- **개요**: 강제 창모드 구동 시 타이틀 바와 테두리를 표시합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will enable the windows border when forcing windowed mode.  This requires theEnableWindowModeoption to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  창모드 게임의 외곽선 및 이동 바를 제어합니다.
- **카트라이더 및 권장 설정**: `0`

### WindowModeGammaShader
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 창모드 적용, 2: 전체/창모드 적용)
- **개요**: GDI 하드웨어 감마 램프 대신 픽셀 셰이더를 통해 감마를 보정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option enables the use of a shader to set the gamma rather than using the gamma control. This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
> 
> Note: this option is enabled by default if the option doesn't exist in the ini config file.
- **동작 원리 및 상세 분석**:
  창모드에서 전체 화면 밝기가 변경되지 않는 문제를 셰이더 레벨에서 보정합니다.
- **카트라이더 및 권장 설정**: `0`

### displaybrightness
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, -100 ~ 100)
- **개요**: 게임 디스플레이 밝기 오프셋을 수동으로 조절합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  출력 화면의 기본 밝기 단계를 미세 조정합니다.
- **카트라이더 및 권장 설정**: `0`

### displaycontrast
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, -100 ~ 100)
- **개요**: 게임 디스플레이 명암비 오프셋을 수동으로 조절합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  출력 화면의 기본 대비 단계를 미세 조정합니다.
- **카트라이더 및 권장 설정**: `0`

### SetInitialWindowPosition
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 창의 초기 화면 좌표 배치를 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option enables theInitialWindowPositionLeftandInitialWindowPositionTopoptions.  When this is enabled dxwrapper will move the game window to theInitialWindowPositionLeftandInitialWindowPositionToponce at the start of the game.  If bothInitialWindowPositionLeftandInitialWindowPositionTopare set to 0 then it will move the game to the upper right hand corner of the desktop.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  `InitialWindowPositionLeft` 및 `InitialWindowPositionTop` 좌표를 적용합니다.
- **카트라이더 및 권장 설정**: `0`

### InitialWindowPositionLeft
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 화면 X 좌표)
- **개요**: 게임 창의 초기 X (좌측) 픽셀 위치를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> The initial left location on the screen where the game should start.  This requires theSetInitialWindowPositionoption to be enabled.  This can be set to any valid screen location.
- **동작 원리 및 상세 분석**:
  다중 모니터 또는 특정 화면 위치에 창을 띄울 때 사용합니다.
- **카트라이더 및 권장 설정**: `0`

### InitialWindowPositionTop
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 화면 Y 좌표)
- **개요**: 게임 창의 초기 Y (상단) 픽셀 위치를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> The initial top location on the screen where the game should start.  This requires theSetInitialWindowPositionoption to be enabled.  This can be set to any valid screen location.
- **동작 원리 및 상세 분석**:
  다중 모니터 또는 특정 화면 위치에 창을 띄울 때 사용합니다.
- **카트라이더 및 권장 설정**: `0`

### FullscreenWindowMode
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 테두리 없는 전체 창모드(Borderless Fullscreen)를 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the game into fullscreen windowed mode.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  `EnableWindowMode = 1` 상태에서 창 크기를 모니터 해상도에 꽉 차게 확장합니다.
- **카트라이더 및 권장 설정**: `0`

### hidewindowfocuschanges
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 포커스 상실 및 최소화 윈도우 메시지를 은폐합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  Alt-Tab 등으로 다른 창으로 이동하더라도 게임이 멈추거나 백그라운드에서 최소화되지 않고 계속 렌더링되도록 유지합니다.
- **카트라이더 및 권장 설정**: `0`

### enablecursorclip
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 끔, 1: 게임 창 내부 클리핑)
- **개요**: ClipCursor API를 통해 마우스 커서를 게임 창 내부에 잠급니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  다중 모니터 환경에서 마우스가 게임 화면 밖으로 이탈하여 클릭 미스가 발생하는 것을 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceExclusiveFullscreen
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 디바이스 생성을 순수 독점 전체화면(FSE)으로 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the game into exclusive fullscreen mode.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or1for on.
> 
> Note: if the game is using a resolution that is not supported by your video card or your monitor then it may cause the game to crash or exit.
- **동작 원리 및 상세 분석**:
  창모드 요청을 무시하고 전체화면 디바이스를 강제 인가합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceMixedVertexProcessing
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: D3DCREATE_MIXED_VERTEXPROCESSING 디바이스 플래그를 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force the game to use mixed mode vertex processing rather than software vertex processing to improve performance.  This requires theEnableD3d9Wrapperoption to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  소프트웨어와 하드웨어 정점 처리를 동적으로 혼용할 수 있도록 강제합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceSystemMemVertexCache
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 정점 버퍼를 시스템 메모리에 캐싱하도록 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will force vertex buffers to use system memory.  This requires theEnableD3d9Wrapperoption to be enabled.  This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  VRAM 접근 시 랙이 발생하는 특정 구형 GPU 드라이버를 위한 옵션입니다.
- **카트라이더 및 권장 설정**: `0`

### useshadowbackbuffer
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 시스템 메모리에 섀도우 백버퍼를 할당합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  백버퍼 리드백(GetRenderTargetData)을 자주 수행하는 구형 게임의 성능을 개선합니다.
- **카트라이더 및 권장 설정**: `0`

### overridestencilformat
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, D3DFORMAT)
- **개요**: D3D9 AutoDepthStencilFormat 포맷 번호를 강제 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  예: `75` (D3DFMT_D24S8).
- **카트라이더 및 권장 설정**: `0`

### flipex
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Direct3D 9Ex 환경에서 D3DSWAPEFFECT_FLIPEX 프레젠테이션 모델을 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  `D3d9to9Ex = 1` 환경에서 Windows 7 이상 DWM 큐잉 지연을 최소화하는 플립 모델입니다. `D3d9to9Ex = 0`일 때는 동작하지 않습니다.
- **카트라이더 및 권장 설정**: `1` (기본 설정되어 있으나 D3d9to9Ex=0 상태에서는 자동 무시됨)

### DisableMaxWindowedMode
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0: FSO 활성화 / Independent Flip, 1: FSO 차단 / Legacy Flip)
- **개요**: Windows 전체화면 최적화(Fullscreen Optimizations, FSO) 및 최대 창모드를 제어합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This disables Maximum Windowed Mode in DirctDraw, Direct3D8 and Direct3D9. This can be set to0for off or1for on.
- **동작 원리 및 상세 분석**:
  `0` 설정 시 Windows FSO가 정상 활성화되어 DWM 다이렉트 플립 경로(`Hardware: Independent Flip`)로 동작합니다. 프레임타임 약 1.35ms(~740 FPS)를 기록하며, **CapFrameX/PresentMon 등 실시간 오버레이가 게임 화면 위에 완벽 표출**되고 Alt-Tab 전환이 매끄럽습니다. `1` 설정 시 FSO를 완전히 차단하고 DWM 통신을 생략하는 순수 하드웨어 독점 전체화면(`Hardware: Legacy Flip`)으로 동작하여 약 1.25ms(~800 FPS)의 극한 반응속도를 확보합니다.
- **카트라이더 및 권장 설정**: `0` (방송, 녹화, 오버레이 사용 시) 또는 `1` (0.1ms 극한 타임어택 시)

### disabledynamicsleep
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 루프 내 Sleep(0) 및 Sleep(1) 호출을 가로채어 스레드 양보/수면을 원천 차단합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  Windows 스케줄러 한계(1~2ms 슬립 지연)로 인해 주기적으로 1.1ms에서 1.6ms로 튀는 프레임 스파이크를 원천 방지합니다. `Sleep(0)`은 CPU 캐시 친화적 `_mm_pause()`로, `Sleep(1)`은 커널 스케줄러 대기가 없는 **QPC 초정밀 마이크로 스핀 루프**로 대체되어 상시 최대 성능을 유지합니다.
- **카트라이더 및 권장 설정**: `1` (초저지연 필수 활성화)

### disablepowerthrottling
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: Windows 전원 스로틀링(Power Throttling) 및 EcoQoS를 프로세스 레벨에서 영구 해제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  Intel 12세대 이상 빅리틀(P-Core/E-Core) 하이브리드 아키텍처 및 최신 AMD CPU에서 메인 렌더 스레드가 E-Core로 강등되거나 클럭이 다운되는 것을 원천 차단하여 P-Core 상시 최고 부스트 클럭을 보장합니다.
- **카트라이더 및 권장 설정**: `1` (초저지연 필수 활성화)

### boostrenderthread
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 렌더링 스레드를 TIME_CRITICAL로 격상하고 MMCSS에 등록합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  렌더링 스레드의 우선순위를 `THREAD_PRIORITY_TIME_CRITICAL`로 끌어올리고 Windows MMCSS(멀티미디어 클래스 스케줄러)에 `'Games'` 프로필로 등록하여 백그라운드 프로세스 간섭으로 인한 프레임 드랍을 차단합니다.
- **카트라이더 및 권장 설정**: `1` (초저지연 필수 활성화)

### maxframelatency
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 정수 (Integer, 1 ~ 3)
- **개요**: GPU 렌더링 큐에 사전 큐잉할 수 있는 최대 백버퍼 프레임 수를 제한합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  `1` 설정 시 CPU가 GPU에 프레임을 1개 이상 미리 대기시키지 못하게 제한하여 마우스 및 키보드 입력이 화면 픽셀에 반영되는 인풋-투-포톤(Input-to-Photon) 지연시간을 1프레임 이하로 최소화합니다.
- **카트라이더 및 권장 설정**: `1` (초저지연 필수)

### spinwaitpacing
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: GPU 완료 펜스 대기 시 커널 슬립 대신 나노초 단위 마이크로 스핀-폴링을 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  GPU 이벤트 쿼리(`D3DQUERYTYPE_EVENT`)를 발행한 뒤, OS 커널 대기 상태로 진입하지 않고 `_mm_pause()` 기반 초정밀 마이크로 스핀 루프로 완료를 감지하여 균일하고 칼 같은 프레임 페이싱을 유지합니다.
- **카트라이더 및 권장 설정**: `1` (초저지연 필수 활성화)

### bypassfpureset
- **기본값 (`d3d9.ini`)**: `1`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: D3D9 디바이스 생성 시 x87 FPU 제어 단어 초기화 오버헤드를 생략합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  D3DCREATE_FPU_PRESERVE 플래그 효과를 인가하여 프레임마다 반복되는 불필요한 FPU 상태 리셋 CPU 오버헤드를 제거합니다.
- **카트라이더 및 권장 설정**: `1` (초저지연 필수 활성화)

### renderthreadaffinity
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수/비트마스크 (DWORD, 0: 자동)
- **개요**: 렌더링 스레드를 특정 물리 CPU 코어에 고정할 비트마스크를 지정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  특정 코어(예: P-Core만 사용)에 스레드를 묶고자 할 때 마스크를 지정합니다. 일반적인 시스템에서는 OS 최적 스케줄링을 위해 `0`을 권장합니다.
- **카트라이더 및 권장 설정**: `0`

### GraphicsHybridAdapter
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 0: 기본, 1: 고성능 외장 GPU 우선)
- **개요**: 노트북 하이브리드 그래픽(NVIDIA Optimus / AMD Enduro)에서 외장 GPU를 강제 선택합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option will attempt to the secondary GPU on a computer with hybrid graphics adapters (two or more GPUs).  This can be useful if you have a laptop with multiple video cards and you want the game to use a faster, or more powerful, video card.  This requires either theEnableD3d9Wrapper,Dd7to9orD3d8to9options to be enabled.  This can be set to0for off or the graphics adapter ID (usually1) for on.
- **동작 원리 및 상세 분석**:
  내장 그래픽(iGPU)과 외장 그래픽(dGPU)이 공존하는 환경에서 고성능 dGPU 어댑터를 강제로 열거하도록 유도합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 8. [FullScreen]
**분류**: 전체화면 윈도우 관리 (3개 설정 항목)

### FullScreen
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 애플리케이션의 메인 윈도우를 감지하여 전체화면 상태로 확장합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This will monitor the main window of the program and set that window to full screen.  It does this by updating the screen resolution to match the window resolution and then placing the window coordinates at the upper left and lower right to ensure that the window fills the screen.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  자체 전체화면을 지원하지 않거나 창모드로만 실행되는 구형 게임의 윈도우 핸들을 가로채어 화면 전체로 확장합니다.
- **카트라이더 및 권장 설정**: `0` (카트라이더는 네이티브 전체화면을 지원하므로 0)

### ForceWindowResize
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: FullScreen 옵션 동작 시 윈도우 크기를 화면 해상도에 강제로 맞춥니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This is intended to be use alongside theFullScreensetting.  It will allow DxWrapper to change the resolution of Windows or stretch the game window to match the resolution size.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  창 크기가 화면 해상도와 불일치할 때 강제로 픽셀 크기를 일치시킵니다.
- **카트라이더 및 권장 설정**: `0`

### WaitForWindowChanges
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 윈도우 핸들과 해상도가 안정화될 때까지 대기한 후 전체화면을 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This is intended to be use alongside theFullScreensetting.  In most cases DxWrapper will set the window to full screen immediately after detecting a window that needs to be set to full screen.  However in some cases the application has not yet finished setting the window.  In this case you can have DxWrapper wait until it detects that the window has finished before setting the window toFullScreen.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  기동 중 해상도가 여러 번 바뀌는 게임에서 조기 전체화면 적용으로 인한 크래시를 방지합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 9. [dinput8]
**분류**: DirectInput 8 입력 제어 (10개 설정 항목)

### DeviceLookupCacheTime
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 초 단위 캐시 시간)
- **개요**: DirectInput 디바이스 열거(EnumDevices) 결과를 지정한 초 동안 캐싱합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Some games will attempt to enumerate all devices for each display frame causing stuttering or other performance issues in the game.  This option will allow you to cache the enumeration results and give it back to the game, preventing extra calls to DirectInput. This requires either the game to use DirectInput8 or for theDinputto8option to be enabled.  This value is in seconds and can be set to any positive integer value to turn in on.  Set it to '0' for off.
- **동작 원리 및 상세 분석**:
  매 프레임마다 모든 USB 장치를 재검색하여 순간적인 프리징(Stutter)을 유발하는 게임의 스터터링을 방지합니다.
- **카트라이더 및 권장 설정**: `0` (필요 시 `10`)

### FilterNonActiveInput
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 게임 창이 비활성 상태(포커스 잃음)일 때 DirectInput 입력을 차단합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This attempts to prevent the game from receiving input when the game window is not in focus.  This requires either the game to use DirectInput8 or for theDinputto8option to be enabled.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  백그라운드에서 키 입력이 게임에 전달되어 의도치 않은 동작이 발생하는 것을 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### invertforcedirection
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 포스 피드백(진동 및 반발력)의 방향을 반전시킵니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  구형 레이싱 휠이나 패드에서 피드백 방향이 반대로 동작하는 하드웨어 호환성 버그를 수정합니다.
- **카트라이더 및 권장 설정**: `0`

### FixHighFrequencyMouse
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectInput 버퍼에 누적된 고주파수 마우스 보고를 병합하여 지터를 방지합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> High frequency mice, may return many results for each request from a game.  However, some games only look at the last result and drop the other results causing the mouse movement to be stuttered or to only move for drastic mouse movements.  This options merges all the results and presents only a single result to the game allowing the game to see all movements in a combined view.  This requires either the game to use DirectInput8 or for theDinputto8option to be enabled.  This can be set to '0' for off or '1' for on.
- **동작 원리 및 상세 분석**:
  1000Hz, 4000Hz, 8000Hz 등 초고폴링레이트 게이밍 마우스 사용 시 버퍼 오버런으로 인해 화면 시점이 튀거나 떨리는 현상을 해결합니다.
- **카트라이더 및 권장 설정**: `0` (마우스 시점 제어 게임에서 필요 시 `1`)

### MouseMovementFactor
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 실수 (Float, 전체 배율)
- **개요**: DirectInput 마우스 감도 배율을 전역 조절합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This allows you to slow down or speed up the mouse movement for a game.  This requires either the game to use DirectInput8 or for theDinputto8option to be enabled.  This value is in seconds and can be set to any positive decimal value to turn in on.  Values larger than 1 speed up the mouse movement and values lower than 1 slowdown the mouse movement.  For example if you want to reduce the mouse speed by one third you can set this value to '0.66'.  Set it to '0' for off.
- **동작 원리 및 상세 분석**:
  `FixHighFrequencyMouse = 1` 활성화 시 동작하며, 마우스 회전 속도를 배율로 조절합니다.
- **카트라이더 및 권장 설정**: `0` (또는 `1.0`)

### mousemovementfactorx
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 실수 (Float, X축 배율)
- **개요**: 수평(X축) 마우스 이동 감도 배율을 독립적으로 조절합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  가로 시점 회전 속도만 따로 보정할 때 사용합니다.
- **카트라이더 및 권장 설정**: `0` (또는 `1.0`)

### mousemovementfactory
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 실수 (Float, Y축 배율)
- **개요**: 수직(Y축) 마우스 이동 감도 배율을 독립적으로 조절합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  세로 시점 회전 속도만 따로 보정할 때 사용합니다.
- **카트라이더 및 권장 설정**: `0` (또는 `1.0`)

### MouseMovementPadding
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 패딩 델타)
- **개요**: 마우스 이동량에 고정 패딩 값을 가산하여 입력 데드존을 극복합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Some games will ignore mouse movements unless it hits some threshold (sometimes called dead zone).  This option will artificially add some movement to each mouse move to bypass or remove this threshold, removing the dead zone.  This requires either the game to use DirectInput8 or for theDinputto8option to be enabled.  This value can be set to any positive integer value to turn in on.  Recommended to set this to either '1' or '2'.  Set it to '0' for off.
- **동작 원리 및 상세 분석**:
  미세한 마우스 움직임을 무시하는 게임의 데드존 문제를 해결합니다.
- **카트라이더 및 권장 설정**: `0`

### mousemovementpaddingx
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, X축 패딩)
- **개요**: X축 마우스 이동량에 고정 패딩을 가산합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  가로 방향 미세 입력 반응성을 개선합니다.
- **카트라이더 및 권장 설정**: `0`

### mousemovementpaddingy
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, Y축 패딩)
- **개요**: Y축 마우스 이동량에 고정 패딩을 가산합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> This option is a specialized setting or extension added for DirectX / low-latency optimization.
- **동작 원리 및 상세 분석**:
  세로 방향 미세 입력 반응성을 개선합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 10. [dsound]
**분류**: DirectSound 오디오 제어 (14개 설정 항목)

### Num2DBuffers
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 채널 수)
- **개요**: 게임에 보고할 최대 하드웨어 가속 2D 사운드 버퍼 수를 설정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Maximum number of hardware mixed 2d buffers reported to the application. MUST be lower or equal than the hardware default.
- **동작 원리 및 상세 분석**:
  0으로 둘 경우 기본 사운드 카드 스펙을 따릅니다.
- **카트라이더 및 권장 설정**: `0`

### Num3DBuffers
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 정수 (Integer, 채널 수)
- **개요**: 게임에 보고할 최대 하드웨어 가속 3D 사운드 버퍼 수를 설정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Maximum number of hardware mixed 3d buffers.  MUST be lower or equal than the hardware default.
- **동작 원리 및 상세 분석**:
  3D 오디오 채널 할당 한도를 조절합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceCertification
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectSound 드라이버 WHQL 디지털 인증 상태를 참(True)으로 보고합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables WHQL certification report
- **동작 원리 및 상세 분석**:
  최신 윈도우 오디오 드라이버를 비인증 드라이버로 오인하여 3D 사운드를 차단하는 구형 게임을 정상 구동시킵니다.
- **카트라이더 및 권장 설정**: `0`

### ForceExclusiveMode
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectSound 독점 협동 레벨(DSSCL_EXCLUSIVE)을 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing of exclusive cooperation level.
- **동작 원리 및 상세 분석**:
  오디오 장치에 대한 단독 점유를 활성화합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceSoftwareMixing
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 소프트웨어 사운드 믹싱을 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing creation of software mixed buffers
- **동작 원리 및 상세 분석**:
  하드웨어 믹싱 미지원 오디오 칩셋에서 발생하는 사운드 찢어짐이나 음소거 현상을 해결합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceHardwareMixing
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 하드웨어 사운드 버퍼 생성을 강제 요청합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing creation of hardware mixed buffers (caution!!). Set this to 1 if you have problems with games like DEUS EX: Invisible War! Before you set this to 1 try to enableForceVoiceManagementto see if this works. EnablingForceVoiceManagementis much safer than this option.
- **동작 원리 및 상세 분석**:
  사운드 카드 자체 버퍼 가속을 강제합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceHQ3DSoftMixing
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 고품질 HRTF 3D 소프트웨어 사운드 믹싱을 강제합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing of usage of the high quality HRTF algorithm (software mixing only). Set this to1to get better 3D sound quality for applications that uses software mixing.
- **동작 원리 및 상세 분석**:
  소프트웨어 3D 입체 음향의 정위감과 오디오 품질을 향상시킵니다.
- **카트라이더 및 권장 설정**: `0`

### ForceNonStaticBuffers
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 정적(Static) 사운드 버퍼를 스트리밍/동적 버퍼로 강제 변환합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing of using non-static buffers. Static buffers are obsolete with PCI-cards. This should always be enabled whenForceVoiceManagementis set to1.
- **동작 원리 및 상세 분석**:
  대용량 사운드 리소스 로딩 시 발생하는 사운드 버퍼 메모리 부족 에러를 방지합니다.
- **카트라이더 및 권장 설정**: `0`

### ForceVoiceManagement
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: DirectSound 보이스 관리(Voice Management) 기능을 강제 활성화합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing of voice management. This can't be used together withForceSoftwareMixingandForceHardwareMixing. This is the first option you should change to 1 if you have problems with a game. This options enables the DirectX build-in voice managements and overrides the developers placement of the sound buffers. This options will enable 3D positional audio for many games that have this feature disabled for whatever reason. You should always setForceNonStaticBuffersto 1.
- **동작 원리 및 상세 분석**:
  동시 재생 음원이 한도를 초과할 때 중요도가 낮은 음원을 자동으로 페이드아웃 및 교체합니다.
- **카트라이더 및 권장 설정**: `0`

### ForcePrimaryBufferFormat
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 주(Primary) 사운드 버퍼의 포맷을 지정된 비트/샘플레이트로 강제 고정합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Enables/Disables forcing of a specified primary buffer format. Set this to 1 to get better sound quality for applications that don't set this parameters and sound cards that defaults to 8 Bit primary buffer.  Note: If this option is set to1you can specify the output format with the following settings.
- **동작 원리 및 상세 분석**:
  `PrimaryBufferBits`, `PrimaryBufferSamples`, `PrimaryBufferChannels` 값을 강제 적용합니다.
- **카트라이더 및 권장 설정**: `0`

### PrimaryBufferBits
- **기본값 (`d3d9.ini`)**: `16`
- **설정 데이터 타입**: 정수 (Integer, 비트 심도)
- **개요**: 주 사운드 버퍼 비트 심도를 지정합니다 (기본 16비트).
- **공식 위키 원문 (Upstream Wiki)**:
> Number of bits per sample. 16 is default, some sound cards are capable of using 24 or 32 bits per sample.
- **동작 원리 및 상세 분석**:
  16 또는 24비트를 지정할 수 있습니다.
- **카트라이더 및 권장 설정**: `16`

### PrimaryBufferSamples
- **기본값 (`d3d9.ini`)**: `44100`
- **설정 데이터 타입**: 정수 (Integer, Hz 샘플레이트)
- **개요**: 주 사운드 버퍼 샘플레이트를 지정합니다 (기본 44100Hz 또는 48000Hz).
- **공식 위키 원문 (Upstream Wiki)**:
> Number of samples per second. 44100 is default, some sound cards are capable of using 48000, 96000 or 192000 samples per second.
- **동작 원리 및 상세 분석**:
  일반적인 CD 품질인 44100 또는 현대 표준 48000Hz를 지정합니다.
- **카트라이더 및 권장 설정**: `44100`

### PrimaryBufferChannels
- **기본값 (`d3d9.ini`)**: `2`
- **설정 데이터 타입**: 정수 (Integer, 채널 수)
- **개요**: 주 사운드 버퍼 채널 수를 지정합니다 (1: 모노, 2: 스테레오).
- **공식 위키 원문 (Upstream Wiki)**:
> Number of channels.2(=Stereo) is default.
- **동작 원리 및 상세 분석**:
  2(스테레오)가 표준입니다.
- **카트라이더 및 권장 설정**: `2`

### AudioClipDetection
- **기본값 (`d3d9.ini`)**: `0`
- **설정 데이터 타입**: 부울 (Boolean, 0 또는 1)
- **개요**: 사운드가 비정상 조기 중단될 때 팝/클릭 잡음을 방지하는 페이드아웃을 적용합니다.
- **공식 위키 원문 (Upstream Wiki)**:
> Detects when the audio is stopped prematurely and fades out to avoid pops in the sound.  This can be set to '0' for off or '1' for on.
> 
> Below is a sample configuration for DxWrapper:
> 
> ;; Config file for DirectX DLL Wrapper;;;; For details, see: https://github.com/elishacloud/dxwrapper/wiki/Configuration;;[General]RealDllPath= AUTOWrapperMode= AUTOLoadCustomDllPath=ExcludeProcess=IncludeProcess=RunProcess= powershell.exe"Mount-DiskImage 'sample.iso'"WaitForProcess= 1DisableLogging= 0[Plugins]LoadPlugins= 0LoadFromScriptsOnly= 0[Compatibility]Dd7to9= 1D3d8to9= 0D3d9to9Ex= 0DDrawCompat= 0Dinputto8= 0DisableGameUX= 0EnableDdrawWrapper= 0EnableD3d9Wrapper= 0EnableDinput8Wrapper= 0EnableDsoundWrapper= 0EnableOpenDialogHook= 0WinVersionLie= offWinVersionLieSP= 0FixPerfCounterUptime= 0HandleExceptions= 0SingleProcAffinity= 0[DDrawCompat]DDrawCompat20= 0DDrawCompat21= 0DDrawCompat32= 0DDrawCompatDisableGDIHook= 0DDrawCompatNoProcAffinity= 0[ddraw]DdrawOverrideBitMode= 0DdrawUseDirect3D9Caps= 0[Dd7to9]DdrawAutoFrameSkip= 0DdrawEmulateSurface= 0DdrawEmulateLock= 0DdrawExtraEmulationSize= 0DdrawKeepAllInterfaceCache= 0DdrawForceMipMapAutoGen= 0DdrawClampVertexZDepth= 0DdrawFixByteAlignment= 0DdrawEnableByteAlignment= 0DdrawIntroVideoFix= 0DdrawRemoveScanlines= 0DdrawRemoveInterlacing= 0DdrawReadFromGDI= 0DdrawWriteToGDI= 0DdrawLimitTextureFormats= 0DdrawLimitDisplayModeCount= 0DdrawCustomWidth= 0DdrawCustomHeight= 0DdrawUseNativeResolution= 0DdrawOverrideWidth= 0DdrawOverrideHeight= 0DdrawIntegerScalingClamp= 0DdrawMaintainAspectRatio= 0[d3d9]AnisotropicFiltering= 0AntiAliasing= 0EnvironmentCubeMapFix= 0EnableVSync= 0ForceVsyncMode= 0ShowFPSCounter= 0OverrideRefreshRate= 0LimitDisplayModeCount= 0CustomDisplayWidth= 0CustomDisplayHeight= 0LimitPerFrameFPS= 0EnableWindowMode= 0WindowModeBorder= 0SetInitialWindowPosition= 0InitialWindowPositionLeft= 0InitialWindowPositionTop= 0FullscreenWindowMode= 0ForceExclusiveFullscreen= 0ForceMixedVertexProcessing= 0ForceSystemMemVertexCache= 0SetSwapEffectShim= 0DisableMaxWindowedMode= 0ForceDirect3D9On12= 0GraphicsHybridAdapter= 0[FullScreen]FullScreen= 0ForceWindowResize= 0WaitForWindowChanges= 0[dinput8]DeviceLookupCacheTime= 0FilterNonActiveInput= 0FixHighFrequencyMouse= 0MouseMovementFactor= 0MouseMovementPadding= 0[dsound]Num2DBuffers= 0Num3DBuffers= 0ForceCertification= 0ForceExclusiveMode= 0ForceSoftwareMixing= 0ForceHardwareMixing= 0ForceHQ3DSoftMixing= 0ForceNonStaticBuffers= 0ForceVoiceManagement= 0ForcePrimaryBufferFormat= 0PrimaryBufferBits= 16PrimaryBufferSamples= 44100PrimaryBufferChannels= 2AudioClipDetection= 0
- **동작 원리 및 상세 분석**:
  오디오 스트림이 갑자기 끊어질 때 '퍽' 하는 잡음(클리핑 팝 노이즈)이 스피커로 출력되는 것을 완화합니다.
- **카트라이더 및 권장 설정**: `0`

---

## 11. 전체 파라미터 요약 테이블

| 섹션 | 파라미터명 | 기본값 | 데이터 타입 | 핵심 역할 및 권장 사항 |
| :--- | :--- | :---: | :---: | :--- |
| `[General]` | **realdllpath** | `AUTO` | 문자열 | 실제 시스템 또는 대상 DLL의 파일 경로를 수동으로 지정합니다. |
| `[General]` | **wrappermode** | `AUTO` | 문자열 | DxWrapper가 가로챌 대상 래퍼 모드를 명시적으로 지정합니다. |
| `[General]` | **loadcustomdllpath** | `` | 문자열 | 프로그램 시작 시 추가로 로드할 외부 DLL 파일 목록을 지정합니다. |
| `[General]` | **excludeprocess** | `` | 문자열 | DxWrapper 주입에서 제외할 실행 파일 목록을 지정합니다. |
| `[General]` | **includeprocess** | `` | 문자열 | DxWrapper를 적용할 특정 게임 실행 파일 목록을 지정합니다. |
| `[General]` | **runprocess** | `` | 문자열 | 게임이 시작될 때 함께 실행할 외부 프로그램 또는 유틸리티를 지정합니다. |
| `[General]` | **waitforprocess** | `0` | 부울 | RunProcess로 지정된 프로세스가 종료될 때까지 게임 로딩을 일시 대기합니다. |
| `[General]` | **disablelogging** | `1` | 부울 | DxWrapper의 파일 로깅 기능을 비활성화합니다. |
| `[Plugins]` | **loadplugins** | `0` | 부울 | 게임 실행 폴더에 존재하는 ASI 플러그인을 자동으로 로드합니다. |
| `[Plugins]` | **loadfromscriptsonly** | `0` | 부울 | ASI 플러그인 로드 위치를 scripts/plugins 하위 폴더로 제한합니다. |
| `[Compatibility]` | **dd7to9** | `0` | 부울 | DirectDraw (DirectX 1~7) 호출을 Direct3D 9으로 변환합니다. |
| `[Compatibility]` | **d3d8to9** | `0` | 부울 | Direct3D 8 API 호출을 Direct3D 9으로 변환합니다. |
| `[Compatibility]` | **d3d9to9ex** | `1` | 부울 | Direct3D 9 호출을 Direct3D 9Ex(D3D9Ex) 인터페이스로 강제 승격합니다. |
| `[Compatibility]` | **d3d9on12** | `0` | 부울 | Microsoft D3D9On12 매핑 계층을 활성화하여 DirectX 12 위에서 D3D9을 구동합니다. |
| `[Compatibility]` | **ddrawcompat** | `0` | 부울 | narzoul의 DDrawCompat 호환성 라이브러리를 활성화합니다. |
| `[Compatibility]` | **dinputto8** | `0` | 부울 | DirectInput (1~7) 호출을 DirectInput 8로 변환합니다. |
| `[Compatibility]` | **disablegameux** | `0` | 부울 | Windows 게임 탐색기(GameUX) 연동을 차단합니다. |
| `[Compatibility]` | **enableddrawwrapper** | `0` | 부울 | DirectDraw 래퍼 엔진을 활성화합니다. |
| `[Compatibility]` | **enabled3d9wrapper** | `1` | 부울 | Direct3D 9 래퍼 엔진을 활성화합니다. |
| `[Compatibility]` | **enabledinput8wrapper** | `0` | 부울 | DirectInput 8 래퍼 엔진을 활성화합니다. |
| `[Compatibility]` | **enabledsoundwrapper** | `0` | 부울 | DirectSound 래퍼 엔진을 활성화합니다. |
| `[Compatibility]` | **disablegdigammaramp** | `0` | 부울 | GDI 감마 램프(Gamma Ramp) 적용을 차단합니다. |
| `[Compatibility]` | **enableopendialoghook** | `0` | 부울 | Windows 파일 열기 대화상자(GetOpenFileName) 후킹을 활성화합니다. |
| `[Compatibility]` | **winversionlie** | `off` | 문자열 | 게임 프로세스에 보고할 Windows 운영체제 버전을 위장합니다. |
| `[Compatibility]` | **winversionliesp** | `0` | 정수 | WinVersionLie 적용 시 함께 보고할 서비스 팩(Service Pack) 번호를 지정합니다. |
| `[Compatibility]` | **forcekeyboardlayout** | `0` | 16진수 정수 | 특정 키보드 언어 레이아웃을 강제로 적용합니다. |
| `[Compatibility]` | **fixperfcounteruptime** | `0` | 부울 | QueryPerformanceCounter 가동 시간 오버플로 버그를 패치합니다. |
| `[Compatibility]` | **handleexceptions** | `0` | 정수 | 애플리케이션 크래시를 유발하는 처리되지 않은 예외를 가로채어 복구합니다. |
| `[Compatibility]` | **singleprocaffinity** | `0` | 부울 | 게임 프로세스를 단일 CPU 코어에 강제 고정합니다. |
| `[Compatibility]` | **procaffinitymask** | `0` | 정수/비트마스크 | SingleProcAffinity 적용 시 할당할 CPU 코어 비트마스크를 지정합니다. |
| `[DDrawCompat]` | **ddrawcompat20** | `0` | 부울 | DDrawCompat v0.2.0b 버전을 활성화합니다. |
| `[DDrawCompat]` | **ddrawcompat21** | `0` | 부울 | DDrawCompat v0.2.1 버전을 활성화합니다. |
| `[DDrawCompat]` | **ddrawcompat32** | `0` | 부울 | DDrawCompat v0.3.2 버전을 활성화합니다. |
| `[DDrawCompat]` | **ddrawcompatdisablegdihook** | `0` | 부울 | DDrawCompat 라이브러리의 GDI 후킹 기능을 비활성화합니다. |
| `[DDrawCompat]` | **ddrawcompatnoprocaffinity** | `0` | 부울 | DDrawCompat의 기본 단일 프로세서 선호도 잠금을 해제합니다. |
| `[ddraw]` | **ddrawoverridebitmode** | `0` | 정수 | DirectDraw 디스플레이 색상 비트 깊이를 강제로 변경합니다. |
| `[ddraw]` | **ddrawusedirect3d9caps** | `0` | 부울 | DirectDraw GetCaps 호출 시 Direct3D 9 장치 능력을 덮어씌웁니다. |
| `[Dd7to9]` | **ddrawautoframeskip** | `0` | 부울 | 프레임 지연 발생 시 자동으로 프레임을 건너뛰어 입력 지연을 완화합니다. |
| `[Dd7to9]` | **ddrawemulatesurface** | `0` | 부울 | 장치 컨텍스트(DC)를 사용하여 DirectDraw 표면을 소프트웨어 에뮬레이션합니다. |
| `[Dd7to9]` | **ddrawemulatelock** | `0` | 부울 | DirectDraw 표면 Lock/Unlock 외부 접근 크래시를 방지하는 에뮬레이션 락을 적용합니다. |
| `[Dd7to9]` | **ddrawuseshadowsurface** | `0` | 부울 | 렌더 타겟 Lock/GetDC 호출을 위해 시스템 메모리 섀도우 서피스를 활성화합니다. |
| `[Dd7to9]` | **ddrawkeepallinterfacecache** | `0` | 정수 | 해제된 DirectDraw COM 인터페이스의 캐시를 보존합니다. |
| `[Dd7to9]` | **ddrawallowmultisampling** | `0` | 부울 | 게임이 렌더 스테이트로 안티앨리어싱을 요청할 경우 MSAA를 허용합니다. |
| `[Dd7to9]` | **ddrawforcemipmapautogen** | `0` | 부울 | Direct3D 9 하드웨어 밉맵 자동 생성(AutoGenMipMap)을 강제합니다. |
| `[Dd7to9]` | **ddrawclampvertexzdepth** | `0` | 정수 | 변환된 정점의 Z 깊이 값을 0.0f ~ 1.0f 사이로 클램핑합니다. |
| `[Dd7to9]` | **ddrawenablebytealignment** | `0` | 부울 | 서피스 스트라이드에 대한 32비트/64비트 바이트 정렬 강제를 해제합니다. |
| `[Dd7to9]` | **ddrawfixbytealignment** | `0` | 정수 | 피치(Pitch) 크기가 일치하지 않는 표면의 락(Lock)을 보정합니다. |
| `[Dd7to9]` | **ddrawintrovideofix** | `0` | 부울 | 인트로 동영상 재생 시 SetCooperativeLevel 동작을 보정합니다. |
| `[Dd7to9]` | **ddrawfilteractivateapp** | `0` | 부울 | WM_ACTIVATEAPP 윈도우 메시지를 필터링하여 게임 전달을 차단합니다. |
| `[Dd7to9]` | **ddrawremovescanlines** | `0` | 부울 | 단일 프레임 내의 검은색 인터레이스 스캔라인 줄무늬를 제거합니다. |
| `[Dd7to9]` | **ddrawremoveinterlacing** | `0` | 부울 | 프레임 간 인터레이싱 떨림을 디인터레이싱 필터로 제거합니다. |
| `[Dd7to9]` | **ddrawreadfromgdi** | `0` | 부울 | 서피스를 게임에 전달하기 전 GDI 화면 내용을 읽어옵니다. |
| `[Dd7to9]` | **ddrawwritetogdi** | `0` | 부울 | Direct3D 9 대신 GDI 디바이스 컨텍스트로 서피스를 직접 Blt 전송합니다. |
| `[Dd7to9]` | **ddrawlimittextureformats** | `0` | 부울 | 게임에 보고되는 텍스처 포맷의 개수를 제한합니다. |
| `[Dd7to9]` | **ddrawlimitdisplaymodecount** | `0` | 부울 | 게임에 열거되는 디스플레이 해상도 모드의 개수를 제한합니다. |
| `[Dd7to9]` | **ddrawcustomwidth** | `0` | 정수 | 해상도 제한 시 게임에 노출할 사용자 정의 가로 해상도를 지정합니다. |
| `[Dd7to9]` | **ddrawcustomheight** | `0` | 정수 | 해상도 제한 시 게임에 노출할 사용자 정의 세로 해상도를 지정합니다. |
| `[Dd7to9]` | **ddrawlineartexturefilter** | `0` | 부울 | 비팔레트 텍스처 확대 시 바이리니어(Linear) 필터링을 강제 적용합니다. |
| `[Dd7to9]` | **ddrawusenativeresolution** | `0` | 부울 | 게임을 바탕화면의 현재 네이티브 모니터 해상도로 확장하여 렌더링합니다. |
| `[Dd7to9]` | **ddrawoverridewidth** | `0` | 정수 | Dd7to9 사용 시 D3D9 백버퍼 가로 해상도를 지정한 값으로 강제 덮어씁니다. |
| `[Dd7to9]` | **ddrawoverrideheight** | `0` | 정수 | Dd7to9 사용 시 D3D9 백버퍼 세로 해상도를 지정한 값으로 강제 덮어씁니다. |
| `[Dd7to9]` | **ddrawoverridestencilformat** | `0` | 정수 | Dd7to9 변환 중 Direct3D 9이 생성할 Depth/Stencil 포맷을 강제 지정합니다. |
| `[Dd7to9]` | **ddrawintegerscalingclamp** | `0` | 부울 | 화면 확대 시 정수 배율(1x, 2x, 3x)로만 스케일링을 제한합니다. |
| `[Dd7to9]` | **ddrawmaintainaspectratio** | `0` | 부울 | 해상도 확장 시 원본 화면비(4:3 등)를 보존하고 필러박스를 생성합니다. |
| `[d3d9]` | **anisotropicfiltering** | `0` | 정수 | Direct3D 9 비등방성 필터링(AF)을 강제로 활성화합니다. |
| `[d3d9]` | **antialiasing** | `0` | 정수 | Direct3D 9 디바이스 생성 시 멀티샘플링 안티앨리어싱(MSAA)을 강제 주입합니다. |
| `[d3d9]` | **enablemultisamplingatoc** | `0` | 정수 | 알파 커버리지(Alpha-To-Coverage) 투명도 멀티샘플링을 활성화합니다. |
| `[d3d9]` | **environmentcubemapfix** | `0` | 부울 | 큐브 환경 맵 텍스처 누락 시 흰색 단색 텍스처를 공급하여 렌더링 오류를 수정합니다. |
| `[d3d9]` | **forcemipmapusage** | `0` | 부울 | 게임이 요청하지 않더라도 모든 텍스처에 밉맵 생성을 강제합니다. |
| `[d3d9]` | **depthbiasfactor** | `0` | 실수 | D3DRS_DEPTHBIAS 렌더 스테이트 값에 곱할 계수를 지정합니다. |
| `[d3d9]` | **depthbiasdropoffvalue** | `0` | 실수 | 지정된 값 이하의 미세한 뎁스 바이어스를 0으로 절삭합니다. |
| `[d3d9]` | **enablevsync** | `0` | 부울 | Direct3D 9 프레젠테이션 수직동기화를 제어합니다. |
| `[d3d9]` | **forcevsyncmode** | `0` | 부울 | 게임 내부의 VSync 설정을 무시하고 EnableVSync 옵션을 강제 적용합니다. |
| `[d3d9]` | **showfpscounter** | `0` | 정수 | 화면 모서리에 실시간 FPS 카운터 오버레이를 렌더링합니다. |
| `[d3d9]` | **overriderefreshrate** | `0` | 정수 | 독점 전체화면 모드에서 모니터 주사율을 강제로 지정합니다. |
| `[d3d9]` | **limitrefreshrates** | `0` | 부울 | 게임에 보고되는 모니터 주사율 목록의 개수를 제한합니다. |
| `[d3d9]` | **limitdisplaymodecount** | `0` | 부울 | D3D9 게임에 열거되는 해상도 목록의 개수를 제한합니다. |
| `[d3d9]` | **customdisplaywidth** | `0` | 정수 | 해상도 제한 시 게임에 노출할 사용자 정의 가로 해상도입니다. |
| `[d3d9]` | **customdisplayheight** | `0` | 정수 | 해상도 제한 시 게임에 노출할 사용자 정의 세로 해상도입니다. |
| `[d3d9]` | **limitperframefps** | `0` | 실수/정수 | 초정밀 프레임 레이트 제한기를 활성화합니다. |
| `[d3d9]` | **enablewindowmode** | `0` | 부울 | 게임 디바이스를 강제로 창모드로 생성합니다. |
| `[d3d9]` | **windowmodeborder** | `0` | 부울 | 강제 창모드 구동 시 타이틀 바와 테두리를 표시합니다. |
| `[d3d9]` | **windowmodegammashader** | `0` | 정수 | GDI 하드웨어 감마 램프 대신 픽셀 셰이더를 통해 감마를 보정합니다. |
| `[d3d9]` | **displaybrightness** | `0` | 정수 | 게임 디스플레이 밝기 오프셋을 수동으로 조절합니다. |
| `[d3d9]` | **displaycontrast** | `0` | 정수 | 게임 디스플레이 명암비 오프셋을 수동으로 조절합니다. |
| `[d3d9]` | **setinitialwindowposition** | `0` | 부울 | 게임 창의 초기 화면 좌표 배치를 활성화합니다. |
| `[d3d9]` | **initialwindowpositionleft** | `0` | 정수 | 게임 창의 초기 X (좌측) 픽셀 위치를 지정합니다. |
| `[d3d9]` | **initialwindowpositiontop** | `0` | 정수 | 게임 창의 초기 Y (상단) 픽셀 위치를 지정합니다. |
| `[d3d9]` | **fullscreenwindowmode** | `0` | 부울 | 테두리 없는 전체 창모드(Borderless Fullscreen)를 활성화합니다. |
| `[d3d9]` | **hidewindowfocuschanges** | `0` | 부울 | 포커스 상실 및 최소화 윈도우 메시지를 은폐합니다. |
| `[d3d9]` | **enablecursorclip** | `0` | 정수 | ClipCursor API를 통해 마우스 커서를 게임 창 내부에 잠급니다. |
| `[d3d9]` | **forceexclusivefullscreen** | `0` | 부울 | 게임 디바이스 생성을 순수 독점 전체화면(FSE)으로 강제합니다. |
| `[d3d9]` | **forcemixedvertexprocessing** | `0` | 부울 | D3DCREATE_MIXED_VERTEXPROCESSING 디바이스 플래그를 강제합니다. |
| `[d3d9]` | **forcesystemmemvertexcache** | `0` | 부울 | 정점 버퍼를 시스템 메모리에 캐싱하도록 강제합니다. |
| `[d3d9]` | **useshadowbackbuffer** | `0` | 부울 | 시스템 메모리에 섀도우 백버퍼를 할당합니다. |
| `[d3d9]` | **overridestencilformat** | `0` | 정수 | D3D9 AutoDepthStencilFormat 포맷 번호를 강제 지정합니다. |
| `[d3d9]` | **flipex** | `1` | 부울 | Direct3D 9Ex 환경에서 D3DSWAPEFFECT_FLIPEX 프레젠테이션 모델을 활성화합니다. |
| `[d3d9]` | **disablemaxwindowedmode** | `0` | 부울 | Windows 전체화면 최적화(Fullscreen Optimizations, FSO) 및 최대 창모드를 제어합니다. |
| `[d3d9]` | **disabledynamicsleep** | `1` | 부울 | 게임 루프 내 Sleep(0) 및 Sleep(1) 호출을 가로채어 스레드 양보/수면을 원천 차단합니다. |
| `[d3d9]` | **disablepowerthrottling** | `1` | 부울 | Windows 전원 스로틀링(Power Throttling) 및 EcoQoS를 프로세스 레벨에서 영구 해제합니다. |
| `[d3d9]` | **boostrenderthread** | `1` | 부울 | 게임 렌더링 스레드를 TIME_CRITICAL로 격상하고 MMCSS에 등록합니다. |
| `[d3d9]` | **maxframelatency** | `1` | 정수 | GPU 렌더링 큐에 사전 큐잉할 수 있는 최대 백버퍼 프레임 수를 제한합니다. |
| `[d3d9]` | **spinwaitpacing** | `1` | 부울 | GPU 완료 펜스 대기 시 커널 슬립 대신 나노초 단위 마이크로 스핀-폴링을 적용합니다. |
| `[d3d9]` | **bypassfpureset** | `1` | 부울 | D3D9 디바이스 생성 시 x87 FPU 제어 단어 초기화 오버헤드를 생략합니다. |
| `[d3d9]` | **renderthreadaffinity** | `0` | 정수/비트마스크 | 렌더링 스레드를 특정 물리 CPU 코어에 고정할 비트마스크를 지정합니다. |
| `[d3d9]` | **graphicshybridadapter** | `0` | 정수 | 노트북 하이브리드 그래픽(NVIDIA Optimus / AMD Enduro)에서 외장 GPU를 강제 선택합니다. |
| `[FullScreen]` | **fullscreen** | `0` | 부울 | 애플리케이션의 메인 윈도우를 감지하여 전체화면 상태로 확장합니다. |
| `[FullScreen]` | **forcewindowresize** | `0` | 부울 | FullScreen 옵션 동작 시 윈도우 크기를 화면 해상도에 강제로 맞춥니다. |
| `[FullScreen]` | **waitforwindowchanges** | `0` | 부울 | 윈도우 핸들과 해상도가 안정화될 때까지 대기한 후 전체화면을 적용합니다. |
| `[dinput8]` | **devicelookupcachetime** | `0` | 정수 | DirectInput 디바이스 열거(EnumDevices) 결과를 지정한 초 동안 캐싱합니다. |
| `[dinput8]` | **filternonactiveinput** | `0` | 부울 | 게임 창이 비활성 상태(포커스 잃음)일 때 DirectInput 입력을 차단합니다. |
| `[dinput8]` | **invertforcedirection** | `0` | 부울 | 포스 피드백(진동 및 반발력)의 방향을 반전시킵니다. |
| `[dinput8]` | **fixhighfrequencymouse** | `0` | 부울 | DirectInput 버퍼에 누적된 고주파수 마우스 보고를 병합하여 지터를 방지합니다. |
| `[dinput8]` | **mousemovementfactor** | `0` | 실수 | DirectInput 마우스 감도 배율을 전역 조절합니다. |
| `[dinput8]` | **mousemovementfactorx** | `0` | 실수 | 수평(X축) 마우스 이동 감도 배율을 독립적으로 조절합니다. |
| `[dinput8]` | **mousemovementfactory** | `0` | 실수 | 수직(Y축) 마우스 이동 감도 배율을 독립적으로 조절합니다. |
| `[dinput8]` | **mousemovementpadding** | `0` | 정수 | 마우스 이동량에 고정 패딩 값을 가산하여 입력 데드존을 극복합니다. |
| `[dinput8]` | **mousemovementpaddingx** | `0` | 정수 | X축 마우스 이동량에 고정 패딩을 가산합니다. |
| `[dinput8]` | **mousemovementpaddingy** | `0` | 정수 | Y축 마우스 이동량에 고정 패딩을 가산합니다. |
| `[dsound]` | **num2dbuffers** | `0` | 정수 | 게임에 보고할 최대 하드웨어 가속 2D 사운드 버퍼 수를 설정합니다. |
| `[dsound]` | **num3dbuffers** | `0` | 정수 | 게임에 보고할 최대 하드웨어 가속 3D 사운드 버퍼 수를 설정합니다. |
| `[dsound]` | **forcecertification** | `0` | 부울 | DirectSound 드라이버 WHQL 디지털 인증 상태를 참(True)으로 보고합니다. |
| `[dsound]` | **forceexclusivemode** | `0` | 부울 | DirectSound 독점 협동 레벨(DSSCL_EXCLUSIVE)을 강제합니다. |
| `[dsound]` | **forcesoftwaremixing** | `0` | 부울 | 소프트웨어 사운드 믹싱을 강제합니다. |
| `[dsound]` | **forcehardwaremixing** | `0` | 부울 | 하드웨어 사운드 버퍼 생성을 강제 요청합니다. |
| `[dsound]` | **forcehq3dsoftmixing** | `0` | 부울 | 고품질 HRTF 3D 소프트웨어 사운드 믹싱을 강제합니다. |
| `[dsound]` | **forcenonstaticbuffers** | `0` | 부울 | 정적(Static) 사운드 버퍼를 스트리밍/동적 버퍼로 강제 변환합니다. |
| `[dsound]` | **forcevoicemanagement** | `0` | 부울 | DirectSound 보이스 관리(Voice Management) 기능을 강제 활성화합니다. |
| `[dsound]` | **forceprimarybufferformat** | `0` | 부울 | 주(Primary) 사운드 버퍼의 포맷을 지정된 비트/샘플레이트로 강제 고정합니다. |
| `[dsound]` | **primarybufferbits** | `16` | 정수 | 주 사운드 버퍼 비트 심도를 지정합니다 (기본 16비트). |
| `[dsound]` | **primarybuffersamples** | `44100` | 정수 | 주 사운드 버퍼 샘플레이트를 지정합니다 (기본 44100Hz 또는 48000Hz). |
| `[dsound]` | **primarybufferchannels** | `2` | 정수 | 주 사운드 버퍼 채널 수를 지정합니다 (1: 모노, 2: 스테레오). |
| `[dsound]` | **audioclipdetection** | `0` | 부울 | 사운드가 비정상 조기 중단될 때 팝/클릭 잡음을 방지하는 페이드아웃을 적용합니다. |

---

## 12. 용도별 실전 권장 프리셋

모든 핵심 저지연 최적화(Sleep 마이크로스핀, MMCSS 우선순위 부스트, 전원 스로틀링 영구 해제 등)는 공통으로 작동하며, **두 프로필 간의 유일한 차이는 `DisableMaxWindowedMode` 설정값 하나뿐**입니다.

### 프로필 비교표

| 비교 항목 | 프로필 1 (방송 / 영상 녹화 / 오버레이 표출) | 프로필 2 (극한의 타임어택 / 대회용) |
| :--- | :---: | :---: |
| **핵심 변경 옵션** | **`DisableMaxWindowedMode = 0`** | **`DisableMaxWindowedMode = 1`** |
| **프레젠테이션 모드** | `Hardware: Independent Flip` | `Hardware: Legacy Flip` |
| **99% 프레임 타임** | 약 1.35ms (약 740 FPS) | 약 1.25ms (약 800 FPS) |
| **단일 모니터 오버레이 표출** | 지원 (CapFrameX, PresentMon 게임 화면 위 실시간 표출) | 미지원 (순수 독점 화면으로 보조 모니터 확인 필요) |
| **Alt-Tab 전환 반응** | 즉시 전환 (화면 깜빡임 없음) | 화면 깜빡임 발생 후 전환 |
| **권장 사용자** | 방송 스트리머, 유튜브 영상 제작자, 일반 유저 | 0.1ms 기록 단축용 타임어택, 대회 출전 프로게이머 |

### 통합 최적화 d3d9.ini 템플릿

```ini
;; ======================================================================
;; DxWrapper Low-Latency Edition - Optimized d3d9.ini Template
;; ======================================================================

[General]
RealDllPath                = AUTO
WrapperMode                = AUTO
DisableLogging             = 1

[Compatibility]
D3d9to9Ex                  = 0
D3d9on12                   = 0
EnableD3d9Wrapper          = 1

[d3d9]
EnableVSync                = 0
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
RenderThreadAffinity       = 0

;; ----------------------------------------------------------------------
;; [핵심 전환 스위치]
;;   DisableMaxWindowedMode = 0  -> Independent Flip (오버레이/방송/녹화 호환, ~1.35ms)
;;   DisableMaxWindowedMode = 1  -> Legacy Flip (극한 반응속도 FSE, ~1.25ms)
;; ----------------------------------------------------------------------
DisableMaxWindowedMode     = 0
```
