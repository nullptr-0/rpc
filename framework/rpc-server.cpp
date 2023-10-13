﻿#include <iostream>
#include <thread>
#include "rpc-server.hpp"
#include "unisock.hpp"
#include "crypto/ecc.hpp"

RpcFunctions rpc;

void RpcFunctions::registerFunction(const std::string& name, const std::function<ReturnValue(const std::vector<Argument>&)>& func) {
    registeredFunctions[name] = func;
}

ReturnValue RpcFunctions::callFunction(const std::string& name, const std::vector<Argument>& args) {
    if (registeredFunctions.find(name) != registeredFunctions.end()) {
        return registeredFunctions[name](args);
    }
    else {
        throw std::runtime_error("Function not registered");
    }
}

void RegFuncs();
void ExecFunc(const std::string funcName, const std::vector<Argument>& args, json& retVal);

class RpcServer {
public:
    RpcServer(int port) : port(port), pSocket(nullptr) {
        RegFuncs();
    }

    void start() {
        EccKeyGen(
            cryptoInfo.finiteField,
            cryptoInfo.paramA,
            cryptoInfo.basePoint.first, cryptoInfo.basePoint.second,
            cryptoInfo.publicKey.first, cryptoInfo.publicKey.second,
            cryptoInfo.privateKey
        );
        try {
            pSocket = new RawSocket;
            if (!pSocket) {
                throw std::runtime_error("Cannot create socket instance");
            }
            pSocket->listen(port);
            std::thread newThread(&RpcServer::process, this);
            newThread.detach();
            std::cout << "Server started on port " << port << std::endl;
            std::cin.get();
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void process() {
        try {
            if (!pSocket) {
                throw std::runtime_error("Socket instance no longer available");
            }
            RawSocket clientSocket = pSocket->accept();

            std::thread newThread(&RpcServer::process, this);
            newThread.detach();

            json responseData;
            std::string response;
            responseData["FiniteField"] = cryptoInfo.finiteField;
            responseData["ParamA"] = cryptoInfo.paramA;
            responseData["BasePoint"] = cryptoInfo.basePoint;
            responseData["PublicKey"] = cryptoInfo.publicKey;
            response = responseData.dump();
            clientSocket.send(response.c_str(), response.size());
            responseData.clear();

            size_t bufferSize = 1024 * 1024;
            std::vector<uint8_t> buffer(bufferSize);
            std::string request;
            buffer.resize(clientSocket.receive(buffer.data(), buffer.size()));
            buffer = EccDec(
                buffer,
                cryptoInfo.finiteField,
                cryptoInfo.paramA,
                cryptoInfo.privateKey
            );
            request.assign(buffer.begin(), buffer.end());

            json requestData = json::parse(request);
            EccCryptoInfo sessionCryptoInfo;
            sessionCryptoInfo.finiteField = requestData["FiniteField"];
            sessionCryptoInfo.paramA = requestData["ParamA"];
            sessionCryptoInfo.basePoint = requestData["BasePoint"];
            sessionCryptoInfo.publicKey = requestData["PublicKey"];
            std::string functionName = requestData["function"];
            std::vector<Argument> args;
            for (size_t i = 1; requestData.find("arg" + std::to_string(i)) != requestData.end(); ++i)
            {
                // check if arg exists in the JSON object before adding it
                args.push_back(requestData["arg" + std::to_string(i)]);
            }

            try {
                ExecFunc(functionName, args, responseData["result"]);
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                responseData["error"] = e.what();
            }
            response = responseData.dump();

            buffer.resize(response.size());
            buffer.assign(response.begin(), response.end());
            buffer = EccEnc(
                buffer,
                sessionCryptoInfo.finiteField,
                sessionCryptoInfo.paramA,
                sessionCryptoInfo.basePoint.first,
                sessionCryptoInfo.basePoint.second,
                sessionCryptoInfo.publicKey.first,
                sessionCryptoInfo.publicKey.second
            );
            clientSocket.send(buffer.data(), buffer.size());
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

private:
    int port;
    RawSocket* pSocket;
    EccCryptoInfo cryptoInfo;
};

int main(int argc, char* argv[]) {
    int port = 12345;
    if (argc == 1)
    {
        std::cout << "No parameters. Using default port " << port << std::endl;
    }
    else if (argc == 2)
    {
        for (size_t i = 0; i < strlen(argv[1]); i++)
        {
            if (!isdigit(argv[1][i]))
            {
                std::cerr << "Error: Invalid parameter for port" << std::endl;
            }
        }
        port = atoi(argv[1]);
    }
    else if (argc > 2)
    {
        std::cerr << "Error: Too many parameters" << std::endl;
    }
    RpcServer server(port);
    server.start();
    return 0;
}

// user content
//void RegFuncs() {
//    RPC_REG_FUNC(
//        add,
//        2,
//        RPC_REG_FUNC_ARG(1, int, arg1),
//        RPC_REG_FUNC_ARG(2, int, arg2)
//    );
//    // register other funcs here...
//    return;
//}
//
//void ExecFunc(const std::string funcName, const std::vector<Argument>& args, json& retVal) {
//    if (funcName == "add") // example
//    {
//        retVal = RPC_CALL_FUNC("add", int, args);
//    }
//    // for other funcs
//    // else if (funcName == "OtherFunc")
//    // {
//    //     retVal = RPC_CALL_FUNC("OtherFunc", int, args);
//    // }
//    else
//    {
//        // your error handling here
//    }
//    return;
//}
