#pragma once 
#include <vector>
#include "spdlog/spdlog.h"
#include "../../main/include/gbengine.h"
namespace retro {
extern std::vector<char> LoadRom(std::string file_path);
Application LoadMainConfig(const std::string& file_path);

}
