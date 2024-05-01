#pragma once
#include "include/logger.h"

const char* retro::GbErrorToString(Error error) {
  switch (error) {
    case GB_SUCCESS:
      return "GB_SUCCESS";
    case GB_FAILED_TO_FIND_PATH:
      return "GB_FAILED_TO_FIND_PATH";
    default:
      return "GB_UNDEFINED_ERROR";
  }
}