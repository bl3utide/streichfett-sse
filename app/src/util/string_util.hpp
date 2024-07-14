#pragma once
#include <atlstr.h>

namespace StringUtil
{

// Hint: '%%' escapes '%'
template <typename... Args>
std::string format(const std::string& fmt, Args ... args)
{
    size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
    std::vector<char> buf(len + 1);
    std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
    return std::string(&buf[0], &buf[0] + len);
}

inline bool isInteger(const std::string& str)
{
    return std::regex_match(str, std::regex("[(-|+)|][0-9]*"));
}

enum class FileVersion : int
{
    ProductVersionMajorOnly = 0,
    ProductVersionFull = 1,
    InternalName,
    ProductName,
    CompanyName,
    Copyright,
};

const inline std::string getExeVersionInfo(FileVersion order) noexcept
{
    TCHAR file_name[MAX_PATH + 1];
    GetModuleFileNameA(nullptr, file_name, sizeof(file_name));
    auto size = GetFileVersionInfoSizeA(file_name, nullptr);
    auto version_ptr = new BYTE[size];

    if (GetFileVersionInfoA(file_name, NULL, size, version_ptr))
    {
        VS_FIXEDFILEINFO* file_info_ptr = nullptr;
        UINT query_len;

        if (order <= FileVersion::ProductVersionFull)
        {
            VerQueryValueA(version_ptr, "\\", (void**)&file_info_ptr, &query_len);
            auto v1 = HIWORD(file_info_ptr->dwProductVersionMS);
            auto v2 = LOWORD(file_info_ptr->dwProductVersionMS);
            auto v3 = HIWORD(file_info_ptr->dwProductVersionLS);
            if (order == FileVersion::ProductVersionMajorOnly)
            {
                return format("%hu.%hu", v1, v2);
            }
            else
            {
                return format("%hu.%hu.%hu", v1, v2, v3);
            }
        }
        else
        {
            WORD* lang_code_ptr = nullptr;
            CString sub_block;
            TCHAR* str_info_ptr = nullptr;
            VerQueryValueA(version_ptr, "\\VarFileInfo\\Translation", (void**)&lang_code_ptr, &query_len);

            std::string order_str;
            if (order == FileVersion::InternalName)
            {
                order_str = std::string("InternalName");
            }
            else if (order == FileVersion::ProductName)
            {
                order_str = std::string("ProductName");
            }
            else if (order == FileVersion::CompanyName)
            {
                order_str = std::string("CompanyName");
            }
            else if (order == FileVersion::Copyright)
            {
                order_str = std::string("LegalCopyright");
            }
            sub_block.Format("\\StringFileInfo\\%04x%04x\\%s", lang_code_ptr[0], lang_code_ptr[1], order_str.c_str());

            VerQueryValueA(version_ptr, (LPTSTR)(LPCTSTR)sub_block, (void**)&str_info_ptr, &query_len);
            if (query_len > 0)
            {
                return format("%s", str_info_ptr);
            }
            else
            {
                return "";
            }
        }
    }

    return "";
}

} // StringUtil
