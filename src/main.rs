#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod patch;
mod util;
mod wnd_main;
use wnd_main::WndMain;

fn main() {
	if let Err(e) = WndMain::new().run() {
		eprintln!("{}", e);
	}
}
