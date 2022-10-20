pub use miette;
use miette::SourceSpan;
use std::fmt;

/// A span represents a region of source code,
/// consisting of a low byte and an offset.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Span {
  /// The low byte of the span.
  pub low: usize,

  /// The offset of the span (ie. the length)
  pub offset: usize,
}

impl Span {
  /// Creates a new span with the given low byte and offset.
  pub fn new(low: usize, offset: usize) -> Self {
    debug_assert!(low <= (low + offset), "low must be less than low + offset");
    
    Span {
      low: low,
      offset: offset,
    }
  }

  /// Returns a new span with the same low byte of a given span,
  /// and a new length.
  pub fn from_span_offset(span: Span, length: usize) -> Self {
    Span::new(span.low, length)
  }

  /// Returns the length of the span.
  pub fn len(&self) -> usize {
    self.offset
  }

  /// Returns false if the span is empty.
  pub fn is_empty(&self) -> bool {
    self.offset == 0
  }
}

impl Into<SourceSpan> for Span {
  fn into(self) -> SourceSpan {
    SourceSpan::new(self.low.into(), self.offset.into())
  }
}

impl From<(usize, usize)> for Span {
  fn from(tuple: (usize, usize)) -> Self {
    Span::new(tuple.0, tuple.1)
  }
}

impl From<std::ops::Range<usize>> for Span {
  fn from(range: std::ops::Range<usize>) -> Self {
    Span::new(range.start, range.end - range.start)
  }
}

impl fmt::Display for Span {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    write!(f, "{}..{}", self.low, self.low + self.offset)
  }
}
