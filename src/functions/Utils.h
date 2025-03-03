#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Utils
{
    std::string getCurrentPath();
    std::vector<std::string> listSubFolder(const std::string& folder);
    std::vector<std::string> listSubFiles(const std::string& folder);
    bool isFolderExist(const std::string& folder);
    bool isFileExist(const std::string& fileName);
    bool startsWith(const std::string& value, const std::string& prefix);
    bool endsWith(const std::string& value, const std::string& postfix);
    std::vector<std::string> split(const std::string& value, const std::string& delimiters);
    std::string readFile(const std::string& fileName);
    std::string readBinaryFile(const std::string& fileName);
    void writeBinaryFile(const std::string& fileName, const std::string& fileContent);
    std::string timestampToString(int timestamp);
    std::string currentTime();
    void outputLog(const std::string& log);
    bool match(const std::string& value, const std::string& pattern);

    std::string removeHtmlTags(const std::string& htmls);
    std::string getXmlAttributeByPath(const std::string& xmlContent, const std::string& path, const std::string& attr);
    std::string getXmlNodeByPath(const std::string& xmlContent, const std::string& path);

    std::string stringToHexString(const std::string& input);
    std::string hexStringToString(const std::string& input);
};

#endif // UTILS_H
