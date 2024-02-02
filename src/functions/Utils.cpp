#include "Utils.h"

#include <iostream>

#include <regex>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

using std::string;
using std::ifstream;
using std::vector;
using std::unordered_map;

bool Utils::isFileExist(const string &fileName)
{
    // C++17以前
    ifstream ifile(fileName.c_str());
    return ifile.good();

    // C++17以后
    // std::filesystem::exists
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