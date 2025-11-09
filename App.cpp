#include "Header Files/App.h"
#include "Header Files/WindowThrowMacros.h"
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
  const float t = timer.Peek();
  std::ostringstream oss;
  oss << "Feature Engine - " << t << " FPS";
  wnd.SetTitle(oss.str());
}
