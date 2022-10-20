use errors::miette::Context;
use miette::IntoDiagnostic;
use session::{Parser, Session};
use span::miette;

fn main() -> miette::Result<()> {
  miette::set_panic_hook();

  let mut session: Session = Session::parse();
  session
    .init()
    .into_diagnostic()
    .wrap_err(format!("Failed to compile `{}`", session.path.display()))?;
  
  

  Ok(())
}
