#include "hdtree/Version.h"

namespace hdtree {

const std::string& VERSION() {
  static std::string the_version = "0.4.5";
  return the_version;
}

const std::string& API() {
  static std::string the_api = "cpp";
  return the_api;
}

}
