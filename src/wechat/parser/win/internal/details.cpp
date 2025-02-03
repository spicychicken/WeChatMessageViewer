#include "details.h"

#include <iostream>
#include <codecvt>

#include <Windows.h>
#include <tlhelp32.h>   // tool help library

using std::string;
using std::wstring;
using std::vector;
using std::tuple;
using std::unordered_map;

using namespace wechat::parser::win;

unordered_map<string, unsigned long> offsetOfRawKey = {
    {"3.9.8.25", 0x3DFDAD0},
    {"3.9.9.43", 0x40E9F90},
    {"3.9.10.27", 0x5AB8590}
};

wstring s2ws(const string& s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(s);
}

string w2ss(const wstring& s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(s);
}

static DWORD getProcessIDByName(const string& name)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(PROCESSENTRY32);

    DWORD processID = -1;
    if (Process32First(hSnapShot, &processInfo))
    {
        do
        {
            if (wstring(processInfo.szExeFile).find(s2ws(name)) != wstring::npos)
            {
                processID = processInfo.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapShot, &processInfo));
    }

    CloseHandle(hSnapShot);

    return processID;
}

static string getFileVersionInfoByPath(const string& path)
{
    int left = path.find('['), right = path.find(']', left);
    return path.substr(left + 1, right - left - 1);
    /* DWORD dwHandle;  
    DWORD dwVersionInfoSize = GetFileVersionInfoSize(path.c_str(), &dwHandle);

    if (dwVersionInfoSize != 0)
    {
        char* pVersionInfo = new char[dwVersionInfoSize];  
        if (FALSE != GetFileVersionInfo(path.c_str(), 0, dwVersionInfoSize, pVersionInfo))  
        {
            std::cout << pVersionInfo << std::endl;
        }
        delete[] pVersionInfo;
    }
    return ""; */
}

static tuple<string, BYTE*> getWeChatWinDllVersionAndBaseAddr(DWORD wechatProcessID)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, wechatProcessID);
    MODULEENTRY32 moduleInfo;
    moduleInfo.dwSize = sizeof(MODULEENTRY32);

    string version;
    BYTE* baseAddr = nullptr;
    if (Module32First(hSnapShot, &moduleInfo)) {
        do {
            if (w2ss(moduleInfo.szModule) == "WeChatWin.dll") {
                // version = moduleInfo
                version = getFileVersionInfoByPath(w2ss(moduleInfo.szExePath));
                baseAddr = moduleInfo.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapShot, &moduleInfo));
    }

    CloseHandle(hSnapShot);

    return std::make_tuple(version, baseAddr);
}

static string getSqliteRawKey(DWORD processID, const string& version, BYTE* baseAddr) {
    unsigned long long addr = (unsigned long long)baseAddr + offsetOfRawKey[version];
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);

    unsigned long long pointer = 0;
    size_t byteReaded = 0;
    ReadProcessMemory(handle, (LPCVOID)addr, &pointer, 8, &byteReaded);

    unsigned char rawKeys[32] = {0};
    ReadProcessMemory(handle, (LPCVOID)pointer, &rawKeys, 32, &byteReaded);

    CloseHandle(handle);

    return string((char*)rawKeys, 32);
}

string details::detectSqliteRawKey()
{
    DWORD processID = getProcessIDByName("WeChat.exe");
    auto [version, baseAddr] = getWeChatWinDllVersionAndBaseAddr(processID);

    if (offsetOfRawKey.count(version))
    {
        return getSqliteRawKey(processID, version, baseAddr);
    }
    return "";
}