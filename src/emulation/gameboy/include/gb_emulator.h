#include <string>
#include "gb_instruction.h" 
namespace binary::gb {
extern void test();
extern void Emulate(GameBoy* gameboy, bool running);
void LoadRom(std::string file_path, void* data);
}


