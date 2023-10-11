#pragma once
#include <any>
#include "json.hpp"

using json = nlohmann::json;

// Define a variant type to store various argument types
using Argument = std::any;

#define RPC_SET_ARGS(argIndex, val) requestData["arg" + std::to_string(argIndex)] = val

// param 1: string of function name
// param 2: implementation of setting values to arguments
// param 3: return type
// param 4: argument list
#define RPC_REQ_FUNC(funcNameStr, setArgsImpl, retType, ...) [__VA_ARGS__]() -> retType { \
    json requestData; \
    setArgsImpl \
    return requestRPC(funcNameStr, requestData); \
}()

json requestRPC(std::string funcName, json& requestData);