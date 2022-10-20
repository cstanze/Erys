use std::borrow::Cow;
pub use span::{Span, miette};
pub use miette::Severity;
use miette::{Diagnostic as MietteDiagnostic};
use session::Session;

pub type DiagnosticMessage<'a> = Cow<'a, str>;

#[derive(Debug, Clone)]
struct LabeledSpan<'a> {
  label: DiagnosticMessage<'a>,
  span: Span,
}

impl<'a> LabeledSpan<'a> {
  pub fn new(label: DiagnosticMessage<'a>, span: Span) -> Self {
    Self { label, span }
  }

  pub fn as_miette(&self) -> miette::LabeledSpan {
    miette::LabeledSpan::new(Some(self.label.clone().into()), self.span.low, self.span.len())
  }
}

#[derive(Debug)]
pub struct Diagnostic<'a> {
  pub(crate) code: Option<DiagnosticMessage<'a>>,
  pub(crate) severity: Severity,
  pub(crate) help: Option<DiagnosticMessage<'a>>,
  pub(crate) labels: Vec<LabeledSpan<'a>>,
  pub(crate) message: DiagnosticMessage<'a>,
  pub(crate) session: &'a Session,
}

impl std::fmt::Display for Diagnostic<'_> {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    match self.severity {
      Severity::Error => write!(f, "error"),
      Severity::Warning => write!(f, "warning"),
      Severity::Advice => write!(f, "note"),
    }?;

    write!(f, ": {}", self.message)
  }
}

impl std::error::Error for Diagnostic<'_> {}

impl MietteDiagnostic for Diagnostic<'_> {
  fn code<'a>(&'a self) -> Option<Box<dyn std::fmt::Display + 'a>> {
    if let Some(code) = &self.code {
      Some(Box::new(code.to_string()))
    } else {
      None
    }
  }

  fn severity(&self) -> Option<Severity> {
    Some(self.severity)
  }

  fn help<'a>(&'a self) -> Option<Box<dyn std::fmt::Display + 'a>> {
    if let Some(help) = &self.help {
      Some(Box::new(help.to_string()))
    } else {
      None
    }
  }

  fn source_code(&self) -> Option<&dyn span::miette::SourceCode> {
    Some(&self.session.named_source)
  }

  fn labels(&self) -> Option<Box<dyn Iterator<Item = span::miette::LabeledSpan> + '_>> {
    Some(
      Box::new(
        self.labels
          .iter()
          .map(|label| label.as_miette())
      )
    )
  }
}

pub struct DiagnosticBuilder<'a> {
  pub(crate) diag: Diagnostic<'a>,
}

impl<'a> DiagnosticBuilder<'a> {
  pub(crate) fn new(session: &'a Session, severity: Severity, message: DiagnosticMessage<'a>) -> Self {
    Self {
      diag: Diagnostic {
        code: None,
        severity,
        help: None,
        labels: vec![],
        message,
        session,
      }
    }
  }

  pub fn error(session: &'a Session, message: DiagnosticMessage<'a>) -> Self {
    Self::new(session, Severity::Error, message)
  }

  pub fn warning(session: &'a Session, message: DiagnosticMessage<'a>) -> Self {
    Self::new(session, Severity::Warning, message)
  }

  pub fn advice(session: &'a Session, message: DiagnosticMessage<'a>) -> Self {
    Self::new(session, Severity::Advice, message)
  }

  pub fn with_code(mut self, code: DiagnosticMessage<'a>) -> Self {
    self.diag.code = Some(code);
    self
  }

  pub fn with_help(mut self, help: DiagnosticMessage<'a>) -> Self {
    self.diag.help = Some(help);
    self
  }

  pub fn with_label(mut self, label: DiagnosticMessage<'a>, span: Span) -> Self {
    self.diag.labels.push(LabeledSpan::new(label, span));
    self
  }

  pub fn emit(self) {
    if self.diag.labels.is_empty() {
      panic!("no labels for diagnostic");
    }

    self.diag.session.emit(&self.diag);
  }
}
