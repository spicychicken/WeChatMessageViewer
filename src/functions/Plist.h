#ifndef PLIST_H
#define PLIST_H

#include <string>
#include <unordered_map>

namespace Plist
{
    std::unordered_map<std::string, std::string> toMap(const std::string& content);
    std::unordered_map<std::string, std::string> arrayToMap(const std::string& content, const std::string& key = "$objects");
}
#endif // PLIST_H