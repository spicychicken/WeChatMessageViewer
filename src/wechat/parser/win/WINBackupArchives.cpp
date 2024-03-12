#include "WINBackupArchives.h"

#include <iostream>
#include <algorithm>
#include <chrono>

#include "Constants.h"
#include "functions/Utils.h"

using std::string;
using std::vector;
using std::function;

using wechat::parser::win::WINBackupArchives;

void WINBackupArchives::setArchivesPath(const std::string& path) {
    folderName      = path;
}

vector<string> WINBackupArchives::listLoginUserNames()
{
    vector<string> folders;
    for (auto& folder : Utils::listSubFolder(folderName))
    {
        if (Utils::isFolderExist(folderName + "/" + folder + "/Msg"))
        {
            folders.push_back(folder);
        }
    }
    return folders;
}

vector<string> WINBackupArchives::listMsgDBNames(const string& userName)
{
    vector<string> dbNames;
    for (auto& folder : Utils::listSubFiles(folderName + "/" + userName + "/Msg/Multi"))
    {
        if (Utils::startsWith(folder, "MSG") && Utils::endsWith(folder, ".db"))
        {
            dbNames.push_back(folder);
        }
    }
    return dbNames;
}

vector<string> WINBackupArchives::listAudioDBNames(const string& userName)
{
    vector<string> dbNames;
    for (auto& folder : Utils::listSubFiles(folderName + "/" + userName + "/Msg/Multi"))
    {
        if (Utils::startsWith(folder, "MediaMSG") && Utils::endsWith(folder, ".db"))
        {
            dbNames.push_back(folder);
        }
    }
    return dbNames;
}

vector<string> WINBackupArchives::listUsedUserNameFor(const string& newUserName)
{
    vector<string> usedNames;
    for (auto& folder : Utils::listSubFolder(folderName + "/" + newUserName))
    {
        if (Utils::startsWith(folder, "account_"))
        {
            usedNames.push_back(folder.substr(9));
        }
    }
    return usedNames;
}

// [To-Do]
string WINBackupArchives::correctPath(const string& userName, const string& path)
{
    string newPath = path;
    if (!path.empty())
    {
        if (path.length() > folderName.length() && path.compare(0, folderName.length(), folderName) == 0)
        {
            // do nothing
        }
        else
        {
            newPath = folderName + "/" + path;
        }

        /* std::cout << "Before: " << path << std::endl;
        std::cout << "After: " << newPath << std::endl;
        std::cout << std::endl; */
    }

    return newPath;
}