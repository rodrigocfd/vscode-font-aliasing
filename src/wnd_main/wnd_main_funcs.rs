use winsafe::{prelude::*, self as w, co, gui};

use crate::patch;
use super::{ids, WndMain};

impl WndMain {
	pub fn new() -> Self {
		let hv_none = (gui::Horz::None, gui::Vert::None);

		let wnd            = gui::WindowMain::new_dlg(ids::DLG_MAIN, Some(ids::ICO_MAIN), None);
		let lbl_path       = gui::Label::new_dlg(&wnd, ids::LBL_PATH, hv_none);
		let txt_path       = gui::Edit::new_dlg(&wnd, ids::TXT_PATH, hv_none);
		let btn_choose     = gui::Button::new_dlg(&wnd, ids::BTN_CHOOSE, hv_none);
		let btn_patch_font = gui::Button::new_dlg(&wnd, ids::BTN_PATCH_FONT, hv_none);
		let btn_patch_icon = gui::Button::new_dlg(&wnd, ids::BTN_PATCH_ICON, hv_none);

		let self2 = Self {
			wnd,
			lbl_path, txt_path, btn_choose,
			btn_patch_font, btn_patch_icon,
		};
		self2._events();
		self2
	}

	pub fn run(&self) -> w::AnyResult<i32> {
		self.wnd.run_main(None)
	}
}
