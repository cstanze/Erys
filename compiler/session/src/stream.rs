use std::fmt;

/// A wrapper around stdout that allows
/// for streaming output through `fmt::Write`
pub struct StreamingWriter;

impl StreamingWriter {
  /// Create a new streaming writer
  pub fn new() -> Self {
    StreamingWriter
  }
}

impl fmt::Write for StreamingWriter {
  fn write_str(&mut self, s: &str) -> fmt::Result {
    print!("{}", s);
    Ok(())
  }
}
