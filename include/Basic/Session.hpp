#ifndef basic_session
#define basic_session

#include <string>
#include <vector>
#include <filesystem>
#include <llvm/ADT/Optional.h>

namespace erys {

struct Session {
  using Path = std::filesystem::path;
  
  std::string source;
  std::vector<size_t> lineBeginnings;
  llvm::Optional<Path> path;
  bool useColor;

  static auto calculateLineBeginnings(const std::string &source) -> std::vector<size_t> {
    std::vector<size_t> lineBeginnings;
    lineBeginnings.push_back(0);
    for (size_t i = 0; i < source.size(); i++) {
      if (source[i] == '\n' || source[i] == '\r') {
        lineBeginnings.push_back(i + 1);
      }
    }
    return lineBeginnings;
  }

  Session(const std::string &source)
      : source(source), useColor(true),
        lineBeginnings(Session::calculateLineBeginnings(source)),
        path(llvm::None) {}
  
  auto setPath(const std::string &path) -> bool {
    // Verify the path is valid
    if (!std::filesystem::exists(path)) {
      return false;
    }

    this->path = Path(path);
    return true;
  }

  auto setUseColor(bool useColor) -> void {
    this->useColor = useColor;
  }

  auto fileName() const -> llvm::Optional<std::string> {
    if (path) {
      return path->filename().string();
    }
    return llvm::None;
  }

  auto relativePath() const -> llvm::Optional<std::string> {
    if (path) {
      return std::filesystem::relative(path.getValue(), std::filesystem::current_path()).string();
    }
    return llvm::None;
  }
};

}

#endif
