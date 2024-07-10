#pragma once
#include <span>
#include <string>
#include <vector>
#include <Windows.h>

namespace lib::str {

constexpr size_t SSO_LEN = std::string{}.capacity();

[[nodiscard]] bool contains(std::wstring_view s, std::wstring_view what);
[[nodiscard]] bool endsWith(std::wstring_view s, std::wstring_view theEnd);
[[nodiscard]] bool endsWithI(std::wstring_view s, std::wstring_view theEnd);
[[nodiscard]] LPCWSTR guessLineBreak(std::wstring_view s);
[[nodiscard]] std::wstring parse(std::span<BYTE> src);
void removeDiacritics(std::wstring& s);
[[nodiscard]] std::vector<std::wstring> split(std::wstring_view s, std::wstring_view delimiter);
[[nodiscard]] std::vector<std::wstring> splitLines(std::wstring_view s);
[[nodiscard]] bool startsWith(std::wstring_view s, std::wstring_view theStart);
[[nodiscard]] bool startsWithI(std::wstring_view s, std::wstring_view theStart);
[[nodiscard]] std::string toAnsi(std::wstring_view s);
[[nodiscard]] std::wstring toLower(std::wstring_view s);
[[nodiscard]] std::wstring toUpper(std::wstring_view s);
[[nodiscard]] std::vector<BYTE> toUtf8Blob(std::wstring_view s, bool writeBom = false);
[[nodiscard]] std::wstring toWide(std::string_view s);
void trim(std::wstring& s);
void trimNulls(std::wstring& s);

}
