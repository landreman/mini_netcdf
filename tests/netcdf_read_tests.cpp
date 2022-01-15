#include "doctest.h"
#include "mini_netcdf.hpp"

using doctest::Approx;
using namespace mini_netcdf;

TEST_CASE("hw") {
  hw();
  CHECK(1 == 1);

  NetcdfReader nr("wout_circular_tokamak.nc");
  CHECK(nr.get_ndims() == 11);
  CHECK(nr.get_dims()[0] == 100);
  CHECK(nr.get_dim_names()[0] == "dim_00100");
  CHECK(nr.get_dims()[9] == 17);
  CHECK(nr.get_dim_names()[9] == "radius");
}
