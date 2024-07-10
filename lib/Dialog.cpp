#include <algorithm>
#include <Windows.h>
#include <CommCtrl.h>
#include <ShObjIdl.h>
#include "Dialog.h"
#include "Com.h"
using namespace lib;

INT_PTR CALLBACK Dialog::_DlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp)
{
	Dialog *pSelf = nullptr;
	
	if (uMsg == WM_INITDIALOG) {
		pSelf = reinterpret_cast<Dialog*>(lp);
		SetWindowLongPtrW(hDlg, DWLP_USER, reinterpret_cast<LONG_PTR>(pSelf));
		*pSelf->_hWndPtr() = hDlg;
	} else [[likely]] {
		pSelf = reinterpret_cast<Dialog*>(GetWindowLongPtrW(hDlg, DWLP_USER));
	}

	if (!pSelf) [[unlikely]] {
		// No pointer stored, nothing is done.
		// Prevents processing before WM_INITDIALOG and after WM_NCDESTROY.
		return FALSE;
	}

	INT_PTR userRet = pSelf->dlgProc(uMsg, wp, lp);

	if (uMsg == WM_NCDESTROY) {
		SetWindowLongPtrW(hDlg, DWLP_USER, 0);
		*pSelf->_hWndPtr() = nullptr;
		userRet = TRUE;
	}
	return userRet;
}

std::vector<std::wstring> Dialog::droppedFiles(HDROP hDrop) const
{
	UINT count = DragQueryFileW(hDrop, 0xffff'ffff, nullptr, 0);
	std::vector<std::wstring> paths;
	paths.reserve(count);

	for (UINT i = 0; i < count; ++i) {
		WCHAR buf[MAX_PATH + 1] = {L'\0'};
		DragQueryFileW(hDrop, i, buf, MAX_PATH + 1);
		paths.emplace_back(buf);
	}

	DragFinish(hDrop);
	return paths;
}

static std::vector<COMDLG_FILTERSPEC> _makeFilters(
	std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts)
{
	std::vector<COMDLG_FILTERSPEC> rawFilters; // {L"Word Document (*.doc)", L"*.doc"}
	rawFilters.reserve(namesExts.size());
	for (const auto& nameExt : namesExts)
		rawFilters.emplace_back(nameExt.first.data(), nameExt.second.data());
	return rawFilters;
}

static std::wstring _shellItemPath(const ComPtr<IShellItem>& shi)
{
	PWSTR ptrPath = nullptr;
	shi->GetDisplayName(SIGDN_FILESYSPATH, &ptrPath);
	std::wstring strPath{(ptrPath == nullptr ? L"" : ptrPath)};
	CoTaskMemFree(ptrPath);
	return strPath;
}

std::optional<std::vector<std::wstring>> Dialog::fileOpenMany(
	std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts) const
{
	std::vector<COMDLG_FILTERSPEC> filters = _makeFilters(namesExts);

	ComPtr<IFileOpenDialog> fsd{CLSID_FileOpenDialog};
	fsd->SetOptions(FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | FOS_ALLOWMULTISELECT);
	fsd->SetFileTypes(static_cast<int>(filters.size()), filters.data());
	fsd->SetFileTypeIndex(1);
	if (fsd->Show(hWnd()) == S_OK) {
		ComPtr<IShellItemArray> shiArr;
		fsd->GetResults(shiArr.pptr());

		DWORD count = 0;
		shiArr->GetCount(&count);

		std::vector<std::wstring> strPaths;
		strPaths.reserve(count);
		for (size_t i = 0; i < count; ++i) {
			ComPtr<IShellItem> shi;
			shiArr->GetItemAt(static_cast<DWORD>(i), shi.pptr());
			strPaths.emplace_back(_shellItemPath(shi));
		}

		std::sort(strPaths.begin(), strPaths.end(), [](const auto& a, const auto& b) -> bool {
			return lstrcmpiW(a.c_str(), b.c_str()) < 1;
		});
		return strPaths;
	} else {
		return std::nullopt;
	}
}

std::optional<std::wstring> Dialog::_fileOpenSave(bool isOpen, bool isFolder,
	std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> namesExts) const
{
	std::vector<COMDLG_FILTERSPEC> filters = _makeFilters(namesExts);

	ComPtr<IFileDialog> fd{(isOpen ? CLSID_FileOpenDialog : CLSID_FileSaveDialog)};
	fd->SetOptions(FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | (isFolder ? FOS_PICKFOLDERS : 0));
	if (!isFolder) {
		fd->SetFileTypes(static_cast<int>(filters.size()), filters.data());
		fd->SetFileTypeIndex(1);
	}
	if (fd->Show(hWnd()) == S_OK) {
		ComPtr<IShellItem> shi;
		fd->GetResult(shi.pptr());
		return _shellItemPath(shi);
	} else {
		return std::nullopt;
	}
}

int Dialog::msgBox(std::wstring_view title, std::wstring_view mainInstruction,
	std::wstring_view body, int tdcbfButtons, LPWSTR tdIcon) const
{
	TASKDIALOGCONFIG tdc = {
		.cbSize = sizeof(TASKDIALOGCONFIG),
		.hwndParent = hWnd(),
		.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_POSITION_RELATIVE_TO_WINDOW,
		.dwCommonButtons = tdcbfButtons,
		.pszWindowTitle = title.data(),
		.pszMainIcon = tdIcon,
		.pszMainInstruction = mainInstruction.empty() ? nullptr : mainInstruction.data(),
		.pszContent = body.data(),
	};

	int pnButton = 0;
	TaskDialogIndirect(&tdc, &pnButton, nullptr, nullptr);
	return pnButton;
}
