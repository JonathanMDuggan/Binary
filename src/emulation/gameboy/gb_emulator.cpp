#include "include/gb_emulator.h"
#include <spdlog/spdlog.h>
#include <fstream>
void binary::gb::test() {
  binary::gb::GameBoy gb;
  return;
}
void binary::gb::LoadRom(std::string file_path, void* data) {
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

void binary::gb::Emulate(GameBoy* gameboy, bool running) {
  using namespace binary::gb::instructionset;
  constexpr uint16_t k_PrefixOffset = 256;
  std::array<Opcode, 512> opcode_table;
  InitOpcodeTable(opcode_table);
  while (running) {
    const uint16_t k_Instruction = 
        gameboy->memory_[gameboy->reg_.program_counter_]; 

    if (gameboy->cb_prefixed == false) {
      opcode_table.at(k_Instruction).execute_(gameboy);
    } else {
      opcode_table.at(k_Instruction + k_PrefixOffset).execute_(gameboy);
      gameboy->cb_prefixed = false;
    }
    Fetch(gameboy); 
  }
}

void binary::gb::instructionset::Fetch(GameBoy* gb) {
  
  gb->idu_                  = gb->reg_.program_counter_;
  gb->address_bus_          = gb->idu_;
  gb->idu_++;                 
  gb->reg_.program_counter_ = gb->idu_;
  gb->read_signal_          = true;
  gb->reg_.instruction_     = gb->memory_[gb->address_bus_];
  gb->cycles_++;
  return;
}
