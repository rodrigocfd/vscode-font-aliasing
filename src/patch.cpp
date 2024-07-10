#include <system_error>
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

void patch::doPatch(std::wstring_view vscodePath)
{
	std::wstring cssPath{vscodePath};
	cssPath.append(L"\\");
	cssPath.append(L"resources\\app\\out\\vs\\workbench\\workbench.desktop.main.css");

	std::wstring cssContents = lib::FileMapped::ReadAllStr(cssPath);



}
