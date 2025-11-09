#pragma once
#include "FeatureTimer.h"
#include "Window.h"

class App {
public:
  App();
  int Go();
  ~App() = default;

private:
  void DoFrame();

private:
  Window wnd;
  FeatureTimer timer;
};