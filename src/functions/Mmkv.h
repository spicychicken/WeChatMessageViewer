#ifndef MMKV_H
#define MMKV_H

#include <string>
#include <unordered_map>

namespace Mmkv
{
    std::unordered_map<std::string, std::string> toMap(const std::string& filePath, const std::string& fileCrcPath);
}
#endif // MMKV_H