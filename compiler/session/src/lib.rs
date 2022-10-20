mod stream;

pub use clap::Parser;
use pathdiff::diff_paths;
use span::miette::{Diagnostic, GraphicalReportHandler, NamedSource};
use std::path::PathBuf;
use stream::StreamingWriter;

#[derive(Debug, Parser)]
pub struct Session {
  #[clap(parse(from_os_str))]
  pub path: PathBuf,

  #[clap(skip)]
  pub source: String,
  #[clap(skip)]
  pub line_beginnings: Vec<usize>,
  #[clap(skip = NamedSource::new("", ""))]
  pub named_source: NamedSource,

  #[clap(skip)]
  report_handler: Option<GraphicalReportHandler>,
}

impl Session {
  pub fn init(&mut self) -> std::io::Result<()> {
    self.source = std::fs::read_to_string(&self.path)?;
    self.line_beginnings = self.source.lines().map(|line| line.len() + 1).collect();
    self.named_source = NamedSource::new(
      self
        .relative_path()
        .unwrap_or(self.path.clone())
        .to_str()
        .unwrap_or("<unknown>"),
      self.source.to_string(),
    );
    self.report_handler = Some(
      GraphicalReportHandler::new()
        .with_context_lines(3)
        .with_urls(true)
        .with_links(true)
    );

    Ok(())
  }

  pub fn relative_path(&self) -> Option<PathBuf> {
    diff_paths(self.path.as_path(), std::env::current_dir().ok()?)
  }

  pub fn emit(&self, diagnostic: &dyn Diagnostic) {
    let mut out = StreamingWriter::new();
    self
      .report_handler
      .as_ref()
      .unwrap()
      .render_report(&mut out, diagnostic)
      .expect("Failed to render diagnostic report");
  }
}
