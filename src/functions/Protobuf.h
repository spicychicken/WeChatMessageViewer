#ifndef PROTOBUF_H
#define PROTOBUF_H

#include <string>
#include <unordered_map>

namespace Protobuf
{
    std::unordered_map<std::string, std::string> toMap(const std::string& content, int level = 2);
}
#endif // PROTOBUF_H