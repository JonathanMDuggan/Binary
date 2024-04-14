#pragma once
#include "include/gb_emulator.h"

gbengine::Gameboy::Gameboy(std::string rom_file_path, gbVersion version)
    : sm83_(version),  
  memory_(rom_file_path, &sm83_){

}