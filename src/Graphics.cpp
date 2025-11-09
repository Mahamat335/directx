#include "Graphics.h"
#include <d3d11.h>

Graphics::Graphics(HWND hwnd) {
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 0;
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = 1;
  sd.OutputWindow = hwnd;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.Flags = 0;

  // create device and front/back buffers, swap chain and rendering context
  D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwap,
                                &pDevice, nullptr, &pContext);
  // gain access to texture subresource in swap chain (back buffer)
  ID3D11Texture2D *pBackBuffer = nullptr;
  pSwap->GetBuffer(0, __uuidof(ID3D11Resource),
                   reinterpret_cast<void **>(&pBackBuffer));
  pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
  pBackBuffer->Release();
}

Graphics::~Graphics() {
  if (pTarget) {
    pTarget->Release();
  }
  if (pSwap) {
    pSwap->Release();
  }

  if (pContext) {
    pContext->Release();
  }

  if (pDevice) {
    pDevice->Release();
  }
}

void Graphics::EndFrame() { pSwap->Present(1u, 0u); }

void Graphics::ClearBuffer(float r, float g, float b) noexcept {
  const float color[] = {r, g, b, 1.0f};
  pContext->ClearRenderTargetView(pTarget, color);
}