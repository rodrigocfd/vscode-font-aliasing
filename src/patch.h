#pragma once
#include <string>

namespace patch {

[[nodiscard]] bool isVscodeRunning();
void doPatch(std::wstring_view installPath, bool doFont, bool doIcon);

}
