#pragma once

#include <windows.h>
#include <dxgi.h>

namespace dxgi
{
	HMODULE Load(const char* ProxyDll = nullptr, const char* MyDllName = nullptr);
}

extern "C" {
	HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** ppFactory);
	HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory);
	HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void** ppFactory);
	HRESULT WINAPI DXGIGetDebugInterface1(UINT Flags, REFIID riid, void** pDebug);
	HRESULT WINAPI DXGIDeclareAdapterRemovalSupport();
	HRESULT WINAPI DXGIReportAdapterConfiguration();
	void WINAPI ApplyCompatResolutionQuirking();
	void WINAPI CompatString();
	void WINAPI CompatValue();
	void WINAPI DXGIDumpJournal();
	void WINAPI PIXBeginCapture();
	void WINAPI PIXEndCapture();
	void WINAPI PIXGetCaptureState();
	void WINAPI SetAppCompatStringPointer();
	void WINAPI UpdateHMDEmulationStatus();
	HRESULT WINAPI DXGID3D10CreateDevice(HMODULE hModule, IDXGIFactory* pFactory, IDXGIAdapter* pAdapter, UINT Flags, void* pUnknown, void** ppDevice);
	HRESULT WINAPI DXGID3D10CreateLayeredDevice();
	size_t WINAPI DXGID3D10GetLayeredDeviceSize();
	HRESULT WINAPI DXGID3D10RegisterLayers();
}
