#include <msgpack_module.hpp>

namespace ArkMsgpack
{
    Value pack(std::vector<Value>& args, Ark::VM* vm)
    {
        // pack(Value src) and return a buffer's string packed 
        if(args.size() != 1)
            throw std::runtime_error("ArgError : This function must have 1 arguments");
        ValueType type {args[0].valueType()};
        std::stringstream buffer;
        CObject value_src {get_cobject(args[0], type)};
        Value packed;
        
        buffer.seekg(0);
        if(type == ValueType::True || type == ValueType::False)
        {
            bool src {std::get<bool>(value_src)};
            msgpack::pack(buffer, src);
            packed = Value(buffer.str());
        }
        else if(type == ValueType::Number)
        {
            double src {std::get<double>(value_src)};
            msgpack::pack(buffer, src);
            packed = Value(buffer.str());
        }
        else if(type == ValueType::String)
        {
            std::string src {std::get<std::string>(value_src)};
            msgpack::pack(buffer, src);
            packed = Value(buffer.str());
        }
        else
        {
            std::vector<Value> src {std::get<std::vector<Value>>(value_src)};
            packed = list_packing(src);
        }

        return packed;
    }

    Value unpack(std::vector<Value>& args, Ark::VM* vm)
    {
        //unpack(Value packed(string or list)) and return an object unpacked 
        if(args.size() != 1)
            throw std::runtime_error("ArgError : This function must have 1 arguments");
        if(args[0].valueType() != ValueType::String && args[0].valueType() != ValueType::List)
            throw Ark::TypeError("The packed buffer must be a string or a list");    
        Value dst;
        bool ark_bool;
        double ark_number;
        std::string ark_string;
        msgpack::object deserialized;
        auto type_test = [&](void) {
            std::string packed {static_cast<Value>(args[0]).stringRef().toString()};
            deserialized = msgpack::unpack(packed.data(), packed.size()).get();
            try
            {
                deserialized.convert(ark_bool);
                dst = Value(ark_bool);
            }
            catch(const std::bad_cast &e)
            {
                try
                {
                    deserialized.convert(ark_number);
                    dst = Value(ark_number);
                }
                catch(const std::bad_cast &e) 
                {
                    try
                    {
                        deserialized.convert(ark_string);
                        dst = Value(ark_string);
                    }
                    catch(const std::exception &e) {}
                }
            }
        };

        if(args[0].valueType() == ValueType::List)
        {
            std::vector<Value> packed {static_cast<Value>(args[0]).list()};
            dst = list_unpacking(packed);
        }
        else
        {
            type_test();
        }

        return dst;
    }

    Value object_str(std::vector<Value>& args, Ark::VM* vm)
    {
        if(args.size() != 1)
            throw std::runtime_error("ArgError : This function must have 1 argument");
        if(args[0].valueType() != ValueType::String && args[0].valueType() != ValueType::List)
            throw Ark::TypeError("The packed buffer must be a string or a list");
        Value msg_object_str;
        std::ostringstream str_buffer; 
        msgpack::object deserialized;

        if(args[0].valueType() == ValueType::List)
        {
            list_unpacked_str(static_cast<Value>(args[0]).list(), str_buffer);
            msg_object_str = Value(str_buffer.str());
        }
        else
        {
            std::string packed {static_cast<Value>(args[0]).stringRef().toString()};
            deserialized =  msgpack::unpack(packed.data(), packed.size()).get();
            str_buffer << deserialized;
            msg_object_str = Value(str_buffer.str());
        }
        
        return msg_object_str;
    }
}

ARK_API_EXPORT mapping* getFunctionsMapping()
{
    mapping* map = mapping_create(3);

    mapping_add(map[0], "msgPack", ArkMsgpack::pack);
    mapping_add(map[1], "msgUnpack", ArkMsgpack::unpack);
    mapping_add(map[2], "msgObjectStr", ArkMsgpack::object_str);

    return map;
}