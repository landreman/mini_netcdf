#ifndef MINI_NETCDF_NETCDF_READER
#define MINI_NETCDF_NETCDF_READER

#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace mini_netcdf;

inline void mini_netcdf::hw() {
  std::cout << "Hello world!" << std::endl;
  int32_t test = 1;
  
  union {
    int32_t i;
    char c[4];
  } e;
  e.i = 1;
  std::cout << "c[0]: " << (int)e.c[0] << "  c[3]: " << (int)e.c[3] << std::endl;
  
  std::cout << "is_big_endian_cast: " << is_big_endian_cast() << std::endl;
  std::cout << "is_big_endian_union: " << is_big_endian_union() << std::endl;
  std::cout << "IS_BIG_ENDIAN_DEFINE: " << IS_BIG_ENDIAN_DEFINE << std::endl;
  std::cout << "big_endian_to_native(1): " << big_endian_to_native(1) << std::endl;
  std::cout << "native_to_big_endian(1): " << native_to_big_endian(1) << std::endl;
} 

inline NetcdfReader::NetcdfReader(std::string filename) {
  if (debug) std::cout << "Opening file " << filename << std::endl;
  std::ifstream file(filename, std::ios::binary);
  if (!file)
    throw std::runtime_error("Unable to open NetCDF file");
  
  char cdf[3];
  file.read(cdf, 3);
  if (debug) {
    if (cdf[0] == 'C') {
      std::cout << "First byte is C" << std::endl;
    } else {
      std::cout << "First byte is NOT C! " << cdf[0] << std::endl;
    }
    
    if (cdf[1] == 'D') {
      std::cout << "Second byte is D" << std::endl;
    } else {
      std::cout << "Second byte is NOT D! " << cdf[1] << std::endl;
    }
    
    if (cdf[2] == 'F') {
      std::cout << "Third byte is F" << std::endl;
    } else {
      std::cout << "Third byte is NOT F! " << cdf[2] << std::endl;
    }
  }
  if (cdf[0] != 'C' || cdf[1] != 'D' || cdf[2] != 'F')
    throw std::runtime_error("File does not begin with CDF. Probably not a NetCDF classic file.");

  // Handle version byte
  char offset_byte;
  file.read(&offset_byte, 1);
  if (debug) std::cout << "Offset byte: " << (int) offset_byte << std::endl;
  int offset_int = offset_byte;
  bool offset64;
  if (offset_int == 1) {
    offset64 = false;
  } else if (offset_int == 2) {
    offset64 = true;
  } else {
    throw std::runtime_error("Version bit is not 1 or 2. Not supported.");
  }

  // Handle numrecs
  char numrecs_raw[4];
  bool is_streaming;
  file.read(numrecs_raw, 4);
  int32_t numrecs = big_endian_to_native(numrecs_raw);
  is_streaming = (numrecs == STREAMING);
  if (debug) {
    if (is_streaming)
      std::cout << "numrecs is STREAMING" << std::endl;
    else
      std::cout << "numrecs is " << numrecs << std::endl;
  }

  // Handle dim_list size
  char dim_list_1_raw[4], dim_list_2_raw[4];
  file.read(dim_list_1_raw, 4);
  file.read(dim_list_2_raw, 4);
  int32_t dim_list_1 = big_endian_to_native(dim_list_1_raw);
  int32_t ndims = big_endian_to_native(dim_list_2_raw);
  if (debug)
    std::cout << "dim_list_1: " << dim_list_1 << "  ndims: " << ndims << std::endl;
  if (dim_list_1 == ZERO) {
    assert (ndims == 0);
  } else if (dim_list_1 != NC_DIMENSION) {
    throw std::runtime_error("dim_list is neither ABSENT nor NC_DIMENSION");
  }
  
  file.close();
}

#endif
