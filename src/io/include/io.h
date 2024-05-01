#pragma once 
#include <fstream>
#include <array>
#include <vector>
#include "spdlog/spdlog.h"
namespace retro {
extern std::vector<char> LoadRom(std::string file_path);
}
