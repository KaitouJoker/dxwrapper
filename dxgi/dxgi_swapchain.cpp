#include "dxgi_swapchain.h"
#include "Settings\Settings.h"
#include "Logging\Logging.h"
#include "Utils\Utils.h"
#include "External\detours\src\detours.h"

void ApplyDeviceLatencyOptimizations(IUnknown* pDevice)
{
	if (!pDevice) return;

	IDXGIDevice1* pDXGIDevice1 = nullptr;
	if (SUCCEEDED(pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDevice1)) && pDXGIDevice1)
	{
		if (Config.MaxFrameLatency > 0)
		{
			pDXGIDevice1->SetMaximumFrameLatency(Config.MaxFrameLatency);
			Logging::Log() << "IDXGIDevice1::SetMaximumFrameLatency(" << Config.MaxFrameLatency << ") applied.";
		}
		if (Config.SetGPUThreadPriority >= 0 && Config.SetGPUThreadPriority <= 7)
		{
			pDXGIDevice1->SetGPUThreadPriority(Config.SetGPUThreadPriority);
			Logging::Log() << "IDXGIDevice1::SetGPUThreadPriority(" << Config.SetGPUThreadPriority << ") applied.";
		}
		pDXGIDevice1->Release();
	}
}

// ======================================================================
// m_IDXGISwapChain Implementation
// ======================================================================

m_IDXGISwapChain::m_IDXGISwapChain(IDXGISwapChain* pReal, IUnknown* pDevice)
	: m_pReal(pReal), m_pDevice(pDevice), m_RefCount(1)
{
	if (m_pReal)
	{
		m_pReal->QueryInterface(__uuidof(IDXGISwapChain1), (void**)&m_pReal1);
		m_pReal->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&m_pReal2);
		m_pReal->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_pReal3);
		m_pReal->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&m_pReal4);
	}
	if (m_pDevice)
	{
		m_pDevice->AddRef();
	}
}

m_IDXGISwapChain::~m_IDXGISwapChain()
{
	if (m_pReal4) m_pReal4->Release();
	if (m_pReal3) m_pReal3->Release();
	if (m_pReal2) m_pReal2->Release();
	if (m_pReal1) m_pReal1->Release();
	if (m_pReal) m_pReal->Release();
	if (m_pDevice) m_pDevice->Release();
}

STDMETHODIMP m_IDXGISwapChain::QueryInterface(REFIID riid, void** ppvObject)
{
	if (!ppvObject) return E_POINTER;

	if (riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDXGIObject) ||
		riid == __uuidof(IDXGIDeviceSubObject) ||
		riid == __uuidof(IDXGISwapChain) ||
		riid == __uuidof(IDXGISwapChain1) ||
		riid == __uuidof(IDXGISwapChain2) ||
		riid == __uuidof(IDXGISwapChain3) ||
		riid == __uuidof(IDXGISwapChain4))
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}

	return m_pReal->QueryInterface(riid, ppvObject);
}

STDMETHODIMP_(ULONG) m_IDXGISwapChain::AddRef()
{
	return ++m_RefCount;
}

STDMETHODIMP_(ULONG) m_IDXGISwapChain::Release()
{
	ULONG ref = --m_RefCount;
	if (ref == 0)
	{
		delete this;
	}
	return ref;
}

STDMETHODIMP m_IDXGISwapChain::SetPrivateData(REFGUID Name, UINT DataSize, const void* pData)
{
	return m_pReal->SetPrivateData(Name, DataSize, pData);
}

STDMETHODIMP m_IDXGISwapChain::SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown)
{
	return m_pReal->SetPrivateDataInterface(Name, pUnknown);
}

STDMETHODIMP m_IDXGISwapChain::GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData)
{
	return m_pReal->GetPrivateData(Name, pDataSize, pData);
}

STDMETHODIMP m_IDXGISwapChain::GetParent(REFIID riid, void** ppParent)
{
	return m_pReal->GetParent(riid, ppParent);
}

STDMETHODIMP m_IDXGISwapChain::GetDevice(REFIID riid, void** ppDevice)
{
	if (m_pDevice)
	{
		return m_pDevice->QueryInterface(riid, ppDevice);
	}
	return m_pReal->GetDevice(riid, ppDevice);
}

STDMETHODIMP m_IDXGISwapChain::Present(UINT SyncInterval, UINT Flags)
{
	Utils::OptimizeRenderThread();

	if (SyncInterval == 0 && Config.AllowTearing)
	{
		Flags |= DXGI_PRESENT_ALLOW_TEARING;
	}

	if (Config.LimitPerFrameFPS > 0)
	{
		Utils::ApplyPacingLimit(Config.LimitPerFrameFPS);
	}

	return m_pReal->Present(SyncInterval, Flags);
}

STDMETHODIMP m_IDXGISwapChain::GetBuffer(UINT Buffer, REFIID riid, void** ppSurface)
{
	return m_pReal->GetBuffer(Buffer, riid, ppSurface);
}

STDMETHODIMP m_IDXGISwapChain::SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget)
{
	return m_pReal->SetFullscreenState(Fullscreen, pTarget);
}

STDMETHODIMP m_IDXGISwapChain::GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget)
{
	return m_pReal->GetFullscreenState(pFullscreen, ppTarget);
}

STDMETHODIMP m_IDXGISwapChain::GetDesc(DXGI_SWAP_CHAIN_DESC* pDesc)
{
	return m_pReal->GetDesc(pDesc);
}

STDMETHODIMP m_IDXGISwapChain::ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	if (Config.AllowTearing)
	{
		SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	}
	return m_pReal->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

STDMETHODIMP m_IDXGISwapChain::ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters)
{
	return m_pReal->ResizeTarget(pNewTargetParameters);
}

STDMETHODIMP m_IDXGISwapChain::GetContainingOutput(IDXGIOutput** ppOutput)
{
	return m_pReal->GetContainingOutput(ppOutput);
}

STDMETHODIMP m_IDXGISwapChain::GetFrameStatistics(DXGI_FRAME_STATISTICS* pStats)
{
	return m_pReal->GetFrameStatistics(pStats);
}

STDMETHODIMP m_IDXGISwapChain::GetLastPresentCount(UINT* pLastPresentCount)
{
	return m_pReal->GetLastPresentCount(pLastPresentCount);
}

STDMETHODIMP m_IDXGISwapChain::GetDesc1(DXGI_SWAP_CHAIN_DESC1* pDesc)
{
	if (m_pReal1) return m_pReal1->GetDesc1(pDesc);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pDesc)
{
	if (m_pReal1) return m_pReal1->GetFullscreenDesc(pDesc);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetHwnd(HWND* pHwnd)
{
	if (m_pReal1) return m_pReal1->GetHwnd(pHwnd);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetCoreWindow(REFIID refiid, void** ppUnk)
{
	if (m_pReal1) return m_pReal1->GetCoreWindow(refiid, ppUnk);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::Present1(UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
	Utils::OptimizeRenderThread();

	if (SyncInterval == 0 && Config.AllowTearing)
	{
		PresentFlags |= DXGI_PRESENT_ALLOW_TEARING;
	}

	if (Config.LimitPerFrameFPS > 0)
	{
		Utils::ApplyPacingLimit(Config.LimitPerFrameFPS);
	}

	if (m_pReal1)
	{
		return m_pReal1->Present1(SyncInterval, PresentFlags, pPresentParameters);
	}
	return m_pReal->Present(SyncInterval, PresentFlags);
}

STDMETHODIMP_(BOOL) m_IDXGISwapChain::IsTemporaryMonoSupported()
{
	if (m_pReal1) return m_pReal1->IsTemporaryMonoSupported();
	return FALSE;
}

STDMETHODIMP m_IDXGISwapChain::GetRestrictToOutput(IDXGIOutput** ppRestrictToOutput)
{
	if (m_pReal1) return m_pReal1->GetRestrictToOutput(ppRestrictToOutput);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::SetBackgroundColor(const DXGI_RGBA* pColor)
{
	if (m_pReal1) return m_pReal1->SetBackgroundColor(pColor);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetBackgroundColor(DXGI_RGBA* pColor)
{
	if (m_pReal1) return m_pReal1->GetBackgroundColor(pColor);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::SetRotation(DXGI_MODE_ROTATION Rotation)
{
	if (m_pReal1) return m_pReal1->SetRotation(Rotation);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetRotation(DXGI_MODE_ROTATION* pRotation)
{
	if (m_pReal1) return m_pReal1->GetRotation(pRotation);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::SetSourceSize(UINT Width, UINT Height)
{
	if (m_pReal2) return m_pReal2->SetSourceSize(Width, Height);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetSourceSize(UINT* pWidth, UINT* pHeight)
{
	if (m_pReal2) return m_pReal2->GetSourceSize(pWidth, pHeight);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::SetMaximumFrameLatency(UINT MaxLatency)
{
	if (m_pReal2) return m_pReal2->SetMaximumFrameLatency(MaxLatency);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetMaximumFrameLatency(UINT* pMaxLatency)
{
	if (m_pReal2) return m_pReal2->GetMaximumFrameLatency(pMaxLatency);
	return E_NOINTERFACE;
}

STDMETHODIMP_(HANDLE) m_IDXGISwapChain::GetFrameLatencyWaitableObject()
{
	if (m_pReal2) return m_pReal2->GetFrameLatencyWaitableObject();
	return nullptr;
}

STDMETHODIMP m_IDXGISwapChain::SetMatrixTransform(const DXGI_MATRIX_3X2_F* pMatrix)
{
	if (m_pReal2) return m_pReal2->SetMatrixTransform(pMatrix);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::GetMatrixTransform(DXGI_MATRIX_3X2_F* pMatrix)
{
	if (m_pReal2) return m_pReal2->GetMatrixTransform(pMatrix);
	return E_NOINTERFACE;
}

STDMETHODIMP_(UINT) m_IDXGISwapChain::GetCurrentBackBufferIndex()
{
	if (m_pReal3) return m_pReal3->GetCurrentBackBufferIndex();
	return 0;
}

STDMETHODIMP m_IDXGISwapChain::CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE ColorSpace, UINT* pColorSpaceSupport)
{
	if (m_pReal3) return m_pReal3->CheckColorSpaceSupport(ColorSpace, pColorSpaceSupport);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::SetColorSpace1(DXGI_COLOR_SPACE_TYPE ColorSpace)
{
	if (m_pReal3) return m_pReal3->SetColorSpace1(ColorSpace);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGISwapChain::ResizeBuffers1(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue)
{
	if (Config.AllowTearing)
	{
		SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	}
	if (m_pReal3) return m_pReal3->ResizeBuffers1(BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask, ppPresentQueue);
	return m_pReal->ResizeBuffers(BufferCount, Width, Height, Format, SwapChainFlags);
}

STDMETHODIMP m_IDXGISwapChain::SetHDRMetaData(DXGI_HDR_METADATA_TYPE Type, UINT Size, void* pMetaData)
{
	if (m_pReal4) return m_pReal4->SetHDRMetaData(Type, Size, pMetaData);
	return E_NOINTERFACE;
}

// ======================================================================
// m_IDXGIFactory Implementation
// ======================================================================

m_IDXGIFactory::m_IDXGIFactory(IDXGIFactory* pReal)
	: m_pReal(pReal), m_RefCount(1)
{
	if (m_pReal)
	{
		m_pReal->QueryInterface(__uuidof(IDXGIFactory1), (void**)&m_pReal1);
		m_pReal->QueryInterface(__uuidof(IDXGIFactory2), (void**)&m_pReal2);
	}
}

m_IDXGIFactory::~m_IDXGIFactory()
{
	if (m_pReal2) m_pReal2->Release();
	if (m_pReal1) m_pReal1->Release();
	if (m_pReal) m_pReal->Release();
}

STDMETHODIMP m_IDXGIFactory::QueryInterface(REFIID riid, void** ppvObject)
{
	if (!ppvObject) return E_POINTER;

	if (riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDXGIObject) ||
		riid == __uuidof(IDXGIFactory) ||
		riid == __uuidof(IDXGIFactory1) ||
		riid == __uuidof(IDXGIFactory2))
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}

	return m_pReal->QueryInterface(riid, ppvObject);
}

STDMETHODIMP_(ULONG) m_IDXGIFactory::AddRef()
{
	return ++m_RefCount;
}

STDMETHODIMP_(ULONG) m_IDXGIFactory::Release()
{
	ULONG ref = --m_RefCount;
	if (ref == 0)
	{
		delete this;
	}
	return ref;
}

STDMETHODIMP m_IDXGIFactory::SetPrivateData(REFGUID Name, UINT DataSize, const void* pData)
{
	return m_pReal->SetPrivateData(Name, DataSize, pData);
}

STDMETHODIMP m_IDXGIFactory::SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown)
{
	return m_pReal->SetPrivateDataInterface(Name, pUnknown);
}

STDMETHODIMP m_IDXGIFactory::GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData)
{
	return m_pReal->GetPrivateData(Name, pDataSize, pData);
}

STDMETHODIMP m_IDXGIFactory::GetParent(REFIID riid, void** ppParent)
{
	return m_pReal->GetParent(riid, ppParent);
}

STDMETHODIMP m_IDXGIFactory::EnumAdapters(UINT Adapter, IDXGIAdapter** ppAdapter)
{
	return m_pReal->EnumAdapters(Adapter, ppAdapter);
}

STDMETHODIMP m_IDXGIFactory::MakeWindowAssociation(HWND WindowHandle, UINT Flags)
{
	return m_pReal->MakeWindowAssociation(WindowHandle, Flags);
}

STDMETHODIMP m_IDXGIFactory::GetWindowAssociation(HWND* pWindowHandle)
{
	return m_pReal->GetWindowAssociation(pWindowHandle);
}

STDMETHODIMP m_IDXGIFactory::CreateSwapChain(IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	if (!pDesc || !ppSwapChain)
	{
		return m_pReal->CreateSwapChain(pDevice, pDesc, ppSwapChain);
	}

	DXGI_SWAP_CHAIN_DESC desc = *pDesc;

	if (Config.ForceFlipModel)
	{
		if (desc.SwapEffect == DXGI_SWAP_EFFECT_DISCARD || desc.SwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
		{
			Logging::Log() << "Upgrading swapchain from Blt model (" << desc.SwapEffect << ") to DXGI_SWAP_EFFECT_FLIP_DISCARD";
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			if (desc.BufferCount < 2)
			{
				desc.BufferCount = 2;
			}
			if (Config.AllowTearing)
			{
				desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
	}

	IDXGISwapChain* pRealSC = nullptr;
	HRESULT hr = m_pReal->CreateSwapChain(pDevice, &desc, &pRealSC);
	if (FAILED(hr) && Config.ForceFlipModel)
	{
		Logging::Log() << "FLIP_DISCARD failed (0x" << std::hex << hr << "). Falling back to original swapchain settings.";
		hr = m_pReal->CreateSwapChain(pDevice, pDesc, &pRealSC);
	}

	if (SUCCEEDED(hr) && pRealSC)
	{
		ApplyDeviceLatencyOptimizations(pDevice);
		*ppSwapChain = new m_IDXGISwapChain(pRealSC, pDevice);
		return hr;
	}

	*ppSwapChain = pRealSC;
	return hr;
}

STDMETHODIMP m_IDXGIFactory::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter** ppAdapter)
{
	return m_pReal->CreateSoftwareAdapter(Module, ppAdapter);
}

STDMETHODIMP m_IDXGIFactory::EnumAdapters1(UINT Adapter, IDXGIAdapter1** ppAdapter)
{
	if (m_pReal1) return m_pReal1->EnumAdapters1(Adapter, ppAdapter);
	return E_NOINTERFACE;
}

STDMETHODIMP_(BOOL) m_IDXGIFactory::IsCurrent()
{
	if (m_pReal1) return m_pReal1->IsCurrent();
	return FALSE;
}

STDMETHODIMP_(BOOL) m_IDXGIFactory::IsWindowedStereoEnabled()
{
	if (m_pReal2) return m_pReal2->IsWindowedStereoEnabled();
	return FALSE;
}

STDMETHODIMP m_IDXGIFactory::CreateSwapChainForHwnd(IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
	if (!m_pReal2) return E_NOINTERFACE;
	if (!pDesc || !ppSwapChain)
	{
		return m_pReal2->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
	}

	DXGI_SWAP_CHAIN_DESC1 desc1 = *pDesc;

	if (Config.ForceFlipModel)
	{
		if (desc1.SwapEffect == DXGI_SWAP_EFFECT_DISCARD || desc1.SwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
		{
			Logging::Log() << "Upgrading SwapChainForHwnd to DXGI_SWAP_EFFECT_FLIP_DISCARD";
			desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			if (desc1.BufferCount < 2)
			{
				desc1.BufferCount = 2;
			}
			if (Config.AllowTearing)
			{
				desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}
			desc1.SampleDesc.Count = 1;
			desc1.SampleDesc.Quality = 0;
		}
	}

	IDXGISwapChain1* pRealSC1 = nullptr;
	HRESULT hr = m_pReal2->CreateSwapChainForHwnd(pDevice, hWnd, &desc1, pFullscreenDesc, pRestrictToOutput, &pRealSC1);
	if (FAILED(hr) && Config.ForceFlipModel)
	{
		Logging::Log() << "CreateSwapChainForHwnd FLIP_DISCARD failed. Falling back.";
		hr = m_pReal2->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, &pRealSC1);
	}

	if (SUCCEEDED(hr) && pRealSC1)
	{
		ApplyDeviceLatencyOptimizations(pDevice);
		*ppSwapChain = new m_IDXGISwapChain(pRealSC1, pDevice);
		return hr;
	}

	*ppSwapChain = pRealSC1;
	return hr;
}

STDMETHODIMP m_IDXGIFactory::CreateSwapChainForCoreWindow(IUnknown* pDevice, IUnknown* pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
	if (m_pReal2) return m_pReal2->CreateSwapChainForCoreWindow(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGIFactory::GetSharedResourceAdapterLuid(HANDLE hResource, LUID* pLuid)
{
	if (m_pReal2) return m_pReal2->GetSharedResourceAdapterLuid(hResource, pLuid);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGIFactory::RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie)
{
	if (m_pReal2) return m_pReal2->RegisterStereoStatusWindow(WindowHandle, wMsg, pdwCookie);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGIFactory::RegisterStereoStatusEvent(HANDLE hEvent, DWORD* pdwCookie)
{
	if (m_pReal2) return m_pReal2->RegisterStereoStatusEvent(hEvent, pdwCookie);
	return E_NOINTERFACE;
}

STDMETHODIMP_(void) m_IDXGIFactory::UnregisterStereoStatus(DWORD dwCookie)
{
	if (m_pReal2) m_pReal2->UnregisterStereoStatus(dwCookie);
}

STDMETHODIMP m_IDXGIFactory::RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie)
{
	if (m_pReal2) return m_pReal2->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	return E_NOINTERFACE;
}

STDMETHODIMP m_IDXGIFactory::RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD* pdwCookie)
{
	if (m_pReal2) return m_pReal2->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
	return E_NOINTERFACE;
}

STDMETHODIMP_(void) m_IDXGIFactory::UnregisterOcclusionStatus(DWORD dwCookie)
{
	if (m_pReal2) m_pReal2->UnregisterOcclusionStatus(dwCookie);
}

STDMETHODIMP m_IDXGIFactory::CreateSwapChainForComposition(IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
	if (m_pReal2) return m_pReal2->CreateSwapChainForComposition(pDevice, pDesc, pRestrictToOutput, ppSwapChain);
	return E_NOINTERFACE;
}

// ======================================================================
// D3D11CreateDeviceAndSwapChain Hook
// ======================================================================

typedef HRESULT(WINAPI* PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)(
	IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
	const D3D_FEATURE_LEVEL*, UINT, UINT,
	const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
	ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

static PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN TrueD3D11CreateDeviceAndSwapChain = nullptr;

static HRESULT WINAPI Hooked_D3D11CreateDeviceAndSwapChain(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	IDXGISwapChain** ppSwapChain,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext)
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	const DXGI_SWAP_CHAIN_DESC* pActualDesc = pSwapChainDesc;

	if (pSwapChainDesc && Config.ForceFlipModel)
	{
		desc = *pSwapChainDesc;
		if (desc.SwapEffect == DXGI_SWAP_EFFECT_DISCARD || desc.SwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
		{
			Logging::Log() << "Hooked_D3D11CreateDeviceAndSwapChain: Upgrading to DXGI_SWAP_EFFECT_FLIP_DISCARD";
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			if (desc.BufferCount < 2) desc.BufferCount = 2;
			if (Config.AllowTearing) desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			pActualDesc = &desc;
		}
	}

	HRESULT hr = TrueD3D11CreateDeviceAndSwapChain(
		pAdapter, DriverType, Software, Flags,
		pFeatureLevels, FeatureLevels, SDKVersion,
		pActualDesc, ppSwapChain, ppDevice,
		pFeatureLevel, ppImmediateContext);

	if (FAILED(hr) && pActualDesc == &desc)
	{
		hr = TrueD3D11CreateDeviceAndSwapChain(
			pAdapter, DriverType, Software, Flags,
			pFeatureLevels, FeatureLevels, SDKVersion,
			pSwapChainDesc, ppSwapChain, ppDevice,
			pFeatureLevel, ppImmediateContext);
	}

	if (SUCCEEDED(hr))
	{
		if (ppDevice && *ppDevice)
		{
			ApplyDeviceLatencyOptimizations(*ppDevice);
		}
		if (ppSwapChain && *ppSwapChain)
		{
			IUnknown* dev = ppDevice ? *ppDevice : nullptr;
			*ppSwapChain = new m_IDXGISwapChain(*ppSwapChain, dev);
		}
	}

	return hr;
}

void HookD3D11()
{
	static bool s_D3D11Hooked = false;
	if (s_D3D11Hooked) return;

	HMODULE hD3D11 = GetModuleHandleA("d3d11.dll");
	if (!hD3D11)
	{
		hD3D11 = LoadLibraryA("d3d11.dll");
	}

	if (hD3D11)
	{
		TrueD3D11CreateDeviceAndSwapChain = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)GetProcAddress(hD3D11, "D3D11CreateDeviceAndSwapChain");
		if (TrueD3D11CreateDeviceAndSwapChain)
		{
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)TrueD3D11CreateDeviceAndSwapChain, Hooked_D3D11CreateDeviceAndSwapChain);
			LONG err = DetourTransactionCommit();
			if (err == NO_ERROR)
			{
				s_D3D11Hooked = true;
				Logging::Log() << "Successfully hooked D3D11CreateDeviceAndSwapChain via Detours.";
			}
			else
			{
				Logging::Log() << "Failed to hook D3D11CreateDeviceAndSwapChain. Error: " << err;
			}
		}
	}
}
