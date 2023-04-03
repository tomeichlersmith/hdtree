#pragma once

#include <string>
#include <exception>
#include <ostream>

namespace hdtree {

/**
 * user-facing exception class for hdtree
 *
 * all of HDTree's C++ API exceptions are derived
 * from this base exception and use the same API.
 */
class HDTreeException : public std::exception {
  /// the short message explaining the error
  std::string message_;
  /// an optional, longer message providing help for the situation
  std::string help_;
 public:
  /**
   * Create a new exception
   *
   * @param[in] msg message of exception
   * @param[in] help extra help information (optional)
   */
  HDTreeException(const std::string& msg, const std::string& help = "") noexcept;

  /**
   * Override the message from the base class
   *
   * @return C-style string with message
   */
  const char* what() const noexcept;

  /**
   * Stream the message of this exception into the input output stream
   *
   * The output message looks like the following.
   * ```
   * [HDTree]: <message>
   * Help:
   * <help>
   * ```
   * where the last two help lines are only included if the help
   * string is non-empty.
   *
   * @param[in] o output stream to write to
   * @return modified output stream
   */
  std::ostream& stream(std::ostream& o) const noexcept;
};

}

/**
 * Stream an exception using the streaming operator
 *
 * @see hdtree::HDTreeException::stream for how the
 * message is formatted
 *
 * @param[in] o output stream to write to
 * @param[in] e exception to write out
 * @return modified output stream
 */
std::ostream& operator<<(std::ostream& o, const hdtree::HDTreeException& e);
