#pragma once
#include <string>
#include "gb_processor.h"
#include "gb_memory.h"
#include "gb_disassembly.h"
#include "gb_peripheral.h"
#include "gb_opcode.h"
namespace gbengine {
enum gbVersion { 
  M0 = 0, 
  M1 = 1, 
  M2 = 2
};
class Gameboy {
 public: 
  SM83 sm83_;
  Memory memory_;
  gbVersion gb_version_;
  
  Gameboy(std::string rom_file_path, gbVersion version); 
};

}