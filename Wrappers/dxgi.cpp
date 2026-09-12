#include "dxgi.h"
#include "dxgi\dxgi_swapchain.h"
#include "Settings\Settings.h"
#include "Logging\Logging.h"
#include "Utils\Utils.h"

namespace Wrapper
{
	bool ValidProcAddress(FARPROC ProcAddress)
	{
		return (ProcAddress != nullptr);
	}

	bool CheckWrapperName(const char* WrapperMode)
	{
		if (!WrapperMode) return false;
		return (_stricmp(WrapperMode, "dxgi.dll") == 0 || _stricmp(WrapperMode, "dxgi") == 0);
	}
}

namespace dxgi
{
	static HMODULE hRealDXGI = nullptr;

	typedef HRESULT(WINAPI* PFN_CreateDXGIFactory)(REFIID riid, void** ppFactory);
	typedef HRESULT(WINAPI* PFN_CreateDXGIFactory1)(REFIID riid, void** ppFactory);
	typedef HRESULT(WINAPI* PFN_CreateDXGIFactory2)(UINT Flags, REFIID riid, void** ppFactory);
	typedef HRESULT(WINAPI* PFN_DXGIGetDebugInterface1)(UINT Flags, REFIID riid, void** pDebug);
	typedef HRESULT(WINAPI* PFN_DXGIDeclareAdapterRemovalSupport)();
	typedef HRESULT(WINAPI* PFN_DXGIReportAdapterConfiguration)();
	typedef void(WINAPI* PFN_VoidFunc)();
	typedef HRESULT(WINAPI* PFN_DXGID3D10CreateDevice)(HMODULE, IDXGIFactory*, IDXGIAdapter*, UINT, void*, void**);
	typedef size_t(WINAPI* PFN_DXGID3D10GetLayeredDeviceSize)();

	static PFN_CreateDXGIFactory Real_CreateDXGIFactory = nullptr;
	static PFN_CreateDXGIFactory1 Real_CreateDXGIFactory1 = nullptr;
	static PFN_CreateDXGIFactory2 Real_CreateDXGIFactory2 = nullptr;
	static PFN_DXGIGetDebugInterface1 Real_DXGIGetDebugInterface1 = nullptr;
	static PFN_DXGIDeclareAdapterRemovalSupport Real_DXGIDeclareAdapterRemovalSupport = nullptr;
	static PFN_DXGIReportAdapterConfiguration Real_DXGIReportAdapterConfiguration = nullptr;
	static PFN_VoidFunc Real_ApplyCompatResolutionQuirking = nullptr;
	static PFN_VoidFunc Real_CompatString = nullptr;
	static PFN_VoidFunc Real_CompatValue = nullptr;
	static PFN_VoidFunc Real_DXGIDumpJournal = nullptr;
	static PFN_VoidFunc Real_PIXBeginCapture = nullptr;
	static PFN_VoidFunc Real_PIXEndCapture = nullptr;
	static PFN_VoidFunc Real_PIXGetCaptureState = nullptr;
	static PFN_VoidFunc Real_SetAppCompatStringPointer = nullptr;
	static PFN_VoidFunc Real_UpdateHMDEmulationStatus = nullptr;
	static PFN_DXGID3D10CreateDevice Real_DXGID3D10CreateDevice = nullptr;
	static PFN_VoidFunc Real_DXGID3D10CreateLayeredDevice = nullptr;
	static PFN_DXGID3D10GetLayeredDeviceSize Real_DXGID3D10GetLayeredDeviceSize = nullptr;
	static PFN_VoidFunc Real_DXGID3D10RegisterLayers = nullptr;

	HMODULE Load(const char* ProxyDll, const char* MyDllName)
	{
		if (hRealDXGI) return hRealDXGI;

		char szSysPath[MAX_PATH] = {};
		GetSystemDirectoryA(szSysPath, MAX_PATH);
		strcat_s(szSysPath, "\\dxgi.dll");

		hRealDXGI = LoadLibraryA(szSysPath);
		if (!hRealDXGI)
		{
			Logging::Log() << "Failed to load system dxgi.dll from: " << szSysPath;
			return nullptr;
		}

		Logging::Log() << "Loaded system dxgi.dll: " << szSysPath;

		Real_CreateDXGIFactory = (PFN_CreateDXGIFactory)GetProcAddress(hRealDXGI, "CreateDXGIFactory");
		Real_CreateDXGIFactory1 = (PFN_CreateDXGIFactory1)GetProcAddress(hRealDXGI, "CreateDXGIFactory1");
		Real_CreateDXGIFactory2 = (PFN_CreateDXGIFactory2)GetProcAddress(hRealDXGI, "CreateDXGIFactory2");
		Real_DXGIGetDebugInterface1 = (PFN_DXGIGetDebugInterface1)GetProcAddress(hRealDXGI, "DXGIGetDebugInterface1");
		Real_DXGIDeclareAdapterRemovalSupport = (PFN_DXGIDeclareAdapterRemovalSupport)GetProcAddress(hRealDXGI, "DXGIDeclareAdapterRemovalSupport");
		Real_DXGIReportAdapterConfiguration = (PFN_DXGIReportAdapterConfiguration)GetProcAddress(hRealDXGI, "DXGIReportAdapterConfiguration");
		Real_ApplyCompatResolutionQuirking = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "ApplyCompatResolutionQuirking");
		Real_CompatString = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "CompatString");
		Real_CompatValue = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "CompatValue");
		Real_DXGIDumpJournal = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "DXGIDumpJournal");
		Real_PIXBeginCapture = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "PIXBeginCapture");
		Real_PIXEndCapture = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "PIXEndCapture");
		Real_PIXGetCaptureState = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "PIXGetCaptureState");
		Real_SetAppCompatStringPointer = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "SetAppCompatStringPointer");
		Real_UpdateHMDEmulationStatus = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "UpdateHMDEmulationStatus");
		Real_DXGID3D10CreateDevice = (PFN_DXGID3D10CreateDevice)GetProcAddress(hRealDXGI, "DXGID3D10CreateDevice");
		Real_DXGID3D10CreateLayeredDevice = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "DXGID3D10CreateLayeredDevice");
		Real_DXGID3D10GetLayeredDeviceSize = (PFN_DXGID3D10GetLayeredDeviceSize)GetProcAddress(hRealDXGI, "DXGID3D10GetLayeredDeviceSize");
		Real_DXGID3D10RegisterLayers = (PFN_VoidFunc)GetProcAddress(hRealDXGI, "DXGID3D10RegisterLayers");

		// Also hook D3D11CreateDeviceAndSwapChain if D3D11 is used
		HookD3D11();

		return hRealDXGI;
	}
}

extern "C" {

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** ppFactory)
{
	dxgi::Load();
	if (!dxgi::Real_CreateDXGIFactory) return E_FAIL;

	HRESULT hr = dxgi::Real_CreateDXGIFactory(riid, ppFactory);
	if (SUCCEEDED(hr) && ppFactory && *ppFactory && Config.EnableDxgiWrapper)
	{
		*ppFactory = new m_IDXGIFactory((IDXGIFactory*)*ppFactory);
	}
	return hr;
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory)
{
	dxgi::Load();
	if (!dxgi::Real_CreateDXGIFactory1) return E_FAIL;

	HRESULT hr = dxgi::Real_CreateDXGIFactory1(riid, ppFactory);
	if (SUCCEEDED(hr) && ppFactory && *ppFactory && Config.EnableDxgiWrapper)
	{
		*ppFactory = new m_IDXGIFactory((IDXGIFactory*)*ppFactory);
	}
	return hr;
}

HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void** ppFactory)
{
	dxgi::Load();
	if (!dxgi::Real_CreateDXGIFactory2) return E_FAIL;

	HRESULT hr = dxgi::Real_CreateDXGIFactory2(Flags, riid, ppFactory);
	if (SUCCEEDED(hr) && ppFactory && *ppFactory && Config.EnableDxgiWrapper)
	{
		*ppFactory = new m_IDXGIFactory((IDXGIFactory*)*ppFactory);
	}
	return hr;
}

HRESULT WINAPI DXGIGetDebugInterface1(UINT Flags, REFIID riid, void** pDebug)
{
	dxgi::Load();
	if (dxgi::Real_DXGIGetDebugInterface1) return dxgi::Real_DXGIGetDebugInterface1(Flags, riid, pDebug);
	return E_FAIL;
}

HRESULT WINAPI DXGIDeclareAdapterRemovalSupport()
{
	dxgi::Load();
	if (dxgi::Real_DXGIDeclareAdapterRemovalSupport) return dxgi::Real_DXGIDeclareAdapterRemovalSupport();
	return S_OK;
}

HRESULT WINAPI DXGIReportAdapterConfiguration()
{
	dxgi::Load();
	if (dxgi::Real_DXGIReportAdapterConfiguration) return dxgi::Real_DXGIReportAdapterConfiguration();
	return S_OK;
}

void WINAPI ApplyCompatResolutionQuirking()
{
	dxgi::Load();
	if (dxgi::Real_ApplyCompatResolutionQuirking) dxgi::Real_ApplyCompatResolutionQuirking();
}

void WINAPI CompatString()
{
	dxgi::Load();
	if (dxgi::Real_CompatString) dxgi::Real_CompatString();
}

void WINAPI CompatValue()
{
	dxgi::Load();
	if (dxgi::Real_CompatValue) dxgi::Real_CompatValue();
}

void WINAPI DXGIDumpJournal()
{
	dxgi::Load();
	if (dxgi::Real_DXGIDumpJournal) dxgi::Real_DXGIDumpJournal();
}

void WINAPI PIXBeginCapture()
{
	dxgi::Load();
	if (dxgi::Real_PIXBeginCapture) dxgi::Real_PIXBeginCapture();
}

void WINAPI PIXEndCapture()
{
	dxgi::Load();
	if (dxgi::Real_PIXEndCapture) dxgi::Real_PIXEndCapture();
}

void WINAPI PIXGetCaptureState()
{
	dxgi::Load();
	if (dxgi::Real_PIXGetCaptureState) dxgi::Real_PIXGetCaptureState();
}

void WINAPI SetAppCompatStringPointer()
{
	dxgi::Load();
	if (dxgi::Real_SetAppCompatStringPointer) dxgi::Real_SetAppCompatStringPointer();
}

void WINAPI UpdateHMDEmulationStatus()
{
	dxgi::Load();
	if (dxgi::Real_UpdateHMDEmulationStatus) dxgi::Real_UpdateHMDEmulationStatus();
}

HRESULT WINAPI DXGID3D10CreateDevice(HMODULE hModule, IDXGIFactory* pFactory, IDXGIAdapter* pAdapter, UINT Flags, void* pUnknown, void** ppDevice)
{
	dxgi::Load();
	if (dxgi::Real_DXGID3D10CreateDevice) return dxgi::Real_DXGID3D10CreateDevice(hModule, pFactory, pAdapter, Flags, pUnknown, ppDevice);
	return E_FAIL;
}

HRESULT WINAPI DXGID3D10CreateLayeredDevice()
{
	dxgi::Load();
	if (dxgi::Real_DXGID3D10CreateLayeredDevice)
	{
		dxgi::Real_DXGID3D10CreateLayeredDevice();
		return S_OK;
	}
	return E_FAIL;
}

size_t WINAPI DXGID3D10GetLayeredDeviceSize()
{
	dxgi::Load();
	if (dxgi::Real_DXGID3D10GetLayeredDeviceSize) return dxgi::Real_DXGID3D10GetLayeredDeviceSize();
	return 0;
}

HRESULT WINAPI DXGID3D10RegisterLayers()
{
	dxgi::Load();
	if (dxgi::Real_DXGID3D10RegisterLayers)
	{
		dxgi::Real_DXGID3D10RegisterLayers();
		return S_OK;
	}
	return E_FAIL;
}

} // extern "C"
