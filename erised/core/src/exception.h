#pragma once

#include <sstream>
#include <string>
#include <memory>

namespace erised {

enum class Error {
  INVALID_ARGUMENT,
  OUT_OF_RANGE
};

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type {
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

/**
 * @brief Class passed to an error.
 *
 * This class encapsulates all or almost all necessary
 * information about the error happened in the program. The exception is
 * usually constructed and thrown implicitly via ERISED_Error macros.
 */
class Exception : public std::exception {
 public:
  /**
   * Default constructor
   */
  Exception();

  /**
   * Full constructor. Normally the constuctor is not called explicitly.
   * Instead, the macro ERISED_Error is used.
   */
  Exception(Error code, const std::string& err, const std::string& func,
            const std::string& src_file, unsigned int line)
    : code_(code)
    , err_(err)
    , func_(func)
    , src_file_(src_file)
    , line_(line)
    {
      FormatMessage();
    }

  virtual ~Exception() noexcept  = default;

  /**
   * @return the error description and the context as a text string.
   */
  virtual const char* what() const noexcept {
    return msg_.c_str();
  }

  virtual void FormatMessage() {
    std::stringstream ss;
    ss << "Error " << as_integer(code_) << ": " << err_ << " on: " << func_ << " in: "
       << src_file_ << " line: " << line_;
    msg_ = ss.str();
  }

 private:
  std::string msg_;         // The formatted error message

  Error code_;              // error code
  std::string err_;         // error description
  std::string func_;        // function name. Available only when the compiler supports
                            // getting it
  std::string src_file_;    // source file name where the error has occured
  unsigned int line_;       // line number in the source file where the error has occured
};

/**
 * @brief std::string formatting like sprintf
 */
template <typename... Ts>
std::string format (const std::string &fmt, Ts&&... vs) {
  char b;
  // See comments: the +1 is necessary, while the first parameter
  // can also be set to nullptr
  unsigned required = std::snprintf(&b, 0, fmt.c_str(), std::forward<Ts>(vs)...) + 1;

  std::unique_ptr<char[]> bytes(new char[required]);

  std::snprintf(bytes.get(), required, fmt.c_str(), std::forward<Ts>(vs)...);

  return std::string(bytes.get());
}

#define ERISED_Error(code, ...)                                               \
  throw Exception(code, format(__VA_ARGS__), __FUNCTION__, __FILE__, __LINE__)

}
