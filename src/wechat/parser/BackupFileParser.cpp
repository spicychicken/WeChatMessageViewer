#include "BackupFileParser.h"

#include "ios/IOSBackupParser.h"

using namespace wechat;

parser::BackupFileParser* parser::createParser(ParserType type, const std::string& path)
{
    if (type == parser::ParserType::ParserType_IOS)
    {
        return new parser::ios::IOSBackupParser(path);
    }
    return nullptr;
}