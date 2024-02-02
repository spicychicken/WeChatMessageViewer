#include "Mmkv.h"

#include "mmkv/CodedInputData.h"
#include "mmkv/PBUtility.h"
#include "mmkv/MMKVMetaInfo.hpp"
#include "functions/Utils.h"

#include <vector>

using std::string;
using std::unordered_map;

using namespace Mmkv;

class MMKVReader {
private:
    const unsigned char *m_ptr;
    size_t m_size;
    mutable size_t m_position;

public:
    MMKVReader(const unsigned char *ptr, size_t size) : m_ptr(ptr), m_size(size), m_position(0) {}
    std::string readKey() const {
        std::string key;
        uint32_t keyLength = 0;
        const unsigned char *data = calcVarint32Ptr(m_ptr + m_position, m_ptr + m_size, &keyLength);
        m_position += data - (m_ptr + m_position);

        if (keyLength > 0) {
            auto s_size = static_cast<size_t>(keyLength);
            if (s_size <= m_size - m_position) {
                key.assign((char *) (m_ptr + m_position), s_size);
                m_position += s_size;
            } else {
                m_position = m_size;
            }
        }

        return key;
    }

    void skipValue() const {
        uint32_t valueLength = 0;
        const unsigned char *data = calcVarint32Ptr(m_ptr + m_position, m_ptr + m_size, &valueLength);
        m_position += data - (m_ptr + m_position);
        if (valueLength > 0) {
            if ((m_position + valueLength) > m_size) {
                m_position = m_size;
            } else {
                m_position += valueLength;
            }
        }
    }

    std::string readStringValue() const {
        // MMBuffer
        std::string value;
        uint32_t valueLength = 0;
        const unsigned char *data = calcVarint32Ptr(m_ptr + m_position, m_ptr + m_size, &valueLength);
        m_position += data - (m_ptr + m_position);
        if (valueLength > 0) {
            // MMBuffer
            uint32_t mbbLength = 0;
            const unsigned char *ptr = m_ptr + m_position;
            ptr = calcVarint32Ptr(ptr, m_ptr + m_size, &mbbLength);

            if (mbbLength > 0) {
                auto s_size = static_cast<size_t>(mbbLength);
                if (s_size <= m_size - m_position) {
                    value.assign((char *) (ptr), s_size);
                    m_position += valueLength;
                } else {
                    m_position = m_size;
                }
            } else {
                m_position += valueLength;
            }
        }

        return value;
    }

    void seek(size_t position) const { m_position = position; }

    size_t getPos() const { return m_position; }

    bool isAtEnd() const {
        return m_position >= m_size;
    }

    static const char *GetVarint32PtrFallback(const char *p, const char *limit, uint32_t *value) {
        uint32_t result = 0;
        for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
            uint32_t byte = *(reinterpret_cast<const unsigned char *>(p));
            p++;
            if (byte & 128) {
                // More bytes are present
                result |= ((byte & 127) << shift);
            } else {
                result |= (byte << shift);
                *value = result;
                return reinterpret_cast<const char *>(p);
            }
        }
        return NULL;
    }

    static const char *calcVarint32Ptr(const char *p, const char *limit, uint32_t *value) {
        if (p < limit) {
            uint32_t result = *(reinterpret_cast<const unsigned char *>(p));
            if ((result & 0x80) == 0) {
                *value = result;
                return p + 1;
            }
        }
        return GetVarint32PtrFallback(p, limit, value);
    }

    static const unsigned char *calcVarint32Ptr(const unsigned char *p, const unsigned char *limit, uint32_t *value) {
        const char *p1 =
            calcVarint32Ptr(reinterpret_cast<const char *>(p), reinterpret_cast<const char *>(limit), value);
        return reinterpret_cast<const unsigned char *>(p1);
    }

    static std::unordered_map<string, string> parse(const std::string &path, const std::string &crcPath) {
        unordered_map<string, string> mmkvMapValues;

        uint32_t lastActualSize = 0;
        // 86: usrName
        // 87: name
        // 88: DisplayName
        string contents = Utils::readBinaryFile(crcPath);
        if (contents.size() >= 36) {
            memcpy(&lastActualSize, contents.c_str() + 32, 4);
        } else {
        }

        contents = Utils::readBinaryFile(path);

        uint32_t actualSize = 0;
        if (contents.size() >= 4) {
            memcpy(&actualSize, contents.c_str(), 4);
        }
        if (actualSize <= 0) {
            actualSize = lastActualSize;
        }

        if (actualSize <= 0) {
            return mmkvMapValues;
        }

        actualSize += 4;
        if (contents.size() < actualSize) {
        }

        MMKVReader reader((const unsigned char*)contents.c_str(), actualSize);
        reader.seek(8);

        while (!reader.isAtEnd()) {

            const auto k = reader.readKey();
            if (k.empty()) {
                break;
            }

            mmkvMapValues.insert(std::make_pair(k, reader.readStringValue()));
        }

        return mmkvMapValues;
    }
};

/*
std::unordered_map<std::string, std::string> Mmkv::toMap(const std::string& filePath, const std::string& fileCrcPath)
{
    std::unordered_map<std::string, std::string> results;
    std::string contents = Utils::readBinaryFile(filePath);
    mmkv::CodedInputData  cid(contents.c_str(), contents.length());
    cid.seek(mmkv::pbFixed32Size());
    
    while (!cid.isAtEnd())
    {
        mmkv::KeyValueHolder kvHolder;
        const auto &key = cid.readString(kvHolder);
        if (key.length() > 0) {
            cid.readData(kvHolder);
            if (kvHolder.valueSize > 0) {
                auto data = kvHolder.toMMBuffer(contents.c_str() + mmkv::pbFixed32Size());
                mmkv::CodedInputData input(data.getPtr(), data.length());
                results.insert(std::make_pair(key, input.readString()));
            }
        }
    }
    return results;
}
*/

size_t readActualSize(const std::string& contents, const std::string& fileCrcPath)
{
    uint32_t actualSize = 0;
    memcpy(&actualSize, contents.c_str(), mmkv::pbFixed32Size());

    if (actualSize == 0)
    {
        mmkv::MMKVMetaInfo    meta;
        meta.read(Utils::readBinaryFile(fileCrcPath).c_str());
        return meta.m_lastConfirmedMetaInfo.lastActualSize;
    }
    return actualSize;
}

std::unordered_map<std::string, std::string> Mmkv::toMap(const std::string& filePath, const std::string& fileCrcPath)
{
    std::unordered_map<std::string, std::string> results;
    std::string contents = Utils::readBinaryFile(filePath);
    size_t actualSize = readActualSize(contents, fileCrcPath);
    mmkv::CodedInputData  cid(contents.c_str() + mmkv::pbFixed32Size(), contents.length() - mmkv::pbFixed32Size());
    cid.seek(actualSize);

    while (!cid.isAtEnd())
    {
        mmkv::KeyValueHolder kvHolder;
        const auto &key = cid.readString(kvHolder);
        if (key.length() > 0) {
            cid.readData(kvHolder);
            if (kvHolder.valueSize > 0) {
                auto data = kvHolder.toMMBuffer(contents.c_str());
                mmkv::CodedInputData input(data.getPtr(), data.length());
                results.insert(std::make_pair(key, input.readString()));
            }
        }
    }

    return results;
}

/*
std::unordered_map<std::string, std::string> Mmkv::toMap(const std::string& filePath, const std::string& fileCrcPath)
{
    std::unordered_map<std::string, std::string> results;
    std::string contents = Utils::readBinaryFile(filePath);
    mmkv::CodedInputData  cid(contents.c_str(), contents.length());
    cid.seek(mmkv::pbFixed32Size());
    
    while (!cid.isAtEnd())
    {
        const auto& key = cid.readString();
        const auto& buffer = cid.readData();
        if (buffer.length() > 0)
        {
            mmkv::CodedInputData input(buffer.getPtr(), buffer.length());
            results.insert(std::make_pair(key, input.readString()));
        }
    }
    return results;
}
*/