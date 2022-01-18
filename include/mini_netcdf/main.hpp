#ifndef MINI_NETCDF_MAIN
#define MINI_NETCDF_MAIN

#include <string>
#include <cstdint>
#include <vector>
#include <cassert>
#include <fstream>

#define debug true

// http://esr.ibiblio.org/?p=5095
#define IS_BIG_ENDIAN_DEFINE (*(uint16_t *)"\0\xff" < 0x100)

namespace mini_netcdf {

  // https://vietlq.github.io/2018/06/17/endianness-detection-the-hard-way/
  inline bool is_big_endian_union() {
    union {
      int32_t i;
      char c[4];
    } e;
    e.i = 1;
    return (e.c[3] == 1);
  }

  inline bool is_big_endian_cast() {
    const int32_t i = 1;
    return ((reinterpret_cast<const char*>(&i))[3] == 1);
  }

  // https://vietlq.github.io/2019/06/22/endianness-agnostic-ntoh/
  inline int32_t big_endian_to_native(int32_t n) {
    const char * temp = reinterpret_cast<const char *>(&n);
    return (temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | temp[3];
  }
  
  inline int32_t big_endian_to_native_32(char* temp) {
    return (temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | temp[3];
  }
  
  inline int64_t big_endian_to_native_64(char* temp) {
    return (static_cast<int64_t>(temp[0]) << 56)
      | (static_cast<int64_t>(temp[1]) << 48)
      | (static_cast<int64_t>(temp[2]) << 40)
      | (static_cast<int64_t>(temp[3]) << 32)
      | (static_cast<int64_t>(temp[4]) << 24)
      | (static_cast<int64_t>(temp[5]) << 16)
      | (static_cast<int64_t>(temp[6]) << 8)
      | static_cast<int64_t>(temp[7]);
  }
  
  inline int32_t native_to_big_endian(int32_t oldnum) {
    int32_t newnum;
    char * temp = reinterpret_cast<char *>(&newnum);
    temp[0] = (oldnum >> 24);
    temp[1] = (oldnum >> 16) & 0xFF;
    temp[2] = (oldnum >> 8) & 0xFF;
    temp[3] = (oldnum) & 0xFF;
    return newnum;
  }
  
  const int32_t STREAMING    = 0xFFFFFF;
  const int32_t ZERO         = 0x000000;
  const int32_t NC_DIMENSION = 0x00000A;
  const int32_t NC_VARIABLE  = 0x00000B;
  const int32_t NC_ATTRIBUTE = 0x00000C;
  const int32_t NC_BYTE      = 0x000001;
  const int32_t NC_CHAR      = 0x000002;
  const int32_t NC_SHORT     = 0x000003;
  const int32_t NC_INT       = 0x000004;
  const int32_t NC_FLOAT     = 0x000005;
  const int32_t NC_DOUBLE    = 0x000006;

  inline std::string type_str(int32_t t) {
    switch (t) {
    case NC_BYTE:
      return "byte";
      break;
    case NC_CHAR:
      return "char";
      break;
    case NC_SHORT:
      return "short";
      break;
    case NC_INT:
      return "int";
      break;
    case NC_FLOAT:
      return "float";
      break;
    case NC_DOUBLE:
      return "double";
      break;
    default:
      throw std::runtime_error("Invalid type");
    }
  }
  
  struct NetcdfDimension {
    std::string name;
    int32_t size;
  };
  
  struct NetcdfAttribute {
    std::string name;
    int32_t type;
    // Eventually, 'value' will need to allow for other types.
    std::string value;
  };
  
  struct NetcdfVariable {
    std::string name;
    int32_t rank;
    std::vector<int> dim_ids;
    std::vector<NetcdfAttribute> attributes;
    int32_t type;
    int32_t vsize;
    int64_t begin;
  };
  
  class NetcdfReader {
  private:
    bool is_streaming;
    bool offset64;
    int32_t numrecs;
    int32_t n_dimensions;
    int32_t n_global_attributes;
    int32_t n_variables;
    std::vector<NetcdfDimension> dimensions;
    std::vector<NetcdfVariable> variables;
    std::ifstream file;

    int32_t unpack_int();
    int64_t unpack_int64();
    std::string unpack_string();
    
  public:
    NetcdfReader(std::string filename);
    int32_t get_n_dimensions() {return dimensions.size();};
    std::vector<NetcdfDimension> get_dimensions() {return dimensions;};
    int32_t get_n_variables() {return variables.size();};
    std::vector<NetcdfVariable> get_variables() {return variables;};
  };

}

#endif
