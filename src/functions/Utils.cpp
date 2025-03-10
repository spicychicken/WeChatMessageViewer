#include "Utils.h"

#include <iostream>
#include <filesystem>

#include <regex>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include <tinyxml2.h>

using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::unordered_map;
using namespace tinyxml2;

string Utils::getCurrentPath()
{
    return std::filesystem::current_path().string();
}

vector<string> Utils::listSubFolder(const string& folder)
{
    vector<string> subFolder;
    for (auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (entry.is_directory()) {
            subFolder.push_back(entry.path().filename().string());
        }
    }
    return subFolder;
}

vector<string> Utils::listSubFiles(const string& folder)
{
    vector<string> subFolder;
    for (auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_directory()) {
            subFolder.push_back(entry.path().filename().string());
        }
    }
    return subFolder;
}

bool Utils::isFolderExist(const std::string& folder)
{
    return std::filesystem::exists(folder);
}

bool Utils::isFileExist(const string &fileName)
{
    // C++17以前
    // ifstream ifile(fileName.c_str());
    // return ifile.good();

    // C++17以后
    return std::filesystem::exists(fileName);
}

bool Utils::startsWith(const string& value, const string& prefix)
{
    return value.compare(0, prefix.length(), prefix) == 0;
}

bool Utils::endsWith(const string& value, const string& postfix)
{
    return value.compare(value.length() - postfix.length(), postfix.length(), postfix) == 0;
}

vector<string> Utils::split(const string& value, const string& delimiters)
{
    vector<string> results;
    auto pos1 = 0;
    auto pos2 = value.find(delimiters);
    while (pos2 != string::npos)
    {
        if (pos1 != pos2)
        {
            results.push_back(value.substr(pos1, pos2 - pos1));
        }
        pos1 = pos2 + delimiters.length();
        pos2 = value.find(delimiters, pos1);
    }

    if (pos1 != value.length())
    {
        results.push_back(value.substr(pos1));
    }

    return results;
}

string Utils::readFile(const string& fileName)
{
    // file must be exist
    ifstream ifile(fileName.c_str());
    return string(std::istreambuf_iterator<char>{ifile}, std::istreambuf_iterator<char>{});
}

string Utils::readBinaryFile(const string& fileName)
{
    // file must be exist
    ifstream ifile(fileName.c_str(), std::ios::in | std::ios::binary);
    return string(std::istreambuf_iterator<char>{ifile}, std::istreambuf_iterator<char>{});
}

void Utils::writeBinaryFile(const std::string& fileName, const std::string& fileContent)
{
    ofstream ofile(fileName.c_str(), std::ios::out | std::ios::binary);
    std::ostreambuf_iterator<char> output_it(ofile);
    std::copy(fileContent.begin(), fileContent.end(), output_it);
    ofile.flush();
    ofile.close();
}

std::string Utils::timestampToString(int timestamp)
{
    std::chrono::system_clock::time_point tp(std::chrono::seconds::duration(timestamp));
    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%F %T");
    return ss.str();
}

std::string Utils::currentTime() {
    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(currentTime);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%F %T");
    return ss.str();
}

void Utils::outputLog(const std::string& log) {
    std::cout << log << std::endl;
}

bool Utils::match(const std::string& value, const std::string& pattern)
{
    std::regex patternRegex(pattern, std::regex_constants::icase);
    return std::regex_search(value, patternRegex);
}

std::string Utils::removeHtmlTags(const std::string& htmls)
{
    std::string output = htmls;
    std::regex html_tags("<[^>]*>");
    output = std::regex_replace(output, html_tags, "");
    return output;
}


string Utils::getXmlAttributeByPath(const string& xmlContent, const string& path, const string& attr)
{
    string attrValue;
    XMLDocument doc;
    if (doc.Parse(xmlContent.c_str()) == XML_SUCCESS)
    {
        vector<string> paths = Utils::split(path, "/");

        XMLElement* element = doc.RootElement();
        for (int i = 1; i < paths.size() && element; ++i)
        {
            element = element->FirstChildElement(paths[i].c_str());
        }

        if (element)
        {
            const XMLAttribute* attrNode = element->FindAttribute(attr.c_str());
            if (attrNode)
            {
                attrValue.assign(attrNode->Value());
            }
        }
    }
    return attrValue;
}

string Utils::getXmlNodeByPath(const string& xmlContent, const string& path)
{
    string value;
    XMLDocument doc;
    if (doc.Parse(xmlContent.c_str()) == XML_SUCCESS)
    {
        vector<string> paths = Utils::split(path, "/");

        XMLElement* element = doc.RootElement();
        for (int i = 1; i < paths.size() && element; ++i)
        {
            element = element->FirstChildElement(paths[i].c_str());
        }

        if (element && element->FirstChild())
        {
            value.assign(element->FirstChild()->Value());
        }
    }
    return value;
}

string Utils::stringToHexString(const string& input)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0'); // 设置填充字符为'0'
    for (char byte : input) {
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(byte)); // 转换并设置每个字节的宽度为2
    }
    return ss.str();
}

string Utils::hexStringToString(const string& input)
{
    std::string result;
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    for (size_t i = 0; i < input.length(); i += 2) {
        std::string byteStr = input.substr(i, 2);
        unsigned int n;
        ss << std::hex << byteStr;
        ss >> n;
        ss.clear(); // 重置stringstream
        result.push_back(static_cast<char>(n));
    }
    return result;
}

