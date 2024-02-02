#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Utils
{
    bool isFileExist(const std::string& fileName);
    bool startsWith(const std::string& value, const std::string& prefix);
    bool endsWith(const std::string& value, const std::string& postfix);
    std::vector<std::string> split(const std::string& value, const std::string& delimiters);
    std::string readFile(const std::string& fileName);
    std::string readBinaryFile(const std::string& fileName);
    std::string timestampToString(int timestamp);
    std::string currentTime();
    void outputLog(const std::string& log);
    bool match(const std::string& value, const std::string& pattern);

    std::string removeHtmlTags(const std::string& htmls);
};

#endif // UTILS_H
