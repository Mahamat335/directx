#include "App.h"
#include "Box.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <memory>


App::App() : wnd(800, 600, "Feature Engine") {
  std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
  std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 1.0f);
  std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
  std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
  for (auto i = 0; i < 180; i++) {
    boxes.push_back(
        std::make_unique<Box>(wnd.Gfx(), rng, adist, ddist, odist, rdist));
  }
  wnd.Gfx().SetProjection(
      DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

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
  // boxes
  auto dt = timer.Mark();
  wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
  for (auto &b : boxes) {
    b->Update(dt);
    b->Draw(wnd.Gfx());
  }

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
