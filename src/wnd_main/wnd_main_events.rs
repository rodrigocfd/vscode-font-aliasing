use winsafe::{prelude::*, self as w, co};

use crate::patch;
use crate::util;
use super::WndMain;

impl WndMain {
	pub(super) fn _events(&self) {

		let self2 = self.clone();
		self.wnd.on().wm_command_accel_menu(co::DLGID::CANCEL, move || {
			self2.wnd.close(); // close on Esc
			Ok(())
		});

		let self2 = self.clone();
		self.btn_choose.on().bn_clicked(move || {
			let fileo = w::CoCreateInstance::<w::IFileOpenDialog>(
				&co::CLSID::FileOpenDialog, None, co::CLSCTX::INPROC_SERVER)?;

			fileo.SetOptions(
				fileo.GetOptions()?
					| co::FOS::FILEMUSTEXIST
					| co::FOS::PICKFOLDERS,
			)?;

			if fileo.Show(self2.wnd.hwnd())? {
				self2.txt_path.set_text(
					&fileo.GetResult()?
						.GetDisplayName(co::SIGDN::FILESYSPATH)?,
				);

				self2.btn_patch_font.hwnd().EnableWindow(true);
				self2.btn_patch_icon.hwnd().EnableWindow(true);
				self2.btn_patch_font.focus();
			}

			Ok(())
		});

		let self2 = self.clone();
		self.btn_patch_font.on().bn_clicked(move || {
			if !self2._ok_if_running()? {
				return Ok(())
			}

			let clock = util::Timer::start();
			match patch::patch_font(&self2.txt_path.text()) {
				Err(e) => self2.wnd.hwnd().TaskDialog(
					Some("Patching failed"),
					None,
					Some(&e.to_string()),
					co::TDCBF::OK,
					w::IconRes::Error,
				)?,
				Ok(_) => self2.wnd.hwnd().TaskDialog(
					Some("Operation successful"),
					None,
					Some(&format!("Font successfully patched in {:.2}ms.", clock.now_ms())),
					co::TDCBF::OK,
					w::IconRes::Info,
				)?,
			};

			Ok(())
		});

		let self2 = self.clone();
		self.btn_patch_icon.on().bn_clicked(move || {
			if !self2._ok_if_running()? {
				return Ok(())
			}

			let clock = util::Timer::start();
			match patch::patch_icon(&self2.txt_path.text()) {
				Err(e) => self2.wnd.hwnd().TaskDialog(
					Some("Patching failed"),
					None,
					Some(&e.to_string()),
					co::TDCBF::OK,
					w::IconRes::Error,
				)?,
				Ok(_) => self2.wnd.hwnd().TaskDialog(
					Some("Operation successful"),
					None,
					Some(&format!("Suggestion box icon successfully patched in {:.2}ms.", clock.now_ms())),
					co::TDCBF::OK,
					w::IconRes::Info,
				)?,
			};

			Ok(())
		});
	}
}
