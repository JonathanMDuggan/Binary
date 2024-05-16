#include "include/gb_emulator.h"
#include <spdlog/spdlog.h>
#include <fstream>
void retro::gb::test() {
  retro::gb::GameBoy gameboy;
  gameboy.sm83_.PrintCurrentProgramCounterValue();
  return;
}
void retro::gb::LoadRom(std::string file_path, void* data) {
  std::string file_extension;
  std::ifstream file(file_path);
  // Failed to find file in OS
  if (!file.is_open()) {
    spdlog::error(
        "Failed to load Gameboy rom from {},"
        "are you sure your ROM is located there?", file_path);
    file.close();
    return; 
  }
  // Now we check if the rom is a supported file extension
  size_t dot_pos = file_path.find_last_of('.');
  if (dot_pos != std::string::npos) {
    file_extension = file_path.substr(dot_pos + 1);
  } else {
    spdlog::error(
      "File doesn't have a file extension: {}, Here are possible solutions:\n"
      "  * Check if the file you're trying to load is the correct file\n"
      "  * Rename the file by appending the file name with .gb\n", file_extension
        
    );
  }
  file.read(file_path.data(), sizeof(data));

}

void retro::gb::Emulate(GameBoy* gameboy, bool running) {
  using namespace retro::gb::instructionset;
  Fetch(&gameboy->sm83_, gameboy); 
}

void retro::gb::instructionset::Fetch(SM83* sm83, GameBoy* gb) {
  
  sm83->idu_                  = sm83->reg_.program_counter_;
  sm83->address_bus_          = sm83->idu_;
  sm83->idu_++;
  sm83->reg_.program_counter_ = sm83->idu_;
  sm83->read_signal_          = true;
  sm83->reg_.instruction_     = gb->memory_[sm83->address_bus_];
  sm83->cycles_++;
  return;
}
