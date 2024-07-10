#include "DlgMain.h"
#include "../res/resource.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow)
{
	//lib::Com comLib;
	DlgMain d;
	return d.runMain(hInst, DLG_MAIN, cmdShow);
}

INT_PTR DlgMain::dlgProc(UINT uMsg, WPARAM wp, LPARAM lp)
{
	switch (uMsg) {
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

INT_PTR DlgMain::onBtnBrowse()
{
	return TRUE;
}

INT_PTR DlgMain::onBtnPatch()
{
	return TRUE;
}
