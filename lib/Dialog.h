#pragma once
#include <optional>
#include <string>
#include <vector>
#include "Window.h"

namespace lib {

// Base to all dialog windows.
class Dialog : public Window {
public:
	virtual ~Dialog() { }

	constexpr Dialog() = default;
	Dialog(const Dialog&) = delete;
	Dialog(Dialog&&) = delete;
	Dialog& operator=(const Dialog&) = delete;
	Dialog& operator=(Dialog&&) = delete;

protected:
	virtual INT_PTR dlgProc(UINT uMsg, WPARAM wp, LPARAM lp) = 0; // to be overriden in user class
	static INT_PTR CALLBACK _DlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp);

	// Calls DragQueryFile() for each file, then DragFinish().
	[[nodiscard]] std::vector<std::wstring> droppedFiles(HDROP hDrop) const;

	[[nodiscard]] std::optional<std::wstring> fileOpen(std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts) const { return _fileOpenSave(true, false, namesExts); }
	[[nodiscard]] std::optional<std::vector<std::wstring>> fileOpenMany(std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts) const;
	[[nodiscard]] std::optional<std::wstring> fileSave(std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts) const { return _fileOpenSave(false, false, namesExts); }
	[[nodiscard]] std::optional<std::wstring> folderOpen() const { return _fileOpenSave(true, true, {}); }
	[[nodiscard]] std::optional<std::wstring> _fileOpenSave(bool isOpen, bool isFolder,
		std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts) const;

	// Returns IDOK, IDCANCEL, etc.
	int msgBox(std::wstring_view title, std::wstring_view mainInstruction,
		std::wstring_view body, int tdcbfButtons, LPWSTR tdIcon) const;

private:
	Window::_hWndPtr;
};

}
