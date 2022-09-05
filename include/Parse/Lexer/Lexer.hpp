#ifndef parse_lexer_lexer
#define parse_lexer_lexer

#include "Tokens.hpp"

#include <cstddef>
#include <iterator>
#include <map>
#include <iostream>
#include <llvm/ADT/Optional.h>

namespace erys {

// TODO: add unicode support
class Lexer {
  /// Current offset in the input stream.
  size_t offset;

  /// The input stream.
  std::string source;

  /// String -> Keyword map
  std::map<std::string, TokenKind> keywords = {
    {"let", TokenKind::Let},
    {"mut", TokenKind::Mut},
    {"return", TokenKind::Return},
    {"if", TokenKind::If},
    {"else", TokenKind::Else},
    {"for", TokenKind::For},
    {"in", TokenKind::In},
    {"continue", TokenKind::Continue},
    {"break", TokenKind::Break},
    {"fn", TokenKind::Fn},
    {"extern", TokenKind::Extern},
    {"struct", TokenKind::Struct},
    {"as", TokenKind::As},
    {"inout", TokenKind::InOut},
    {"impl", TokenKind::Impl},
    {"pub", TokenKind::Pub},
    {"trait", TokenKind::Trait},
    {"implements", TokenKind::Implements},
    {"implicit", TokenKind::Implicit},
    {"consume", TokenKind::Consume},
    {"loop", TokenKind::Loop},
  };

  /// True if char is whitespace (space, tab, newline, carriage return, etc.)
  static auto isWhitespace(char c) -> bool {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  // Digits/Alpha

  /// True if char is digit
  static auto isDigit(char c) -> bool {
    return c >= '0' && c <= '9';
  }

  /// True if char is alpha
  static auto isAlpha(char c) -> bool {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  /// True if char is valid alpha/digit
  static auto isAlphaNumeric(char c) -> bool { return isAlpha(c) || isDigit(c); }

  // Identifiers

  /// True if char is valid ID start
  static auto isIdStart(char c) -> bool {
    return isAlpha(c) || c == '_';
  }

  /// True if char is valid ID continue
  static auto isIdContinue(char c) -> bool {
    return isAlphaNumeric(c) || c == '_';
  }

  // Punctuation

  /// True if character is a punctuation character.
  static auto isPunct(char c) -> bool {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
         c == '.' || c == ',' || c == ':' || c == ';' || c == '=' || c == '+' ||
         c == '-' || c == '*' || c == '/' || c == '%' || c == '!' || c == '?' ||
         c == '<' || c == '>' || c == '&' || c == '|' || c == '~' || c == '^' ||
         c == '\'' || c == '\"' || c == '\\';
  }

  // Utils

  /// True if EOF
  auto isEof() -> bool;

  /// Advance the offset by one
  auto advance() -> llvm::Optional<char>;

  /// Do not advance the offset but return the next character
  /// (or None if at the end of the file)
  auto peek() -> llvm::Optional<char>;

  /// Calculate the span of the current token (from the current offset)
  auto spanFromLength(size_t length) -> Span;

  /// Consume n characters from the input
  auto consume(size_t n = 1) -> void;

  /// Collect chars while predicate is true (with function of (char) -> bool)
  /// return (false, vector) if predicate finished before eof and
  /// (true, vector) if predicate stopped due to eof
  auto collectWhile(std::function<bool(char)> predicate) -> std::pair<bool, std::vector<char>>;

public:
  Lexer(std::string &source) : offset(0), source(source) {}

  auto eof() -> bool { return isEof(); }

  /// Try to analyze another token, return None if EOF
  auto nextToken() -> llvm::Optional<Token>;

  struct Iterator {
    // Instead of using the C++ standard type iterator, we use a custom
    // iterator type to avoid the use of algorithm functions (mainly
    // because they're not useful here).

    Iterator(Lexer &lexer) {
      // Initialise the iterator with a vector
      // containing all the tokens
      while (auto token = lexer.nextToken())
        tokens.push_back(token.getValue());
    }

    /// Move to next token
    auto next() -> llvm::Optional<Token> {
      if (i >= tokens.size())
        return llvm::None;

      return tokens[i++];
    }

    /// Move to previous token
    auto prev() -> llvm::Optional<Token> {
      if (i <= 0)
        return llvm::None;

      return tokens[--i];
    }

  private:
    std::vector<Token> tokens;
    size_t i = 0;
  };
};

} // namespace erys

#endif
