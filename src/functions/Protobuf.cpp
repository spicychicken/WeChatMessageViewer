#include "Protobuf.h"

#include <memory>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>

using google::protobuf::FileDescriptorProto;
using google::protobuf::DescriptorPool;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::UnknownFieldSet;
using google::protobuf::UnknownField;

using namespace Protobuf;

class ProtoParserInitializer
{
public:
    ProtoParserInitializer()
    {
        fdproto = new FileDescriptorProto();
        fdproto->set_name("empty_message.proto");
        fdproto->add_message_type()->set_name("EmptyMessage");

        dpool = new DescriptorPool();
        dpool->BuildFile(*fdproto);

        const Descriptor *descriptor = dpool->FindMessageTypeByName("EmptyMessage");

        dmfactory = new DynamicMessageFactory(dpool);
        emptyMsgType = dmfactory->GetPrototype(descriptor);
    }

    ~ProtoParserInitializer()
    {

    }

    Message* clone()
    {
        return emptyMsgType->New();
    }

private:
    DescriptorPool*         dpool;
    FileDescriptorProto*    fdproto;
    DynamicMessageFactory*  dmfactory;
    const Message*          emptyMsgType;
};

static ProtoParserInitializer initializer;

std::string getUnknownFieldData(const UnknownField& field)
{
    std::string value;
    switch (field.type())
    {
    case UnknownField::TYPE_LENGTH_DELIMITED:
        {
            value.assign(field.length_delimited().c_str(), field.GetLengthDelimitedSize());
        }
        break;
    case UnknownField::TYPE_VARINT:
        {
            value = std::to_string(field.varint());
        }
        break;
    case UnknownField::TYPE_FIXED32:
		{
			value = std::to_string(field.fixed32());
		}
		break;
    case UnknownField::TYPE_FIXED64:
		{
			value = std::to_string(field.fixed64());
		}
		break;
    default:
        break;
    }
    return value;
}

static int countKeyWithPrefix(const std::unordered_map<std::string, std::string>& results, const std::string& key)
{
    int count = 0;

}

static void increaseKeyAndSetValue(std::unordered_map<std::string, std::string>& results, const std::string& key, const std::string& value)
{
    // add index
    if (results.count(key + ".0") == 0)
    {
        results[key + ".0"] = value;
    }
    else
    {
        int count = std::count_if(results.begin(), results.end(), [&key](auto item) {
            if (item.first.length() < key.length()) {
                return false;
            }
            else if (item.first.substr(0, key.length()) != key) {
                return false;
            }
            return true;
        });
        results[key + "." + std::to_string(count)] = value;
    }
}

// [To-Do] memory leak?
static void toMapImpl(const std::string& parentID, const std::string& content, std::unordered_map<std::string, std::string>& results)
{
    Message* message = initializer.clone();
    if (message->ParseFromString(content) || message->ParseFromString(u8"\n" + content))
    {
        const UnknownFieldSet& ufs = message->GetReflection()->GetUnknownFields(*message);
        for (int i = 0; i < ufs.field_count(); ++i)
        {
            auto field = ufs.field(i);

            std::string fieldID = (parentID == "" ? "" : parentID + ".") + std::to_string(field.number());
            // std::string fieldID = (parentID == "" ? "" : parentID + ".") + std::to_string(i);

            if (field.type() == UnknownField::TYPE_GROUP)
            {
                results[fieldID] = "group";
            }
            else if (field.type() == UnknownField::TYPE_LENGTH_DELIMITED)
            {
                toMapImpl(fieldID, getUnknownFieldData(field), results);
            }
            else
            {
                increaseKeyAndSetValue(results, fieldID, getUnknownFieldData(field));
            }
        }
    }
    else{
        increaseKeyAndSetValue(results, parentID, content);
    }
}

std::unordered_map<std::string, std::string> Protobuf::toMap(const std::string& content)
{
    std::unordered_map<std::string, std::string> results;
    toMapImpl("", content, results);
    return results;
}
