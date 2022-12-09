#include "hdtree/Atomic.h"

namespace hdtree {

HighFive::EnumType<Bool> create_enum_bool() {
  return {{"TRUE" , Bool::TRUE },
          {"FALSE", Bool::FALSE}};
}

}  // namespace hdtree

template<>
HighFive::DataType HighFive::create_datatype<hdtree::Bool>() {
  return hdtree::create_enum_bool();
}

