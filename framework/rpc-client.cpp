#include <iostream>
#include "rpc-client.hpp"
#include "unisock.hpp"
#include "crypto/ecc.hpp"

std::string rpcHost = "127.0.0.1";
int rpcPort = 12345;

namespace rpc {
    void SetServer(std::string rHost, int rPort) {
        rpcHost = rHost;
        rpcPort = rPort;
        return;
    }
}

json requestRPC(std::string funcName, json& requestData) {
    RawSocket clientSocket;

    clientSocket.connect(rpcHost, rpcPort);

    size_t bufferSize = 1024 * 1024;
    std::vector<uint8_t> buffer(bufferSize);
    std::string response;
    json responseData;

    buffer.resize(clientSocket.receive(buffer.data(), buffer.size()));
    response.assign(buffer.begin(), buffer.end());
    responseData = json::parse(response);
    EccCryptoInfo keyCryptoInfo;
    keyCryptoInfo.finiteField = responseData["FiniteField"];
    keyCryptoInfo.paramA = responseData["ParamA"];
    keyCryptoInfo.basePoint = responseData["BasePoint"];
    keyCryptoInfo.publicKey = responseData["PublicKey"];
    response.clear();
    responseData.clear();

    EccCryptoInfo sessionCryptoInfo;
    EccKeyGen(
        sessionCryptoInfo.finiteField,
        sessionCryptoInfo.paramA,
        sessionCryptoInfo.basePoint.first, sessionCryptoInfo.basePoint.second,
        sessionCryptoInfo.publicKey.first, sessionCryptoInfo.publicKey.second,
        sessionCryptoInfo.privateKey
    );
    requestData["FiniteField"] = sessionCryptoInfo.finiteField;
    requestData["ParamA"] = sessionCryptoInfo.paramA;
    requestData["BasePoint"] = sessionCryptoInfo.basePoint;
    requestData["PublicKey"] = sessionCryptoInfo.publicKey;

    requestData["function"] = funcName;

    std::string request = requestData.dump();
    buffer.resize(request.size());
    buffer.assign(request.begin(), request.end());
    buffer = EccEnc(
        buffer,
        keyCryptoInfo.finiteField,
        keyCryptoInfo.paramA,
        keyCryptoInfo.basePoint.first,
        keyCryptoInfo.basePoint.second,
        keyCryptoInfo.publicKey.first,
        keyCryptoInfo.publicKey.second
    );

    clientSocket.send(buffer.data(), buffer.size());

    buffer.resize(bufferSize);
    buffer.resize(clientSocket.receive(buffer.data(), buffer.size()));
    buffer = EccDec(
        buffer,
        sessionCryptoInfo.finiteField,
        sessionCryptoInfo.paramA,
        sessionCryptoInfo.privateKey
    );
    response.assign(buffer.begin(), buffer.end());
    responseData = json::parse(response);
    if (responseData.find("error") != responseData.end()) {
        std::string error = responseData["error"];
        throw std::runtime_error("Error: " + error);
    }
    else if (responseData.find("result") != responseData.end()) {
        return responseData["result"];
    }
    else {
        throw std::runtime_error("Error: Bad response");
    }
}

// user content
//namespace rpc {
//    int add(int arg1, int arg2) {
//        return
//            RPC_REQ_FUNC(
//                "add",
//                RPC_SET_ARGS(1, arg1);
//                RPC_SET_ARGS(2, arg2); ,
//                int,
//                arg1, arg2
//            );
//    }
//}
