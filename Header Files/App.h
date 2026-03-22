#pragma once
#include "Box.h"
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
  std::vector<std::unique_ptr<class Box>> boxes;
};