#ifndef parse_lexer_tokens
#define parse_lexer_tokens

#include "Basic/Span.hpp"

#include <llvm/Support/ErrorHandling.h>
#include <fmt/format.h>
#include <magic_enum.hpp>

namespace erys {

enum class TokenKind {
  // "// comment"
  LineComment,

  /// `/* block comment */`
  /// Important to note that block comments
  /// are recursive so the following will result in an
  /// error in parsing/lexing (unterminated block comment):
  /// `/* /* */` (the second `/*` is not closed)
  BlockComment,

  /// "ident"
  Ident,

  /// Keywords
  Let,
  Mut,
  Return,
  If,
  Else,
  For,
  In,
  Continue,
  Break,
  Fn,
  Extern,
  Struct,
  As,
  InOut,
  Impl,
  Pub,
  Trait,
  Implements,
  Implicit,
  Consume,
  Loop,
  
  /// "12", "12.34", chars, strings, etc.
  Literal,

  // One-char tokens
  /// ';'
  Semicolon,
  /// ','
  Comma,
  /// '.'
  Dot,
  /// '('
  OpenParen,
  /// ')'
  CloseParen,
  /// '['
  OpenBracket,
  /// ']'
  CloseBracket,
  /// '{'
  OpenBrace,
  /// '}'
  CloseBrace,
  /// '@'
  At,
  /// '#'
  Pound,
  /// '~'
  Tilde,
  /// '?'
  Question,
  /// ':'
  Colon,
  /// '$'
  Dollar,
  /// '='
  Eq,
  /// '!'
  Bang,
  /// '<'
  Lt,
  /// '>'
  Gt,
  /// '+'
  Plus,
  /// '-'
  Minus,
  /// '&'
  And,
  /// '|'
  Or,
  /// '*'
  Star,
  /// '/'
  Slash,
  /// '^'
  Caret,
  /// '%'
  Percent,
  
  /// Unknown token, not expected by the lexer
  Unknown,
};

enum class LiteralKind {
  Int,
  Float,
  Char,
  String
};

enum class Base {
  /// Literal starts with '0b'
  Binary,
  /// Literal starts with '0o'
  Octal,
  /// Literal starts with '0x'
  Hexadecimal,
  /// Literal does not contain any prefix
  Decimal
};

struct Literal {
  LiteralKind kind;
  union {
    bool isTerminated; // true is string/char is terminated by closing quote
    Base base; // base of the literal if it is a float/int
  };
  std::string value;

  Literal(LiteralKind kind, std::string value, bool isTerminated)
    : kind(kind), isTerminated(isTerminated), value(value) {}
  
  Literal(LiteralKind kind, std::string value, Base base)
    : kind(kind), base(base), value(value) {}

  ~Literal() = default;

  auto operator=(const Literal &other) -> Literal& {
    kind = other.kind;
    if (isNumber()) {
      base = other.base;
    } else {
      isTerminated = other.isTerminated;
    }
    fmt::print("initialing literal with value: '{}'\n", other.value);
    value = other.value;

    return *this;
  }

  auto isNumber() const -> bool {
    return kind == LiteralKind::Int || kind == LiteralKind::Float;
  }

  auto typeToString() const -> std::string {
    switch (kind) {
      case LiteralKind::Int:
        return "int";
      case LiteralKind::Float:
        return "float";
      case LiteralKind::Char:
        return "char";
      case LiteralKind::String:
        return "string";
      default:
        llvm_unreachable("unexpected literal kind");
    }
  }

  auto baseToString() const -> std::string {
    switch (base) {
      case Base::Binary:
        return "Binary";
      case Base::Octal:
        return "Octal";
      case Base::Hexadecimal:
        return "Hexadecimal";
      case Base::Decimal:
        return "Decimal";
      default:
        llvm_unreachable("unexpected literal base");
    }
  }

  auto toString() const -> std::string {
    auto front = fmt::format("Literal {}: {}", typeToString(), value);

    if (isNumber()) {
      return fmt::format("{} (base {})", front, baseToString());
    }

    return fmt::format("{} (terminated: {})", front, isTerminated);
  }
};

struct Comment {
  bool isDocComment;
  std::string content;

  ~Comment() = default;

  auto toString() const -> std::string {
    return fmt::format("Comment (doc: {})", isDocComment);
  }
};

struct Token {
  TokenKind kind;
  union {
    Literal literal;
    std::string ident;
    Comment comment;
  };
  Span span;

  Token(TokenKind kind, Span span)
    : kind(kind), span(span), ident("") {}

  Token(std::string ident, Span span)
    : kind(TokenKind::Ident), ident(ident), span(span) {}
  
  Token(Literal literal, Span span)
    : kind(TokenKind::Literal), literal(literal), span(span) {}
  
  Token(Comment comment, Span span)
    : kind(TokenKind::LineComment), comment(comment), span(span) {}
  
  Token(TokenKind kind, char c, size_t start)
    : kind(kind), ident(&c, 1), span(start, 1) {}
  
  Token(const Token &other)
    : kind(other.kind), span(other.span) {
    if (other.isLiteral()) {
      literal = other.literal;
    } else if (other.isComment()) {
      comment = other.comment;
    } else {
      ident = other.ident;
    }
  }

  ~Token() {
    if (isLiteral()) {
      literal.~Literal();
    } else if (isComment()) {
      comment.~Comment();
    }
  }

  auto isLiteral() const -> bool {
    return kind == TokenKind::Literal;
  }

  auto isIdent() const -> bool {
    return kind == TokenKind::Ident;
  }

  auto isComment() const -> bool {
    return kind == TokenKind::LineComment || kind == TokenKind::BlockComment;
  }

  /// Returns true if the token is skippable (whitespace, comments, etc.)
  auto isSkippable() const -> bool {
    return kind == TokenKind::LineComment || kind == TokenKind::BlockComment;
  }

  auto kindToString() const -> std::string_view {
    return magic_enum::enum_name(kind);
  }

  auto dataToString() const -> std::string {
    switch (kind) {
      case TokenKind::LineComment:
      case TokenKind::BlockComment:
        return comment.toString();
      case TokenKind::Literal:
        return literal.toString();
      case TokenKind::Ident:
        return fmt::format("Identifier: {}", ident);
      default:
        return fmt::format("Punct: {}", kindToString());
    }
  }

  auto toString() const -> std::string {
    return fmt::format("{} ({})", dataToString(), span.toString());
  }
};

}

#endif
