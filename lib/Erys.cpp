#include "Config.hpp"

#include "Basic/Session.hpp"
#include "Parse/Lexer/Lexer.hpp"

#include <iostream>
#include <cxxopts.hpp>
#include <llvm/Support/MemoryBuffer.h>

using llvm::MemoryBuffer;

auto main(int argc, char **argv) -> int {
  // Arg parsing options
  cxxopts::Options opts("erysc", "Proto compiler for the Erys langauage");
  opts.add_options()
    ("h,help", "Prints this help message")
    ("v,version", "Prints the compiler version")
    ("input", "Input file to compile", cxxopts::value<std::string>());

  // Oddly enough, you need to parse this
  // before parsing the other otpions
  opts.parse_positional({ "input" });
  auto result = opts.parse(argc, argv);

  if (result.count("help")) {
    std::cout << opts.help() << std::endl;
    
    return 0;
  }

  if (result.count("version")) {
    std::cout << "erysc " << ConfigVersion << " ("
              << ConfigGitRev << " on " << ConfigGitBranch
              << ")" << std::endl;
    std::cout << "Built " << ConfigBuildDate << std::endl;
    
    return 0;
  }

  if (!result.count("input")) {
    std::cout << "error: No input file" << std::endl;
    return 1;
  }

  auto buf = MemoryBuffer::getFile(result["input"].as<std::string>(), true);
  if (!buf) {
    std::cout << "error: failed to read input (" << buf.getError().message() << ")" << std::endl;

    return 1;
  }

  erys::Session session(buf->get()->getBuffer().str());
  if (!session.setPath(result["input"].as<std::string>())) {
    std::cout << "error: invalid input file" << std::endl;
    return 1;
  }

  erys::Lexer lexer(session.source);

  while (auto tok = lexer.nextToken()) {
    std::cout << tok->toString() << std::endl;
  }

  return 0;
}
