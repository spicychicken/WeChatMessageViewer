#include "Plist.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "plist/plist++.h"

using namespace Plist;

template<class NodeType>
NodeType* plistAsType(PList::Node* node) {
    return dynamic_cast<NodeType*>(node);
}

static std::string secondToDateTime(int second)
{
    std::chrono::system_clock::time_point tp(std::chrono::seconds::duration(second));
    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%F %T");
    return ss.str();
}

std::string getValueAsString(PList::Node* node)
{
    std::string value;
    switch (node->GetType())
    {
    case plist_type::PLIST_BOOLEAN:
        value = std::to_string(plistAsType<PList::Boolean>(node)->GetValue());
        break;
    
    case plist_type::PLIST_DATA:
        {
            auto data = plistAsType<PList::Data>(node)->GetValue();
            value.assign(data.begin(), data.end());
        }
        break;
    
    case plist_type::PLIST_DATE:
        {
            int32_t tv_sec = 0;
            int32_t tv_usec = 0;
            plist_get_date_val(node->GetPlist(), &tv_sec, &tv_usec);
            value = secondToDateTime(tv_sec + 978307200);
        }
        break;
    
    case plist_type::PLIST_KEY:
        value = plistAsType<PList::Key>(node)->GetValue();
        break;

    case plist_type::PLIST_REAL:
        value = std::to_string(plistAsType<PList::Real>(node)->GetValue());
        break;

    case plist_type::PLIST_STRING:
        value = plistAsType<PList::String>(node)->GetValue();
        break;

    case plist_type::PLIST_UID:
        value = std::to_string(plistAsType<PList::Uid>(node)->GetValue());
        break;

    case plist_type::PLIST_UINT:
        value = std::to_string(plistAsType<PList::Integer>(node)->GetValue());
        break;

    default:
        break;
    }
    return value;
}

void toMapImpl(PList::Node* node, PList::Array* objectsNode, const std::string& parentKey, std::unordered_map<std::string, std::string>& results)
{
    switch (node->GetType())
    {
    case plist_type::PLIST_DICT:
        {
            PList::Dictionary* pDict = plistAsType<PList::Dictionary>(node);
            for (PList::Dictionary::iterator iter = pDict->Begin(); iter != pDict->End(); ++iter)
            {
                if (iter->second)
                {
                    auto key = (parentKey == "" ? iter->first : parentKey + "." + iter->first);
                    toMapImpl(iter->second, objectsNode, key, results);
                }
            }
        }
        break;
    
    case plist_type::PLIST_ARRAY:
        {
            PList::Array* pArray = plistAsType<PList::Array>(node);
            int size = plist_array_get_size(pArray->GetPlist());
            for (int i = 0; i < size; ++i)
            {
                auto child = (*pArray)[i];
                if (child)
                {
                    auto key = (parentKey == "" ? std::to_string(i) : parentKey + "." + std::to_string(i));
                    toMapImpl(child, objectsNode, key, results);
                }
            }
        }
        break;

    case plist_type::PLIST_UID:
        {
            auto uid = getValueAsString(node);
            toMapImpl((*objectsNode)[std::stoi(uid)], objectsNode, parentKey, results);
        }
        break;

    default:
        results[parentKey] = getValueAsString(node);
        break;
    }
}

std::unordered_map<std::string, std::string> Plist::toMap(const std::string& content)
{
    std::unordered_map<std::string, std::string> results;
    plist_t node = nullptr;
    plist_from_memory(content.c_str(), content.length(), &node);
    if (node)
    {
        PList::Node* root = PList::Node::FromPlist(node);
        if (root->GetType() == plist_type::PLIST_DICT)
        {
            PList::Dictionary* pDict = plistAsType<PList::Dictionary>(root);
            for (PList::Dictionary::iterator iter = pDict->Begin(); iter != pDict->End(); ++iter)
            {
                if (iter->second)
                {
                    results[iter->first] = getValueAsString(iter->second);
                }
            }
        }
        delete root;
    }
    return results;
}

PList::Array* findObjects(PList::Node* root)
{
    if (root->GetType() == plist_type::PLIST_DICT)
    {
        PList::Dictionary* pDict = plistAsType<PList::Dictionary>(root);
        return plistAsType<PList::Array>((*pDict)["$objects"]);
    }
    return nullptr;
}

std::unordered_map<std::string, std::string> Plist::arrayToMap(const std::string& content, const std::string& key)
{
    std::unordered_map<std::string, std::string> results;
    plist_t node = nullptr;
    plist_from_memory(content.c_str(), content.length(), &node);
    if (node)
    {
        PList::Node* root = PList::Node::FromPlist(node);
        PList::Array* objectsNode = findObjects(root);
        if (root && objectsNode)
        {
            toMapImpl(root, objectsNode, "", results);
        }
        delete root;
    }
    return results;
}