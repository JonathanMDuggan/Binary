#pragma once 
#include <vector>
#include "../../types/include/enums.h"
#include "../../main/include/gbengine.h"
#include <spdlog/spdlog.h>
#define BINARY_LOG_ERROR(message) \
  spdlog::error("[BINARY] {}:({}) {} - {}", __FILE__, __LINE__, __FUNCTION__, message)
#define BINARY_LOG_WARN(message) \
  spdlog::warn("[BINARY] {}:({}) {} - {}", __FILE__, __LINE__, __FUNCTION__, message)
#define BINARY_LOG_INFO(message) \
  spdlog::info("[BINARY] {}:({}) {} - {}", __FILE__, __LINE__, __FUNCTION__, message)
#define BINARY_LOG_DEBUG(message) \
  spdlog::debug("[BINARY] {}:({}) {} - {}", __FILE__, __LINE__, __FUNCTION__, message)
#define BINARY_LOG_TRACE(message) \
  spdlog::trace("[BINARY] {}:({}) {} - {}", __FILE__, __LINE__, __FUNCTION__, message)

namespace binary {
extern std::vector<char> LoadRom(std::string file_path);
Result LoadMainConfig(const std::string& file_path, Application* app);
Result SetupGlobalLoggers();
}
