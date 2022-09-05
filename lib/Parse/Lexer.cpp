#include "Parse/Lexer/Lexer.hpp"

namespace erys {
// Utils

auto Lexer::isEof() -> bool {
  return offset >= source.size();
}

auto Lexer::advance() -> llvm::Optional<char> {
  if (isEof())
    return llvm::None;
  return source[offset++];
}

auto Lexer::peek() -> llvm::Optional<char> {
  if (isEof())
    return llvm::None;
  return source[offset];
}

auto Lexer::spanFromLength(size_t length) -> Span {
  return Span(offset - length, length);
}

auto Lexer::consume(size_t n) -> void {
  offset += n;
}

auto Lexer::collectWhile(std::function<bool(char)> predicate) -> std::pair<bool, std::vector<char>> {
  std::vector<char> result;
  while (!isEof() && predicate(source[offset])) {
    result.push_back(source[offset]);
    offset++;
  }
  return {isEof(), result};
}

// TODO: comment support
auto Lexer::nextToken() -> llvm::Optional<Token> {
  auto ch = peek();

  if (!ch) {
    return llvm::None;
  }

  if (isWhitespace(*ch)) {
    advance();
    return nextToken();
  }

  if (isIdStart(*ch)) {
    auto [_eof, chars] = collectWhile([&](char c) { return isIdContinue(c); });
    auto ident = std::string(chars.begin(), chars.end());

    if (keywords.find(ident) != keywords.end()) {
      return Token(keywords.at(ident), spanFromLength(ident.size()));
    }

    return Token(ident, spanFromLength(ident.size()));
  }

  if (isDigit(*ch)) {
    auto lastDotLoc = 0;
    auto isValidDigit = true;

    auto [_eof, chars] = collectWhile([&](char c) {
      if (!isDigit(c) && c != '.') {
        return false;
      }

      if (lastDotLoc != 0 && c == '.') {
        isValidDigit = false;
        return false;
      }

      if (c == '.')
        lastDotLoc = offset - 1;
      
      return true;
    });

    if (!isValidDigit) {
      return llvm::None;
    }

    return Token(
      Literal(
        lastDotLoc != 0
        ? LiteralKind::Float
        : LiteralKind::Int,
        std::string(chars.begin(), chars.end()),
        Base::Decimal // TODO: support other bases
      ),
      spanFromLength(chars.size())
    );
  }

  if (*ch == '\'' || *ch == '\"') {
    auto isChar = *ch == '\'';
    auto broken = false; // broken by newline?
    
    advance(); // skip ' or "
    auto [eof, chars] = collectWhile([&](char c) {
      if (c == '\n') {
        broken = true;
        return false;
      }
      return c != *ch;
    });

    // char/string validation is done in the second
    // pass of the lexer, so we'll return the full string
    return Token(
      Literal(
        isChar
        ? LiteralKind::Char
        : LiteralKind::String,
        std::string(chars.begin(), chars.end()),
        !(broken || eof) // if broken or eof, then unterminated
      ),
      spanFromLength(chars.size())
    );
  }

  TokenKind pKind;
  switch (*ch) {
  case ';':
    pKind = TokenKind::Semicolon;
    break;
  case ',':
    pKind = TokenKind::Comma;
    break;
  case '(':
    pKind = TokenKind::OpenParen;
    break;
  case ')':
    pKind = TokenKind::CloseParen;
    break;
  case '[':
    pKind = TokenKind::OpenBracket;
    break;
  case ']':
    pKind = TokenKind::CloseBracket;
    break;
  case '{':
    pKind = TokenKind::OpenBrace;
    break;
  case '}':
    pKind = TokenKind::CloseBrace;
    break;
  case '@':
    pKind = TokenKind::At;
    break;
  case '#':
    pKind = TokenKind::Pound;
    break;
  case '~':
    pKind = TokenKind::Tilde;
    break;
  case '?':
    pKind = TokenKind::Question;
    break;
  case ':':
    pKind = TokenKind::Colon;
    break;
  case '$':
    pKind = TokenKind::Dollar;
    break;
  case '=':
    pKind = TokenKind::Eq;
    break;
  case '!':
    pKind = TokenKind::Bang;
    break;
  case '<':
    pKind = TokenKind::Lt;
    break;
  case '>':
    pKind = TokenKind::Gt;
    break;
  case '+':
    pKind = TokenKind::Plus;
    break;
  case '-':
    pKind = TokenKind::Minus;
    break;
  case '|':
    pKind = TokenKind::Or;
    break;
  case '&':
    pKind = TokenKind::And;
    break;
  case '*':
    pKind = TokenKind::Star;
    break;
  case '/':
    pKind = TokenKind::Slash;
    break;
  case '^':
    pKind = TokenKind::Caret;
    break;
  case '%':
    pKind = TokenKind::Percent;
    break;
  default:
    pKind = TokenKind::Unknown;
    break;
  }

  advance();
  return Token(pKind, *ch, offset - 1);
}

}
