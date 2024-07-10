#pragma once
#include <string>

namespace patch {

struct Res final {
	LPWSTR tdIcon;
	std::wstring fontMsg;
	std::wstring iconMsg;
};

[[nodiscard]] bool isVscodeRunning();
[[nodiscard]] Res doPatch(std::wstring_view installPath, bool doFont, bool doIcon);

}
