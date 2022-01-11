#include "doctest.h"
#include "mini_netcdf.hpp"

using doctest::Approx;
using namespace mini_netcdf;

TEST_CASE("hw") {
  hw();
  CHECK(1 == 1);

  NetcdfReader nr("wout_circular_tokamak.nc");
}
