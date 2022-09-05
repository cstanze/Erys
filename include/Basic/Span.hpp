#ifndef basic_span
#define basic_span

#include <string>
#include <cstddef>
#include <llvm/ADT/Optional.h>

namespace erys {

struct Span;
struct SpanContents {
  std::string data;
  Span &span;
  size_t line;
  size_t column;
  size_t usize;
  llvm::Optional<std::string> name;

  SpanContents(std::string data, Span &span, size_t line, size_t column,
               size_t usize, llvm::Optional<std::string> name)
      : data(data), span(span), line(line), column(column), usize(usize),
        name(name) {}

  auto asBytes() const -> const char * { return data.c_str(); }
};

struct Span {
  size_t start;
  size_t length;

  Span()
    : start(0), length(0) {}

  Span(size_t start, size_t length)
    : start(start), length(length) {}
  

  auto toString() const -> std::string {
    // start..start+length
    return std::to_string(start) + ".." + std::to_string(start + length);
  }

  auto operator==(const Span &other) const -> bool {
    return start == other.start && length == other.length;
  }

  auto operator!=(const Span &other) const -> bool {
    return !(*this == other);
  }

  auto operator<(const Span &other) const -> bool {
    return start < other.start || (start == other.start && length < other.length);
  }

  auto operator>(const Span &other) const -> bool {
    return !(*this < other) && !(*this == other);
  }

  auto operator<=(const Span &other) const -> bool {
    return *this < other || *this == other;
  }

  auto operator>=(const Span &other) const -> bool {
    return *this > other || *this == other;
  }

  auto contains(const Span &other) const -> bool {
    return start <= other.start && other.start + other.length <= start + length;
  }

  auto highByte() const -> size_t {
    return start + length;
  }

  static auto lineCount(const std::string &source) -> size_t {
    size_t lineCount = 0;

    for (auto &ch : source) {
      if (ch == '\n') {
        lineCount++;
      }
    }

    return lineCount;
  }

  // auto lineRange() const -> Span {

  // }

  auto source(const std::string &source) const -> llvm::Optional<SpanContents> {
    if (source.size() < start) {
      return llvm::None;
    }

    // auto lineRange =

    return llvm::None;
  }
};

}

#endif
