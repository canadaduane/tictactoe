#![feature(proc_macro_hygiene, decl_macro)]

use std::env;
use std::sync::atomic::{AtomicUsize, Ordering};
#[macro_use]
extern crate rocket;
use rocket::{State, Config};
use rocket::config::Environment;
use rocket::response::status::NotFound;

struct Board{
  a1: AtomicUsize,
  a2: AtomicUsize,
  a3: AtomicUsize,
  a4: AtomicUsize,
  a5: AtomicUsize,
  a6: AtomicUsize,
  a7: AtomicUsize,
  a8: AtomicUsize,
  a9: AtomicUsize,

  b1: AtomicUsize,
  b2: AtomicUsize,
  b3: AtomicUsize,
  b4: AtomicUsize,
  b5: AtomicUsize,
  b6: AtomicUsize,
  b7: AtomicUsize,
  b8: AtomicUsize,
  b9: AtomicUsize,
}

#[get("/<board_id>")]
fn get_board(board_id: usize, board: State<Board>) -> Result<String, NotFound<String>> {
  let (p1, p2, p3, p4, p5, p6, p7, p8, p9) =
  match board_id {
    0 => (
      board.a1.load(Ordering::Relaxed),
      board.a2.load(Ordering::Relaxed),
      board.a3.load(Ordering::Relaxed),
      board.a4.load(Ordering::Relaxed),
      board.a5.load(Ordering::Relaxed),
      board.a6.load(Ordering::Relaxed),
      board.a7.load(Ordering::Relaxed),
      board.a8.load(Ordering::Relaxed),
      board.a9.load(Ordering::Relaxed),
    ),
    1 => (
      board.b1.load(Ordering::Relaxed),
      board.b2.load(Ordering::Relaxed),
      board.b3.load(Ordering::Relaxed),
      board.b4.load(Ordering::Relaxed),
      board.b5.load(Ordering::Relaxed),
      board.b6.load(Ordering::Relaxed),
      board.b7.load(Ordering::Relaxed),
      board.b8.load(Ordering::Relaxed),
      board.b9.load(Ordering::Relaxed),
    ),
    _ => (0, 0, 0, 0, 0, 0, 0, 0, 0)
  };
  if board_id <= 1 {
    Ok(format!("{}.{}.{}.{}.{}.{}.{}.{}.{}", p1, p2, p3, p4, p5, p6, p7, p8, p9))
  } else {
    Err(NotFound(board_id.to_string()))
  }
}

#[put("/<board_id>/<p1>/<p2>/<p3>/<p4>/<p5>/<p6>/<p7>/<p8>/<p9>")]
fn put_board(board_id: usize, board: State<Board>, p1: usize, p2: usize, p3: usize, p4: usize, p5: usize, p6: usize, p7: usize, p8: usize, p9: usize) -> String {
  match board_id {
    0 => {
      board.a1.store(p1, Ordering::Relaxed);
      board.a2.store(p2, Ordering::Relaxed);
      board.a3.store(p3, Ordering::Relaxed);
      board.a4.store(p4, Ordering::Relaxed);
      board.a5.store(p5, Ordering::Relaxed);
      board.a6.store(p6, Ordering::Relaxed);
      board.a7.store(p7, Ordering::Relaxed);
      board.a8.store(p8, Ordering::Relaxed);
      board.a9.store(p9, Ordering::Relaxed);
    },
    1 => {
      board.b1.store(p1, Ordering::Relaxed);
      board.b2.store(p2, Ordering::Relaxed);
      board.b3.store(p3, Ordering::Relaxed);
      board.b4.store(p4, Ordering::Relaxed);
      board.b5.store(p5, Ordering::Relaxed);
      board.b6.store(p6, Ordering::Relaxed);
      board.b7.store(p7, Ordering::Relaxed);
      board.b8.store(p8, Ordering::Relaxed);
      board.b9.store(p9, Ordering::Relaxed);
    },
    _ => {}
  }
  format!("{}.{}.{}.{}.{}.{}.{}.{}.{}", p1, p2, p3, p4, p5, p6, p7, p8, p9)
}

/// Look up our server port number in PORT, for compatibility with Heroku.
fn get_server_port() -> u16 {
  env::var("PORT").ok().and_then(|p| p.parse().ok()).unwrap_or(8080)
}

fn main() {
  let config = Config::build(Environment::Production)
    .port(get_server_port())
    .finalize()
    .unwrap();

  rocket::custom(config)
    .manage(Board {
      a1: AtomicUsize::new(0),
      a2: AtomicUsize::new(0),
      a3: AtomicUsize::new(0),
      a4: AtomicUsize::new(0),
      a5: AtomicUsize::new(0),
      a6: AtomicUsize::new(0),
      a7: AtomicUsize::new(0),
      a8: AtomicUsize::new(0),
      a9: AtomicUsize::new(0),
      
      b1: AtomicUsize::new(0),
      b2: AtomicUsize::new(0),
      b3: AtomicUsize::new(0),
      b4: AtomicUsize::new(0),
      b5: AtomicUsize::new(0),
      b6: AtomicUsize::new(0),
      b7: AtomicUsize::new(0),
      b8: AtomicUsize::new(0),
      b9: AtomicUsize::new(0), })
    .mount("/", routes![get_board, put_board])
    .launch();
}
