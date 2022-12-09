#include "fire/io/Atomic.h"

namespace fire::io {

HighFive::EnumType<Bool> create_enum_bool() {
  return {{"TRUE" , Bool::TRUE },
          {"FALSE", Bool::FALSE}};
}

}  // namespace fire::io

template<>
HighFive::DataType HighFive::create_datatype<fire::io::Bool>() {
  return fire::io::create_enum_bool();
}

