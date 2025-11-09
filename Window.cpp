#include "Header Files/Window.h"
#include "Header Files/WindowThrowMacros.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {
  WNDCLASSEX wc = {0};
  wc.cbSize = sizeof(wc);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = Window::HandleMsgSetup;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetInstance();
  wc.hIcon = static_cast<HICON>(LoadImage(nullptr, "Resources/icon_32.ico",
                                          IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
  wc.hCursor = nullptr;
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = GetName();
  wc.hIconSm = static_cast<HICON>(LoadImage(
      nullptr, "Resources/icon_16.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
  RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() { UnregisterClass(wndClassName, hInst); }

const char *Window::WindowClass::GetName() noexcept { return wndClassName; }

HINSTANCE Window::WindowClass::GetInstance() noexcept { return wndClass.hInst; }

Window::Window(int width, int height, const char *name) {
  RECT wr;
  wr.left = 100;
  wr.right = width + wr.left;
  wr.top = 100;
  wr.bottom = height + wr.top;
  AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

  hWnd = CreateWindowEx(0, WindowClass::GetName(), name,
                        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
                        CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
                        nullptr, nullptr, WindowClass::GetInstance(), this);

  ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window() { DestroyWindow(hWnd); }

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) noexcept {
  if (msg == WM_NCCREATE) {
    const CREATESTRUCTW *const pCreate =
        reinterpret_cast<CREATESTRUCTW *>(lParam);
    Window *const pWnd = static_cast<Window *>(pCreate->lpCreateParams);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
    SetWindowLongPtr(hWnd, GWLP_WNDPROC,
                     reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) noexcept {
  Window *const pWnd =
      reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam,
                          LPARAM lParam) noexcept {
  switch (msg) {
  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Exception Stuff
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept {
  char *pMsgBuf = nullptr;
  // windows will allocate memory for err string and make our pointer point to
  // it
  const DWORD nMsgLen = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
  // 0 string length returned indicates a failure
  if (nMsgLen == 0) {
    return "Unidentified error code";
  }
  // copy error string from windows-allocated buffer to std::string
  std::string errorString = pMsgBuf;
  // free windows buffer
  LocalFree(pMsgBuf);
  return errorString;
}

Window::HrException::HrException(int line, const char *file,
                                 HRESULT hr) noexcept
    : Exception(line, file), hr(hr) {}

const char *Window::HrException::what() const noexcept {
  std::ostringstream oss;
  oss << GetType() << std::endl
      << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
      << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
      << "[Description] " << GetErrorDescription() << std::endl
      << GetOriginString();
  whatBuffer = oss.str();
  return whatBuffer.c_str();
}

const char *Window::HrException::GetType() const noexcept {
  return "Feature Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept { return hr; }

std::string Window::HrException::GetErrorDescription() const noexcept {
  return Exception::TranslateErrorCode(hr);
}

const char *Window::NoGfxException::GetType() const noexcept {
  return "Feature Window Exception [No Graphics]";
}

void Window::SetTitle(const std::string &title) {
  if (SetWindowText(hWnd, title.c_str()) == 0) {
    throw CHWND_LAST_EXCEPT();
  }
}

std::optional<int> Window::ProcessMessages() {
  MSG msg;
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      return msg.wParam;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return {};
}