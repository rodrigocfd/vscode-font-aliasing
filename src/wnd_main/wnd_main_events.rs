use winsafe::{prelude::*, self as w, co};

use crate::patch;
use crate::util;
use super::WndMain;

impl WndMain {
	pub(super) fn events(&self) {
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
			if self2.prompt_continue_if_running()? {
				let clock = util::Timer::start();
				self2.msg_after_process(
					patch::patch_font(&self2.txt_path.text()),
					&format!("Font successfully patched in {:.2}ms.", clock.now_ms()),
				)?;
			}
			Ok(())
		});

		let self2 = self.clone();
		self.btn_patch_icon.on().bn_clicked(move || {
			if self2.prompt_continue_if_running()? {
				let clock = util::Timer::start();
				self2.msg_after_process(
					patch::patch_icon(&self2.txt_path.text()),
					&format!("Suggestion box icon successfully patched in {:.2}ms.", clock.now_ms()),
				)?;
			}
			Ok(())
		});
	}

	fn prompt_continue_if_running(&self) -> w::AnyResult<bool> {
		if !patch::is_vscode_running()? {
			return Ok(true) // it's not even running
		}

		let (clicked_btn, _, _) = w::TaskDialogIndirect(&w::TASKDIALOGCONFIG {
			hwnd_parent: Some(self.wnd.hwnd()),
			window_title: Some("VS Code appears to be running"),
			content: Some("It's recommended to close VS Code before patching.\n\
				If you run the patch now, you must reload VS Code.\n\n\
				Patch anyway?"),
			main_icon: w::IconIdTd::Td(co::TD_ICON::WARNING),
			common_buttons: co::TDCBF::OK | co::TDCBF::CANCEL,
			flags: co::TDF::ALLOW_DIALOG_CANCELLATION | co::TDF::POSITION_RELATIVE_TO_WINDOW,
			..Default::default()
		})?;

		Ok(clicked_btn == co::DLGID::OK)
	}

	fn msg_after_process(&self, res: w::AnyResult<()>, txt_success: &str) -> w::AnyResult<()> {
		match res {
			Err(e) => w::TaskDialogIndirect(&w::TASKDIALOGCONFIG {
				hwnd_parent: Some(self.wnd.hwnd()),
				window_title: Some("Patching failed"),
				content: Some(&e.to_string()),
				main_icon: w::IconIdTd::Td(co::TD_ICON::ERROR),
				common_buttons: co::TDCBF::OK,
				flags: co::TDF::ALLOW_DIALOG_CANCELLATION | co::TDF::POSITION_RELATIVE_TO_WINDOW,
				..Default::default()
			})?,
			Ok(_) => w::TaskDialogIndirect(&w::TASKDIALOGCONFIG {
				hwnd_parent: Some(self.wnd.hwnd()),
				window_title: Some("Operation successful"),
				content: Some(txt_success),
				main_icon: w::IconIdTd::Td(co::TD_ICON::INFORMATION),
				common_buttons: co::TDCBF::OK,
				flags: co::TDF::ALLOW_DIALOG_CANCELLATION | co::TDF::POSITION_RELATIVE_TO_WINDOW,
				..Default::default()
			})?,
		};
		Ok(())
	}
}
