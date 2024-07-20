#include <optional>
#include <system_error>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include <TlHelp32.h>
#include <wee/lib.h>
#include "patch.h"

bool patch::isVscodeRunning()
{
	struct HandleSnap32 final {
		HANDLE h32;
		~HandleSnap32() { CloseHandle(h32); }
	};

	HandleSnap32 h{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)};
	if (h.h32 == INVALID_HANDLE_VALUE) [[unlikely]] {
		throw std::system_error(GetLastError(), std::system_category(), "CreateToolhelp32Snapshot failed");
	}

	PROCESSENTRY32W pe = {.dwSize = sizeof(PROCESSENTRY32W)};
	if (!Process32FirstW(h.h32, &pe)) {
		DWORD err = GetLastError();
		if (err == ERROR_NO_MORE_FILES) [[likely]] {
			return false;
		} else [[unlikely]] {
			throw std::system_error(err, std::system_category(), "Process32First failed");
		}
	}

	for (;;) {
		if (lib::str::eqI(pe.szExeFile, L"Code.exe")) // process is running
			return true;

		if (!Process32NextW(h.h32, &pe)) {
			DWORD err = GetLastError();
			if (err == ERROR_NO_MORE_FILES) [[likely]] {
				return false;
			} else [[unlikely]] {
				throw std::system_error(err, std::system_category(), "Process32Next failed");
			}
		}
	}
}

static std::wstring _patchFont(std::wstring_view cssContents)
{
	LPCWSTR END_OF_COMMS = L"-*/";
	LPCWSTR MAGIC_PATCH = L"\n*{text-shadow:transparent 0px 0px 0px, rgba(0, 0, 0, 0.5) 0px 0px 0px !important;}";
	
	std::optional<size_t> maybeIdxStartCode = lib::str::position(cssContents, END_OF_COMMS);
	if (!maybeIdxStartCode.has_value()) [[unlikely]] {
		throw std::runtime_error("CSS end of comments not found.");
	}
	size_t idxStartCode = maybeIdxStartCode.value() + lstrlenW(END_OF_COMMS);

	// Is our magic path the first thing past the comments block?
	if (lib::str::position(cssContents, MAGIC_PATCH, idxStartCode) == idxStartCode) {
		throw std::runtime_error("Font already patched.");
	}

	auto newContents = lib::str::newReserved(cssContents.length() + lstrlenW(MAGIC_PATCH));
	newContents.append(cssContents.begin(), cssContents.begin() + idxStartCode); // comments block
	newContents.append(MAGIC_PATCH);
	newContents.append(cssContents.begin() + idxStartCode, cssContents.end()); // rest of file
	return newContents;
}

static std::wstring _patchIcon(std::wstring_view cssContents)
{
	LPCWSTR NATURAL = L".monaco-editor .suggest-widget .monaco-list .monaco-list-row.focused .codicon{color:var(--vscode-editorSuggestWidget-selectedIconForeground)}";
	LPCWSTR PATCHED = L" /*.monaco-editor .suggest-widget .monaco-list .monaco-list-row.focused .codicon{color:var(--vscode-editorSuggestWidget-selectedIconForeground)}*/ ";

	if (lib::str::contains(cssContents, PATCHED)) {
		throw std::runtime_error("Suggestion box icon already patched.");
	}

	std::optional<size_t> maybeIdx = lib::str::position(cssContents, NATURAL);
	if (!maybeIdx.has_value()) [[unlikely]] {
		throw std::runtime_error("Suggestion box icon CSS entry not found.");
	}
	size_t idx = maybeIdx.value();

	auto newContents = lib::str::newReserved(cssContents.length() + lstrlenW(PATCHED) - lstrlenW(NATURAL));
	newContents.append(cssContents.begin(), cssContents.begin() + idx); // all code up to part
	newContents.append(PATCHED);
	newContents.append(cssContents.begin() + idx + lstrlenW(NATURAL), cssContents.end()); // rest of file
	return newContents;
}

patch::Res patch::doPatch(std::wstring_view installPath, bool doFont, bool doIcon)
{
	std::wstring cssPath{installPath};
	cssPath.append(L"\\");
	cssPath.append(L"resources\\app\\out\\vs\\workbench\\workbench.desktop.main.css");

	bool fontOk = true, iconOk = true;
	Res res = {.fontMsg = L"Font unpatched.", .iconMsg = L"Suggestion icon unpatched."};

	std::wstring cssContents = lib::FileMapped::ReadAllStr(cssPath);
	if (doFont) {
		try {
			cssContents = _patchFont(cssContents);
			res.fontMsg = L"Font patched successfully.";
		} catch (const std::runtime_error& err) {
			fontOk = false;
			res.fontMsg = lib::str::toWide(err.what());
		}
	}
	if (doIcon) {
		try {
			cssContents = _patchIcon(cssContents);
			res.iconMsg = L"Suggestion icon patched successfully.";
		} catch (const std::runtime_error& err) {
			iconOk = false;
			res.iconMsg = lib::str::toWide(err.what());
		}
	}

	if (!fontOk && !iconOk) {
		res.tdIcon = TD_ERROR_ICON;
	} else if (!fontOk || !iconOk) {
		res.tdIcon = TD_WARNING_ICON;
	} else {
		res.tdIcon = TD_INFORMATION_ICON;
	}

	lib::File::EraseAndWriteStr(cssPath, cssContents);
	return res;
}
