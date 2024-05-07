#include "../include/gb_cpu.h"
#include <gtest/gtest.h>
TEST(gb_cpu, Test123) { 
  EXPECT_EQ(retro::gb::Test123(), 123); 
}