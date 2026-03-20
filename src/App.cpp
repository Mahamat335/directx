#include "App.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

App::App() : wnd(800, 600, "Feature Engine") {}

int App::Go() {
  while (true) {
    if (const auto ecode = Window::ProcessMessages()) {
      return *ecode;
    }
    DoFrame();
    Sleep(1);
  }
  /*
    const auto dt = timer.Mark() * speed_factor;
    HandleInput(dt); */
}

void App::DoFrame() {
  const float t = sin(timer.Peek()) / 2.0f + 0.5f;
  wnd.Gfx().ClearBuffer(t, t, 1.0f);
  wnd.Gfx().DrawTestTriangle(timer.Peek(), wnd.mouse.GetPosX() / 400.0f - 1.0f,
                             -wnd.mouse.GetPosY() / 300.0f + 1.0f);
  wnd.Gfx().DrawTestTriangle(timer.Peek(), 0.0f, 0.0f);

  // imgui
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  static bool show_demo_window = false;

  if (show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  // present
  wnd.Gfx().EndFrame();
}
