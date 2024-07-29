#include <stdexcept>
#include <Windows.h>
#include <CommCtrl.h>
#include "DlgMain.h"
#include "patch.h"
#include "../res/resource.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow)
{
	DlgMain d;
	return lib::runMain(d, hInst, DLG_MAIN, cmdShow, ICO_VSCGREEN, ACC_MAIN);
}

INT_PTR DlgMain::dlgProc(UINT uMsg, WPARAM wp, LPARAM lp)
{
	switch (uMsg) {
		case WM_INITDIALOG: return onInitDialog();
		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case CHK_PATCH_FONT:
				case CHK_PATCH_ICON: return onChkChange();
				case BTN_BROWSE:     return onBtnBrowse();
				case BTN_PATCH:      return onBtnPatch();
				case MNU_ABOUT:      return onAbout();
				case IDCANCEL:       PostMessage(hWnd(), WM_CLOSE, 0, 0); return TRUE;
				default:             return FALSE;
			}
		case WM_CLOSE:     DestroyWindow(hWnd()); return TRUE;
		case WM_NCDESTROY: PostQuitMessage(0); return TRUE;
		default: return FALSE;
	}
}

INT_PTR DlgMain::onInitDialog()
{
	lib::CheckRadio{this, CHK_PATCH_FONT}.check();
	lib::CheckRadio{this, CHK_PATCH_ICON}.check();
	return TRUE;
}

INT_PTR DlgMain::onChkChange()
{
	EnableWindow(GetDlgItem(hWnd(), BTN_PATCH),
		lib::CheckRadio{this, CHK_PATCH_FONT}.isChecked()
		|| lib::CheckRadio{this, CHK_PATCH_ICON}.isChecked());
	return TRUE;
}

INT_PTR DlgMain::onBtnBrowse()
{
	std::optional<std::wstring> maybeFolder = dlg.showOpenFolder();
	if (maybeFolder.has_value()) {
		lib::NativeControl{this, TXT_PATH}.setText(maybeFolder.value());
		lib::NativeControl{this, BTN_PATCH}.focus();
	}
	return TRUE;
}

INT_PTR DlgMain::onBtnPatch()
{
	std::wstring installPath = lib::NativeControl{this, TXT_PATH}.text();
	if (!lib::path::exists(installPath) || !lib::path::isDir(installPath)) {
		dlg.msgBox(L"Bad path", L"", L"The chosen installation path is not valid.", TDCBF_OK_BUTTON, TD_ERROR_ICON);
		lib::NativeControl{this, BTN_BROWSE}.focus();
		return TRUE;
	}

	if (patch::isVscodeRunning()
			&& dlg.msgBox(L"VS Code appears to be running", L"",
				L"It's recommended to close VS Code before patching.\n"
				L"If you run the patch now, you must reload VS Code.\n\n"
				L"Patch anyway?",
			TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON, TD_WARNING_ICON) == IDCANCEL) {
		return TRUE;
	}

	try {
		patch::Res res = patch::doPatch(installPath,
			lib::CheckRadio{this, CHK_PATCH_FONT}.isChecked(),
			lib::CheckRadio{this, CHK_PATCH_ICON}.isChecked());
		dlg.msgBox(L"Patching finished", L"", res.fontMsg + L"\n" + res.iconMsg, TDCBF_OK_BUTTON, res.tdIcon);
	} catch (const std::runtime_error& err) {
		dlg.msgBox(L"Patching failed", L"", lib::str::toWide(err.what()), TDCBF_OK_BUTTON, TD_ERROR_ICON);
	}
	return TRUE;
}

INT_PTR DlgMain::onAbout()
{
	lib::VersionInfo vi;
	std::wstring_view productName = vi.strInfo(vi.langsCps()[0], L"ProductName");
	std::array<WORD, 4> ver = vi.verNum();
	auto body = lib::str::fmt(L"Version %u.%u.%u.\nWritten in C++20.", ver[0], ver[1], ver[2]);
	
	dlg.msgBox(L"About", productName, body, TDCBF_OK_BUTTON, TD_INFORMATION_ICON);
	return TRUE;
}
