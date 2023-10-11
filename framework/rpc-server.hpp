#pragma once
#include <functional>
#include <map>
#include <any>
#include "json.hpp"

using json = nlohmann::json;

// Define a variant type to store various argument types
using Argument = std::any;

// Define a variant type to store various return types
using ReturnValue = std::any;

class RpcFunctions {
public:
    void registerFunction(const std::string& name, const std::function<ReturnValue(const std::vector<Argument>&)>& func);
    ReturnValue callFunction(const std::string& name, const std::vector<Argument>& args);

private:
    std::map<std::string, std::function<ReturnValue(const std::vector<Argument>&)>> registeredFunctions;
};

extern RpcFunctions rpc;

#define RPC_REG_FUNC_ARG(argIndex, argType) std::any_cast<json>(args[argIndex - 1]).get<argType>()

// param 1: function name
// param 2: amount of arguments
// param 3: implementation of registering arguments
#define RPC_REG_FUNC(funcName, argCount, ...) { \
      rpc.registerFunction(#funcName, [](const std::vector<Argument>& args) -> ReturnValue { \
          if (args.size() != argCount) { \
              throw std::runtime_error("Invalid argument count"); \
          } \
          ReturnValue result = funcName(__VA_ARGS__); \
          return result; \
      }); \
}

#define RPC_CALL_FUNC(funcNameStr, retType, argVec) std::any_cast<retType>(rpc.callFunction(funcNameStr, argVec))
