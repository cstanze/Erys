mod tokens;

use span::Span;
use std::{collections::HashMap, iter::Peekable, str::Chars};
use tokens::{Token, TokenKind, CommentKind, Base, LiteralKind, KeywordKind};

#[macro_export]
/// Returns a hashmap from a list of `key => value` pairs.
macro_rules! hash_map {
  ($($key:expr => $value:expr),*) => {
    {
      let mut map = HashMap::new();
      $(map.insert($key, $value);)*
      map
    }
  };
}

pub struct Lexer<'a> {
  /// Peekable iterator over source chars.
  chars: Peekable<Chars<'a>>,

  /// Keyword map of keywords to their respective kinds.
  keywords: HashMap<&'a str, KeywordKind>,

  /// The current offset of the lexer.
  offset: usize,
}

impl<'a> Lexer<'a> {
  pub fn new(input: &'a str) -> Self {
    Lexer {
      chars: input.chars().peekable(),
      keywords: hash_map! {
        "let" => KeywordKind::Let,
        "mut" => KeywordKind::Mut,
        "return" => KeywordKind::Return,
        "if" => KeywordKind::If,
        "else" => KeywordKind::Else,
        "for" => KeywordKind::For,
        "in" => KeywordKind::In,
        "continue" => KeywordKind::Continue,
        "break" => KeywordKind::Break,
        "fn" => KeywordKind::Fn,
        "extern" => KeywordKind::Extern,
        "struct" => KeywordKind::Struct,
        "as" => KeywordKind::As,
        "inout" => KeywordKind::InOut,
        "impl" => KeywordKind::Impl,
        "pub" => KeywordKind::Pub,
        "trait" => KeywordKind::Trait,
        "implements" => KeywordKind::Implements,
        "implicit" => KeywordKind::Implicit,
        "consume" => KeywordKind::Consume,
        "loop" => KeywordKind::Loop
      },
      offset: 0,
    }
  }

  pub fn advance(&mut self) -> Option<char> {
    self.offset += 1;
    self.chars.next()
  }

  pub fn next_token(&mut self) -> Option<Token> {
    let ch = self.advance();

    match ch {
        
    }
  }
}
