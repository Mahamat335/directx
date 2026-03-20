#pragma once
#include "FeatureTimer.h"
#include "ImguiManager.h"
#include "Window.h"

class App {
public:
  App();
  int Go();
  ~App() = default;

private:
  void DoFrame();

private:
  ImguiManager imgui;
  Window wnd;
  FeatureTimer timer;
};