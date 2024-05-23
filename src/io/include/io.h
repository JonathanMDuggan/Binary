#pragma once 
#include <vector>
#include "../../types/include/enums.h"
#include "../../main/include/gbengine.h"
namespace binary {
extern std::vector<char> LoadRom(std::string file_path);
Result LoadMainConfig(const std::string& file_path, Application* app);
Result SetupGlobalLoggers();
}
