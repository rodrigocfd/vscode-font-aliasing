#include <Windows.h>
#include <CommCtrl.h>
#include "DlgMain.h"
#include "patch.h"
#include "../res/resource.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow)
{
	DlgMain d;
	return d.runMain(hInst, DLG_MAIN, cmdShow);
}

INT_PTR DlgMain::dlgProc(UINT uMsg, WPARAM wp, LPARAM lp)
{
	switch (uMsg) {
		case WM_INITDIALOG: return onInitDialog();
		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case BTN_BROWSE: return onBtnBrowse();
				case BTN_PATCH:  return onBtnPatch();
				case IDCANCEL:   PostMessage(hWnd(), WM_CLOSE, 0, 0); return TRUE;
				default:         return FALSE;
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

INT_PTR DlgMain::onBtnBrowse()
{
	std::optional<std::wstring> maybeFolder = this->folderOpen();
	if (maybeFolder.has_value())
		lib::NativeControl{this, TXT_PATH}.setText(maybeFolder.value());
	lib::NativeControl{this, BTN_PATCH}.focus();
	return TRUE;
}

INT_PTR DlgMain::onBtnPatch()
{
	if (patch::isVscodeRunning()
			&& this->msgBox(L"VS Code appears to be running", L"",
				L"It's recommended to close VS Code before patching.\n"
				L"If you run the patch now, you must reload VS Code.\n\n"
				L"Patch anyway?",
			TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON, TD_WARNING_ICON) == IDCANCEL) {
		return TRUE;
	}


	return TRUE;
}
