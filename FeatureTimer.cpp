#include "Header Files/FeatureTimer.h"

using namespace std::chrono;

FeatureTimer::FeatureTimer() noexcept : last(steady_clock::now()) {}

float FeatureTimer::Mark() noexcept {
  const auto old = last;
  last = steady_clock::now();
  const duration<float> frameTime = last - old;
  return frameTime.count();
}

float FeatureTimer::Peek() const noexcept {
  const auto now = steady_clock::now();
  const duration<float> frameTime = now - last;
  return frameTime.count();
}