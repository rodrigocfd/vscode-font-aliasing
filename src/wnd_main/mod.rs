use winsafe::gui;

mod ids;
mod wnd_main_events;
mod wnd_main_funcs;

#[allow(dead_code)]
#[derive(Clone)]
pub struct WndMain {
	wnd:            gui::WindowMain,
	lbl_path:       gui::Label,
	txt_path:       gui::Edit,
	btn_choose:     gui::Button,
	btn_patch_font: gui::Button,
	btn_patch_icon: gui::Button,
}
