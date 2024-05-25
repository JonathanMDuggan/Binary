# Naming Conventions
## Namespaces
Namespaces are used to organize large code bases and avoid naming conflicts,
here are the list of rules before declaring your namespaces:
* Everything written outside a library should be under a "main" namespace
* Any new namespaces are nested under the main namespace. 
* Namespaces are under case with no under scores. 
* New namespaces are declared like this ```main::newname```. 
* 3 is the maximum nested namespace size.

```cpp
// In this example "binary" is the main namespace 

// Good
namespace binary::gb::instructionset{}

// Bad formating
namespace binary{
namespace gb{
}}

// Bad declaration
namespace gb::instructionset{} // Not under the "main" namespace

// Bad name
namespace Binary::GB::InstructionSet // has uppercase

// Bad name
namespace binary::gb::instruction_set // has underscore
```

## Constants 

Constants start with a ```k_``` then camel cased.

```cpp
// Good
const uint16_t k_ProgramStart = 0x100;

// Bad
const uint16_t kProgramStart   = 0x100; // Google Style Guide version
const uint16_t k_PROGRAM_START = 0x100; // Just No
const uint16_t PROGRAM_START   = 0x100; // Only used for opcodes or macros
```

However you can break this rule only if the constant represents an 
opcode mnemonic. 

```cpp
// Good 
const uint8_t LOAD_A_B = 0x23; // This represents an opcode mnemonic!

// Bad
const uint8_t k_LoadAB = 0x23 // This is bad only for opcode mnemonic!
```

Click here for more information about opcode naming.

## Functions
