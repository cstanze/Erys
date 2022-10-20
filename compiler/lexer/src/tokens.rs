use std::fmt;
use span::Span;

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum TokenKind {
  /// Comment
  Comment { kind: CommentKind, doc: bool },
  
  /// "ident"
  Ident,

  /// "let", "if", "return", etc.
  Keyword(KeywordKind),

  /// "12", "12.34", chars, strings, etc.
  Literal(LiteralKind),

  // One char tokens
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
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum CommentKind {
  /// "// comment"
  Line,

  /// `/* block comment */`
  /// Important to note that block comments
  /// are recursive so the following will result in an
  /// error in parsing/lexing (unterminated block comment):
  /// `/* /* */` (the second `/*` is not closed)
  Block
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum KeywordKind {
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
  Loop
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum LiteralKind {
  Int(Base),
  Float(Base),
  Char,
  String
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum Base {
  Binary,
  Octal,
  Hexadecimal,
  Decimal
}

#[derive(Debug)]
pub struct Token {
  /// The kind of token
  pub kind: TokenKind,

  /// Span of the token
  pub span: Span,
  
  /// The value of the token
  /// This is used for literals, identifiers, and comments
  /// In the case of comments, this is the text of the comment
  pub value: Option<String>
}

impl Token {
  pub fn new(kind: TokenKind, span: Span, value: Option<String>) -> Token {
    Token {
      kind,
      span,
      value
    }
  }

  pub fn skippable(&self) -> bool {
    match self.kind {
      TokenKind::Comment { .. } => true,
      _ => false
    }
  }
}

impl fmt::Display for Token {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    writeln!(
      f,
      "Token({:?}, {}, {:?})",
      self.kind,
      self.span,
      self.value
    )
  }
}
