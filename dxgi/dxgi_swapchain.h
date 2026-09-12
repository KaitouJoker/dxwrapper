#pragma once

#include <windows.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <d3d11.h>
#include <atomic>

void ApplyDeviceLatencyOptimizations(IUnknown* pDevice);
void HookD3D11();

class m_IDXGISwapChain : public IDXGISwapChain4
{
private:
	IDXGISwapChain* m_pReal = nullptr;
	IDXGISwapChain1* m_pReal1 = nullptr;
	IDXGISwapChain2* m_pReal2 = nullptr;
	IDXGISwapChain3* m_pReal3 = nullptr;
	IDXGISwapChain4* m_pReal4 = nullptr;
	IUnknown* m_pDevice = nullptr;
	std::atomic<ULONG> m_RefCount = 1;

public:
	m_IDXGISwapChain(IDXGISwapChain* pReal, IUnknown* pDevice);
	virtual ~m_IDXGISwapChain();

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override;
	STDMETHOD_(ULONG, AddRef)() override;
	STDMETHOD_(ULONG, Release)() override;

	// IDXGIObject
	STDMETHOD(SetPrivateData)(REFGUID Name, UINT DataSize, const void* pData) override;
	STDMETHOD(SetPrivateDataInterface)(REFGUID Name, const IUnknown* pUnknown) override;
	STDMETHOD(GetPrivateData)(REFGUID Name, UINT* pDataSize, void* pData) override;
	STDMETHOD(GetParent)(REFIID riid, void** ppParent) override;

	// IDXGIDeviceSubObject
	STDMETHOD(GetDevice)(REFIID riid, void** ppDevice) override;

	// IDXGISwapChain
	STDMETHOD(Present)(UINT SyncInterval, UINT Flags) override;
	STDMETHOD(GetBuffer)(UINT Buffer, REFIID riid, void** ppSurface) override;
	STDMETHOD(SetFullscreenState)(BOOL Fullscreen, IDXGIOutput* pTarget) override;
	STDMETHOD(GetFullscreenState)(BOOL* pFullscreen, IDXGIOutput** ppTarget) override;
	STDMETHOD(GetDesc)(DXGI_SWAP_CHAIN_DESC* pDesc) override;
	STDMETHOD(ResizeBuffers)(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) override;
	STDMETHOD(ResizeTarget)(const DXGI_MODE_DESC* pNewTargetParameters) override;
	STDMETHOD(GetContainingOutput)(IDXGIOutput** ppOutput) override;
	STDMETHOD(GetFrameStatistics)(DXGI_FRAME_STATISTICS* pStats) override;
	STDMETHOD(GetLastPresentCount)(UINT* pLastPresentCount) override;

	// IDXGISwapChain1
	STDMETHOD(GetDesc1)(DXGI_SWAP_CHAIN_DESC1* pDesc) override;
	STDMETHOD(GetFullscreenDesc)(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pDesc) override;
	STDMETHOD(GetHwnd)(HWND* pHwnd) override;
	STDMETHOD(GetCoreWindow)(REFIID refiid, void** ppUnk) override;
	STDMETHOD(Present1)(UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters) override;
	STDMETHOD_(BOOL, IsTemporaryMonoSupported)() override;
	STDMETHOD(GetRestrictToOutput)(IDXGIOutput** ppRestrictToOutput) override;
	STDMETHOD(SetBackgroundColor)(const DXGI_RGBA* pColor) override;
	STDMETHOD(GetBackgroundColor)(DXGI_RGBA* pColor) override;
	STDMETHOD(SetRotation)(DXGI_MODE_ROTATION Rotation) override;
	STDMETHOD(GetRotation)(DXGI_MODE_ROTATION* pRotation) override;

	// IDXGISwapChain2
	STDMETHOD(SetSourceSize)(UINT Width, UINT Height) override;
	STDMETHOD(GetSourceSize)(UINT* pWidth, UINT* pHeight) override;
	STDMETHOD(SetMaximumFrameLatency)(UINT MaxLatency) override;
	STDMETHOD(GetMaximumFrameLatency)(UINT* pMaxLatency) override;
	STDMETHOD_(HANDLE, GetFrameLatencyWaitableObject)() override;
	STDMETHOD(SetMatrixTransform)(const DXGI_MATRIX_3X2_F* pMatrix) override;
	STDMETHOD(GetMatrixTransform)(DXGI_MATRIX_3X2_F* pMatrix) override;

	// IDXGISwapChain3
	STDMETHOD_(UINT, GetCurrentBackBufferIndex)() override;
	STDMETHOD(CheckColorSpaceSupport)(DXGI_COLOR_SPACE_TYPE ColorSpace, UINT* pColorSpaceSupport) override;
	STDMETHOD(SetColorSpace1)(DXGI_COLOR_SPACE_TYPE ColorSpace) override;
	STDMETHOD(ResizeBuffers1)(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue) override;

	// IDXGISwapChain4
	STDMETHOD(SetHDRMetaData)(DXGI_HDR_METADATA_TYPE Type, UINT Size, void* pMetaData) override;
};

class m_IDXGIFactory : public IDXGIFactory2
{
private:
	IDXGIFactory* m_pReal = nullptr;
	IDXGIFactory1* m_pReal1 = nullptr;
	IDXGIFactory2* m_pReal2 = nullptr;
	std::atomic<ULONG> m_RefCount = 1;

public:
	m_IDXGIFactory(IDXGIFactory* pReal);
	virtual ~m_IDXGIFactory();

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override;
	STDMETHOD_(ULONG, AddRef)() override;
	STDMETHOD_(ULONG, Release)() override;

	// IDXGIObject
	STDMETHOD(SetPrivateData)(REFGUID Name, UINT DataSize, const void* pData) override;
	STDMETHOD(SetPrivateDataInterface)(REFGUID Name, const IUnknown* pUnknown) override;
	STDMETHOD(GetPrivateData)(REFGUID Name, UINT* pDataSize, void* pData) override;
	STDMETHOD(GetParent)(REFIID riid, void** ppParent) override;

	// IDXGIFactory
	STDMETHOD(EnumAdapters)(UINT Adapter, IDXGIAdapter** ppAdapter) override;
	STDMETHOD(MakeWindowAssociation)(HWND WindowHandle, UINT Flags) override;
	STDMETHOD(GetWindowAssociation)(HWND* pWindowHandle) override;
	STDMETHOD(CreateSwapChain)(IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain) override;
	STDMETHOD(CreateSoftwareAdapter)(HMODULE Module, IDXGIAdapter** ppAdapter) override;

	// IDXGIFactory1
	STDMETHOD(EnumAdapters1)(UINT Adapter, IDXGIAdapter1** ppAdapter) override;
	STDMETHOD_(BOOL, IsCurrent)() override;

	// IDXGIFactory2
	STDMETHOD_(BOOL, IsWindowedStereoEnabled)() override;
	STDMETHOD(CreateSwapChainForHwnd)(IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override;
	STDMETHOD(CreateSwapChainForCoreWindow)(IUnknown* pDevice, IUnknown* pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override;
	STDMETHOD(GetSharedResourceAdapterLuid)(HANDLE hResource, LUID* pLuid) override;
	STDMETHOD(RegisterStereoStatusWindow)(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie) override;
	STDMETHOD(RegisterStereoStatusEvent)(HANDLE hEvent, DWORD* pdwCookie) override;
	STDMETHOD_(void, UnregisterStereoStatus)(DWORD dwCookie) override;
	STDMETHOD(RegisterOcclusionStatusWindow)(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie) override;
	STDMETHOD(RegisterOcclusionStatusEvent)(HANDLE hEvent, DWORD* pdwCookie) override;
	STDMETHOD_(void, UnregisterOcclusionStatus)(DWORD dwCookie) override;
	STDMETHOD(CreateSwapChainForComposition)(IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override;
};
