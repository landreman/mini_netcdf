#ifndef MINI_NETCDF_MAIN
#define MINI_NETCDF_MAIN

#include <string>
#include <cstdint>
#include <vector>
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
  
  inline int32_t big_endian_to_native(char* temp) {
    return (temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | temp[3];
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
  
  void hw();

  const int32_t STREAMING    = 0xFFFFFF;
  const int32_t ZERO         = 0x000000;
  const int32_t NC_DIMENSION = 0x00000A;
  const int32_t NC_VARIABLE  = 0x00000B;
  const int32_t NC_ATTRIBUTE = 0x00000C;
  
  class NetcdfReader {
  private:
    bool is_streaming;
    bool offset64;
    int32_t numrecs;
    int32_t ndims;
    std::vector<int32_t> dims;
    std::vector<std::string> dim_names;
    std::ifstream file;

    int32_t unpack_int();
    std::string unpack_string();
    
  public:
    NetcdfReader(std::string filename);
    int32_t get_ndims() {return ndims;};
    std::vector<int32_t> get_dims() {return dims;};
    std::vector<std::string> get_dim_names() {return dim_names;};
  };

}

#endif
