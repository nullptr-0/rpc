#pragma once

#define RPC_SERIALIZE_PUBLIC_MEMBER(memberName) j[#memberName] = instance.memberName

#define RPC_SERIALIZE_NONPUBLIC_MEMBER(memberName, getterMethod) j[#memberName] = instance.getterMethod()

// RPC_CUSTOM_TYPE_SERIALIZER(typeName, serializeImpl)
#define RPC_CUSTOM_TYPE_SERIALIZER(typeName, ...) void to_json(json& j, const typeName& instance) \
{ \
    __VA_ARGS__; \
}

#define RPC_DESERIALIZE_PUBLIC_MEMBER(memberName) j.at(#memberName).get_to(instance.memberName)

#define RPC_DESERIALIZE_NONPUBLIC_MEMBER(memberName, setterMethod) instance.setterMethod(j[#memberName])

// RPC_CUSTOM_TYPE_DESERIALIZER(typeName, deserializeImpl)
#define RPC_CUSTOM_TYPE_DESERIALIZER(typeName, ...) void from_json(const json& j, typeName& instance) \
{ \
    __VA_ARGS__; \
}
