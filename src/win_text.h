#ifndef ROAD_OF_LIFE_WIN_TEXT_H
#define ROAD_OF_LIFE_WIN_TEXT_H

#include <windows.h>
#include <string>

namespace wintext {
inline std::wstring widen(const std::string& text)
{
    if (text.empty()) return std::wstring();
    int count = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring result(count, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &result[0], count);
    return result;
}

inline std::string narrow(const std::wstring& text)
{
    if (text.empty()) return std::string();
    int count = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    std::string result(count, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &result[0], count, nullptr, nullptr);
    return result;
}

inline std::wstring trim(const std::wstring& value)
{
    const std::wstring whitespace = L" \t\r\n";
    const std::wstring::size_type first = value.find_first_not_of(whitespace);
    if (first == std::wstring::npos) return std::wstring();
    return value.substr(first, value.find_last_not_of(whitespace) - first + 1);
}
}

#endif
