#include <optional>
#include <system_error>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include "../lib/lib.h"
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
		if (!lstrcmpiW(pe.szExeFile, L"Code.exe")) {
			return true;
		}

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
	throw std::runtime_error("Xamba xamba.");
	return {};
}

void patch::doPatch(std::wstring_view installPath, bool doFont, bool doIcon)
{
	std::wstring cssPath{installPath};
	cssPath.append(L"\\");
	cssPath.append(L"resources\\app\\out\\vs\\workbench\\workbench.desktop.main.css");

	std::wstring cssContents = lib::FileMapped::ReadAllStr(cssPath);
	std::vector<std::wstring> errors;
	if (doFont) {
		try {
			cssContents = _patchFont(cssContents);
		} catch (const std::runtime_error& err) {
			errors.emplace_back(lib::str::toWide(err.what()));
		}
	}
	if (doIcon) {
		try {
			cssContents = _patchIcon(cssContents);
		} catch (const std::runtime_error& err) {
			errors.emplace_back(lib::str::toWide(err.what()));
		}
	}

	if (!errors.empty()) { // accumulate all exceptions in a single string, and throw it
		std::wstring finalMsg = lib::str::join(errors, L"\n");
		throw std::runtime_error(lib::str::toAnsi(finalMsg));
	}



}
