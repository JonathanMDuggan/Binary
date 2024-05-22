#include "include/chip8_processor.h"
#include "spdlog/spdlog.h"
#include <vector>
namespace binary::chip8 {
Register::Register() {
  program_counter_ = k_StackBase;
  status_ = &general_purpose_[k_VF];
}
Chip8::Chip8(Register* reg_) { 
  if (reg_  == nullptr) {
    spdlog::error(
      "Register type is NULL!: {}.\nInitialize reg_ before passing it", (size_t)reg_);
  }
  // Initialize memory space for chip8 4k Memory layout
  // +------------------+ 0xFFF
  // | Chip-8 Program   |
  // | Data Space       |
  // |                  |
  // |                  |
  // |                  |
  // |                  |
  // |                  |
  // |                  |
  // |                  |
  // |                  |
  // |                  |
  // +------------------+ 0x200
  // | Nothing Space    |
  // +------------------+ 0x080
  // | Font Set Space   |
  // |                  |
  // +------------------+ 0x000
  std::vector<uint8_t> k_FontSetData = {
    // 0
    0b01100000,
    0b10010000,
    0b10010000,
    0b10010000,
    0b01100000,
    // 1
    0b01000000,
    0b11000000,
    0b01000000,
    0b01000000,
    0b11100000,
    // 2
    0b11110000,
    0b00010000,
    0b11110000,
    0b10000000,
    0b11110000,
    // 3
    0b11110000,
    0b00010000,
    0b11110000,
    0b00010000,
    0b11110000,
    // 4
    0b10010000,
    0b10010000,
    0b11110000,
    0b00010000,
    0b00010000,
    // 5
    0b11110000,
    0b10000000,
    0b11110000,
    0b00010000,
    0b11110000,
    // 6
    0b11110000,
    0b10000000,
    0b11110000,
    0b10010000,
    0b11110000,
    // 7
    0b11110000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b01000000,
    // 8
    0b11110000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b11110000,
    // 9
    0b11110000,
    0b10010000,
    0b11110000,
    0b00010000,
    0b11110000,
    // A
    0b11110000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b10010000,
    // B
    0b11100000,
    0b10010000,
    0b11100000,
    0b10010000,
    0b11100000,
    // C
    0b11110000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b11110000,
    // D
    0b11100000,
    0b10010000,
    0b10010000,
    0b10010000,
    0b11100000,
    // E
    0b11110000,
    0b10000000,
    0b11110000,
    0b10000000,
    0b11110000,
    // F
    0b11110000,
    0b10000000,
    0b11110000,
    0b10000000,
    0b10000000
  }; 
  std::memcpy(memory_.data(), k_FontSetData.data(), k_FontSetData.size());
  this->reg_ = reg_;
}
}