#include "doctest.h"
#include "mini_netcdf.hpp"

using doctest::Approx;
using namespace mini_netcdf;

TEST_CASE("big endian conversion") {
  int32_t num_int32 = 0x12345678;
  int32_t num_int32_byteswap = 0x78563412;
  
  CHECK(native_to_big_endian(native_to_big_endian(num_int32)) == num_int32);
  CHECK(big_endian_to_native(big_endian_to_native(num_int32)) == num_int32);
  CHECK(big_endian_to_native(native_to_big_endian(num_int32)) == num_int32);
  CHECK(native_to_big_endian(big_endian_to_native(num_int32)) == num_int32);

  CHECK(is_big_endian_cast() == is_big_endian_union());
  CHECK(is_big_endian_cast() == IS_BIG_ENDIAN_DEFINE);
  
  if (is_big_endian_cast()) {
    CHECK(native_to_big_endian(num_int32) == num_int32);
    CHECK(big_endian_to_native(num_int32) == num_int32);
    CHECK(native_to_big_endian(num_int32_byteswap) == num_int32_byteswap);
    CHECK(big_endian_to_native(num_int32_byteswap) == num_int32_byteswap);
  } else {
    // System is little-endian
    CHECK(native_to_big_endian(num_int32) == num_int32_byteswap);
    CHECK(big_endian_to_native(num_int32) == num_int32_byteswap);
    CHECK(native_to_big_endian(num_int32_byteswap) == num_int32);
    CHECK(big_endian_to_native(num_int32_byteswap) == num_int32);
  }
}
