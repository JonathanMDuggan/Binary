#pragma once
#include <string>
#include <vector>
#include <functional>
#include "gb_emulator.h"
#include "../../io/include/logger.h"
namespace gbengine {
typedef struct Cycles {
  uint8_t best_case; 
  uint8_t worst_case;
};
typedef struct InstructionSet {
  uint16_t opcode;
  Cycles cycle_time;
  std::string mnemonic;
  bool prefix_cb;
  std::function<void(Gameboy)> excute;
}InstructionSet;
extern Error InitOpcodeLoopUpTable(
    std::array<InstructionSet, 512>* kOpcodeLookUpTable); 
extern Error OutputDisassembly(std::string output_path,
                               std::vector<uint8_t> ROM);
}  // namespace gbengine
