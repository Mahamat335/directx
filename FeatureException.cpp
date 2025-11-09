#include "Header Files/FeatureException.h"
#include <sstream>

FeatureException::FeatureException(int line, const char *file) noexcept
    : line(line), file(file) {}

const char *FeatureException::what() const noexcept {
  std::ostringstream oss;
  oss << GetType() << std::endl << GetOriginString();
  whatBuffer = oss.str();
  return whatBuffer.c_str();
}

const char *FeatureException::GetType() const noexcept {
  return "Feature Exception";
}

int FeatureException::GetLine() const noexcept { return line; }

const std::string &FeatureException::GetFile() const noexcept { return file; }

std::string FeatureException::GetOriginString() const noexcept {
  std::ostringstream oss;
  oss << "[File] " << file << std::endl << "[Line] " << line;
  return oss.str();
}