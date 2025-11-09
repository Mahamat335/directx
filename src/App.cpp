#include "App.h"
#include "WindowThrowMacros.h"
#include <sstream>

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
  wnd.Gfx().EndFrame();
}
