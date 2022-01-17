#include "doctest.h"
#include "mini_netcdf.hpp"

using doctest::Approx;
using namespace mini_netcdf;

TEST_CASE("Try reading a netcdf file") {
  
  NetcdfReader nr("wout_circular_tokamak.nc");
  
  // Check that the dimensions were read correctly:
  CHECK(nr.get_n_dimensions() == 11);
  CHECK(nr.get_dimensions()[0].size == 100);
  CHECK(nr.get_dimensions()[0].name == "dim_00100");
  CHECK(nr.get_dimensions()[9].size == 17);
  CHECK(nr.get_dimensions()[9].name == "radius");
  CHECK(nr.get_n_variables() == 104);
}
