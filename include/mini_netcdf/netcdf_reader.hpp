#ifndef MINI_NETCDF_NETCDF_READER
#define MINI_NETCDF_NETCDF_READER

#include <iostream>
#include <stdexcept>
#include <cassert>
#include "mini_netcdf.hpp"

using namespace mini_netcdf;

/**
 * Read an integer from the netcdf file.
 */
inline int32_t NetcdfReader::unpack_int() {
  char char4[4];
  file.read(char4, 4);
  return big_endian_to_native_32(char4);
}

/**
 * Read a 64-bit integer from the netcdf file.
 */
inline int64_t NetcdfReader::unpack_int64() {
  char char8[8];
  file.read(char8, 8);
  return big_endian_to_native_64(char8);
}

/**
 * Read a string from the netcdf file. Before reading the string
 * itself, the length of the string is also read.
 */
inline std::string NetcdfReader::unpack_string() {
    int32_t length = unpack_int();

    char * binary_string = new char[length];
    file.read(binary_string, length);

    // Handle padding
    int npad = ((-length) % 4 + 4) % 4;
    char buffer[npad];
    file.read(buffer, npad);

    return std::string(binary_string, length);
}

/**
 * Constructor. This constructor does all the work of actually reading
 * in the file.
 */
inline NetcdfReader::NetcdfReader(std::string filename) {
  int j, k;
  
  if (debug) std::cout << "Opening file " << filename << std::endl;
  file = std::ifstream(filename, std::ios::binary);
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
  if (offset_int == 1) {
    offset64 = false;
  } else if (offset_int == 2) {
    offset64 = true;
  } else {
    throw std::runtime_error("Version bit is not 1 or 2. Not supported.");
  }

  // Handle numrecs
  numrecs = unpack_int();
  is_streaming = (numrecs == STREAMING);
  if (debug) {
    if (is_streaming)
      std::cout << "numrecs is STREAMING" << std::endl;
    else
      std::cout << "numrecs is " << numrecs << std::endl;
  }

  // Handle dim_list size
  int32_t dim_list_1 = unpack_int();
  n_dimensions = unpack_int();
  if (debug)
    std::cout << "dim_list_1: " << dim_list_1 << "  n_dimensions: " << n_dimensions << std::endl;
  if (dim_list_1 == ZERO) {
    assert (n_dimensions == 0);
  } else if (dim_list_1 != NC_DIMENSION) {
    throw std::runtime_error("dim_list is neither ABSENT nor NC_DIMENSION");
  }

  dimensions.resize(n_dimensions);

  // Read in the dimension names and sizes.
  for (j = 0; j < n_dimensions; j++) {
    dimensions[j].name = unpack_string();
    dimensions[j].size = unpack_int();

    if (debug) std::cout << "Read dimension. name length: " << dimensions[j].name.size()
			 << ". name: " << dimensions[j].name
			 << ". size: " << dimensions[j].size << std::endl;
  }

  // See if there are global attributes
  int32_t global_attributes_1 = unpack_int();
  n_global_attributes = unpack_int();
  if (debug) std::cout << "global_attributes_1: " << global_attributes_1
		       << "  n_global_attributes: " << n_global_attributes << std::endl;
  if (global_attributes_1 == ZERO) {
    assert (n_global_attributes == 0);
  } else if (global_attributes_1 != NC_ATTRIBUTE) {
    throw std::runtime_error("First int of gatt_list is neither ABSENT nor NC_ATTRIBUTE");
  }

  if (n_global_attributes > 0)
    throw std::runtime_error("Reading of global attributes is not yet implemented.");

  // Get the number of variables
  int32_t variables_1 = unpack_int();
  n_variables = unpack_int();
  if (debug) std::cout << "variables_1: " << variables_1
		       << "  n_variables: " << n_variables << std::endl;
  if (variables_1 == ZERO) {
    assert (n_variables == 0);
  } else if (variables_1 != NC_VARIABLE) {
    throw std::runtime_error("First int of var_list is neither ABSENT nor NC_VARIABLE");
  }

  variables.resize(n_variables);
  
  // Read variables
  int32_t attributes_1, n_attributes;
  for (j = 0; j < n_variables; j++) {
    variables[j].name = unpack_string();
    variables[j].rank = unpack_int();
    if (debug) std::cout << "Reading variable " << variables[j].name << "  rank: " << variables[j].rank << std::endl;
    variables[j].dim_ids.resize(variables[j].rank);
    for (k = 0; k < variables[j].rank; k++) {
      variables[j].dim_ids[k] = unpack_int();
      if (debug) std::cout << "  dim id: " << variables[j].dim_ids[k];
    }
    if (debug && variables[j].rank > 0) std::cout << std::endl;
    
    // Get the number of attributes associated with this variable.
    attributes_1 = unpack_int();
    n_attributes = unpack_int();
    if (debug) std::cout << "  attributes_1: " << attributes_1 << "  n_attributes: " << n_attributes << std::endl;
    if (attributes_1 == ZERO) {
      assert (n_attributes == 0);
    } else if (attributes_1 != NC_ATTRIBUTE) {
      throw std::runtime_error("First int of att_list is neither ABSENT nor NC_ATTRIBUTE");
    }

    // Read attributes
    variables[j].attributes.resize(n_attributes);
    for (k = 0; k < n_attributes; k++) {
      variables[j].attributes[k].name = unpack_string();
      variables[j].attributes[k].type = unpack_int();
      if (variables[j].attributes[k].type != NC_CHAR)
	throw std::runtime_error("For now only string attributes are supported.");
      variables[j].attributes[k].value = unpack_string();
      if (debug) std::cout << "  attribute " << variables[j].attributes[k].name
			   << "  type: " << type_str(variables[j].attributes[k].type)
			   << "  value: " << variables[j].attributes[k].value << std::endl;
    }

    // Read other properties of the variables
    variables[j].type = unpack_int();
    variables[j].vsize = unpack_int();
    if (offset64) {
      variables[j].begin = unpack_int64();
    } else {
      variables[j].begin = unpack_int();
    }
    if (debug) std::cout << "  type: " << type_str(variables[j].type)
			 << "  vsize: " << variables[j].vsize
			 << "  begin: " << variables[j].begin << std::endl;
  }
  
  file.close();
}

#endif
