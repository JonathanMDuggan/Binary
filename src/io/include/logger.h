#pragma once
#include "spdlog/spdlog.h"
namespace retro {
enum Error {
  GB_SUCCESS = 1,
  GB_FAILURE = 1,
  GB_FAILED_TO_FIND_PATH = -1,
  GB_ROM_SIZE_TOO_LARGE = -2,
  GB_ROM_IS_EMPTY = -3
};
const char* GbErrorToString(Error error);
}