#include "hdtree/Exception.h"

namespace hdtree {

HDTreeException::HDTreeException(const std::string& msg, const std::string& help) noexcept
  : std::exception{}, message_{msg}, help_{help} {}

const char* HDTreeException::what() const noexcept {
  return message_.c_str();
}

std::ostream& HDTreeException::stream(std::ostream& o) const noexcept {
  o << "[HDTree]: " << message_;
  if (not help_.empty()) {
    o << "Help:\n" << help_;
  }
  return o;
}

}

std::ostream& operator<<(std::ostream& o, const hdtree::HDTreeException& e) {
  return e.stream(o);
}
